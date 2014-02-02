#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>
#include "wincard.h"

Linkedlist::Linkedlist()
{
    firstcard=lastcard=topcard=NULL;
}

Linkedlist::~Linkedlist()               /* destructor for the linked list */
{
    Node * temp;
    while (firstcard != NULL)       /* while there are cards left */
    {
        temp = firstcard;       /* get the first card */
        if (temp != lastcard) firstcard = firstcard->next;
        /* if the first card isn't the last card */
        else
        {
            firstcard = NULL;     /* if there's only one card left */
            lastcard = NULL;
            topcard = NULL;
        }
        temp->onecard.~Wincard();
        delete temp;            /* delete the card */
    };
}

/*      Copy constructor                                                */

Linkedlist::Linkedlist(const Linkedlist & Fromlist)
{
    Node * temp;
    temp = Fromlist.firstcard;

    /*      Set up the first card                                           */

    Node * nextcard = new Node;
    nextcard->onecard = temp->onecard;
    firstcard = nextcard;
    lastcard = nextcard;
    nextcard->previous = nextcard;
    nextcard->next = nextcard;
    temp = temp->next;

    /*      Then add the rest                                               */

    while (temp != Fromlist.firstcard)
    {
        Node * nextcard = new Node;
        nextcard->onecard = temp->onecard;
        nextcard->next = firstcard;
        lastcard-> next = nextcard;
        nextcard-> previous = lastcard;
        lastcard = nextcard;
        firstcard->previous = lastcard;
        temp = temp->next;
    }
    topcard = firstcard;
}

/*      Assignment operator                                     */

Linkedlist & Linkedlist::operator=(const Linkedlist & Fromlist)
{
    if (this == &Fromlist) return *this;

    /*      First delete the old list if it exists                      */

    Node * temp;
    while (firstcard != NULL)
    {
        temp = firstcard;
        if (firstcard->next != firstcard) firstcard = firstcard->next;
        else firstcard = lastcard = topcard = NULL;
        delete temp;
    };

    /*      Then set the temporary pointer to the first card of the list    */
    /*      and set up the first card                                       */

    temp = Fromlist.firstcard;
    Node * nextcard = new Node;
    nextcard->onecard = temp->onecard;
    firstcard = nextcard;
    lastcard = nextcard;
    nextcard->previous = nextcard;
    nextcard->next = nextcard;
    temp = temp->next;

    /*      While the next card in the copied-from list is not the same     */
    /*      as the first card in the copied-from list, add more cards       */

    while (temp != Fromlist.firstcard)
    {
        Node * nextcard = new Node;
        nextcard->onecard = temp->onecard;
        nextcard->next = firstcard;
        lastcard-> next = nextcard;
        nextcard-> previous = lastcard;
        lastcard = nextcard;
        firstcard->previous = lastcard;
        temp = temp->next;
    }
    topcard = firstcard;
    return *this;
}

bool Linkedlist::insertcard(const Card & newcard)
{

    Node * nextcard = new Node;
    nextcard->onecard=newcard;

    if (firstcard == NULL)
    {
        firstcard = nextcard;
        topcard = nextcard;
        lastcard = nextcard;
        nextcard->previous = nextcard;
        nextcard->next = nextcard;
    }
    else
    {
        /*      If the card is in the right place already, drop through and insert it   */
        /*      If not, start at the first card and find where it goes                  */
        /*      Yes, all these parentheses are necessary!!                              */
        if (! (((newcard < (topcard->onecard)) || (newcard == (topcard->onecard))) &&
        ((newcard > (topcard->previous)->onecard) || (newcard == (topcard->previous)->onecard))))
        {
            topcard=firstcard;
            while ((newcard > (topcard->onecard))  && (newcard < lastcard->onecard))
                topcard = topcard->next;
        }
        nextcard->previous = topcard->previous;
        nextcard->next = topcard;
        (topcard->previous)->next = nextcard;
        topcard->previous = nextcard;
        topcard = nextcard;

        /*      Card could be at either the front or back of the list -- check and      */
        /*      update firstcard/lastcard pointers if true                              */

        if ((topcard->next) == firstcard || firstcard == lastcard)
        {
            if (topcard->onecard > (topcard->previous)->onecard ||
            topcard->onecard == (topcard->previous)->onecard)
                lastcard = topcard;
            else firstcard = topcard;
        }
    }
    return true;
}

bool Linkedlist::delcard()
{
    Node * temp;

    /*      If this is the only card, take special measures         */
    if ((lastcard == firstcard) && (topcard == firstcard)) return false;
    if (topcard == firstcard) firstcard = topcard->next;
    if (topcard == lastcard) lastcard = lastcard->previous;
    (topcard->previous)->next = topcard->next;
    (topcard->next)->previous = topcard->previous;
    temp = topcard;
    topcard = topcard->next;
    delete temp;
    return true;
}

Card Linkedlist::showcard(int cardnumber)
{
    int pickcard = 0;
    Node * temp = topcard;    /* set temp pointer to top card */
    while (pickcard != cardnumber)
    {
        if (cardnumber >0)
        {
            pickcard++;
            temp=temp->next;
        }
        else
        {
            pickcard--;
            temp=temp->previous;
        }
    };
    return temp->onecard;
}

void Linkedlist::movecard(int cardnumber)
{
    int cardmove = 0;
    if (cardnumber > 0)
    {
        while (cardmove < cardnumber)
        {
            cardmove++;
            topcard = topcard->next;
        }
    }
    else
    {
        while (cardmove > cardnumber)
        {
            cardmove--;
            topcard = topcard->previous;
        }
    }
    return;
}

Wincard::Wincard()
{
    numcardchars=0;
    cardinfo=NULL;
}

