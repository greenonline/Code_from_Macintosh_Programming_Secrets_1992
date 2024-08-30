/* StandardStuff.r                                     */
/* Taken from Creation.r from Programmers Guide to MPW */
/* For Chapter 4 - Dialogs                             */
/* Added About box - not used yet                      */

#include "Types.r"
#include "SysTypes.r" 
//#include "StandardStuff.h"
#include "Standard_Stuff_ResDefs.h"


resource 'MBAR' (rMenuBar, preload) {
//    { mApple, mFile, mEdit };
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

// New stuff below here

resource 'MENU' (mDialog, preload) {

mDialog, textMenuProc, 0b1111111111111111111111110111111, enabled, "Dialog",
    {
        "Modal",
            noicon, nokey, nomark, plain;
        "Modeless",
            noicon, nokey, nomark, plain; 
        "Moveable Modal",
            noicon, nokey, nomark, plain;
    }
};

resource 'DLOG' (256, "kMusicMakerDialog") {
    {40, 40, 390, 460}, 
    dBoxProc,
    visible,
    noGoAway, 
    0x0,
    256, 
    "kMusicMakerDialog"
};


resource 'DLOG' (257, "kModelessDialog") {
    {100, 120, 250, 430}, 
    noGrowDocProc,
    visible,
    noGoAway, 
    0x0,
    257, 
    "kModelessDialog"
};

resource 'DLOG' (258, "kMovableModalDialog") {
    {100, 120, 250, 430}, 
    movableDBoxProc,
    visible,
    noGoAway, 
    0x0,
    258, 
    "kMovableModalDialog"
};

resource 'DITL' (256, "kMusicMakerDialog") {
    { /* array DITLarray: 26 elements */ 
        /* [1] */
        {307,275,327,333},
        Button {
            enabled,
            "OK"
        };
        /* [2] */ 
        {307,89,327,147}, 
        Button {
            enabled,
            "Cancel" 
        };
        /* [3] */
        {3,163, 19, 276}, 
        StaticText {
            disabled,
            "Music Maker"
        },
        /* [4] */
        {34, 99, 50, 151}, 
        StaticText {
            disabled,
            "Artist"
        },
        /* [5] */
        {31, 159, 47, 234}, 
        EditText {
            enabled,
            ""
        },
        /* [6] */
        {61, 99, 77, 148}, 
        StaticText {
            disabled,
            "Album" 
        },
        /* [7] */
        {62, 159, 78, 234}, 
        EditText {
            enabled,
            ""
        },
        /* [8] */
        {106, 35, 124, 245}, 
        RadioButton {
            enabled,
            "Bat out of Hell"
        },
        /* [9] */
        {126, 35, 144, 223}, 
        RadioButton {
            enabled,
            "Hot Summer Night"
        },
        /* [10] */
        {146, 35, 164, 283}, 
        RadioButton {
            enabled,
            "Heaven Can Wait"
        },
        /* [11] */
        {166, 35, 184, 311}, 
        RadioButton {
            enabled,
            "All Revved Up With No Place To Go"
        }
        /* [12] */
        {186, 35, 204, 282}, 
        RadioButton {
            enabled,
            "Two out Of Three Ain't Bad"
        }
        /* [13] */
        {206, 35, 224, 284}, 
        RadioButton {
            enabled,
            "Paradise By The Dashboard Light"
        }
        /* [14] */
        {226, 35, 244, 249}, 
        RadioButton {
            enabled,
            "For Crying Out Loud"
        }
        /* [15] */
        {261, 31, 279, 156}, 
        CheckBox {
            enabled,
            "Surround Sound"
        }
        /* [16] */
        {261, 198, 279, 275}, 
        CheckBox {
            enabled,
            "Reverb"
        }
        /* [17] */
        {106, 326, 119, 354}, 
        StaticText {
            disabled,
            "B" 
        },
        /* [18] */
        {126, 326, 139, 401}, 
        StaticText {
            disabled,
            "H" 
        },
        /* [19] */
        {146, 326, 159, 401}, 
        StaticText {
            disabled,
            "W" 
        },
        /* [20] */
        {166, 326, 179, 401}, 
        StaticText {
            disabled,
            "A" 
        },
        /* [21] */
        {186, 326, 199, 401}, 
        StaticText {
            disabled,
            "T" 
        },
        /* [22] */
        {206, 326, 219, 401}, 
        StaticText {
            disabled,
            "P" 
        },
        /* [23] */
        {226, 326, 239, 401}, 
        StaticText {
            disabled,
            "F" 
        },
        /* [24] */
        {263, 159, 279, 191}, 
        StaticText {
            disabled,
            "S" 
        },
        /* [25] */
        {262, 286, 278, 361}, 
        StaticText {
            disabled,
            "R" 
        },
        /* [26] */
        {13, 27, 45, 59}, 
        Picture {
            disabled,
            128 /* 'PICT' ID */
        },
    }
};


resource 'DITL' (257, "kModelessDialog") {
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
        {90,18,109,198}, 
        Control {
            disabled, 
            kPopUpCNTL
        }
        {65,18,84,198}, 
        Control {
            disabled, 
            kPopUpCNTL2
        }
    }
};

