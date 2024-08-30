/* StandardStuffPlusPlus.h                             */
/* Taken from Creation.r from Programmers Guide to MPW */
/* Has additional Select All in Edit menu              */
/* AND Font, Size, Style menus                         */

#include "Types.r"; 
#include "SysTypes.r" 
#include "Creation.h"

resource 'MBAR' (rMenuBar, preload) {
    { mApple, mFile, mEdit, mFont, mSize, mStyle };
};

resource 'MENU' (mApple, preload) {
    mApple, textMenuProc,
    0b1111111111111111111111111111101, /* disable dashed line, */enabled, apple,/* enable About and DAs*/
    {
        "About Creation",
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
        "-",
            noicon, nokey, nomark, plain; 
        "Select All",
            noicon, "A", nomark, plain
    }
};

resource 'MENU' (mFont, preload) {
mFont, textMenuProc,
0b1111111111111111111111111111111, enabled, "Font",
    {} 
};

resource 'MENU' (mSize, preload) {
mSize, textMenuProc,
0b1111111111111111111111111111111, enabled, "Size",
    {
        "6",
            noicon, nokey, nomark, plain;
        "9",
            noicon, nokey, nomark, plain;  
        "10",
            noicon, nokey, nomark, plain; 
        "12",
            noicon, nokey, nomark, plain;
        "14",
            noicon, nokey, nomark, plain;
        "18",
            noicon, nokey, nomark, plain;
        "24",
            noicon, nokey, nomark, plain;
        "36",
            noicon, nokey, nomark, plain;
        "48",
            noicon, nokey, nomark, plain; 
        "60",
            noicon, nokey, nomark, plain;
        "72",
            noicon, nokey, nomark, plain
    }
};

resource 'MENU' (mStyle, preload) {
mStyle, textMenuProc,
0b1111111111111111111111111111111, enabled, "Style",
    {
        "Plain",
            noicon, nokey, nomark, plain;
        "Bold",
            noicon, nokey, nomark, plain;
        "Italic",
            noicon, nokey, nomark, plain;
        "Underline",
            noicon, nokey, nomark, plain;
        "Outline",
            noicon, nokey, nomark, plain;
        "Shadow",
            noicon, nokey, nomark, plain
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
