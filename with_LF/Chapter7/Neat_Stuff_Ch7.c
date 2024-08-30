/* Listing 7-2. Neat Stuff.c. */

#include "Standard_Stuff_Ch7.h"
#include "Neat_Stuff.h_Ch7"
#include "ZoomRect.h"

/********************************************************************************

    Global variables

********************************************************************************/

short gUntitledWindowNumber = 0;  /* Number for next new window. These
                                  /* numbers start at zero and keep
                                  /* going up; they are not recycled. */
WindowPtr gFirstWindow;           /* We keep track of our windows in
                                  /* the order in which they wore
                                  /* created. This variable points to
                                     the first (oldest window. */


/* Used when matching up windows to their menu items, and vice-versa.  */

short gMenuItem;
Boolean gDoneCounting;
WindowPtr gFoundWindow;
WindowPtr gTargetWindow;

/* Variables set up by ForEachWindow() and ForEachWindowPerScreenDo () */

short gWindowNumber;              /* Ordinal number of the current
                                  /* window on the current monitor.  */





short gNumberOfWindows;     /* Total number of window on
                               current monitor.   */
GDHandle gScreenDevice;     /* Reference to the current monitor. */
Rect gScreenRect;           /* Bounds of the current monitor. Takes
                               into account the menu bar if this
                               is the main monitor. */

/* Variables set up  and shared by tiling and stacking routines. */

short gScreenWidth;         /* Width of the current monitor. */
short gScreenHeight;        /* Height of the current monitor. */
short gNewWindowWidth;       /* During tiling or stacking, every
                               window is essentially the same
                               size. This variable holds the
                               window's new width. */
short gNewWindowHeight;     /* Ditto, but for height. */


/********************************************************************************

    DoNewWindow

    Create a window from a WIND resource. If that succeeds, show the window
    and add it to the Windows menu. When showing the window, use a zoom
    effect that zooms the window from the Apple menu to its final position.

    By passing in NIL as the window pointer in the call to GetNewWindow, we
    tell the system to create window record on the heap for us. This means
    that we must close this  window with a call to DisposeWindow rather than
    CloseWindow.

********************************************************************************/

void DoNewWindow(void)
{
    WindowPtr newWindow;
    Str255 untitledTitle;
    Str255 numberAsString;
    Rect rect1, rect2;

    newWindow = GetNewWindow(kNewWindowID, NIL, (WindowPtr) -1);
    if (newWindow != NIL) {
        GetIndString(untitledTitle, rMiscStrings, sUntitledTitle);
        NumToString(++gUntitledWindowNumber, numberAsString);
        CatenatePStrings(untitledTitle, numberAsString);
        SetWTitle(newWindow, untitledTitle);

        SetRect(&rect1, 0, 0, 0, 0);
        rect2 = GetWindowStructureRect(newWindow);
        ZoomRect(rect1, rect2, kZoomOut);

        ShowWindow(newWindow);
        AddWindowToMenu(newWindow);
    }
}

/********************************************************************************

    DoCloseWindow

    Remove the specified window from the windows menu and close it by calling
    DisposeWindow. When closing the window, reverse the zooming effect we did
    when opening the Window.

    Note that we call DisposeWindow rather than CloseWindow. Because we pass
    in NIL as the window record storage when we create the window, we
    dynamically allocate the record in the application's heap. To free up
    that record, we must call DisposeWindow.

********************************************************************************/


void DoCloseWindow(WindowPtr theWindow)
{
    Rect rect1, rect2;

    SetRect (&rect1, 0, 0, 0, 0);
    rect2 = GetWindowStructureRect(theWindow);

    RemoveWindowFromMenu(theWindow);
    DisposeWindow(theWindow);

    ZoomRect(rect2, rect1, kZoomIn);
}


/********************************************************************************

    DoActivateWindow

    When a window becomes active, it's almost the same as saying that it is
    becoming the frontmost window. Similarly, when a window is being
    deactivated, it's almost the same as saying that another window it coming
    to the front. Therefore, when a window is activated or deactivated, we
    adjust where the check mark is placed in the Windows menu.

********************************************************************************/



void DoActivateWindow(WindowPtr theWindow, Boolean becomingActive)
{
    short menuItem;
    MenuHandle windowsMenu;

    windowsMenu = GetMHandle(mWindows);
    menuItem = GetMenuItemForWindow(theWindow);
    if (becomingActive)
        SetItemMark(windowsMenu, menuItem, checkMark);
    else
        SetItemMark(windowsMenu, menuItem, noMark);
}

