#include <bool.h>
#ifndef wincard_h
#define wincard_h

class Wincard
{
    enum { namesize = 41 };

private:

    /*      The name on the card is stored in an array of constant size     */
    /*      The info on the card is stored in an array of variable size     */
    /* The number of characters in the card's data field is an int          */

    char index[namesize];
    char * cardinfo;
    int numcardchars;

public:

    Wincard();
    ~Wincard();
    Wincard(char index[], int numcardchars, char* cardinfo);
    char * showcardinfo() const;
    char * showindexinfo();
    int shownumcardchars() const;

    /*                                                                      */
    /*      Assignment operator is necessary due to use of pointers         */
    /*                                                                      */

    Wincard & operator = (const Wincard & Fromcard);

    /*      Copy constructor also required                                  */

    Wincard(const Wincard & Fromcard);

    /*      Comparison operators                                            */

    bool operator < (const Wincard & Thiscard) const;
    bool operator > (const Wincard & Thiscard) const;
    bool operator < (const char* Indexstring) const;
    bool operator > (const char* Indexstring) const;
    bool operator == (const Wincard & Thiscard) const;
    bool operator == (const char* Indexstring) const;

    /*      Use friend function to reverse operands                         */

    friend int operator < (const char* Indexstring, const Wincard & Thiscard);
    friend int operator > (const char* Indexstring, const Wincard & Thiscard);
    friend bool operator == (const char* Indexstring, const Wincard & Thiscard);

};

typedef Wincard Card;

class Linkedlist
{

    struct Node
    {
        Card onecard;
        struct Node * next;
        struct Node * previous;
    };

private:
    Node * firstcard;
    Node * lastcard;

    /*      Use the topcard pointer to indicate which card is on top of the stack   */

    Node * topcard;

public:
    Linkedlist();
    ~Linkedlist();

    /*                                                                      */
    /*      Copy constructor -- required                                    */
    /*                                                                      */

    Linkedlist (const Linkedlist & Fromlist);

    /*                                                                      */
    /*      Assignment operator -- required                                 */
    /*                                                                      */

    Linkedlist & operator = (const Linkedlist & Fromlist);

    /*      Card handling functions                                         */

    int cardcount() const;
    bool getcards (const Card &onecard);
    bool insertcard (const Card &newcard);
    bool delcard ();
    Card showcard (int cardnumber);
    void movecard (int cardnumber);

};

#endif
