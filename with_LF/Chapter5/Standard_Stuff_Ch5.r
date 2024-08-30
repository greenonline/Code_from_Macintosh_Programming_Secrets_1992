/* StandardStuff.h                                     */
/* Taken from Creation.r from Programmers Guide to MPW */
/* From Chapter 5 - QuickDraw                          */

#include "Types.r"
#include "SysTypes.r" 
//#include "Standard_Stuff.h"
#include "Standard_Stuff_ResDefs.h"

resource 'MBAR' (rMenuBar, preload) {
    { mApple, mFile, mEdit, mDialog };
};

resource 'MENU' (mApple, preload) {
    mApple, textMenuProc,
    0b1111111111111111111111111111101, /* disable dashed line, */enabled, apple,/* enable About and DAs*/
    {
        "About...",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain
    }
};

resource 'MENU' (mFile, preload) {
    mFile, textMenuProc, 0b1111111111111111111101100001001, enabled, "File",
    {
        "New",
            noicon, "N", nomark, plain;
        "Open",
            noicon, "O", nomark, plain;
        "-",
            noicon, nokey, nomark, plain; 
        "Close",
            noicon, "W", nomark, plain; 
        "Save",
            noicon, "S", nomark, plain; 
        "Save As...",
            noicon, nokey, nomark, plain; 
        "Revert",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain; 
        "Page Setup...",
            noicon, nokey, nomark, plain; 
        "Print...",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain; 
        "Quit",
            noicon, "Q", nomark, plain
    }
};

resource 'MENU' (mEdit, preload) {

mEdit, textMenuProc, 0b1111111111111111111111110111101, enabled, "Edit",
    {
        "Undo",
            noicon, "Z", nomark, plain;
        "-",
            noicon, nokey, nomark, plain; 
        "Cut",
            noicon, "X", nomark, plain;

        "Copy",
            noicon, "C", nomark, plain;
        "Paste",
            noicon, "V", nomark, plain;
        "Clear",
            noicon, "B", nomark, plain;
    }
};

resource 'SIZE' (-1) { /* MultiFinder-aware application */
    dontSaveScreen,
    acceptSuspendResumeEvents,
    enableOptionSwitch,
    canBackground,
    multiFinderAware,
    backgroundAndForeground,
    dontGetFrontClicks,
    ignoreChildDiedEvents,
    not32BitCompatible,
    reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
    96*1024,
    64*1024
};


//
// New stuff for Chapter 5 here
//


resource 'MENU' (mDialog, preload) {

mDialog, textMenuProc, 0b1111111111111111111111110111111, enabled, "Dialog",
    {
        "Show Dialog",
            noicon, nokey, nomark, plain;
        "Show Saving Dialog",
            noicon, nokey, nomark, plain; 
    }
};

resource 'WIND' (kWindowID, "Window", purgeable, preload) { 
    {68, 44, 568, 544},
    documentProc,
    invisible,
    goAway, 
    0x0, 
    "Window"
};

resource 'DLOG' (kDialogID, "kModelessDialog") {
    {100, 120, 250, 430}, 
    dBoxProc,
    visible,
    noGoAway, 
    0x0,
    kDialogID, 
    "kModelessDialog"
};

resource 'DITL' (kDialogID, "kModelessDialog") {
    {
        {116, 147, 136, 205},
        Button { 
            enabled,
            "OK"
        },
        {5, 12, 50, 228},
        StaticText { 
            disabled,
            "Blah blah blah..."
        }
    }
};