/********************************************************************************

    DoZoomWindow

    This routine gets called when the user clicks in the zoom box of window.
    It's our own special ZoomWindow procedure that zooms the window to the
    screen that contains the majority of the window.

    DoZoomWindow works by getting three pertinent rectangles: the bounds of
    the monitor the window is on, the bounds of the window's frame (its
    structure), and the bounds of the window's content area. Using these
    rectangles, we calculate where we want the content rectangle of the
    zoomed window to be. We take that resulting rectangle and jam it into
    structure that the default WDEF maintains, which helps it windows in
    and out. Once all that is done, we call ZoomWindow. ZoomWindow will zoom
    the window to the size and location we just calculated.

    Note that in these days of the nineties, there is another step that you might
    want to take if you want your application to get all those ooh's and aah's
    from the customers. With monitors getting larger and larger, it's a good
    idea to zoom your window to be only as large as it needs to be. For
    instance, it the user zooms a word processing document on a 19-inch
    monitor, it would be nice to zoom the window's width out to 8.5 inches
    rather than the full 13 inches. We don't do this here, but it's something
    for you to think about.

********************************************************************************/


void DoZoomWindow(WindowPtr window, short zoomDir, Boolean front)
{
    Rect contentRect, structureRect, deviceRect;

    /* If there is the possibility of multiple gDevices, we must check
       them to make sure we are zooming to the right display device. */

    if ((zoomDir == inZoomOut) && (gMac.hasColorQD)) {

        contentRect = GetWindowContentRect(window);
        structureRect = GetWindowStructureRect(window);
        deviceRect = GetWindowDeviceRectNMB(window);

        deviceRect.left += (contentRect.left - structureRect.left + 2);
        deviceRect.top += (contentRect.top - structureRect.top + 2);
        deviceRect.right -= (structureRect.right - contentRect.right + 2);
        deviceRect.bottom -= (structureRect.bottom - contentRect.bottom   + 2);

        (*(WStateDataHandle)(((WindowPeek)window)->dataHandle))->stdState = deviceRect;
    }
    ZoomWindow(window, zoomDir, front);
}

/********************************************************************************

    DoTileWindows

    Call ForEachWindowPerScreenDo, which is a routine that allows us to
    perform a specified operation on every window. Our task here is to tile
    all the windows, making sure that each is the same size, but in
    nonoverlapping locations. In our TileSetup routine, we calculate the
    window's size and save it off. our TileTheWindow routine, we set each
    window to that size, and position it in the right place.

********************************************************************************/

void DoTileWindows(void)
{
    ForEachWindowPerScreenDo(TileSetup, TileTheWindow, NIL);
}



const short kMinHeight = 100;
const short kMinWidth = 100;
const short kGapBetweenWindows = 2;


short pMaxWindowsPerRow;
short pMaxWindowsPerColumn;
short pNumberOfColumns;
short pNumberOfRows;


//
// This function is called once per monitor by ForEachWindowPerScreenDo.
// Based on the size of the screen and the number of windows that are to
// be tiled on the screen, we determine the number of rows and columns
// that we want tile the windows into. We also determine the size
// each window on this screen should be.
//

void TileSetup(void)
{
    if (gNumberOfWindows > 0) {
        gScreenRect.top += kGapBetweenWindows;
        gScreenRect.left += kGapBetweenWindows;

        gScreenWidth = gScreenRect.right - gScreenRect.left;
        gScreenHeight = gScreenRect.bottom - gScreenRect.top;

        pMaxWindowsPerRow = gScreenWidth/kMinWidth;
        pMaxWindowsPerColumn = gScreenHeight / kMinHeight;

        pNumberOfColumns = (gNumberOfWindows-1)  / pMaxWindowsPerColumn +1;
        pNumberOfRows = (gNumberOfWindows-1) / pNumberOfColumns +1;

        gNewWindowWidth = gScreenWidth / pNumberOfColumns;
        gNewWindowHeight = gScreenHeight / pNumberOfRows;
    }
}

//
// Main window cruncher for tiling. This routine is called once for every 
// window. Make the given window the size we determined in TileSetUp and
// place ft in the appropriate row and column based on its ordering in the
// window list.
//
// Note that our calculations are set up to determine the size and location
// of the window's frame, not its content rectangle (i.e., its port Rect).
// However, MoveWindow and SizeWindow like to work with values that do
// affect the portRect. To solve this problem, we do the actual moving and
// resizing by creating a routine called SetWindowBounds. This utility
// routine translates the frame rectangle into the content rectangle for us
// before calling MoveWindow and SizeWindow.
//


