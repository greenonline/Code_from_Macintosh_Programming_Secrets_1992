//
// From Standard_stuff.h
//

/* ======================================================================= */
/* =========== R E S O U R S E  N U M B E R S ============================ */
/* ======================================================================= */


#define rAboutAlert 128 /* about alert */
#define rErrorAlert 129 /* error user alert */
#define rErrorStrings  128
#define errWimpyROMs   1
#define errWimpySystem 2
#define errWeirdSystem 3
#define errNoMenuBar 4

/* The following constants are used to identify menus and their items. The menu IDs
   have an "m" prefix and the item numbers within them have an "I" prefix.  */

#define rMenuBar 128                        /* Application's menu bar */

#define mApple 128                          /* Apple menu */
#define iAbout 1

#define mFile 129                           /* File menu */
#define iNew 1
#define iOpen 2
#define iLine11 3
#define iClose 4
#define iSave 5
#define iSaveAs 6
#define iRevert 7
#define iLine12 8
#define iPageSetup 9
#define iPrint 10
#define iLine13 11
#define iQuit 12

#define mEdit 130                           /* Edit menu */
#define iUndo 1
#define iLine21 2
#define iCut 3
#define iCopy 4
#define iPaste 5
#define iClear 6


//
// New for chapter 10 
//

// From StandardStuff.h

#define mStuff 131
#define iTestCmdPeriod 1
#define iEnterPassword 2

// From EnterPassword.c

/*
const short  kPasswordDialog = 257;
const short  kEnterPasswordText = 1;
const short  kSecretWordText = 2;
const short  kEditText = 3;
const short kRevealedText = 4;
const short kGoAway = 18;
*/

#define kPasswordDialog 257
#define kEnterPasswordText 1
#define kSecretWordText 2
#define kEditText 3
#define kRevealedText 4
#define kGoAway 18