resource 'DITL' (258, "kMovableModalDialog") {
    {
        {116, 147, 136, 205},
        Button { 
            enabled,
            "OK"
        },
//        {5, 12, 93, 228},
        {5, 12, 23, 228},
        StaticText { 
            disabled,
            "Blah blah blah..."
        }
//        {5, 12, 93, 228},
        {90, 12, 100, 228},
        UserItem { 
            disabled,
            ""
        }
    }
};

data 'PICT' (128) {
	$"0000 0000 0000 0000 0000"                           /* .......... */
};

data 'STR#' (131, "kDialogStrings") {
	$"0001 0C45 7261 7369 6E67 2044 6973 6B"              /* ...Erasing Disk */
};

data 'STR ' (131, "kDialogStrings") {
	$"0C45 7261 7369 6E67 2064 6973 6B"                   /* .Erasing disk */
};


resource 'CNTL' (kPopUpCNTL, preload, purgeable) {
    {90, 18, 109, 198},     /*enclosing rectangle of control*/
    popupTitleLeftJust,     /*title position*/
    visible,                /*make control visible*/
    50,                     /*pixel width of title*/
    kPopUpMenu,             /*'MENU' resource ID*/
    popupMenuCDEFProc,      /*pop-up control definition ID*/
    0,                      /*reference value*/
    "Speed:"                /*control title*/
};

resource 'MENU' (kPopUpMenu, preload, purgeable) {
    mPopUp, 
    textMenuProc,
    0b1111111111111111111111111111111,
    enabled, 
    "Speed",
    {
        "300 bps", noIcon, nokey, nomark, plain;
        "1200 bps", noIcon, nokey, nomark, plain;
        "2400 bps", noIcon, nokey, nomark, plain;
        "9600 bps", noIcon, nokey, nomark, plain;
        "19200 bps", noIcon, nokey, nomark, plain
    }
};

resource 'CNTL' (kPopUpCNTL2, preload, purgeable) {
    {65, 18, 84, 198},     /*enclosing rectangle of control*/
    popupTitleLeftJust,     /*title position*/
    visible,                /*make control visible*/
    50,                     /*pixel width of title*/
    kPopUpMenu2,             /*'MENU' resource ID*/
    popupMenuCDEFProc,      /*pop-up control definition ID*/
    0,                      /*reference value*/
    "Port:"                /*control title*/
};

resource 'MENU' (kPopUpMenu2, preload, purgeable) {
    mPopUp2, 
    textMenuProc,
    0b1111111111111111111111111111111,
    enabled, 
    "Speed",
    {
        "Modem", noicon, nokey, nomark, plain;
        "Printer", noicon, nokey, nomark, plain
    }
};

/* About box */
//resource 'ALRT' (1024, "kAlertResourceID") { 
//    {40, 40, 184, 276},
//    1024,
//    {
//        /* array: 4 elements */ 
//        OK, visible, sound1,
//        OK, visible, sound1,
//        OK, visible, sound1,
//        OK, visible, sound1
//    }
//};
/*
resource 'DITL' (1024, "kAlertResourceID") {
    {
        {5, 12, 93, 228},
        StaticText { 
            disabled,
            "^0, from Macintosh Programming Secrets for the Macintosh, 1993-94"
             " by Scott Knaster and Keith Rollin, Addison-Wesley"
        },
        {116, 147, 136, 205},
        Button { 
            enabled,
            "OK"
        }
    }
};
*/

//
// New Chapter 6 stuff
//
resource 'WIND' (kNewWindowID, "Window", purgeable, preload) { 
    {68, 44, 168, 144},
    documentProc,
    invisible,
    goAway, 
    0x0, 
    "Window"
};