void TileTheWindow(WindowPtr theWindow)
{
    Rect newBounds;
    short row, column;

    row = (gWindowNumber-1) / pNumberOfColumns;
    column = (gWindowNumber-1) % pNumberOfColumns;

    newBounds.left = gScreenRect.left + column * gNewWindowWidth;
    newBounds.top = gScreenRect.top + row * gNewWindowHeight;
    newBounds.right = newBounds.left + gNewWindowWidth  - kGapBetweenWindows;
    newBounds.bottom = newBounds.top + gNewWindowHeight - kGapBetweenWindows;

    SetWindowBounds(theWindow, newBounds);
}

/********************************************************************************

    DoStackWindows

    Call ForEachWindowPerScreenDo, which is a routine that allows us to
    perform a specified operation on every window. Our task here is to stack
    all the windows. In our StackSetup routine, we calculate the number of
    diagonals we'll have on the monitor we are tiling on. We also calculate an
    initial window size, a size which gets smaller as subsequent windows get
    stacked further down and to the right. In our TileTheWindow routine, we
    set each window to the appropriate size and position.

********************************************************************************/


void DoStackWindows(void)
{
    ForEachWindowPerScreenDo(StackSetup, StackTheWindow, NIL);
}

const short kVerticalStagger = 20;
const short kHorizontalStagger = 10;
const short kWindowsPerDiagonal = 8;


//
// This function is called once per monitor by ForEachWindowPerScreenDo.
// Its purpose is twofold. First, it figures out how many diagonals of
// windows will be needed to stack all of the windows on the current screen.
// Right now, we permit only 8 windows to be stacked in a diagonal before we
// need to create a new diagonal of windows that's moved over to the right a
// little bit.
//


// The second thing this function does is figure out a good initial size for
// the stacked windows. This size shrinks for windows that are stacked
// further down and to the right on the screen. The calculations for this
// shrinking in the StackTheWindow function.
//

void StackSetup(void)
{
    short maxWindowsInDiagonal;
    short numberOfBottomWindows;

    InsetRect(&gScreenRect, kGapBetweenWindows, kGapBetweenWindows);

    // Find the height and width of this window

    gScreenWidth = gScreenRect.right - gScreenRect.left;
    gScreenHeight = gScreenRect.bottom - gScreenRect.top;

    // Find the longest diagonal of windows we will be dealing
    // with. This will either be kWindowsPerDiagonal or
    // gNumberOfWindows, whichever is smaller.

    maxWindowsInDiagonal = gNumberOfWindows;
    if (maxWindowsInDiagonal > kWindowsPerDiagonal)
        maxWindowsInDiagonal = kWindowsPerDiagonal; 

    // Find out how many windows will end up in the last
    // position of a diagonal. This number is crucial in
    // determining the horizontal span of all the windows,
    // which is used in calculating the windows' widths.

    numberOfBottomWindows = (gNumberOfWindows /  kWindowsPerDiagonal);

    if (numberOfBottomWindows > 0)
        --numberOfBottomWindows;

    // Figure out the size of the first window to be stacked.
    // We start off with the size of the entire screen and
    // start trimming it back to allow for other windows. First,
    // we trim back the width by an amount equal to the number
    // of pixels that the rightmost window will be from the
    // left edge of the screen. Next, we trim back the height so
    // that we can accommodate the tallest diagonal we'll be dealing
    // with.

    gNewWindowWidth = gScreenWidth
                        - (((maxWindowsInDiagonal -1) + numberOfBottomWindows)
                            * kHorizontalStagger);
    gNewWindowHeight = gScreenHeight
                         -(maxWindowsInDiagonal - 1) * kHorizontalStagger;
}

