/* Listing 10-8. SpinningCursor.c */

#include "SpinningCursor.h" 
#include "Retrace.h"

/**************************************************************************

    Structure defining the acur resource. Note that this resource as defined
    on disk actually has the following structure:

        typedef struct {
            unsigned short numCursors;
            unsigned short index;
            struct {
                short ID;
                short unused;
            } cursors[1];
        } Acur, *Acurptr, **AcurHandle;

    However, we change this structure to the following in InitCursorCtl.

*******************************************************************************/

typedef struct {
    unsigned short numCursors;      /* Number of cursors */
    unsigned short index;           /* Next cursor to show */
    CursHandle cursors[1];          /* Actually unbounded array */
} Acur, *Acurptr, **AcurHandle;

/*******************************************************************************

    Record used for our VBL task. This record is the same as the normal VBL
    task record, except that we've added a field at the end. This extra field
    holds our application's AS value. Our VBL task can then retrieve this
    value when it wants to access our global variables.

*******************************************************************************/

typedef struct {
    VBLTask theTask;
    long A5;
} VBLTaskWithA5, *VBLTaskWithA5Ptr;

/******************************************************************************* 

    Constants and variables

*******************************************************************************/
AcurHandle pCursors;                /* Handle to the acur resource. */ 
VBLTaskWithA5Ptr pCursorTask;       /* Pointer to the VBL task record. */
short pDesiredCount;                /* Value to be placed in the vblCount field
                                       of our VBL task record. */
char CrsrBusy : 0x08CD;             /* Pointer to CrarBusy low-memory global.  */




/*******************************************************************************

    InitCursorCtl

    Call this routine early in your program to initialize some structures.
    After this routine reads in the acur resource, it looks at the data
    inside to find out what CURS resources to read. At this point, the
    acur resource looks like this:

        short number of cursors
        short index of next cursor to show
        short resource ID of first cursor

        short <unused>

        short resource ID of second cursor

        short <unused>
        ...
        short resource ID of last cursor
        short <unused>

    When InitCursorCtl reads in a CURS resource, it places the handle to the
    cursor into the space that used to hold the ID of the resource. This means
    that the preceding structure ends up looking like the following after all 
    cursors have been read in:

        short number of cursors
        short index of next cursor to show
        long handle to first cursor
        long handle to second cursor
        ...
        long handle to last cursor

    This function does not detach any of the resources from the resource
    fork. Although we change the acur resource, we never call
    ChangedResource on it, so the changes never get written out to disk.
    However, make sure that the acur resource is not marked as purgeable or
    our changes will get wiped out.

*******************************************************************************/
void InitCursorCtl(short resID)
{
    short cursorCount; CursHandle* workPtr;

    pCursors = (AcurHandle) GetResource('acur', resID);

    cursorCount = (**pCursors).numCursors;
    (**pCursors).numCursors *= 32;
    (**pCursors).index = 0;

    HLock((Handle) pCursors);
    workPtr = (**pCursors).cursors;


    while (cursorCount--) {
        *workPtr++ = (CursHandle) GetResource('CURS', *(short*) workPtr);
    }
    HUnlock((Handle) pCursors);
}

/******************************************************************************* 

    StartAsyncSpinning

    We call this routine to install a VBL task that calls SpinCursor at
    regular intervals, regardless of whatever else is going on. This way, we
    don't have to worry about calling SpinCursor by hand in the middle of 
    calculations or any other lengthy process.

    Before starting the VBL task, we lock down all the cursor data that
    we'll access in the task. Next, we fill out all of the fields of the
    VBL task record. Because we'll need to access some global variables in the
    VBL task procedure, we define an extended VBL task record with space for
    our AS register at the end. After all the fields are filled out, we simply
    call VInstall.

    Per Apple's Technical Note #180, our VBL routine will only be called when our 
    application is in the foreground. That's OK, since we don't want to spin
    the cursor while we're in the background. However, at some time in your
    life, you may want to install a VBL routine that executes when your
    application is in the background. The solution to this is to install the
    VBL task in the System heap. Apple's documentation is not too clear on
    whether this means the VBL task record or the VBL task procedure. To
    play it safe, you might want to install both in the System heap.

    To do this easily, define the following extended VBL task record:

        typedef struct {
            VBLTask theTask;
            long A5;
            short Jmp;
            void* Routine;
        } VBLTaskWithASSys, *VBLTaskWithASSysPtr;

    This is the same as the extended record we defined at the beginning of
    this file, with two additional fields. These two fields hold a stub VBL
    function that merely jumps to the real VBL function back in your
    application. Here is some code that demonstrates how to use this new
    record:

        pCursorTask (VBLTaskWithASSysPtr)
            NewPtrSys(sizeof(VBLTaskWithASSys));                // CHANGED



        pCursorTask->theTask.qType = vType;
        pCursorTask->theTask.vblAddr =(ProcPtr) &pCursorTask->Jmp;
        pCursorTask->theTask.vblCount = pDesiredCount;
        pCursorTask->theTask.vblPhase  = 0;
        pCursorTask->A5 = (long) CurrentA5;
        pCursorTask->Jmp = 0x4EF9;            // JMP #Routine   // CHANGED
        pCursorTask->Routine = MySpinner;                       // CHANGED

        if (HasFlushRoutines()) { 
            FlushDataCache();
            FlushInstructionCache();
        }

    Note that setting the last two fields to 68000 instructions constitutes
    self-modifying code. OK, so it's not really self-modifying, but it
    fulfills the necessary criteria of cutting instructions from whole cloth.
    For various reasons, this is bad, especially on any Macintosh with a 68040
    in it. Apple has some instruction and data cache flushing routines
    you must call to avoid the problems of self-modifying code. Make sure that
    you call them when they're available.

*******************************************************************************/
void StartAsyncSpinning(short period) 
{
    LockCursorData();

    pDesiredCount = period;

    pCursorTask = (VBLTaskWithA5Ptr) NewPtr(sizeof(VBLTaskWithA5));
    pCursorTask->theTask.qType = vType;
    pCursorTask->theTask.vblAddr =(ProcPtr) MySpinner;
    pCursorTask->theTask.vblCount = pDesiredCount;
    pCursorTask->theTask.vblPhase  = 0;
    pCursorTask->A5 = (long) CurrentA5;

    (void) VInstall((QElemPtr) pCursorTask);
}

