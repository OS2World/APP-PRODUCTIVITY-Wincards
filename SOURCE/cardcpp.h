#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#ifndef cardfile_h
#include "cardfile.h"
#endif

ifstream fin;
ofstream fout;
int numcheck(void);
int fileopen(char*);
int verify (void);
Linkedlist readcardfile(int);
int writecardfile (Linkedlist cardlist, int numcards, char* filename);

int fileopen(char* filename)
{
    char checkfile[3];

    /*      Open the file and perform a cursory check on the file type      */
    /* Get the first 3 characters of the file -- they should be MGC         */

    fin.open(filename, ios::in | ios::binary);
    if (!fin.good()) exit(1);
    fin.get(checkfile,4);
    fin.close();
    if (strcmp(checkfile, "MGC"))
        exit(2);
    return 0;
}

/*      Get number of cards                                             */

int verify(char* filename)
{
    int checksum;
    int numcards;
    int offset;

    fin.open(filename, ios::in | ios::binary);
    /*      Read in the number of cards                                     */
    fin.seekg (3, ios::beg);
    fin.read((int *)&numcards, sizeof numcards);

    /*      Read in number of cards in binary & convert to number of cards; */
    /*      Check once more to make sure file is valid                      */

    fin.seekg (11, ios::beg);
    fin.read ((int*) &offset, sizeof offset);
    fin.close();
    checksum = ((offset - 57)/52) + 1;
    if (checksum != numcards) exit (9999);

    return numcards;
}

/*      Read the cards into the linked list                             */

Linkedlist readcardfile(int numcards, char* filename)
{
    int cardnum = 0;
    static int newoffset = 0;
    int cards, namesize = 41;
    static short numchars;
    char index[namesize];
    char * varinfo;
    Linkedlist cardlist;
    Card anothercard;
    int offset = ((numcards-1)*52)+57;

    /*                                                                      */
    /*      Start reading in cards here                                     */
    /*                                                                      */
fin.open(filename, ios::in | ios::binary);

fin.seekg (16, ios::beg);

    for (int i=1; (i < (numcards+1)) ; i++)
    {
        fin.get(index, namesize);

        fin.seekg (7, ios::cur);
        cardnum++;

        /*      Get the number of characters in the data field;                 */
        /*      the last card has no data on stored info. -- it goes to EOF.    */

        if (i != numcards)
        {
            fin.read((int*)&cards, sizeof cards);
            newoffset = cards;
        }

        fin.seekg(offset+2, ios::beg);

        fin.read((short*)&numchars, sizeof numchars);
        varinfo = new char[numchars+1];
        fin.read(varinfo,numchars+1);

        /*      Go back and get more data                                       */

        fin.seekg((68+(52*(cardnum-1))), ios::beg);
        offset=newoffset;

        /*      Use the information to enter the card                           */
        /*      Add the extra char beyond numchars to make sure that the        */
        /*      end-of-line gets written into the array                         */

        anothercard.Wincard(index, numchars+1, varinfo);
        cardlist.insertcard(anothercard);

        /*      Delete needed because varinfo was created with new              */
        delete varinfo;
    }
    fin.close();

    fin.clear();

    return cardlist;
}

/*      Note -- Linkedlist is passed by value because 1) structure is   */
/*      small and 2) use of the movecard() function means the topcard   */
/*      pointer would have to be reset to the correct card after use    */
/*      of this function; the current setup is probably a little slower */
/*      but is easy to program                                          */

int writecardfile (Linkedlist cardlist, int numcards, char* filename)
{

    static int newoffset = 0;
    int namesize = 41;
    int id = 0;
    static short numchars;
    int offset = ((numcards - 1) * 52) + 57;

    while (! ((cardlist.showcard(-1) > cardlist.showcard(0)) || (id < numcards)))
    {
        cardlist.movecard(1);
        id++;
    }

    fout.open (filename, ios::out | ios::trunc | ios::binary);

    fout << "MGC";

    fout.write ((int*) &numcards, sizeof numcards);

    fout.seekp (11, ios::beg);

    fout.write ((int*) &offset, sizeof offset);
    fout.seekp (16, ios::beg);

    for (int i=0; (i < numcards); i++)
    {
        for (int j=0; j < namesize; j++)
            fout << cardlist.showcard(i).showindexinfo()[j];
    fout.seekp (6, ios::cur);

        numchars = cardlist.showcard(i).shownumcardchars() - 1;
        if (numchars == -1) numchars = 0;
        newoffset = offset + numchars + 4;
        if (i < (numcards - 1))
            fout.write ((int*) &newoffset, sizeof newoffset);

fout.seekp (offset + 2, ios::beg);

        fout.write ((short*) &numchars, sizeof numchars);

        fout << cardlist.showcard(i).showcardinfo();

        fout.seekp(68+(52*i), ios::beg);
        offset = newoffset;
    }

    fout.close();
    return 0;
}
