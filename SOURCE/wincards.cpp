/* wincards.cpp (emx+gcc) */

#include <stdio.h>
#include <stdlib.h>
#define INCL_WIN
#define INCL_GPI
#define LCID_FONT       1
#define ID_BUTTON       1
#define WM_ROLLCARDS WM_USER+1
#define WM_SEARCH    WM_USER+2
#define MLEBUFFER       500

#define POINTSIZE_14            140
#define POINTSIZE_12            120
#define POINTSIZE_10            100
#include <os2.h>
#include "cardcpp.h"
#include "WINCARDS.H"
/*                                                              */
/*                                                              */
/* Define Global Variables                                      */
/*                                                              */
/*                                                              */

/*      Windows                                                 */

MRESULT EXPENTRY ClientWndProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY IndexWndProc (HWND, ULONG, MPARAM, MPARAM);

/*      Dialogs                                                 */

MRESULT EXPENTRY SearchDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AddDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY IndexDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc (HWND, ULONG, MPARAM, MPARAM);

HAB hab;
QMSG qmsg;
FATTRS fat;
PSWP aSwp;
PHWND phwndIndex;
INT iCharheight, iCharwidth, iCardcount, iTempcount, iCardmove = 0;
const int namesize = 41;
CHAR * pszFilename = NULL;
BOOL bChanged = FALSE;
Linkedlist cardlist, templist;
CHAR szMleBuf[MLEBUFFER];

Card newcard;
CHAR blank[40] = {'\0'};

void editcard(HWND);

/*                                                              */
/*              Begin main procedure                            */
/*                                                              */

