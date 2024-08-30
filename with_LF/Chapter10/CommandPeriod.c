/* Listing 10-2. Command Period.c */

#include "CommandPeriod.h" 
#include "StandardStuff.h"
#include "Script.h" 
#include "Processes.h" 
#include "GestaltEqu.h"

/*******************************************************************************

    TestCommandPeriod

    Simple test routine for our Command-period checker. It creates a dialog
    and displays its contents with DrawDialog (as opposed to calling
    ModalDialog to handle the update event). We then go into a tight loop that
    exits only when there is a Command-period in the event queue.

*******************************************************************************/
void TestCommandPeriod()
{
    DialogPtr     dlg;
    dlg = GetNewDialog(256, nil, (WindowPtr) -1);
    DrawDialog(dlg);
    while (!AbortInQueue()) ;
    DisposeDialog(dlg);
}

/*******************************************************************************

    Variables for the Command-period checker.

*******************************************************************************/

#define kModifiersMask 0xFF00 & -cmdKey    /* We need all modifiers
                                              except the command key
                                              for KeyTrans. */
#define kOtherCharCodeMask  0x00FF0000    /* Get the key out of the
                                              ASCIIl byte. */
static const char kPeriod = '.';


/*******************************************************************************

 Call an A/UX service routine.

*******************************************************************************/

pascal long AUXDispatch(short select, void* arg)
= 0xABF9;

/******************************************************************************* 

    AbortInQueue

    Looks for a Command-period in the event queue. If we are running under the
    Mac OS, we find the head of the queue and walk the elements, looking for a
    keyDown event. This won't work under A/UX, which doesn't maintain a normal
    Mac OS event queue. When running under A/UX, we make a special call,
    asking A/UX to look through its private structures for a specific event
    for us.

    Note that this routine returns FALSE if we are running in the background.
    This is because we don't want to stop whatever we're doing if the user
    presses Command-period for the foreground application.

*******************************************************************************/ 
Boolean AbortInQueue()
{
    const short AUX_FIND_EVENT = 8;

    EvQElPtr queueEntryPtr;
    Boolean result;
    struct {
        EventRecord mask;
        EventRecord value;
    } eventFilter;
 
    result = FALSE;
    if (IsFrontProcess()) {
        if (IsRunningUnderAUX()) {
            // To get A/UX to look for a certain event, we fill out a 
            // specification and a mask. This is in the form of two
            // EventRecords. The first one contains the values we want 
            // to look for. The second specifies the fields we want to 
            // search.

            eventFilter.value.what = keyDown;    // Find a keyDown event
            eventFilter.value.message =kPeriod;  // key ='.'
            eventFilter.value.modifiers =cmdKey; // with command key
            eventFilter.mask.what = everyEvent; 
            eventFilter.mask.message = charCodeMask;
            eventFilter.mask.modifiers = cmdKey; 
            eventFilter.mask.when = 0;
            SetPt(&eventFilter.mask.where, 0, 0);

            // We return that there is a Command-period on the queue 
            // if AUXDispatch returns a positive value AND
            // it returns a non-nullEvent in eventFilter.

            result = (AUXDispatch(AUX_FIND_EVENT, &eventFilter) > 0) &&
                  (eventFilter.value.what != nullEvent);
        } else {

            // Running under Mac OS, so walk the queue. The head is in 
            // the low-memory location returned by GetEvQHdr, and
            // follows the normal OS queue conventions. For each entry 
            // in the queue, call "CmdPeriod" to test it.

            result = FALSE;
            queueEntryPtr = (EvQElPtr) GetEvQHdr()->qHead;
            while (!result && (queueEntryPtr != nil)) {
                result = CmdPeriod((EventPtr) &queueEntryPtr->evtQWhat);
                if (!result) {
                    queueEntryPtr = (EvQElPtr) queueEntryPtr->qLink; 
                }
            }  /*Scanning queue */ 
        } /* Running under Mac OS */
    } /* If front process */ 
    return result;
}