// 
// This routine is called once for each window. Its function is to calculate
// the size and location for each window. First, it determines which diagonal
// the specified window should be in and its position within that diagonal.
// Then it determines the horizontal position for the window; because each window
// is always moved to the right from the previous window by a constant
// amount, the horizontal position is a simple function of the window's
// creation rank. Next, we determine the vertical position of the window.
// This, too, 18 simple function, this time of the window's position in the
// diagonal it will be placed in. What this means is that the 1st, 9th, 17th,
// etc., windows will all have the same vertical position. Similarly for the
// 2nd, 10th, 18th, etc., windows, and
//
// Next, we have to determine the size of the window. First, we consider the
// width. Each window in a diagonal is the same size. This size is determined
// by taking the initial window size calculated in StackSetup and
// subtracting an amount that is a function of the diagonal number we are
// working on. First, we subtract an amount such that the right edge of
// window in this diagonal will line up with the corresponding window in the
// previous diagonal. Next, we add back 10 pixels just to give a nice
// staggering effect on the right side as well as the left. After doing
// that, we work on figuring the height of the window. We start off with the
// standard height calculated in StackSetup. Then we subtract a multiple of
// 10 pixels, which is based on how far down in the diagonal the window is.
// This means that the height of the window is a function of the window's
// position in the diagonal, while the width of the window is a function of
// the position of the diagonal itself.
//

void StackTheWindow(WindowPtr theWindow)
{
    Rect newBounds;
    short placeInDiagonal, diagonal;

    placeInDiagonal = (gWindowNumber -1) % kWindowsPerDiagonal;
    diagonal = (gWindowNumber - 1) / kWindowsPerDiagonal;

    newBounds.left = gScreenRect.left + (gWindowNumber -1) * kHorizontalStagger;
    newBounds.top = gScreenRect.top + placeInDiagonal * kVerticalStagger;
    newBounds.right = newBounds.left + gNewWindowWidth - diagonal * (kWindowsPerDiagonal * kHorizontalStagger -10);
    newBounds.bottom = newBounds.top + gNewWindowHeight - placeInDiagonal *10;

    SetWindowBounds(theWindow, newBounds);
}


/********************************************************************************

    DoSelectFromWindowsMenu

    Called when the user selects one of the menu items containing a window's
    name (in the Windows menu). We stash off the menu item that was selected
    and then start iterating over all the windows in chronological order.
    Since the list of window names in the Windows menu is also maintained in
    chronological order, we know that there is direct relationship between
    the menu item number and the window's chronological rank. In other words,
    if we selected the Xth item in the Windows menu (not including the Tile or
    Stack menu items), all we have to do is find the Xth oldest window and
    select it.

********************************************************************************/




void DoSelectFromWindowsMenu(short menuItem)
{
    gMenuItem = menuItem - iFirstWindow + 1;
    ForEachWindowDo(NIL, LookForSelectedWindow, NIL);
}

void LookForSelectedWindow(WindowPtr theWindow)
{
    if (--gMenuItem == 0)
        SelectWindow(theWindow);
}



/********************************************************************************

    AddWindowToMenu

    Called to add a newly created window to the windows menu. We make that
    addition by making a call to AppendMenu to create the new menu item, and
    then SetItem to set that item's name. The reason we don't let
    AppendMenu set the name is because of the metacharacter processing that
    AppendMenu performs. 1f our window's name happened to have something like
    an ":" or "/" it it, what appeared in the menu wouldn't be what we
    expected. SetItem doesn't recognize the metacharacters, so we avoid
    that problem when calling it.

    Next, we insert our window into our own private window list. Normally, you
    don't need to do this. The Window Manage: keeps track of all the window
    on the screen so that you don't have to. However, the list it manages
    links the windows in front to back order. In other words, the frontmost
    window is at the front of the list, the record for the window behind it is
    second in the list, and so on.

    In order to manage our Windows menu, we'd really like to have a list of
    windows in chronological order. Ke do this by using the refCon field of
    the window record. Each window's refCon field will point to the next
    youngest window. The oldest window (the head of the chain) will be kept in
    the global variable gFirstWindow. The refCon field of the last window is
    NIL. If there are no windows, gFirstWindow is NIL.

********************************************************************************/


void AddWindowToMenu(WindowPtr theWindow)
{
    Str255 title;
    MenuHandle windowsMenu;
    WindowPtr lastWindow;

    GetWTitle(theWindow, title);
    windowsMenu = GetMenu(mWindows);
    AppendMenu(windowsMenu, "\pNeed something here or call doesn't work.");
    SetItem(windowsMenu, CountMItems(windowsMenu), title);
 
    lastWindow = GetPreviouslyCreatedWindow(NIL); /* NIL means "get last window" */
    if (lastWindow != NIL)
        SetWRefCon(lastWindow, (long) theWindow);
    else
        gFirstWindow = theWindow;
}

