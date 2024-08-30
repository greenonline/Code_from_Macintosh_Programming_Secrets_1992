/* Listing 10-4. EnterPassword.c */

#include "EnterPassword.h"

/*******************************************************************************

    Constants for our dialog. kPasswordDialog is the ID for our DLOG resource. 
    The next four values represent actual items in the dialog. The final
    value, kGoAway, sort of takes the place of the go away button. There is no 
    OK button in the dialog; the user gets out of it by pressing Return or
    Enter in the edit text field. When this happens, we return the "fake" ID 
    of kGoAway.

*******************************************************************************/

const short  kPasswordDialog = 257;
const short  kEnterPasswordText = 1;
const short  kSecretWordText = 2;
const short  kEditText = 3;
const short kRevealedText = 4;
const short kGoAway = 18;

/*******************************************************************************

    EnterPassword

    Simple tester for our password routines. This function brings up a dialog 
    and runs it until the user presses Return or Enter. The dialog is disposed 
    and we return the caller. All of the password processing is done in the 
    ModalDialog filter procedure PasswordProc.

    If you want to get the text of the password, you can find it in the
    TEHandle in pMirrorEditText.

*******************************************************************************/
void EnterPassword()
{
     DialogPtr     dlg;
     short         itemHit;

    dlg = GetNewDialog(kPasswordDialog, nil, (WindowPtr) -1);

    do {
        ModalDialog(PasswordProc, &itemHit);
    } while (itemHit != kGoAway);

    DisposeDialog(dlg);
}

/*******************************************************************************

    Some constants and variables for our ModalDialog filter.

*******************************************************************************/
static const char kEnter  = (char) 0x03;
static const char kReturn = (char) 0x0D;
static const char kSpace = ' ';
static const char kPeriod  = '.';
static const char kBullet = '•';
TEHandle pMirrorEditText = nil;

/*******************************************************************************

    PasswordProc

    ModalDialog filter procedure. This procedure does three things:

    • Creates a private TextEdit record if one has not been created already. 
    • Checks for Return and Enter keypresses to support the standard
      Modal Dialog interface.
    • Supports the masking of all other printable keypresses, turning
      them into bullets so that the user can't see what was really typed.

    Here's the general idea behind getting a hidden password from the user: 
    what you want to do is catch all keystrokes as the user makes them, stash 
    them off someplace, and substitute a masking character (such as a .)
    that appears on the screen.

    ModalDialog passes all events to the dialog filter as it gets them, so
    that part is easy. We get the key that was pressed and examine it. If it's 
    Return or Enter, we tell the Dialog Manager that it's time for ModalDialog 
    to return to the caller.

    If it's any other nonprinting character, we pass it on to the Dialog
    Manager unchanged. This is so we don't do anything foolish like changing
    the cursor keys into bullets.

    We store any printable characters in a parallel TERecord. This is handy 
    because it lets us simply call TEKey to insert the character along with
    any previous characters the user has typed. Before we call TEKey, though,
    we synchronize the selection range of our private TERecord with the one
    the Dialog Manager is using. This way, if the user uses the mouse to
    select some or all of the bullets on the screen, we reflect the same 
    selection in our private TERecord. Then, when we call TEKey, the selected 
    characters in our private TERecord get replaced with the one just pressed.

    For the purposes of this sample, we also show the password on the screen.
    We do this by copying the text out of the Dialog Manager's TERecord and 
    inserting it into a static text item in the dialog.

*******************************************************************************/
pascal Boolean PasswordProc(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{
    Boolean result; // Value returned as the function result
    char key;       // Key pressed by the user




    Rect dummyRect;              // Scratch rectangle for creating TERecord
    long oldA5;                  // Value of AS register when routine is entered 
    TEHandle dialogTE;           // Handle to TERecord used by Dialog Manager
    Str255 password;             // Scratch variable used to display password

    short iKind;                 // For call to GetDItem
    Handle iHandle;              // For call to GetDItem
    Rect iRect;                  // For call to GetDItem

    oldA5 = SetCurrentA5();

    result = FALSE;

    if (pMirrorEditText == nil) {
        SetRect(&dummyRect, 0, 0, 0, 0);
        pMirrorEditText = TENew(&dummyRect, &dummyRect);
    }

    if ((theEvent->what == keyDown) || (theEvent->what == autoKey)) {
        key = theEvent->message & charCodeMask;
        if (( key == kReturn) || (key == kEnter)){
            if (pMirrorEditText != nil) { 
                TEDispose(pMirrorEditText);
                pMirrorEditText = nil;
            }
            *itemHit = kGoAway; 
            result = TRUE;
        } else {
            if (key >= kSpace) {
                //
                // This is tricky. We want to replace the key in
                // the event record with a bullet. The first line
                // removes just the key from the event record, leaving 
                // everything else untouched. The second line merges a 
                // bullet character into the recently cleared space.
                //
                theEvent->message &= -charCodeMask;
                theEvent->message |= kBullet;
            }

            dialogTE = ( (DialogPeek) theDialog)->textH;
            (*pMirrorEditText)->selStart = (*dialogTE)->selStart; 
            (*pMirrorEditText)->selEnd = (*dialogTE)->selEnd;
            TEKey(key, pMirrorEditText);

            password[0] = (*pMirrorEditText)->teLength; 
            BlockMove(*((*pMirrorEditText)->hText), &password[1], password[0]);

            GetDItem(theDialog, kRevealedText, &iKind, &iHandle, &iRect);
            SetIText(iHandle, password);
        }
    }
    (void) SetA5(oldA5);
    return result;
}

