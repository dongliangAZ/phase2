/* ------------------------------------------------------------------------
   phase2.c

   University of Arizona
   Computer Science 452

   ------------------------------------------------------------------------ */

#include <usloss.h>
#include <usyscall.h>
#include <phase1.h>
#include <phase2.h>
#include <stdio.h>
#include <stdlib.h>

#include <message.h>

/* ------------------------- Prototypes ----------------------------------- */
int start1 (char *);

static void clockHandler(int dev, void *args);
static void diskHandler(int dev, void *args);
static void terminalHandler(int dev, void *args);
static void syscallHandler(int dec, void *args);

/* -------------------------- Globals ------------------------------------- */

int debugflag2 = 0;

// the mail boxes
mailbox MailBoxTable[MAXMBOX];
mailSlot MailSlots[MAXSLOTS];

// also need array of mail slots, array of function ptrs to system call
// handlers, ...





/* --------------------------Our-Functions ----------------------------------- */


static void clockHandler(int dev, void *args) {

}

static void diskHandler(int dev, void *args) {

}

static void terminalHandler(int dev, void *args) {

}

static void syscallHandler(int dev, void *args) {

}

/* ------------------------------------------------------------------------
 Name - currentMode
 Purpose - returns which mode you are in.
 Parameters -
 Returns - 1 is kernal, 0 is user, and -1 is error
 Side Effects -
 ----------------------------------------------------------------------- */

int currentMode() {
	union psrValues psr;
	psr.integerPart = USLOSS_PsrGet();
	return psr.bits.curMode;
}

/* ------------------------------------------------------------------------
 Name - DisableInterrupts
 Purpose - To disable interrups
 Parameters -none
 Returns -  void
 Side Effects - No side effect.
 ----------------------------------------------------------------------- */
void DisableInterrupts() {
	if ((USLOSS_PSR_CURRENT_MODE & USLOSS_PsrGet()) == 0) {
		USLOSS_Console("Error:Not in the kernel mode.");
		USLOSS_Halt(1);
	}      //if it is not in the kernel mode
	else {
		 int disable = USLOSS_PsrSet( USLOSS_PsrGet() & ~USLOSS_PSR_CURRENT_INT );
	}      //in the kernel mode
}

/* ------------------------------------------------------------------------
 Name - EnableInterrupts
 Purpose - To enable interrupts
 Parameters -none
 Returns -  void
 Side Effects - No side effect.
 ----------------------------------------------------------------------- */
