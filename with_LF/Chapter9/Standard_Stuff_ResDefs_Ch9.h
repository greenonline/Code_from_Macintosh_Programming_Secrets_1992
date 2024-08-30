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
// New for chapter 9 
//

// From StandardStuff.h

#define mMenu 131
#define iList 1
#define iWindow 2

#define kListDialog 130
#define kOK 1
#define kList 2

// From Stuff.c/h

#define kWDEF 1000