/*******************************************************************************

    StopAsyncSpinning

    Remove the VBL task that spins the cursor and unlock all of the data it
    used.

*******************************************************************************/


void StopAsyncSpinning()
{
    (void) VRemove((QElemPtr) pCursorTask); 
    DisposePtr((Ptr) pCursorTask); 
    UnlockCursorData();
}

/******************************************************************************* 

    SpinCursor

    Unless you are using the asynchronous routines, you should call this
    function at regular intervals during a lengthy process. Our ·spin index·
    is incremented by the amount passed in. In keeping with MPW's 
    implementation, we only change the cursor every time the index passes
    through a multiple of 32.

*******************************************************************************/ void SpinCursor(short increment)
{
    short oldIndex; 
    short newIndex;

    oldIndex = (**pCursors).index / 32;

    (**pCursors).index += increment;
    (**pCursors).index %= (**pCursors).numCursors; // Take care of wraparound

    newIndex = (**pCursors).index / 32;

    if (newIndex != oldIndex) { 
        SetCursor(*(**pCursors).cursors[newIndex]);
    }
}

/*******************************************************************************

    LockCursorData

    Lock all of the handles used to define our spinning cursor set. These
    handles consist of our acur resource, which in turn is essentially an
    array of handles to our cursors. Each of these is moved high in memory and 
    locked down using HLockHi (this call is new to MPW 3.2 and THINK C 5.0,
    but it will work on any Macintosh).

*******************************************************************************/

void LockCursorData()
{
    short cursorCount; 
    CursHandle* workPtr;

    cursorCount = (**pCursors).numCursors / 32;

    HLockHi((Handle) pCursors); 
    workPtr = (**pCursors).cursors; 
    while (cursorCount--) {
        HLockHi((Handle) *workPtr++);
    }
}

/*******************************************************************************

    UnlockCursorData

    Unlock the data locked by LockCursorData. We call this after turning off
    our asynchronous cursor spinning routines so that we don't keep the data
    locked in memory.

*******************************************************************************/
void UnlockCursorData()
{
    short cursorCount; 
    CursHandle* workPtr;

    cursorCount = (**pCursors).numCursors / 32;

    workPtr =(**pCursors).cursors; 
    while (cursorCount--) {
        HUnlock((Handle) *workPtr++);    }
    HUnlock((Handle) pCursors);
}

/*******************************************************************************

    MySpinner

    VBL task procedure. This function is called at regular intervals to spin
    the cursor. It first checks the low-memory global CrsrBusy to make sure
    that the cursor is available to be changed. If so, we point AS at our  
    application's globals, call SpinCursor to change to the next cursor, 
    reprime our VBL task so it gets called again, and restore AS to its
    original value.

    Note that the way we look at the low-memory global CrsrBusy is very THINK
    C-ish. THINK provides access to low-memory globals using a nonstandard
    syntax. At the start of this file, we declared CrsrBusy to be:

        char CrsrBusy : Ox08CD;

    We could then use it as shown in MySpinner. Under MPW C, we would have to
    do the following:

        #define CrsrBusy Ox08CD
        if ( *(char *) CrsrBusy == 0) { •.. };

*******************************************************************************/
void MySpinner ()
{
    long oldA5; 
    VBLTaskWithA5Ptr myTaskPtr;

    asm {
        move.l A0, myTaskPtr
    }

    if (CrsrBusy == 0) {
        oldA5 = SetA5(myTaskPtr->A5);
        SpinCursor (32);
        myTaskPtr->theTask.vblCount = pDesiredCount;
        (void) SetA5(oldA5);
    }
}