void EnableInterrupts() {
	if ((USLOSS_PSR_CURRENT_MODE & USLOSS_PsrGet()) == 0) {
		USLOSS_Console("Error:Not in the kernel mode.");
		USLOSS_Halt(1);
	}      //if it is not in the kernel mode
	else {
		int enable = USLOSS_PsrSet( USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
	}      //in the kernel mode
}

/* --------------------------Our-Functions-End----------------------------------- */

/* -------------------------- Functions ----------------------------------- */

/* ------------------------------------------------------------------------
   Name - start1
   Purpose - Initializes mailboxes and interrupt vector.
             Start the phase2 test process.
   Parameters - one, default arg passed by fork1, not used here.
   Returns - one to indicate normal quit.
   Side Effects - lots since it initializes the phase2 data structures.
   ----------------------------------------------------------------------- */
int start1(char *arg)
{
    int kidPid;
    int status;

    if (DEBUG2 && debugflag2)
        USLOSS_Console("start1(): at beginning\n");

    DisableInterrupts();
    // Initialize the mail box table, slots, & other data structures.

    int i;
    for (i = 0; i < MAXMBOX; i++){
        MailBoxTable[i].mboxID = -1; //20 unused
        MailBoxTable[i].numSlots = -1;
        MailBoxTable[i].numSlotsUsed = -1;
        MailBoxTable[i].slotSize = -1;
        MailBoxTable[i].head = NULL;
        MailBoxTable[i].end = NULL;
        MailBoxTable[i].status = 20;
    }/*MailBoxTable*/
    for (i = 0; i < MAXSLOTS; i++) {
        MailSlots[i].mboxID = -1;
        MailSlots[i].status = -1;
        MailSlots[i].next = NULL;
        MailSlots[i].message[0] = '\0';
        MailSlots[i].current_size = -1;
    }/*Slots*/


    // Initialize USLOSS_IntVec and system call handlers,
    USLOSS_IntVec[USLOSS_CLOCK_INT] = clockHandler;//There should be a new clockHandler
    USLOSS_IntVec[USLOSS_DISK_INT] = diskHandler;
    USLOSS_IntVec[USLOSS_TERM_INT] = terminalHandler;
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = syscallHandler;
    // allocate mailboxes for interrupt handlers.  Etc...
    

    /*After initializes then enable interrupsts.*/
    EnableInterrupts();
    // Create a process for start2, then block on a join until start2 quits
    if (DEBUG2 && debugflag2)
        USLOSS_Console("start1(): fork'ing start2 process\n");
    kidPid = fork1("start2", start2, NULL, 4 * USLOSS_MIN_STACK, 1);
    if ( join(&status) != kidPid ) {
        USLOSS_Console("start2(): join returned something other than ");
        USLOSS_Console("start2's pid\n");
    }

    return 0;
} /* start1 */


/* ------------------------------------------------------------------------
   Name - MboxCreate
   Purpose - gets a free mailbox from the table of mailboxes and initializes it
   Parameters - maximum number of slots in the mailbox and the max size of a msg
                sent to the mailbox.
   Returns - -1 to indicate that no mailbox was created, or a value >= 0 as the
             mailbox id.
   Side Effects - initializes one element of the mail box array.
   ----------------------------------------------------------------------- */
int MboxCreate(int slots, int slot_size)
{
    if ((USLOSS_PSR_CURRENT_MODE & USLOSS_PsrGet()) == 0) {
		USLOSS_Console("Error:Not in the kernel mode.");
		USLOSS_Halt(1);
	}      //if it is not in the kernel mode
	EnableInterrupts();
    if(slot_size<=0 || slots > MAXSLOTS || slots<=0){//office hours
        return -1;
    }
    int mailbox_id;
    for(mailbox_id = 7; mailbox_id < MAXMBOX; mailbox_id++){
        if(MailBoxTable[mailbox_id].status == 20){
           MailBoxTable[mailbox_id].status = 21;
           DisableInterrupts();
           return mailbox_id;
        }
       
    }
    return -1;
} /* MboxCreate */


/* ------------------------------------------------------------------------
   Name - MboxSend
   Purpose - Put a message into a slot for the indicated mailbox.
             Block the sending process if no slot available.
   Parameters - mailbox id, pointer to data of msg, # of bytes in msg.
   Returns - zero if successful, -1 if invalid args.
   Side Effects - none.
   ----------------------------------------------------------------------- */
int MboxSend(int mbox_id, void *msg_ptr, int msg_size)
{
    return 0;
} /* MboxSend */


/* ------------------------------------------------------------------------
   Name - MboxReceive
   Purpose - Get a msg from a slot of the indicated mailbox.
             Block the receiving process if no msg available.
   Parameters - mailbox id, pointer to put data of msg, max # of bytes that
                can be received.
   Returns - actual size of msg if successful, -1 if invalid args.
   Side Effects - none.
   ----------------------------------------------------------------------- */
int MboxReceive(int mbox_id, void *msg_ptr, int msg_size)
{
    return 0;
} /* MboxReceive */


/* ------------------------------------------------------------------------
   Name - check_io
   Purpose - I don't know
   Parameters - none that I know of
   Returns - duck if I know 
   Side Effects - none.
   ----------------------------------------------------------------------- */
void check_io()
{
    
}