Wincard::Wincard(char tempindex[namesize], int numchars, char * tempinfo)
{

    for (int i=0; i<namesize; i++) index[i] = '\0';
    strcpy(index, tempindex);

    numcardchars=numchars;
    if (numchars < 1)
    {
    numcardchars=1;
    cardinfo=new char[numcardchars];
    cardinfo[0]='\0';
    }
    else
    {
        cardinfo=new char[numcardchars];
        for (int i=0; i<numcardchars; i++) cardinfo[i]=tempinfo[i];
    }
    return;
}

/*      Destructor uses delete because card creation uses new           */

Wincard::~Wincard()
{
    delete [] cardinfo;
}

Wincard & Wincard::operator=(const Wincard & Fromcard)
{
    if (this == &Fromcard) return *this;
    delete [] cardinfo;
    numcardchars = Fromcard.numcardchars;
    cardinfo = new char[numcardchars];
    for (int i=0; i<numcardchars; i++) cardinfo[i]=Fromcard.cardinfo[i];
    for (int i=0; i<namesize; i++) index[i]=Fromcard.index[i];
    return *this;
}

Wincard::Wincard(const Wincard & Fromcard)
{
    numcardchars = Fromcard.numcardchars;
    cardinfo = new char[numcardchars];
    for (int i=0; i<numcardchars; i++) cardinfo[i]=Fromcard.cardinfo[i];
    for (int i=0; i<namesize; i++) index[i]=Fromcard.index[i];
}

/*      Comparison operators                                    */
/*      Note:  Comparison testing is case sensitive ONLY for    */
/*      situations in which strings are otherwise identical!    */

bool Wincard::operator < (const Wincard & Thiscard) const
{
    char firstcard[namesize], secondcard[namesize];

    strcpy(firstcard, index);
    strcpy(secondcard, Thiscard.index);
    for (int i=0; i<namesize; i++)
    {
        if (islower(firstcard[i])) firstcard[i] = toupper(firstcard[i]);
        if (islower(secondcard[i])) secondcard[i] = toupper(secondcard[i]);
    }

    if (strcmp(secondcard, firstcard) > 0)
        return true;
    else if (strcmp(secondcard, firstcard) == 0)
        if (strcmp(Thiscard.index, index) > 0) return true;
    return false;
}

bool Wincard::operator > (const Wincard & Thiscard) const
{
    char firstcard[namesize], secondcard[namesize];

    strcpy(firstcard, index);
    strcpy(secondcard, Thiscard.index);
    for (int i=0; i<namesize; i++)
    {
        if (islower(firstcard[i])) firstcard[i] = toupper(firstcard[i]);
        if (islower(secondcard[i])) secondcard[i] = toupper(secondcard[i]);
    }

    if (strcmp(firstcard, secondcard) > 0) return true;
    else if (strcmp(firstcard, secondcard) == 0)
        if (strcmp(index, Thiscard.index) > 0) return true;
    return false;
}

bool Wincard::operator == (const Wincard & Thiscard) const
{
    if (strcmp(index, Thiscard.index) == 0) return true;
    else return false;
}

bool Wincard::operator < (const char* Indexstring) const
{
    char firstcard[namesize], secondcard[namesize];

    strcpy(firstcard, index);
    strcpy(secondcard, Indexstring);
    for (int i=0; i<namesize; i++)
    {
        if (islower(firstcard[i])) firstcard[i] = toupper(firstcard[i]);
        if (islower(secondcard[i])) secondcard[i] = toupper(secondcard[i]);
    }

    if (strcmp(secondcard, firstcard) > 0) return true;
    else if (strcmp(secondcard, firstcard) == 0)
        if (strcmp(Indexstring, index) > 0) return true;
    return false;
}

bool Wincard::operator > (const char* Indexstring) const
{
    char firstcard[namesize], secondcard[namesize];

    strcpy(firstcard, index);
    strcpy(secondcard, Indexstring);
    for (int i=0; i<namesize; i++)
    {
        if (islower(firstcard[i])) firstcard[i] = toupper(firstcard[i]);
        if (islower(secondcard[i])) secondcard[i] = toupper(secondcard[i]);
    }
    if (strcmp(firstcard, secondcard) > 0) return true;
    else if (strcmp(firstcard, secondcard) == 0)
        if (strcmp(index, Indexstring) > 0) return true;
    return false;
}

bool Wincard::operator == (const char* Indexstring) const
{
    char firstcard[2], secondcard[2];

    /*      If the comparison string is only one character -- compare with  */
    /*      first character of index only                                   */

    if (strlen(Indexstring) == 1)
    {
        strcpy(secondcard, Indexstring);
        firstcard[0] = toupper(index[0]);
        secondcard[0] = toupper(Indexstring[0]);
        firstcard[1] = '\0';
        secondcard[1] = '\0';
        if (strcmp(firstcard, secondcard) == 0) return true;
        else return false;
    }

    if (strcmp(index, Indexstring) == 0) return true;
    else return false;
}

/*      Friend functions here for reversing operand order       */

bool operator > (char* Indexstring, const Wincard & Thiscard)
{
    return (Thiscard < Indexstring);
}

bool operator < (char* Indexstring, const Wincard & Thiscard)
{
    return (Thiscard > Indexstring);
}

bool operator == (const char* Indexstring, const Wincard & Thiscard)
{
    return (Thiscard == Indexstring);
}
/*      End comparison operators                                */

char * Wincard::showcardinfo() const
{
    return cardinfo;
}

char * Wincard::showindexinfo()
{
    return index;
}

int Wincard::shownumcardchars() const
{
    return numcardchars;
}
