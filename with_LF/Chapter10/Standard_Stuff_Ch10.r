/* StandardStuff.r                                     */
/* Taken from Creation.r from Programmers Guide to MPW */
/* From Chapter 10 - Stuff                            */

#include "Types.r"
#include "SysTypes.r" 
#include "StandardStuff_ResDefs_Ch10.h"

resource 'MBAR' (rMenuBar, preload) {
    { mApple, mFile, mEdit, mStuff };
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
    mFile, textMenuProc, 0b1111111111111111111101100001000, enabled, "File",
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
// New stuff for Chapter 10 here
//

resource 'MENU' (mStuff, preload) {

mStuff, textMenuProc, 0b1111111111111111111111111111111, enabled, "Stuff",
    {
        "Test Cmd Period",
            noicon, nokey, nomark, plain;
        "Enter Password",
            noicon, nokey, nomark, plain;
    }
};

resource 'DLOG' (256, "kMovableModalDialog") {
    {100, 120, 250, 430}, 
    dBoxProc,
    //movableDBoxProc,
    visible,
    noGoAway, 
    0x0,
    256, 
    "kMovableModalDialog"
};


resource 'DITL' (256, "kMovableModalDialog") {
    {
        {5, 12, 23, 228},
        StaticText { 
            disabled,
            "Blah blah blah..."
        }
    }
};

resource 'DLOG' (kPasswordDialog, "kPasswordDialog") {
    {100, 120, 250, 430}, 
    dBoxProc,
    //movableDBoxProc,
    //noGrowDocProc,
    visible,
    noGoAway, 
    //goAway, 
    0x0,
    kPasswordDialog, 
    "kMovableModalDialog"
};


resource 'DITL' (kPasswordDialog, "kPasswordDialog") {
    {
        {5, 12, 23, 125},
        StaticText { 
            disabled,
            "Enter Password"
        }
        {35, 12, 53, 125},
        StaticText { 
            disabled,
            "Secret Word"
        }
        {5, 130, 23, 300},
        EditText { 
            disabled,
            "Enter Password"
        }
        {35, 130, 53, 300},
        StaticText { 
            disabled,
            "Revealed Text"
        }
    }
};

