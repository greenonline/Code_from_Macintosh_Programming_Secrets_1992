/* StandardStuff.r                                     */
/* Taken from Creation.r from Programmers Guide to MPW */
/* From Chapter 7 - Windows                            */

#include "Types.r"
#include "SysTypes.r" 
#include "Standard_Stuff_ResDefs_Ch7.h"

resource 'MBAR' (rMenuBar, preload) {
    { mApple, mFile, mEdit, mWindows };
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

// New stuff for Chapter 7 here

resource 'WIND' (kNewWindowID, "Window", purgeable, preload) { 
    {68, 44, 168, 144},
    documentProc,
    invisible,
    goAway, 
    0x0, 
    "Window"
};

resource 'MENU' (mWindows, preload) {

mWindows, textMenuProc, 0b1111111111111111111111111111100, enabled, "Windows",
    {
        "Tile",
            noicon, nokey, nomark, plain;
        "Stack",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain; 
//        "First Window",
//            noicon, "C", nomark, plain;
    }
};

resource 'STR#' (rMiscStrings, "Text for untitled window") {
    {
        /*[1]*/         "Untitled";
    }
}; 