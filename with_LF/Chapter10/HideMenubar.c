/* Listing 10-6. Hide Menubar.c */

#include "HideMenubar.h" 

/*******************************************************************************

    Private constants and variables

*******************************************************************************/

const Boolean kProhibitClicks = FALSE;     /* Set to TRUE to prohibit the user 
                                              from clicking on the menu bar
                                              while it's hidden. If FALSE, the
                                              menu will still respond to 
                                              clicks. */

Boolean gMenuBarHidden = FALSE;            /* Current state of the menu bar. */ 

short gOldeMBarHeight;                     /* Saves the height of the menu bar
                                              while we have it hidden. */

RgnHandle gOldeGrayRgn;                    /* Saves the region defining the
                                              desktop; we change it when 
                                              hiding the menu bar. */





/*******************************************************************************

    ToggleMenuBar

    Routine that can be called by clients to change the state of the menu bar.
    It examines the current state of the menu bar (as recorded by 
    gMenuBarHidden) and calls either ShowMenuBar or HideMenuBar, whichever is 
    appropriate.

*******************************************************************************/
void ToggleMenuBar()
{
    if  (gMenuBarHidden) 
        ShowMenuBar();
    else
        HideMenuBar();
}

/******************************************************************************* 

    HideMenuBar

    Hides the menu bar if it is visible. This is done by adding the space used
    by the menu bar to the desktop region. Normally, the menu bar is clipped out
    by the Window Manager, so nothing ever draws on top of it. We make it so
    we can draw on top of it and then call the Window Manager to refresh the
    desktop.

    If we want to prohibit the user from clicking on the menu bar while it's
    hidden, we set the height of the menu bar to O. Note the instruction we use
    to accomplish this:

         GetMBarHeight()      0;

    The menu bar's height is stored in a low-memory global at OxOBAA. Normally
    we use GetMBarHeight to retrieve this value to help determine things like
    window placement. However, GetMBarHeight isn't really a function; it's a
    macro:

        #define GetMBarHeight() (*(short*)OxOBAA)

    Because it's a macro, we can use it on the left side of the equation. The
    whole instruction expands to:

        (*(short*)0x0BAA) = 0;

    Which is just C's way of saying "store zero into low-memory location
    0x0BAA."

    Note that we should be careful about calling DrawMenuBar while the
    menu bar is hidden. While DrawMenuBar will faithfully draw the menu bar, the 
    resulting image is very volatile. Any window dragged over the menu bar will
    wipe out chunks of it.

*******************************************************************************/

void HideMenuBar()
{
    RgnHandle menuRgn;

    if (!gMenuBarHidden) {
        gOldeMBarHeight = GetMBarHeight();

        if (kProhibitClicks)
            GetMBarHeight() = 0;

        if (gOldeGrayRgn == nil) 
            gOldeGrayRgn = NewRgn();

        CopyRgn(GetGrayRgn(), gOldeGrayRgn);

        menuRgn = NewRgn();
        SetToMenuRect(menuRgn);
        UnionRgn(GetGrayRgn(), menuRgn, GetGrayRgn());

        PaintBehind((WindowPeek) FrontWindow(), menuRgn);
        CalcVisBehind((WindowPeek) FrontWindow(), menuRgn);

        DisposeRgn(menuRgn);

        gMenuBarHidden = TRUE;
    }
}

/*******************************************************************************

    ShowMenuBar

    Restore the old desktop region, making the world safe for menu bars again.
    Call DrawMenuBar to redraw the menu bar. If we previously set the menu bar
    height to zero in HideMenuBar, restore it.

*******************************************************************************/ void ShowMenuBar()
{
    if (gMenuBarHidden) {
        GetMBarHeight() = gOldeMBarHeight;
        CopyRgn(gOldeGrayRgn, GetGrayRgn());
        SetToMenuRect(gOldeGrayRgn);

        PaintBehind((WindowPeek) FrontWindow(), gOldeGrayRgn);
        CalcVisBehind((WindowPeek) FrontWindow(), gOldeGrayRgn);

        DrawMenuBar ();
        gMenuBarHidden = FALSE;

        SetEmptyRgn(gOldeGrayRgn);
    }
}

/*******************************************************************************

    SetToMenuRect

    Handy utility for calculating and returning the bounding rectangle of the
    menu bar. Assumes that gOldeMBarHeight holds the correct height of the
    menu bar. The resulting rectangle is placed in the region passed in to us.

*******************************************************************************/
void SetToMenuRect(RgnHandle rgn)
{
    Rect menuRect;

    menuRect = qd.screenBits.bounds;
    menuRect.bottom = gOldeMBarHeight;
    RectRgn(rgn, &menuRect);
}

