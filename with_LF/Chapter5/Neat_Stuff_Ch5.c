/* Listing 5-2 Neat stuff.c */

#include "Neat_Stuff_Ch5.h"
#include "StdLib.h"                      // for abs()
#include "GestaltEqu.h"
#include "QDoffscreen.h"

#define NIL 0L
const short kWindowID = 128;
const short kDialogID = 130;

GDHandle pOldDevice;
CGrafPtr pOldPort;

/********************************************************************************

    DoNewWindow

    Simply creates a new window and attaches a randomly created QuickDraw
    picture to it to handle the updates.

********************************************************************************/

void DoNewWindow(void)
{
    WindowPtr roomWithAView;
    PicHandle picturePerfect;

    roomWithAView = GetNewWindow(kWindowID, NIL, (WindowPtr) -1);
    if (roomWithAView != NIL) {
        SetPort(roomWithAView);
        picturePerfect = GetRandomPicture(&(roomWithAView->portRect));
        SetWindowPic(roomWithAView, picturePerfect);
        ShowWindow(roomWithAView);
    }
}

/********************************************************************************

    GetRandomPicture

    Creates a random picture that fits within the bounds given to us. First,
    we generate a random number between 40 and 60; this is the number
    of shapes we'll stick in the picture. Next, for each shape, we generate two
    random coordinates within our limiting bounds. After that, we choose a
    random shape to create and use our two pints to create it.

********************************************************************************/

PicHandle GetRandomPicture(Rect* bounds)
{
    short width;
    short height;
    PicHandle pitcher;
    int numberOfShapes;
    int loopy;
    Point p1;
    Point p2;
    Point penSize;
    Rect randomRect;
    int shapeKind;


    qd.randSeed = TickCount();
    width = bounds->right - bounds->left;
    height = bounds->bottom - bounds->top;
    pitcher = OpenPicture(bounds);
    PenMode(patXor);
    numberOfShapes = abs (Random() % 20) + 40;   // 40 to 60 shapes
    for (loopy = 0; loopy <numberOfShapes; loopy++) {
        p1.h = abs(Random() % width) + bounds->left;
        p1.v = abs(Random() % height) + bounds->top;
        p2.h = abs(Random() % width) + bounds->left;
        p2.v = abs(Random() % height) + bounds->top;
        penSize.h = abs(Random() % 10) +1;
        penSize.v = abs(Random() % 10) +1;
        PenSize(penSize.h, penSize.v);
        Pt2Rect(p1, p2, &randomRect);
        shapeKind = abs(Random() %4);
        switch (shapeKind) {
            case 0:
                MoveTo(p1.h, p1.v);
                LineTo(p2.h, p2.v);
                break;
            case 1:
                FrameRect(&randomRect);
                break;
            case 2:
                FrameOval(&randomRect);
                break;
            case 3:
                FrameRoundRect(&randomRect, 16, 16);
                break;
        }
    }
    ClosePicture();
    return pitcher;
}

/********************************************************************************

    ShowDialog

    Shows a simple modal dialog. Nothing fancy, except that it's used to
    contrast against ShowSavingDialog, which saves the background first and
    later restores it.

********************************************************************************/

void ShowDialog(void)
{
    DialogPtr dlg;
    short item;

    dlg = GetNewDialog(kDialogID, NIL, (WindowPtr) -1);

    SetPort(dlg);
    ShowWindow(dlg);
    do {
        ModalDialog(NIL, &item);
    } while ((item != ok) && (item != cancel));
    DisposeDialog(dlg);
}

/********************************************************************************

    ShowSavingDialog

    This is the same as ShowDialog, except that it saves the background
    before showing the window, and restores the background after the
    dialog has been put away.

********************************************************************************/

void ShowSavingDialog(void)
{
    DialogPtr dlg;
    short item;
    SavedBGHandle savedData;

    dlg = GetNewDialog(kDialogID, NIL, (WindowPtr) -1);
    SaveBackground(dlg, &savedData);

    SetPort(dlg);
    ShowWindow(dlg);
    do {
        ModalDialog(NIL, &item);
    } while ((item != ok) && (item != cancel));
    DisposeDialog(dlg);

    RestoreBackground(savedData);
}

/********************************************************************************

    SaveBackground

    Given an invisible window that we'd like to show, this function determines
    what parts of our background windows it will cover, and saves those bits
    in an offscreen buffer. Information pertaining to the area of the
    saved background, as well as the saved bits themselves, are returned
    in the SaveBGHandle and passed back to the caller. This handle is
    Later passed to RestoreBackground to refresh the screen.

********************************************************************************/