/********************************************************************************

    RemoveWindowFromMenu

    Get the menu item that corresponds to this window, and remove that menu
    item from the Windows menu. Remove the window from our chronological list
    of windows.

********************************************************************************/

void RemoveWindowFromMenu(WindowPtr theWindow)
{
    MenuHandle windowsMenu;
    WindowPtr previousWindow;

    windowsMenu = GetMenu(mWindows);
    DelMenuItem(windowsMenu, GetMenuItemForWindow(theWindow));

    if (theWindow == gFirstWindow) {
        gFirstWindow = (WindowPtr) GetWRefCon(theWindow);
    } else {
        previousWindow = GetPreviouslyCreatedWindow(theWindow);
        SetWRefCon(previousWindow, GetWRefCon(theWindow));
    }


    SetWRefCon(theWindow, 0);
}


/********************************************************************************

    ForEachWindowDo

    Routine that iterates over all of the windows. It first counts up the
    number of windows on the monitor we are currently examining. Next, it
    calls a setup routine provided by the caller. After that, it calls an
    action procedure for each window. This action procedure is passed a
    pointer to the current window we are iterating over. It is also able to
    access the total number of windows and the window's rank through the
    global variables gNumberOfWindows and gWindowNumber. After all windows
    have been acted upon, a cleanup routine provided by the caller is called.

    Note that windows are iterated in chronological order, as maintained by
    the links stored in their refCon fields.

********************************************************************************/



void ForEachWindowDo(SetUpProc theStarter,
WindowActionProc theDoer,
FinishUpProc enderWiggin)
{
    WindowPtr currentWindow;
    Boolean haveAWindow;
    Boolean windowIsOurConcern;

    // To get the number of windows, we recursively call ourself
    // with an action procedure that simply increments a counter.
    // So that we don't infinitely recurse, we check the action
    // procedure to see if it's our counting routine. If not,
    // we don't recurse.

    if (theDoer != CountWindows){
        gNumberOfWindows = 0;
        ForEachWindowDo(NIL, CountWindows, NIL);
    }

    if ((theDoer == CountWindows) || (gNumberOfWindows> 0)) {
        // start keeping track of window rank.
        gWindowNumber = 0;
        // If the caller provided a setup routine, call it,


        if (theStarter != NIL)
            (*theStarter)();

        // Start iterating over all the windows, Skip over any DAs or
        // dialog windows. The expression that sets WindowIsOurConcern
        // could be modified also to skip over invisible windows.

        if (theDoer != NIL){
            currentWindow = gFirstWindow;
            while (currentWindow != NIL){
                windowIsOurConcern = IsAppWindow(currentWindow);
                if (windowIsOurConcern) {
                    ++gWindowNumber;
                    (*theDoer)(currentWindow);
                }
                currentWindow = (WindowPtr) GetWRefCon(currentWindow);

            }
        }

        // Done with all the windows. if the caller
        // provided a cleanup routine, call it.

        if (enderWiggin != NIL)
            (*enderWiggin)();
    }
}


/********************************************************************************

    ForEachWindowPerScreenDo

    Mean routine that iterates over all the windows, but in a particular 
    fashion. What it does is first iterate over all the monitors hooked up to
    the machine. For each monitor, it first counts up the number of windows on
    the monitor we are currently examining. Next, it calls a setup routine
    provided by the caller. After that, : calls an action procedure for each
    window on the monitor. This action procedure is passed a pointer to the
    current window we are iterating over. It is also able to access the handle
    to the current monitor, the size of the current monitor, the number of
    windows on the current monitor, and the window's rank on the current
    monitor through the global variables gScreenDevice, gScreenRt,
    gNumberOfWindows, and gWindowNumber. After all windows on the monitor have
    been acted upon, a cleanup routine provided by the caller is called.
    Finally, we move on to the next monitor.

    Note that windows iterated in chronological order, as maintained by
    the links stored in their refCon fields.

********************************************************************************/