int main (int argc, CHAR * argv [])
{
    ULONG flFrameFlags;
    static UCHAR  szClientClass[] = "wincards.child",
    szFonttype[] = "System VIO",
    szFilename[] = "Cardfile/2";
    HPS hps;
    HMQ hmq;
    HDC hdc;
    PFONTMETRICS pfm;
    LONG lVertRes, lHorizRes, cMonoFonts, cFonts = 0;
    SHORT sFontsize;
    HWND hwndFrame, hwndClient;
    INT iFontsize;

    /*      Perform initialization                                          */

    iCardcount = 1;
    cardlist.Linkedlist();

    newcard.Wincard(blank, 0, 0);
    cardlist.insertcard(newcard);

    hab = WinInitialize (0);
    hmq = WinCreateMsgQueue (hab, 0);

    /*      Set up the space for the dynamic variables associated           */
    /*      with the cards and dependent on the number of cards             */

    /*      Use C++ versions of functions to initialize dynamic storage --  */
    /*      C versions are:                                                 */
    /*      aSwp = malloc (iCardcount * sizeof(SWP));                       */
    /*      phwndIndex = malloc (iCardcount * sizeof (HWND));               */

    if (argc > 1) pszFilename = argv[1];

    if (pszFilename != NULL)
    {
        fileopen (pszFilename);
        iCardcount = verify(pszFilename);
        cardlist = readcardfile(iCardcount, pszFilename);
        aSwp = new SWP[iCardcount];
        phwndIndex = new HWND[iCardcount];
    }
    else
    {
        aSwp = new SWP[1];
        phwndIndex = new HWND[1];
    }
    /*                                                                      */
    /*      Register client class                                           */
    /*                                                                      */

    WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0L);

    /*                                                                      */
    /* Omit FCF_SHELLPOS from flFrameFlags in favor of WinSetWindowPos      */
    /*                                                                      */

    flFrameFlags = (    FCF_TITLEBAR      | FCF_SYSMENU |
                        FCF_SIZEBORDER    | FCF_MINMAX   |
                        FCF_MENU          | FCF_ACCELTABLE |
                        FCF_ICON          | FCF_TASKLIST);

    /*                                                                      */
    /*                                                                      */
    /*      Create the standard frame window                                */
    /*                                                                      */

    hwndFrame = WinCreateStdWindow(     HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &flFrameFlags,
                                        szClientClass,
                                        szFilename,
                                        0L,
                                        NULLHANDLE,
                                        ID_WINCARDS,
                                        &hwndClient
                                );

    /*                                                                      */
    /*      Set the window size and position according to the resolution    */
    /*                                                                      */

    lVertRes=lHorizRes=100;

    hdc = WinOpenWindowDC(hwndClient);

    /*                                                                      */
    /*          Query Device Capabilities                                   */
    /*                                                                      */

    DevQueryCaps (hdc, CAPS_HEIGHT, 1L, &lVertRes);
    DevQueryCaps (hdc, CAPS_WIDTH, 1l, &lHorizRes);

    /*                                                              */
    /*      Set up to use the System VIO font for use in this       */
    /*      Presentation Space (font has to be fixed pitch for      */
    /*      purposes of simplification of the program and for       */
    /*      imitation of the Windows version)                       */
    /*                                                              */

    hps = WinBeginPaint (hwndFrame, NULLHANDLE, NULL);

    cMonoFonts = GpiQueryFonts  (
                                        hps,
                                        QF_PUBLIC,
                                        szFonttype,
                                        &cFonts,
                                        0L,
                                        (PFONTMETRICS) NULL
                                );

    /*      Allocate space for the four fonts                       */
    /*      C version of function is:                               */
    /*      pfm = malloc (cMonoFonts*sizeof(FONTMETRICS));          */

    pfm = new FONTMETRICS[cMonoFonts];

    /*      Load the font information into the pfm variable         */

    GpiQueryFonts (hps, QF_PUBLIC, szFonttype,
    &cMonoFonts, sizeof (FONTMETRICS), pfm);

    WinEndPaint (hps);

    /*      Get a reasonable font size -- 14 point for big screens, */
    /*      less for smaller                                        */

    if (lHorizRes >= 1024) sFontsize = POINTSIZE_14;
    else if (lHorizRes >= 800) sFontsize = POINTSIZE_12;
    else sFontsize = POINTSIZE_10;

    iFontsize=0;
    while (pfm[iFontsize].sNominalPointSize < sFontsize) iFontsize++;

    /*      Set up the font                                         */

    fat.usRecordLength = sizeof (fat);
    fat.fsSelection = 0L;
    fat.lMatch = pfm[iFontsize].lMatch;
    iCharheight = pfm[iFontsize].lMaxBaselineExt;
    iCharwidth = pfm[iFontsize].lAveCharWidth;

    strcpy (fat.szFacename, (PCHAR) szFonttype);

    /*      C version is:                                                   */
    /*      free (pfm);                                                     */

    delete (pfm);

    WinSetWindowPos (   hwndFrame,
                        NULLHANDLE,
                        (lHorizRes/2) - 1,
                        (4*lVertRes/9) - 1,     /* x-coord, y-coord */
                        lHorizRes/2,
                        5*lVertRes/9,     /* take 5/9 of the screen */
                        SWP_ACTIVATE | SWP_SIZE | SWP_MOVE
                    );

    while (WinGetMsg (hab, &qmsg, 0L, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    WinDestroyWindow (hwndFrame);
    WinDestroyMsgQueue (hmq);
    WinTerminate (hab);

    return 0;
}

/*                                                                      */
/*              End main procedure                                      */
/*                                                                      */

/*                                                                      */
/*              Begin Client Window procedure                           */
/*                                                                      */

MRESULT EXPENTRY ClientWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HPS hps;
    static FILEDLG fild;
    static MLE_SEARCHDATA mlesrch;
    static HWND hwndButton[4], hwndMLE, hwndDlg, hwndMain, hwndMenu;
    static RECTL rcl;
    INT i, id, iTextlength, iJump = 8;
    LONG lStartsel, lEndsel, lOffset;
    CHAR szCardindex[namesize], szSearchChar[2];
    char* tempinfo;

    static CHAR *szButtonLabel[] = { "<<","<",">",">>"    };
    static UCHAR  szIndexCardClass[] = "Index_value",
                  pszFileopen[10] = "Open File",
                  pszSaveas [8] = "Save As",
                  pszMerge [14] = "File to Merge";
    static CHAR pszFullFile[CCHMAXPATH] = "*.CRD";

    /*      For some reason, the FILEDLG variable won't initialize properly using   */
    /*      memset(...) -- the elements have to be set to zero individually.        */

    FILEDLG InitializeFiledlgFields();

    switch (msg)
    {
    case WM_CREATE:

        szMleBuf[0] = '\0';

        /*                                                              */
        /*      Create the four hot buttons                             */
        /*                                                              */

        for (id = 0; id < 4; id++)
        {
            hwndButton[id] = WinCreateWindow(
                                                hwnd,
                                                WC_BUTTON,
                                                (PUCHAR) szButtonLabel[id],
                                                WS_VISIBLE | BS_PUSHBUTTON,
                                                ((id)*rcl.xRight)+ 1,
                                                2,
                                                rcl.xRight,
                                                rcl.yTop/10,
                                                hwnd,
                                                HWND_BOTTOM,
                                                id+1000,
                                                NULL,
                                                NULL
                                            );
        }

        /*                                                              */
        /*      End hot button creation                                 */
        /*                                                              */

        hwndMLE = WinCreateWindow (     hwnd,
                                        WC_MLE,
                                        NULL,
                                        WS_VISIBLE | MLS_BORDER | MLS_WORDWRAP,
                                        0,
                                        0,
                                        0,
                                        0,
                                        hwnd,
                                        HWND_BOTTOM,
                                        5000,
                                        NULL,
                                        NULL
                                  );

        WinSendMsg (hwndMLE, MLM_SETBACKCOLOR, CLR_BACKGROUND, NULL);

        WinSendMsg (hwndMLE, MLM_SETFONT, (MPARAM) &fat, (MPARAM) 0);

        WinSetAccelTable (hab, NULLHANDLE, hwndMLE);

        /*                                                              */
        /*      Create the index windows for the actual cards           */
        /*                                                              */
        WinRegisterClass (      hab,
                                szIndexCardClass,
                                IndexWndProc,
                                CS_SIZEREDRAW | CS_SYNCPAINT | CS_CLIPSIBLINGS,
                                0L
                         );

        for (i = 0; i < iCardcount; i++)
            phwndIndex[i] = WinCreateWindow(    hwnd,
                                                szIndexCardClass,
                                                NULL,
                                                WS_VISIBLE,
                                                0,
                                                0,
                                                0,
                                                0,
                                                hwnd,
                                                HWND_BOTTOM,
                                                i,       /* Set win id to # of card */
                                                NULL,
                                                NULL
                                           );

        return 0;

    case WM_SIZE:

        WinEnableWindowUpdate(hwnd, FALSE);

        WinQueryWindowRect(hwnd, &rcl);

        rcl.xRight/=4;

        /*                                                              */
        /*      Create the four hot buttons                             */
        /*                                                              */

        for (id = 0; id < 4; id++)
        {

            /*                                                              */
            /*      First destroy any old ones left hanging around          */
            /*                                                              */

            WinDestroyWindow(hwndButton[id]);

            /*                                                              */
            /*      Then create new hot buttons, positioning them in the    */
            /*      frame window as they are made                           */
            /*                                                              */

            hwndButton[id] = WinCreateWindow(
                                                hwnd,
                                                WC_BUTTON,
                                                (PUCHAR) szButtonLabel[id],
                                                WS_VISIBLE | BS_PUSHBUTTON,
                                                ((id)*rcl.xRight)+ 1,
                                                2,
                                                rcl.xRight,
                                                rcl.yTop/10,
                                                hwnd,
                                                HWND_BOTTOM,
                                                id+1000,
                                                NULL,
                                                NULL
                                    );
        }

        /*                                                              */
        /*      End hot button creation                                 */

        /*      Set the window size and position of the MLE window      */
        /*                                                              */

        /*      Set the window position all the way at the back -- the  */
        /*      index value windows will be moved behind it again       */

        WinSetWindowPos (       hwndMLE,
                                HWND_BOTTOM,
                                rcl.xLeft + 2,
                                (rcl.yTop/10) + 4,     /* x-coord, y-coord */
                                int (42.1 * iCharwidth),
                                int (11.8*iCharheight),
                                SWP_ACTIVATE | SWP_SIZE | SWP_MOVE);

        /*                                                              */
        /*      Set the font to the correct value                       */
        /*                                                              */

        WinSendMsg (hwndMLE, MLM_SETFONT, (MPARAM) &fat, (MPARAM) 0);

        /*                                                              */
        /*      Set the bottom limit for text so that it doesn't        */
        /*      scroll off the bottom                                   */
        /*                                                              */

        WinSendMsg (hwndMLE, MLM_SETFORMATRECT, NULL, (MPARAM) MLFFMTRECT_LIMITVERT);

        /*      Place text on the MLE                                   */

        WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT,    MPFROMP (szMleBuf),
        MPFROMSHORT ((USHORT) sizeof (szMleBuf)));

        for (id = 0; id < ((cardlist.showcard(0).shownumcardchars())-1) && (id < 440); id++)
            szMleBuf[id] = (cardlist.showcard(0).showcardinfo())[id];

        WinSendMsg (hwndMLE, MLM_DISABLEREFRESH, NULL, NULL);

        /*      Clear the current contents of the MLE                   */

        iTextlength = (IPT) WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL);

        WinSendMsg (hwndMLE, MLM_DELETE, 0, (MPARAM) iTextlength);

        bChanged = FALSE;

        lOffset = 0;

        WinSendMsg (hwndMLE, MLM_IMPORT, MPFROMP (&lOffset),
        MPFROMSHORT ((USHORT) id));

        bChanged = FALSE;

        WinSendMsg (hwndMLE, MLM_ENABLEREFRESH, NULL, NULL);

        /*      Set the focus to the MLE                                */

        WinSetFocus (HWND_DESKTOP, hwndMLE);

        /*                                                              */
        /*      Now draw the window:                                    */
        /*      First set the window size and position, using the       */
        /*      size of the font and the size of the client window      */
        /*      to set the window size and position                     */
        /*                                                              */

        aSwp[0].hwndInsertBehind = hwndMLE;

        for (id = 0; id < iCardcount; id++)
        {
            if (id > 0) aSwp[id].hwndInsertBehind = phwndIndex[id - 1];
            aSwp[id].hwnd = phwndIndex[id];
            aSwp[id].fl = (SWP_ZORDER | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE);
            aSwp[id].cx = 42 * iCharwidth;
            aSwp[id].cy = 13 * iCharheight;
            aSwp[id].x = ((id * 2) * iCharwidth) + 2;
            aSwp[id].y = (id * (iCharheight +3)) + (rcl.yTop/10) + 4;
        }

        /*      n.b. hab is global                                               */

        WinSetMultWindowPos (hab, aSwp, iCardcount);

        WinEnableWindowUpdate (hwnd, TRUE);

        break;
        return 0;

    case WM_ROLLCARDS:

        if (bChanged) editcard(hwndMLE);

        cardlist.movecard(iCardmove);

        strcpy(szMleBuf, (cardlist.showcard(0).showcardinfo()));

        /*      MLM_DISABLEREFRESH doesn't work!  Have to use WinEnableWindowUpdate   */

        WinEnableWindowUpdate (hwndMLE, FALSE);

        /*      Clear the current contents of the MLE                   */

        iTextlength = (IPT) WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL);

        bChanged = FALSE;

        WinSendMsg (hwndMLE, MLM_DELETE, 0, (MPARAM) iTextlength);

        bChanged = FALSE;

        lOffset = 0;

        /*      Import new text                                         */

        id = ((cardlist.showcard(0).shownumcardchars())-1);
        if (id < 0) id = 0;

        WinSendMsg (hwndMLE, MLM_IMPORT, MPFROMP (&lOffset),
        MPFROMSHORT ((USHORT) id));

        WinShowWindow (hwndMLE, TRUE);

        bChanged = FALSE;

        /*      Set the focus to the MLE                                */

        WinSetFocus (HWND_DESKTOP, hwndMLE);

        /*      Update the index listings                               */

        for (id = 0; id < iCardcount; id++) WinInvalidateRect (phwndIndex[id], NULL, FALSE);

        return 0;

    case WM_CHAR:

        if (SHORT1FROMMP(mp1) &KC_KEYUP) return 0;

        szSearchChar[1] = '\0';
        szSearchChar[0] = (CHAR) SHORT1FROMMP(mp2);

        if ((SHORT1FROMMP(mp1) &KC_SHIFT) && (SHORT1FROMMP(mp1) &KC_CTRL)
                && isalpha(szSearchChar[0]))
        {
            id = 0;
            iCardmove = 0;

            if (cardlist.showcard(1) == szSearchChar) cardlist.movecard(1);
            else
            {
                do
                {
                    cardlist.movecard(1);
                    id++;
                }
                while (!(cardlist.showcard(0) == szSearchChar) && id < iCardcount);
            }
            WinSendMsg(hwnd, WM_ROLLCARDS, NULL, NULL);
        }

        return 0;

    case WM_PAINT:

        hps = WinBeginPaint (hwnd, NULLHANDLE, NULL);

        GpiErase (hps);

        WinEndPaint (hps);

        return 0;

        /*      Process messages from the MLE window                    */

    case WM_CONTROL:

        switch (SHORT2FROMMP (mp1))
        {
        case MLN_OVERFLOW:

            DosBeep(880,100);
            DosBeep(330,100);

            WinMessageBox(HWND_DESKTOP, hwnd, (const PUCHAR) "Too much text!", NULL, 0, MB_OK | MB_ICONEXCLAMATION);

            break;

        case MLN_PIXHORZOVERFLOW:
        case MLN_HSCROLL:

            DosBeep(660,100);
            DosBeep(880,100);
            WinSendMsg(hwndMLE, MLM_UNDO, NULL, NULL);

            break;

        case MLN_CHANGE:

            /*      If the MLM text has changed, prepare to change the associated card      */
            bChanged = TRUE;
            break;
        }
        return 0;

    case WM_COMMAND:

        /*      This section handles the messages sent by the button    */
        /*      windows --  the card number is increased or decremented */

        switch (COMMANDMSG(&msg)->cmd)
        {

        case 1000:

            iCardmove = (-iJump);
            WinSendMsg (hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

        case 1001:

            iCardmove = (-1);
            WinSendMsg (hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

        case 1002:

            iCardmove = 1;
            WinSendMsg (hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

        case 1003:

            iCardmove = iJump;
            WinSendMsg (hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

            /*      Code to override the default cut/paste accelerator table        */

        case SRCH_C:

            szSearchChar[1] = '\0';
            szSearchChar[0] = 'c';
            id = 0;
            iCardmove = 0;

            if (cardlist.showcard(1) == szSearchChar) cardlist.movecard(1);
            else
            {
                do
                {
                    cardlist.movecard(1);
                    id++;
                }
                while (!(cardlist.showcard(0) == szSearchChar) && id < iCardcount);
            }

            WinSendMsg(hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

        case SRCH_V:

            szSearchChar[1] = '\0';
            szSearchChar[0] = 'v';
            id = 0;
            iCardmove = 0;

            if (cardlist.showcard(1) == szSearchChar) cardlist.movecard(1);
            else
            {
                do
                {
                    cardlist.movecard(1);
                    id++;
                }
                while (!(cardlist.showcard(0) == szSearchChar) && id < iCardcount);
            }

            WinSendMsg(hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

        case SRCH_X:

            szSearchChar[1] = '\0';
            szSearchChar[0] = 'x';
            id = 0;
            iCardmove = 0;

            if (cardlist.showcard(1) == szSearchChar) cardlist.movecard(1);
            else
            {
                do
                {
                    cardlist.movecard(1);
                    id++;
                }
                while (!(cardlist.showcard(0) == szSearchChar) && id < iCardcount);
            }

            WinSendMsg(hwnd, WM_ROLLCARDS, NULL, NULL);

            break;

            return (MRESULT) 0;
        }

    case WM_INITMENU:
        hwndMain = WinQueryWindow (hwnd, QW_PARENT);
        hwndMenu = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT), FID_MENU);

        switch (SHORT1FROMMP (mp1))
        {
        case IDM_EDIT:

            /*      First check to see if text is selected; WinSendMsg must */
            /*      be cast to LONG to prevent compiler warnings            */

            lStartsel = (LONG) WinSendMsg ( hwndMLE,
            MLM_QUERYSEL,
            (MPARAM) MLFQS_MINSEL,
            0L );
            lEndsel = (LONG) WinSendMsg (   hwndMLE,
            MLM_QUERYSEL,
            (MPARAM) MLFQS_MAXSEL,
            0L );

            /*      If text is selected, enable copy and cut menu options   */

            if (lEndsel - lStartsel > 0L)
            {
                WinSendMsg (    hwndMenu,
                                MM_SETITEMATTR,
                                MPFROM2SHORT (IDM_COPY, TRUE),
                                MPFROM2SHORT (MIA_DISABLED, 0));

                WinSendMsg (    hwndMenu,
                                MM_SETITEMATTR,
                                MPFROM2SHORT (IDM_CUT, TRUE),
                                MPFROM2SHORT (MIA_DISABLED, 0));
            }

            /*      Otherwise disable copy and cut                          */

            else
            {
                WinSendMsg (    hwndMenu,
                                MM_SETITEMATTR,
                                MPFROM2SHORT (IDM_COPY, TRUE),
                                MPFROM2SHORT (MIA_DISABLED, MIA_DISABLED));

                WinSendMsg (    hwndMenu,
                                MM_SETITEMATTR,
                                MPFROM2SHORT (IDM_CUT, TRUE),
                                MPFROM2SHORT (MIA_DISABLED, MIA_DISABLED));
            }
            return (MRESULT)0;
            break;
        }

        switch (SHORT1FROMMP (mp1))
        {
        case IDM_COPY:

            /*      Copy text from the MLE                                  */

            WinSendMsg (hwndMLE, MLM_COPY, 0L, 0L);

            return 0;

            /*      Cut text from the MLE                                   */

        case IDM_CUT:

            WinSendMsg (hwndMLE, MLM_CUT, 0L, 0L);
            bChanged = TRUE;

            return 0;

            /*      Paste text into the MLE                                 */

        case IDM_PASTE:

            WinSendMsg (hwndMLE, MLM_PASTE, 0L, 0L);
            bChanged = TRUE;

            return 0;

            /*      Edit the card title                                     */

        case IDM_INDEX:

            if (bChanged) editcard (hwndMLE);
            bChanged = FALSE;

            WinDlgBox (HWND_DESKTOP, hwnd, IndexDlgProc, NULLHANDLE, IDD_INDEX, NULL);
            WinSendMsg (hwnd, WM_ROLLCARDS, NULL, NULL);

            return 0;

            /*      Add a card                                              */

        case IDM_ADD:

            if (bChanged) editcard(hwndMLE);

            /*      Once old card has been edited new card does not need changes    */

            bChanged = FALSE;

            WinDlgBox (HWND_DESKTOP, hwnd, AddDlgProc, NULLHANDLE, IDD_ADD, NULL);

            /*      If a card was added, bChanged = TRUE -- delete old      */
            /*      cards and redraw entire window                          */

            if (bChanged)
            {
                for (i = 0; i < iCardcount; i++) WinDestroyWindow(phwndIndex[i]);

                delete [] aSwp;
                delete [] phwndIndex;

                iCardcount = iCardcount + 1;

                aSwp = new SWP[iCardcount];
                phwndIndex = new HWND[iCardcount];

                for (i = 0; i < iCardcount; i++)
                    phwndIndex[i] = WinCreateWindow( hwnd,
                                                     szIndexCardClass,
                                                     NULL,
                                                     WS_VISIBLE,
                                                     0, 0, 0, 0,
                                                     hwnd,
                                                     HWND_BOTTOM,
                                                     i,       /* Set win id to # of card */
                                                     NULL,
                                                     NULL
                                                   );

                bChanged = FALSE;

                WinSendMsg (hwnd, WM_SIZE, NULL, NULL);
            }
            return 0;

            /*      Delete a card                                           */

        case IDM_DELETE:

            /*      Should have a message pop up here                       */

            iCardmove = 0;

            /*      Update the number of cards -- destroy old ones, create new      */

            for (i = 0; i < iCardcount; i++) WinDestroyWindow(phwndIndex[i]);

            /*      If the last card was deleted then start a new file --   */
            /*      Explicitly destroy old list, start with a blank card    */

            if (!cardlist.delcard())
            {
                iCardcount = 1;
                cardlist.~Linkedlist();
                newcard.Wincard(blank, 0, 0);
                cardlist.insertcard(newcard);
            }
            else iCardcount = iCardcount - 1;

            for (i = 0; i < iCardcount; i++)
                phwndIndex[i] = WinCreateWindow( hwnd,
                                                 szIndexCardClass,
                                                 NULL,
                                                 WS_VISIBLE,
                                                 0, 0, 0, 0,
                                                 hwnd,
                                                 HWND_BOTTOM,
                                                 i,
                                                 NULL,
                                                 NULL
                );

            /*      Use WM_SIZE to update the number of card windows        */

            WinSendMsg (hwnd, WM_SIZE, NULL, NULL);

            return 0;

            /*      Duplicate a card                                        */

        case IDM_DUP:

            if (bChanged) editcard (hwndMLE);

            id = 0;

            /*      Get the information on the card and read it into variables      */

            strcpy(szCardindex,(cardlist.showcard(0)).showindexinfo());
            id = (INT) (cardlist.showcard(0)).shownumcardchars();
            tempinfo = new char [id];
            strcpy(tempinfo, (cardlist.showcard(0)).showcardinfo());

            /*      Use the information to create a duplicate card and insert it    */

            newcard.Wincard(szCardindex, id, tempinfo);
            cardlist.insertcard(newcard);
            delete [] tempinfo;

            /*      Destroy old cards, update the number of cards in the list,      */
            /*      and redraw                                                      */

            for (i = 0; i < iCardcount; i++) WinDestroyWindow(phwndIndex[i]);

            delete [] aSwp;
            delete [] phwndIndex;

            iCardcount = iCardcount + 1;

            aSwp = new SWP[iCardcount];
            phwndIndex = new HWND[iCardcount];

            for (i = 0; i < iCardcount; i++)
                phwndIndex[i] = WinCreateWindow(hwnd,
                                                szIndexCardClass,
                                                NULL,
                                                WS_VISIBLE,
                                                0,
                                                0,
                                                0,
                                                0,
                                                hwnd,
                                                HWND_BOTTOM,
                                                i,
                                                NULL,
                                                NULL
                                               );

            bChanged = FALSE;
            iCardmove = 0;

            /*      Use WM_SIZE to recreate the list with the correct number of cards       */

            WinSendMsg (hwnd, WM_SIZE, NULL, NULL);

            return 0;

        case IDM_FIND:

            if (WinDlgBox(HWND_DESKTOP, hwnd, SearchDlgProc, NULLHANDLE, IDD_SEARCH, &mlesrch))
            {
                iCardmove = 1;
                i = 0;

                WinEnableWindowUpdate(hwndMLE, FALSE);

                while (i < iCardcount)
                {
                    i++;
                    cardlist.movecard(iCardmove);

                    strcpy(szMleBuf, (cardlist.showcard(0).showcardinfo()));

                    iTextlength = (IPT) WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL);
                    WinSendMsg (hwndMLE, MLM_DELETE, 0, (MPARAM) iTextlength);

                    bChanged = FALSE;

                    lOffset = 0;
                    id = ((cardlist.showcard(0).shownumcardchars())-1);
                    if (id < 0) id = 0;

                    WinSendMsg (hwndMLE, MLM_IMPORT, MPFROMP (&lOffset), MPFROMSHORT ((USHORT) id));

                    bChanged = FALSE;

                    WinSendMsg (hwndMLE, MLM_SEARCH, MPFROMLONG(MLFSEARCH_SELECTMATCH), MPFROMP(&mlesrch));

                    WinSetFocus (HWND_DESKTOP, hwndMLE);

                    /*      End loop if a match is found                                            */

                    lStartsel = (LONG) WinSendMsg ( hwndMLE, MLM_QUERYSEL, (MPARAM) MLFQS_MINSEL, 0L );
                    lEndsel = (LONG) WinSendMsg (   hwndMLE, MLM_QUERYSEL, (MPARAM) MLFQS_MAXSEL, 0L );

                    /*      If text is selected, a match has been found -- end loop                 */

                    if (lEndsel - lStartsel > 0L)
                    {
                        i = iCardcount;
                        for (id = 0; id < iCardcount; id++) WinInvalidateRect (phwndIndex[id], NULL, FALSE);
                    }
                }

                WinShowWindow(hwndMLE, TRUE);
                bChanged = FALSE;
            }
            return 0;

        case IDM_EXIT:

            /*      Quit and end program                                            */
            /*      Free the memory set aside for the cards and the position info   */
            /*      Use C++ functions to delete dynamically allocated memory --     */
            /*      C versions are: free (aSwp); free (phwndIndex);                 */

            delete [] aSwp;
            delete [] phwndIndex;
            WinSendMsg (hwnd, WM_CLOSE, NULL, NULL);
            return (MRESULT)0;

        case IDM_NEW:

            /*                                                      */
            /*      Nuke all windows and start over                 */
            /*                                                      */
            delete [] aSwp;
            for (id = 0; id < 4; id++) WinDestroyWindow(hwndButton[id]);
            for (id = 0; id < iCardcount; id++) WinDestroyWindow(phwndIndex[id]);
            delete [] phwndIndex;
            WinDestroyWindow(hwndMLE);
            iCardcount = 1;
            pszFilename = NULL;
            /*      Explicit use of destructor here for old list -- a temporary     */
            /*      variable could be used instead to reinitialize the list with    */
            /*      the assignment operator                                         */

            cardlist.~Linkedlist();

            /*      Start the new list with a blank card again              */

            newcard.Wincard(blank, 0, 0);
            cardlist.insertcard(newcard);
            aSwp = new SWP[iCardcount];
            phwndIndex = new HWND[iCardcount];

            /*      C versions of functions are:                            */
            /*      aSwp = malloc (iCardcount * sizeof(SWP));               */
            /*      phwndIndex = malloc (iCardcount * sizeof (HWND));       */

            WinSendMsg (hwnd, WM_CREATE, NULL, NULL);
            WinSendMsg (hwnd, WM_SIZE, NULL, NULL);

            return (MRESULT)0;

        case IDM_OPEN:

            /*                                                      */
            /*      Initialize file dialog box for Open File        */
            /*                                                      */

            fild = InitializeFiledlgFields();
            fild.x = 0;
            fild.y = 0;
            fild.fl = FDS_CENTER | FDS_OPEN_DIALOG;
            fild.pszTitle = pszFileopen;

            strcpy (fild.szFullFile, pszFullFile);

            hwndDlg = WinFileDlg (HWND_DESKTOP, hwnd, &fild);
            if (hwndDlg && (fild.lReturn == DID_OK))
            {
                for (id = 0; id < 4; id++) WinDestroyWindow(hwndButton[id]);
                for (id = 0; id < iCardcount; id++) WinDestroyWindow(phwndIndex[id]);
                WinDestroyWindow(hwndMLE);
                delete [] aSwp;
                delete [] phwndIndex;
                pszFilename = fild.szFullFile;
                fileopen (pszFilename);
                iCardcount = verify(pszFilename);

                /*      Explicit use of destructor to get rid of old list                       */

                cardlist.~Linkedlist();

                cardlist = readcardfile(iCardcount, pszFilename);
                aSwp = new SWP[iCardcount];
                phwndIndex = new HWND[iCardcount];
            }
            else return (MRESULT)0;

            WinFreeFileDlgList(fild.papszFQFilename);

            WinSendMsg (hwnd, WM_CREATE, NULL, NULL);
            WinSendMsg (hwnd, WM_SIZE, NULL, NULL);

            return (MRESULT)0;

        case IDM_SAVE:

            if (bChanged) editcard (hwndMLE);

            if (pszFilename == NULL) WinSendMsg (hwnd, IDM_SAVEAS, NULL, NULL);
            else
                writecardfile (cardlist, iCardcount, pszFilename);

            return (MRESULT)0;

        case IDM_SAVEAS:

            /*                                                      */
            /*      Initialize file dialog box for Save As          */
            /*                                                      */

            fild = InitializeFiledlgFields();
            fild.x = 0;
            fild.y = 0;
            fild.fl = FDS_CENTER | FDS_SAVEAS_DIALOG;
            fild.pszTitle = pszSaveas;

            strcpy (fild.szFullFile, pszFullFile);

            hwndDlg = WinFileDlg (HWND_DESKTOP, hwnd, &fild);
            if (hwndDlg && (fild.lReturn == DID_OK))
            {
                if (bChanged) editcard (hwndMLE);

                pszFilename = fild.szFullFile;
                if (! (pszFilename == NULL))
                    writecardfile (cardlist, iCardcount, pszFilename);

                /* Handle condition */

            }

            WinFreeFileDlgList(fild.papszFQFilename);

            return (MRESULT)0;

        case IDM_MERGE:

            /*                                                      */
            /*      Initialize file dialog box for merge            */
            /*                                                      */

            fild = InitializeFiledlgFields();
            fild.fl = FDS_OPEN_DIALOG;
            fild.pszTitle = pszMerge;
            fild.x = 2 * rcl.xRight;
            fild.y = rcl.yTop/2;

            strcpy (fild.szFullFile, pszFullFile);

            hwndDlg = WinFileDlg (HWND_DESKTOP, hwnd, &fild);
            if (hwndDlg && (fild.lReturn == DID_OK))
            {

                /*      Read in the info for the file to be merged                      */

                iTempcount = verify(fild.szFullFile);
                templist = readcardfile(iTempcount, fild.szFullFile);

                /*      Do the merge                                                    */

                for (i = 0; (i < iTempcount) ; i++) cardlist.insertcard(templist.showcard(i));

                /*      Do cleanup                                                      */

                templist.~Linkedlist();
                for (id = 0; id < 4; id++) WinDestroyWindow(hwndButton[id]);
                for (id = 0; id < iCardcount; id++) WinDestroyWindow(phwndIndex[id]);
                WinDestroyWindow(hwndMLE);
                delete [] aSwp;
                delete [] phwndIndex;

                /*      Create the windows for the merged file                          */

                iCardcount = iCardcount + iTempcount;
                aSwp = new SWP[iCardcount];
                phwndIndex = new HWND[iCardcount];

                WinSendMsg (hwnd, WM_CREATE, NULL, NULL);
                WinSendMsg (hwnd, WM_SIZE, NULL, NULL);
            }

            /*      Otherwise do nothing and return                                 */

            else return (MRESULT)0;

            WinFreeFileDlgList(fild.papszFQFilename);

            return (MRESULT)0;

        case IDM_ABOUT:

            WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc, NULLHANDLE, IDD_ABOUT, NULL);
            return 0;

            return (MRESULT)0;
        }
        break;

    }

    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

/*                                                              */
/*      Procedure to create the index card window               */
/*                                                              */

MRESULT EXPENTRY IndexWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HPS hps;
    static HWND hwndParent;
    RECTL rclIndex;
    POINTL ptl;
    static INT iThiscard;
    INT iIndex;
    CHAR szBuffer[40];

    iThiscard = WinQueryWindowUShort(hwnd, QWS_ID);
    hwndParent = WinQueryWindow (hwnd, QW_PARENT);

    switch (msg)
    {
    case WM_PAINT:

        /*                                                              */
        /*      Get the current size of the Client window               */
        /*                                                              */

        szBuffer[0] = '\0';

        hps=WinBeginPaint (hwnd, NULLHANDLE, NULL);

        GpiCreateLogFont (hps, NULL, LCID_FONT, &fat);
        GpiSetCharSet (hps, LCID_FONT);

        /*                                                              */
        /*      Draw a frame around the window                          */
        /*                                                              */

        WinQueryWindowRect (hwnd,&rclIndex);
        WinDrawBorder (hps, &rclIndex, 1, 1, CLR_NEUTRAL, CLR_BACKGROUND,
        DB_STANDARD | DB_INTERIOR) ;

        /*                                                              */
        /*      Put in a line to set the text on                        */
        /*                                                              */

        ptl.x = rclIndex.xLeft;
        ptl.y = rclIndex.yTop - (int (iCharheight*1.1));
        GpiMove (hps, &ptl);
        ptl.x = rclIndex.xRight;
        GpiLine (hps, &ptl);

        /*      Now place the text on the window                        */

        ptl.x = rclIndex.xLeft + 5;
        ptl.y = rclIndex.yTop - (int (iCharheight * .85));

        strcpy(szBuffer, (cardlist.showcard(iThiscard)).showindexinfo());
        iIndex = strlen(szBuffer);

        GpiCharStringAt (hps, &ptl, iIndex, (PUCHAR) szBuffer);

        /*      Reset to default font to avoid problems                 */

        GpiSetCharSet (hps, LCID_DEFAULT);
        GpiDeleteSetId (hps, LCID_FONT);

        WinEndPaint (hps);

        return (MRESULT)0;

        /*                                                              */
        /*      Process mouse click on the window                       */
        /*                                                              */

    case WM_BUTTON1DOWN:
    case WM_BUTTON1DBLCLK:

        iCardmove = iThiscard;
        WinSendMsg (hwndParent, WM_ROLLCARDS, NULL, NULL);

        return 0;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

/*      Begin dialogs section                                   */

MRESULT EXPENTRY SearchDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static CHAR szSearchfield[41];
    static MLE_SEARCHDATA mlesrchTemp;
    static PMLE_SEARCHDATA pmlesrchTemp;

    switch (msg)
    {
    case WM_INITDLG:

        pmlesrchTemp = PMLE_SEARCHDATA (PVOIDFROMMP (mp2));

        /*      Set the maximum number of characters to 40              */

        WinSendDlgItemMsg (hwnd, IDD_SEARCH, EM_SETTEXTLIMIT, MPFROM2SHORT(40,0), NULL);

        return 0;

    case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
        {
        case DID_OK:

            WinQueryDlgItemText (hwnd, IDD_SEARCH, sizeof(szSearchfield), (PSZ) szSearchfield);

            mlesrchTemp.cb = sizeof (mlesrchTemp);
            mlesrchTemp.pchFind = szSearchfield;
            mlesrchTemp.pchReplace = NULL;
            mlesrchTemp.cchFind = 0;
            mlesrchTemp.cchReplace = 0;
            mlesrchTemp.iptStart = -1;
            mlesrchTemp.iptStop = -1;

            *pmlesrchTemp = mlesrchTemp;

            WinDismissDlg (hwnd, TRUE);
            return 0;

        case DID_CANCEL:
            WinDismissDlg (hwnd, FALSE);
            return 0;
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY AddDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

    CHAR szNewCardIndex[41];
    INT numchars;
    Card newcard;
    HWND hwndParent;

    switch (msg)
    {
    case WM_INITDLG:

        /*      Set the maximum number of characters to 40              */

        WinSendDlgItemMsg (hwnd, IDD_ADD, EM_SETTEXTLIMIT, MPFROM2SHORT(40,0), NULL);
        return 0;

    case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
        {
        case DID_OK:

            szNewCardIndex[0] = '\0';

            WinQueryDlgItemText (hwnd, IDD_ADD, sizeof(szNewCardIndex), (PSZ)szNewCardIndex);

            numchars = 0;
            newcard.Wincard(szNewCardIndex, 0, 0);
            cardlist.insertcard(newcard);

            /*      If a card has been added, set the Changed flag          */

            bChanged = TRUE;

        case DID_CANCEL:

            iCardmove = 0;
            hwndParent = WinQueryWindow (hwnd, QW_PARENT);
            WinDismissDlg (hwnd, FALSE);
            return 0;
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY IndexDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    INT numcardchars;
    CHAR szBuffer[41];
    char* tempinfo;

    switch (msg)
    {
    case WM_INITDLG:

        /*      Set the maximum number of characters to 40              */

        WinSendDlgItemMsg (hwnd, IDD_INDEX, EM_SETTEXTLIMIT, MPFROM2SHORT(40,0), NULL);

        /*      Put the current card's text in the field                */

        strcpy(szBuffer, (cardlist.showcard(0)).showindexinfo());

/*      szBuffer is larger than text limit to account for null byte     */

        WinSetDlgItemText (hwnd, IDD_INDEX, (PUCHAR) szBuffer);

        return 0;

    case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
        {
        case DID_OK:

            WinQueryDlgItemText (hwnd, IDD_INDEX, sizeof(szBuffer), (PSZ)szBuffer);

            numcardchars = (INT) (cardlist.showcard(0)).shownumcardchars();

            tempinfo = new char [numcardchars];

            strcpy(tempinfo, (cardlist.showcard(0)).showcardinfo() );

            newcard.Wincard(szBuffer, numcardchars, tempinfo);

            delete [] tempinfo;

            /*      If this is not the first card                       */

            if (cardlist.delcard()) cardlist.insertcard(newcard);
            else
            {
                cardlist.insertcard(newcard);
                cardlist.movecard(-1);
                cardlist.delcard();
            }

        case DID_CANCEL:

            iCardmove = 0;
            WinDismissDlg (hwnd, FALSE);

            return 0;
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
        {
        case DID_OK:
        case DID_CANCEL:

            WinDismissDlg(hwnd, TRUE);
            return 0;
        }
        break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/*                                                              */
/*      Functions here                                          */
/*                                                              */

/*      Function to copy new MLE text onto a card       */

void editcard(HWND hwndMle)
{
    LONG lOffset;
    ULONG lCopy;
    INT i, numcardchars;

    for (i = 0; i < MLEBUFFER; i++)
        szMleBuf[i] = '\0';
    lOffset = 0;

    WinSendMsg (hwndMle, MLM_SETIMPORTEXPORT, MPFROMP (szMleBuf),
    MPFROMSHORT ((USHORT) sizeof (szMleBuf)));

    lCopy = (ULONG) WinSendMsg (hwndMle, MLM_QUERYFORMATTEXTLENGTH,
    MPFROMLONG (lOffset), MPFROMLONG (-1));

    /*      Cast to int -- required; add one space for the null character   */

    numcardchars = lCopy+1;

    WinSendMsg (hwndMle, MLM_EXPORT, MPFROMP (&lOffset), MPFROMP (&lCopy));

    newcard.Wincard((cardlist.showcard(0).showindexinfo()), (numcardchars), szMleBuf);
    if (cardlist.delcard()) cardlist.insertcard(newcard);
    else
    {
        cardlist.insertcard(newcard);
        cardlist.movecard(1);
        cardlist.delcard();
        iCardmove = 0;
    }
    bChanged = FALSE;
    return;
}

/*      Initialize file dialog                          */

FILEDLG InitializeFiledlgFields()
{
    FILEDLG fild;
    fild.cbSize = sizeof (FILEDLG);
    fild.ulUser = 0;
    fild.lReturn = 0;
    fild.lSRC = 0;
    fild.pszOKButton = 0;
    fild.pfnDlgProc = 0;
    fild.pszIType = 0;
    fild.papszITypeList = 0;
    fild.pszIDrive = 0;
    fild.papszIDriveList = 0;
    fild.hMod = 0;
    fild.szFullFile[CCHMAXPATH] = 0;
    fild.papszFQFilename = 0;
    fild.ulFQFCount = 0;
    fild.usDlgId = 0;
    fild.sEAType = 0;

    return fild;
}