/******************************************************************************* 

    CmdPeriod

    Radical cool way to see if the event record represents a Command-period 
    Keypress. Normally, you might wonder, What's the problem? All you have to 
    do is check the modifiers field to see if the Command key is down, and 
    check the message field to see what key was pressed. Well, the problem is 
    that under some systems, holding down the Command key negates any effect 
    the Shift key has. This means that on systems where the period is a 
    shifted character, when you hold down the Command key, you won't be able 
    to press period.

    The way to fix this is to rerun the sequence of events involved in mapping 
    a key code into an ASCII value, except that this time we don't factor the 
    Command key into the equation.

    The event record has everything we need. It has the modifier keys that 
    were pressed at the time of the event, and it has the key code. What we do 
    is take the modifiers, clear the bit that says the Command key was
    pressed, and pass the modified modifiers and the key code to KeyTrans. 
    After that, we'll be able to examine the resulting ASCII value on its own
    merits.

    From Harvey's Technical Note #263: International Canceling

*******************************************************************************/
Boolean CmdPeriod( EventPtr theEvent )
{
    Boolean result;
    short keyCode;
    long virtualKey, keyInfo, lowChar, highChar, state, keyCID;
    Handle hKCHR;

    result = false;

    if((theEvent->what == keyDown) || (theEvent->what == autoKey)) {

        // See if the Command key is down. If it is, find out the ASCII 
        // equivalent for the accompanying key.

        if((theEvent->modifiers & cmdKey) != 0 ) {
            virtualKey = (theEvent->message & keyCodeMask) >> 8; 

            // Mask out the Command key and merge in the virtual Key
            keyCode = (theEvent->modifiers & kModifiersMask) | virtualKey;
            state = 0;

            keyCID = GetScript(GetEnvirons(smKeyScript), smScriptKeys);
            hKCHR = GetResource('KCHR', keyCID);

            if (hKCHR != nil) {
                keyInfo = KeyTrans(*hKCHR, keyCode, &state);
                ReleaseResource( hKCHR );
            } else {
                keyInfo =theEvent->message;
            }

            lowChar = keyInfo & charCodeMask;
            highChar = (keyInfo & kOtherCharCodeMask) >> 16;
            if((lowChar == kPeriod) || (highChar == kPeriod ))
                result = true;
        } // end the Command key is down
    } // end key down event 
    return result;
}

/*******************************************************************************

    IsFrontProcess

    A System 6.0 and 7.0 groovy routine that checks to see if we are the
    foreground (foregone?) process. Under System 6.0, we rely on keeping track
    of Suspend and Resume events in our main event loop. Under 7.0, we can
    just ask the Process Manager.

    Note that gInBackground is maintained externally, usually by your main
    event loop.

*******************************************************************************/ 
Boolean IsFrontProcess()
{
    Boolean result;
    Boolean hasProcessMgr;
    OSErr  err;
    ProcessSerialNumber aPSN;
    ProcessSerialNumber currentPSN = {0, kCurrentProcess};

    if (HasGestaltAttr(gestaltOSAttr, gestaltLaunchControl)) {
        err = GetFrontProcess(&aPSN);
        err = SameProcess(&aPSN, &currentPSN, &result);
        return result;
    } else {
        return !gInBackground;
    }
}

/******************************************************************************.

    IsRunningUnderAUX

    See if we are running under A/ux. We make this check in two ways. First,
    we call Gestalt to see what it says. If it knows about A/UX, we see if the
    version that was returned is non-zero (indicating that A/UX is running) or
    not. If Gestalt doesn't recognize the A/UX selector, then we check by
    hand by looking into low memory. Bit 9 of HWCfgFlags is set if A/UX is
    running.

*******************************************************************************/
Boolean IsRunningUnderAUX() 
{
    OSErr err;
    long response;
    #define HWCfgFlags 0x0B22

    err =Gestalt(gestaltAUXVersion, &response);
    if (err == noErr) {
        return response != 0; 
    } else {

        // return TRUE if bit 9 of HWCfgFlags is set. 
        return ((*(short *) HWCfgFlags) & (1 << 9)) != 0;
    }
}

/*******************************************************************************

    HasGestaltAttr

    Handy utility to see if the specified bit in the result of the specified
    Gestalt selector is set or not.

*******************************************************************************/
Boolean HasGestaltAttr(OSType itsAttr, short itsBit)
{
    long response;
    return (Gestalt(itsAttr, &response) == noErr) && (((response >> itsBit)&1)!=0);
}