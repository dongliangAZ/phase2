
#define DEBUG2 1

typedef struct mailSlot *slotPtr;
typedef struct mailSlot mailSlot;//office hours
typedef struct mailbox   mailbox;
typedef struct mboxProc *mboxProcPtr;

struct mailbox {
    int             ID;
    int             numSlots;
    int             numSlotsUsed;
    int             slotSize;
    slotPtr         head;
    //slotPtr         end;
    int             status;

};

struct mailSlot {
    int         ID;
    int         status;
    slotPtr     next;
    char        message[MAX_MESSAGE];
    int         current_size;
};

struct psrBits {
    unsigned int curMode:1;
    unsigned int curIntEnable:1;
    unsigned int prevMode:1;
    unsigned int prevIntEnable:1;
    unsigned int unused:28;
};

union psrValues {
    struct psrBits bits;
    unsigned int integerPart;
};