void ForEachWindowPerScreenDo(SetUpProc theStarter,
WindowActionProc theDoer,
FinishUpProc enderWiggin)
{
    WindowPtr currentWindow;
    Boolean haveAWindow;
    Boolean windowIsOurConcern;
    GDHandle currentScreenDevice;

    if (gMac.hasColorQD)
        currentScreenDevice = GetDeviceList();
    else
        currentScreenDevice = NIL;

    do {

        // To get the number of windows on the current monitor of
        // interest, we recursively call ourself with an action
        // procedure that simply increments a counter. So that we
        // don't infinitely recurse, we check the action procedure
        // to see if it's our counting routine. If not, we don't recurse.

        if (theDoer != CountWindows) {
            gNumberOfWindows = 0; 
            ForEachWindowPerScreenDo(NIL, CountWindows, NIL); 
        }

        if ((theDoer == CountWindows) || (gNumberOfWindows > 0)){
            // Start keeping track of window rank on this monitor

            gWindowNumber = 0;

            // Export the handle to the current device

            gScreenDevice = currentScreenDevice;

            // Export the size of the monitor. If we have a valid device
            // handle, use it to get the device's size. If we don't have
            // a valid handle (it is NIL -- indicating that we are running
            // on a machine without Color QuickDraw), get the size of the
            // screen from GetMainScreenRect() (which will effectively
            // return qd.screenBits.bounds in this case).

            if (currentScreenDevice != NIL){
                gScreenRect = (**currentScreenDevice).gdRect;
                if (currentScreenDevice = GetMainDevice())
                    gScreenRect.top += GetMBarHeight();
            } else {
                gScreenRect = GetMainScreenRect();
                gScreenRect.top = GetMBarHeight();
            }

            // If the caller provided a setup routine, call it.

            if (theStarter != NIL)
                (*theStarter)();

            // Start iterating over all the windows. If the majority of the
            // windows are on the monitor we are currently looking at from
            // our outer loop, pass that window to the caller's action
            // procedure. Skip over any DAs or dialog windows. The expression
            // that sets "windowIsOurConcern" could be modified also to skip
            // over invisible windows.

            if (theDoer != NIL) {
                currentWindow = gFirstWindow;
                while (currentWindow != NIL) {
                    windowIsOurConcern = IsAppWindow(currentWindow) &&
(currentScreenDevice == GetWindowDevice(currentWindow));
                    if (windowIsOurConcern) {
                        ++gWindowNumber;
                        (*theDoer)(currentWindow);
                    }
                    currentWindow = (WindowPtr) GetWRefCon(currentWindow);

                }
            } 

            // Done with all the windows on this monitor. If the caller
            // provided a cleanup routine, call it.

            if (enderWiggin != NIL)
                (*enderWiggin) ();
        }

        // Do the next monitor
        if (gMac.hasColorQD)
            currentScreenDevice = GetNextDevice(currentScreenDevice);
    } while (currentScreenDevice != NIL);
}


/********************************************************************************

    CountWindows

    This is a WindowActionProc called by ForEachWindowPerScreenDo and
    ForEachWindowDo to count windows.

********************************************************************************/

void CountWindows (WindowPtr theWindow)
{
    ++gNumberOfWindows;
}

/********************************************************************************

    GetPreviouslyCreatedWindow

    Called to return the window immediately before the target window in our
    chronological list of windows. This is handy for searching backward when
    with a singly linked list of records.

********************************************************************************/

WindowPtr GetPreviouslyCreatedWindow(WindowPtr theWindow)
{
    gFoundWindow = NIL;
    gTargetWindow = theWindow;
    ForEachWindowDo (NIL, LookForPreviousWindow, NIL);
    return gFoundWindow;
}

void LookForPreviousWindow (WindowPtr theWindow)
{
    if (gTargetWindow == (WindowPtr) GetWRefCon(theWindow))
        gFoundWindow = theWindow;
}


/********************************************************************************

    GetMenuItemForWindow

    Given a window pointer, return the number for the Windows menu item that
    corresponds to it. This is done by iterating over all our windows in
    chronological order, incrementing a counter as we go. When we get to the
    window we are interested in, we stop counting. This means that our counter
    ends up holding the chronological rank of our window. This rank is then added
    to iFirstWindow to give us the appropriate menu item number.

********************************************************************************/

short GetMenuItemForWindow (WindowPtr theWindow)
{
    gDoneCounting = FALSE;
    gTargetWindow = theWindow;
    gMenuItem = iFirstWindow - 1;
    ForEachWindowDo(NIL, CountSomeWindow, NIL);

    return gMenuItem;
}

void CountSomeWindow(WindowPtr theWindow)
{
    if (!gDoneCounting) {
        ++gMenuItem;
        if (theWindow == gTargetWindow)
        gDoneCounting = TRUE;
    }
}