void SaveBackground(WindowPtr w, SavedBGHandle* savedData)
{
    RgnHandle coveredWagon;
    Rect coveredRect;
    GrafPtr offWorld;
    GrafPtr desktopPort;

    coveredWagon = GetCoveredArea(w);
    coveredRect = (**coveredWagon).rgnBBox;
    offWorld = CreateOffWorld(coveredRect);

    UseOffWorld(offWorld);
    desktopPort = GetDesktopPort();
    CopyBits(&desktopPort->portBits, &offWorld->portBits, &coveredRect, &offWorld->portRect, srcCopy, NIL);
    DoneWithOffWorld(offWorld);

    *savedData = (SavedBGHandle) NewHandle(sizeof(SavedBGRecord));
    (***savedData).coveredArea = coveredWagon;
    (***savedData).offWorld = offWorld;
}

/********************************************************************************

    RestoreBackground

    Given the data saved in SaveBackground, restore the screen, being careful
    to alter only the contents of our windows and no one else's.

********************************************************************************/

void RestoreBackground(SavedBGHandle savedData)
{
    RgnHandle coveredWagon;
    Rect coveredRect;
    GrafPtr offWorld;
    GrafPtr desktopPort = GetDesktopPort();

    coveredRect = (**(**savedData).coveredArea).rgnBBox;
    UseOffWorld((**savedData).offWorld);
    SetPort(desktopPort);
    ClipRect(&coveredRect);
    SetDesktopDevice();
    CopyBits (&(**savedData).offWorld->portBits, &desktopPort->portBits, &(**savedData).offWorld->portRect, &coveredRect, srcCopy, (**savedData).coveredArea);
    DoneWithOffWorld((**savedData).offWorld);

    ValidateWindows((**savedData).coveredArea);
    DisposeOffWorld((**savedData).offWorld);
    DisposeRgn((**savedData).coveredArea);
    DisposeHandle((Handle)savedData);
}


/********************************************************************************

    GetDesktopPort

    If we are running under a Color QuickDraw environment, return the
    color Window Manager port. Otherwise, return the B*W Window Manager port.

********************************************************************************/

GrafPtr GetDesktopPort()
{
    OSErr err;
    long value;
    GrafPtr desktopPort;

    err = Gestalt(gestaltQuickdrawVersion, &value);

    if ((err == noErr) && (value >= gestalt8BitQD))
        GetCWMgrPort((CGrafPtr*) &desktopPort);
    else
        GetWMgrPort(&desktopPort);

    return desktopPort;
}

/********************************************************************************

    SetDesktopDevice

    If we are running on a Mac with Color QuickDraw, set the current GDevice
    to be the main device. Otherwise, do nothing since GDevices are implemented
    on this machine.

********************************************************************************/

        
void SetDesktopDevice()
{
    OSErr err;
    long value;

    err = Gestalt(gestaltQuickdrawVersion, &value);

    if ((err == noErr) && (value >= gestalt8BitQD))
        SetGDevice(GetMainDevice());
}

/********************************************************************************

    GetCoveredArea

    Given an invisible window, determine the areas of all our other windows
    that would be obscured by it. Return this area as a QuickDraw region in
    global coordinates.

    This routine starts by getting the union of the visRgns of all our
    windows. As we traverse the windows, we get the visRgns, translate
    them into global coordinates, and merge them into one big region.
    After that, we find the rectangle that bounds our invisible window,
    convert it into a region too, and intersect it with our big union
    on visRgns. This resulting region is returned to the caller.

********************************************************************************/

RgnHandle GetCoveredArea(WindowPtr w)
{
    Rect wRect;
    RgnHandle wRgn;
    RgnHandle coveredRgn;
    WindowPtr thisWindow;
    Point windowLocalTopLeft;
    Point delta;

    coveredRgn = NewRgn();
    thisWindow = FrontWindow();
    while (thisWindow != NIL) {
        windowLocalTopLeft = topLeft(thisWindow->portRect);
        delta = GetGlobalTopLeft(thisWindow);
        SubPt(windowLocalTopLeft, &delta);

        OffsetRgn(thisWindow->visRgn, delta.h, delta.v);
        UnionRgn(thisWindow->visRgn, coveredRgn, coveredRgn);
        OffsetRgn(thisWindow->visRgn, -delta.h, -delta.v);
        thisWindow = (WindowPtr) ((WindowPeek) thisWindow)->nextWindow;
    }

    wRgn = NewRgn();
    wRect = GetWindowStructureRect(w);
    RectRgn(wRgn, &wRect);
    SectRgn(wRgn, coveredRgn, coveredRgn);

    return coveredRgn;
}