/********************************************************************************

    SetWindowBounds

    Set the size and location of the given window's FRAME. Note that this
    differs from a simple MoveWindow/SizeWindow combination in that the
    parameters passed to those routines work on the window's content
    rectangle, not the outer frame rectangle.

    What we do to accomplish this ia to take the frame rectangle passed into
    this routine and Calculate what the content rectangle would be for a
    window that had that frame's size. This is done by looking at the
    window's current strucRgn (the region that describes the window's frame)
    and contRgn (the region that describes the inside content area of the
    window). The difference in size between these two regions is determined,
    and is then applied to newBounds. This gives us a rectangle that can be
    passed to MoveWindow and SizeWindow.

    Note that we first hide the window before changing its bounds. This is so
    that we don't see first the effect of MoveWindow and then the affect of
    SizeWindow.

********************************************************************************/

void SetWindowBounds(WindowPtr theWindow, Rect newBounds)
{
    short top;
    short left;
    short height;
    short width;

    short topInset;
    short leftInset;
    short bottomInset;
    short rightInset;

    Rect oldBounds;

    RgnHandle contRgn;
    RgnHandle structRgn;

    contRgn = ((WindowPeek) theWindow)->contRgn;
    structRgn = ((WindowPeek) theWindow)->strucRgn;

    oldBounds = (**structRgn).rgnBBox;

    if (!EqualRect(&oldBounds, &newBounds)) {

        topInset = (**contRgn).rgnBBox.top - (**structRgn).rgnBBox.top;
        leftInset = (**contRgn).rgnBBox.left - (**structRgn).rgnBBox.left;
        bottomInset = (**structRgn).rgnBBox.bottom - (**contRgn).rgnBBox.bottom;
        rightInset = (**structRgn).rgnBBox.right  - (**contRgn).rgnBBox.right;

        top = newBounds.top + topInset;
        left = newBounds.left + leftInset;
        height = newBounds.bottom - top - bottomInset;
        width = newBounds.right - left - rightInset;

        HideWindow(theWindow);
        MoveWindow(theWindow, left, top, FALSE);
        SizeWindow(theWindow, width, height, TRUE);
        ZoomRect(oldBounds, newBounds, kLinear);
        SelectWindow(theWindow);
        ShowWindow(theWindow);
    }
}

/********************************************************************************

    GetWindowContentRect

    Given a window pointer, return the global rectangle that encloses the
    content area of the window.

********************************************************************************/

Rect GetWindowContentRect(WindowPtr window)
{
    WindowPtr oldPort;
    Rect contentRect;

    GetPort(&oldPort);
    SetPort (window);
    contentRect = window->portRect;
    LocalToGlobalRect(&contentRect);
    SetPort(oldPort);
    return contentRect;
}

/********************************************************************************

    GetWindowStructureRect

    This procedure is used to get the rectangle that surrounds the entire
    structure of a window. This works whether or not the window visible. If
    the window is visible, it is a simple matter of using the bounding
    rectangle of the structure region. If the window is invisible, the
    strucRgn is not valid. To make it valid, the window has to be moved way
    off the screen and then made visible. This generates a valid strucRgn
    although it is valid for the position that is way off the screen. It still
    needs to be offset back into the original position. Once the bounding
    rectangle for the strucRgn is obtained, the window can then be hidden
    again and moved back to its correct location. Note that ShowHide is used,
    instead of ShowWindow and HideWindow. HideWindow can change the plane of
    the window. Also, ShowHide does not affect the highlighting of windows.

********************************************************************************/

Rect GetWindowStructureRect (WindowPtr window)
{
    const short kOffScreenLocation = 0x4000;   /* Halfway to infinity */

    GrafPtr oldPort;
    Rect structureRect;
    Point windowLoc;


    if (((WindowPeek)window)->visible)
        structureRect = (*(((WindowPeek)window)->strucRgn))->rgnBBox;
    else {
        GetPort(&oldPort);
        SetPort(window);
        windowLoc = GetGlobalTopLeft(window);
        MoveWindow(window, windowLoc.h, kOffScreenLocation, FALSE);  
        ShowHide(window, TRUE);
        structureRect = (*(((WindowPeek) window)->strucRgn))->rgnBBox;
        ShowHide (window, FALSE);
        MoveWindow(window, windowLoc.h, windowLoc.v, FALSE);
        OffsetRect (&structureRect, 0, windowLoc.v - kOffScreenLocation);
        SetPort(oldPort);
    }
    return structureRect;
}

/********************************************************************************

    GetWindowDeviceRectNMB (No Menu Bar)

    Given a window pointer, find the device that contains most of the window
    and return that device's bounding rectangle. If this device is the main
    device, remove the menu bar area from the rectangle.

********************************************************************************/

Rect GetWindowDeviceRectNMB (WindowPtr window)
{
    Rect deviceRect, tempRect;

    deviceRect = GetWindowDeviceRect(window);
    tempRect = GetMainScreenRect();
    if (EqualRect(&deviceRect, &tempRect))
        deviceRect.top += GetMBarHeight();

    return deviceRect;
}

/********************************************************************************

    GetWindowDeviceRect

    Given a window pointer, find the device that contains most of the window
    return that device's bounding rectangle.

********************************************************************************/


Rect GetWindowDeviceRect (WindowPtr window)
{
    if (gMac.hasColorQD)
        return (*GetWindowDevice(window))->gdRect;
    else
        return GetMainScreenRect();
}

/********************************************************************************

    GetWindowDevice

    Given a window pointer, find the device that contains most of the window
    and return its handle.

********************************************************************************/

GDHandle GetWindowDevice(WindowPtr window)
{
    return GetRectDevice(GetWindowStructureRect(window));
}

/********************************************************************************

    GetRectDevice

    Given rectangle in global coordinates, find the monitor it overlaps the
    most. This is done by iterating over all the monitors with the
    GetDeviceList and GetNext Device calls. For each device, we find the area
    of overlap with the given rectangle. The device that results in the
    greatest overlap is returned to the caller.

    This routine assumes the existence of the Graphics Device Manager.

********************************************************************************/

GDHandle GetRectDevice(Rect globalRect)
{

    long area;
    long maxArea;
    GDHandle device;
    GDHandle deviceToReturn;
    Rect intersection;

    deviceToReturn = GetMainDevice();    /* Use as default choice. */
    maxArea = 0;

    for (device = GetDeviceList(); device != NIL; device = GetNextDevice(device)) {
        if (TestDeviceAttribute(device, screenDevice)
           && TestDeviceAttribute(device, screenActive)
           && SectRect(&globalRect, &((*device)->gdRect), &intersection)) {
            area= (intersection.right - intersection.left) *
                   (intersection.bottom - intersection.top);
            if (area > maxArea) {
                deviceToReturn = device;
                maxArea = area;
            }
        }
    }
    return deviceToReturn;
}

/********************************************************************************

    LocalToGlobalRect

    Convert a rectangle from local coordinates to global coordinates. Like
    QuickDraw's LocalToGlobal, it assumes that the current port 1s set
    correctly.

********************************************************************************/


void LocalToGlobalRect(Rect *aRect)
{
    LocalToGlobal(&topLeft(*aRect));
    LocalToGlobal(&botRight(*aRect));
}

/********************************************************************************

    SetGlobalTopLeft

    Return the op left point of the given window's port in global
    coordinates. This returns the top loft point of the window's content area
    only; it doesn't include the window's drag region (or title bar).

********************************************************************************/


Point GetGlobalTopLeft(WindowPtr window)
{
    GrafPtr oldPort;
    Point globalPt;

    GetPort(&oldPort);
    SetPort (window);
    globalPt = topLeft(window->portRect);
    LocalToGlobal(&globalPt);
    SetPort(oldPort);
    return globalPt;
}

/********************************************************************************

    GetMainScreenRect

    Gets the bounding rectangle of the main screen (the one with tho menu bar
    on it). This rectangle includes the area that contains the menu par. For
    example, on a Mac Classic, this routine should return (0, 0, 512, 342).

********************************************************************************/


Rect GetMainScreenRect(void)
{
    GDHandle mainDevice;
    GrafPtr mainPort;

    if (gMac.hasColorQD) {
        mainDevice = GetMainDevice();
        return (*mainDevice)->gdRect;
    } else {
        GetWMgrPort(&mainPort);
        return mainPort->portRect;
    }
}

/********************************************************************************

    CatenatePStrings

    Catenate two Pascal strings by attaching the second string on the end of
    the first string, If you are running under MPW 3.2 or later, you can
    simply use the PLCatenate library routine instead.

********************************************************************************/

void CatenatePStrings(Str255 targetStr, Str255 appendStr)
{
    short appendLen;

    /* Figure out number of bytes to copy, up to 255 */

    appendLen = MIN(appendStr[0], 255 - targetStr[0]);
    if (appendLen > 0) {
        BlockMove(appendStr+1, targetStr+targetStr[0]+1, (long) appendLen); 
        targetStr[0] += appendLen;
    }
}