/********************************************************************************

    CreateOffWorld

    Given a rectangle, create an offscreen buffer with the same bounds. If
    we are running on a Mac with the GWorld routines, use them. Otherwise,
    as long as we are running on a Classic QuickDraw machine, whip one up by
    hand. In this little example, we don't support the creation of a color
    offscreen buffer on Color QuickDraw machines that don't have the GWorld
    routines.

********************************************************************************/

GrafPtr CreateOffWorld ( Rect globalRect)
{
    OSErr err;
    long value;
    long height;
    long width;
    short depth;
    BitMap bitMap;
    GrafPtr oldPort;
    GDHandle oldDevice;
    
    GrafPtr newPortBeach;
    GWorldPtr braveNewGWorld;


    err = Gestalt(gestaltQuickdrawVersion, &value);

    if (err == noErr) {
        if (value >= gestalt32BitQD12) {

            err = NewGWorld(&braveNewGWorld,    // result
                            0,                  // pixel size (0 = find deepest)
                            &globalRect,        // bounds
                            NIL,                // color table (NIL = default)
                            NIL,                // gDevice (not used in this call)
                            0);                 // flags

           GetGWorld((CGrafPtr*) &oldPort, &oldDevice);

           UseOffWorld((GrafPtr) braveNewGWorld);
           EraseRect(&braveNewGWorld->portRect);
           DoneWithOffWorld((GrafPtr) braveNewGWorld);
           SetGWorld((CGrafPtr) oldPort, oldDevice);

           return (GrafPtr) braveNewGWorld;

       } else if (value == gestaltOriginalQD) {
           height = globalRect.bottom -globalRect.top;
           width = globalRect.right - globalRect.left;

           bitMap.rowBytes = (((width -1)/32)+1) *4;
           bitMap.bounds = globalRect;
           bitMap.baseAddr = (Ptr) NewHandle(height * bitMap.rowBytes);

           newPortBeach = (GrafPtr) NewPtr (sizeof(grafport));
//           newPortBeach = (GrafPtr) NewPtr (sizeof(oldPort));
           OpenPort(newPortBeach);
           SetPortBits(&bitMap);
           newPortBeach->portRect = globalRect;
           ClipRect(&globalRect);
           CopyRgn(newPortBeach->clipRgn, newPortBeach->visRgn);
           GetPort(&oldPort);
           SetPort(newPortBeach);
           EraseRect(&braveNewGWorld->portRect);
           SetPort(oldPort);

           return newPortBeach;

        } else {
           DebugStr("\pSorry, unsupported QuickDraw version.");
        }
    }
    return NIL;
}

/********************************************************************************

    UseOffWorld

    This is somewhat like a SetPort routine for our offscreen buffers. If
    we have created a color offscreen buffer with the GWorld routines, lock
    down our pixels and call SetGWorld to set the grafport and the GDevice.
    If we are running on a B&W machine , lock down our bits and simply
    call SetPort. In both cases, remember the current grafport and (if
    appropriate) GDevice so that we can restore them later in DoneWithOffWorld.

********************************************************************************/

void UseOffWorld (GrafPtr offWorlder)
{

    if (IsColorPort(offWorlder)) {
        GetGWorld(&pOldPort, &pOldDevice);
        (void) LockPixels(GetGWorldPixMap((GWorldPtr) offWorlder));
        SetGWorld((CGrafPtr) offWorlder, NIL);
    } else {
        GetPort((GrafPtr*) &pOldPort);
        HLock ((Handle) offWorlder->portBits.baseAddr);
        offWorlder->portBits.baseAddr = *(Handle) (offWorlder -> portBits.baseAddr);
        SetPort(offWorlder);
    }
}

/********************************************************************************

    DoneWithOffWorld

    This undoes the effects of UseOffWorld. If we are running with the GWorld
    routines, unlock our pixels and reset the grafport and GDevice to what
    they were before we called UseOffWorld. If we are running on a B&W
    machine, unlock our bits and restore the old grafport.
    
********************************************************************************/

void DoneWithOffWorld(GrafPtr offWorlder)
{
    if (IsColorPort(offWorlder)) {
        UnlockPixels(GetGWorldPixMap((GWorldPtr) offWorlder));
        SetGWorld(pOldPort, pOldDevice);
    } else {
        offWorlder->portBits.baseAddr = (Ptr) RecoverHandle(offWorlder->portBits.baseAddr);
        HUnlock((Handle) offWorlder->portBits.baseAddr);
        SetPort((GrafPtr) pOldPort);
    }
}

/********************************************************************************

    IsColorPort

    Simple utility for determining if the offscreen buffer represents color
    or B&W pixels. Since we create a color buffer only if the GWorld routines
    are available to us, we often use this utility to see if we can call
    the GWorld functions in subsequent routines.

********************************************************************************/

Boolean IsColorPort(GrafPtr offWorlder)
{
    return (offWorlder->portBits.rowBytes & 0x08000) != 0;
}

/********************************************************************************

    DisposeOffWorld

    Release the memory for our offscreen buffer. If we created the buffer
    with NewGWorld, call DisposeGWorld. If we created the buffer by hand,
    we dispose of the two blocks of data we created -- the buffer for the
    grafport and the buffer for the bits themselves.

********************************************************************************/

void DisposeOffWorld(GrafPtr offWorlder)
{
    if (IsColorPort(offWorlder)) {
        DisposeGWorld((GWorldPtr) offWorlder);
    } else {
    DisposeHandle((Handle) offWorlder->portBits.baseAddr);
        ClosePort(offWorlder);
        DisposePtr((Ptr) offWorlder);
    }
}

/********************************************************************************

    ValidateWindows

    Given a region in global coordinates, validate the parts of all the
    windows it intersects. To do this, we traverse all open windows, starting
    with FrontWindow(). For each window, we make a temporary copy of glovalRgn,
    translate that copy into the window's local coordinate system, and then
    call ValidRgn with that copy.

********************************************************************************/
void ValidateWindows(RgnHandle globalRgn)
{
    GrafPtr oldPort;
    RgnHandle tempRgn;
    WindowPtr thisWindow;
    Point windowLocalTopLeft;
    Point delta;

   
    GetPort(&oldPort);
    tempRgn = NewRgn();
    thisWindow = FrontWindow();
    while (thisWindow != NIL) {
        SetPort(thisWindow);
        windowLocalTopLeft = topLeft(thisWindow->portRect);
        delta = GetGlobalTopLeft(thisWindow);
        LocalToGlobal(&delta);
        SubPt(windowLocalTopLeft, &delta);

        CopyRgn(globalRgn, tempRgn);
        OffsetRgn(globalRgn, -delta.h, -delta.v);
        SectRgn(thisWindow->visRgn, tempRgn, tempRgn);
        ValidRgn(tempRgn);
        thisWindow = (WindowPtr) ((WindowPeek) thisWindow)->nextWindow;
    }
    DisposeRgn(tempRgn);
    SetPort(oldPort);
}

/********************************************************************************

    GetWindowStructRect

    This procedure is used to get the rectangle that surrounds the entire
    structure of a window. This works whether or not the window is visible. If
    the window is visible, it is a simple matter of using the bounding
    rectangle of the structure region. If the window is invisible, the
    strucRgn is not valid. To make it valid, the window has to be moved way
    off the screen and then made visible. This generates a valid strucRgn,
    although it is valid for the position that is way off the screen. It still
    needs to be offset back into the original position. Once the bounding
    rectangle for the strucRgn is obtained, the window can then be hidden
    again and moved back to the correct location. Note that ShowHide is used,
    instead of ShowWindow and HideWindow. HideWindow can change the plane of
    the window. Also, ShowHide does not affect the highlighting of windows.

********************************************************************************/

Rect GetWindowStructureRect(WindowPtr window)
{
    const short kOffscreenLocation;    /* Halfway to infinity */

    GrafPtr oldPort;
    Rect structureRect;
    Point windowLoc;

    if (((WindowPeek)window) -> visible)
        structureRect = (*(((WindowPeek)window)->strucRgn))->rgnBBox;
    else {
        GetPort(&oldPort);
        SetPort(window);
        windowLoc = GetGlobalTopLeft(window);
        MoveWindow(window, windowLoc.h, kOffscreenLocation, FALSE);
        ShowHide(window, TRUE);
        structureRect = (*(((WindowPeek)window)->strucRgn))->rgnBBox;
        ShowHide(window, FALSE);
        MoveWindow(window, windowLoc.h, windowLoc.v, FALSE);
        OffsetRect(&structureRect, 0, windowLoc.v - kOffscreenLocation);
        SetPort(oldPort);
    }
    return structureRect;
}


/********************************************************************************

    GetGlobalTopLeft

    Return the top left point of the given window's port in global
    coordinates. This returns the top left point of the window's content area
    only; it doesn't include the window's drag region (or title bar)..

********************************************************************************/
Point GetGlobalTopLeft(WindowPtr window)
{
    GrafPtr oldPort;
    Point globalPt;

    GetPort(&oldPort);
    SetPort(window);
    globalPt = topLeft(window->portRect);
    LocalToGlobal(&globalPt);
    SetPort(oldPort);
    return globalPt;
}


