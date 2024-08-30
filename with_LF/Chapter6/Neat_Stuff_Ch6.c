/* Listing 6-2 Neat_Stuff.c */

#include "Standard_Stuff.h"
#include "Neat_Stuff.h"
#include "MouseTracker.h"

const RGBColor kRGBBlack = {0, 0, 0};
const RGBColor kRGBWhite = {0xFFFF, 0xFFFF, 0xFFFF};

Pattern gAntsPattern = {0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 0xE3, 0xC7, 0x8F};
long gLastAntMarch;
const short kMarchTime = 1;


/********************************************************************************

    DoMarchingAnts

    This routine is used to do the actual marching of the ants (animate the
    selection rectangle). It should be called periodically, as in the main
    event loop of the application.

    The routine first makes a few checks. It makes sure that we have the
    animation turned on, that we have a front window, that an adequate time
    has passed since the last time we animated the selection, and that there
    is indeed a selection rectangle for the window. If all those conditions
    hold, we animate the selection rectangle. This is done by shifting all of
    the horizontal lines of the pattern downward by a single line. The line
    at the bottom is moved to the top of the pattern. This gives the following
    transformation:

                    Start               Result
                    ========            ========
                    11111000 ---\       01111100 <--+
                    11110001     \----> 11111000    |
                    11100011 ---\       11110001    |
                    11000111     \----> 11100011    |
                    10001111 ---\       11000111    |
                    00011111     \----> 10001111    |
                    00111111 ---\       00011111    |
                    01111111 -\  \----> 00111111    |
                               \                    |
                                \-------------------+

    Once we have the new pattern, we redraw the selection rectangle with it.     

********************************************************************************/


void DoMarchingAnts(void)
{
  MyWindowPtr theWindow;
  Rect        antsRect;
  short       i;
  unsigned char lastPart;

  theWindow = (MyWindowPtr) FrontWindow();
  if (theWindow && theWindow->antsOnTheMarch && (TickCount() > gLastAntMarch + kMarchTime)){
    antsRect = theWindow->selectionRect;
    if (!EmptyRect(&antsRect)){
        lastPart = gAntsPattern[7];
        for (i = 7; i > 0; --i)
            gAntsPattern[i] = gAntsPattern[i-1];
        gAntsPattern[0] = lastPart;
        PenNormal();
        PenPat(gAntsPattern);
        SetPort((GrafPtr) theWindow);
        FrameRect(&antsRect);
        gLastAntMarch = TickCount();
    }
  }
}

/********************************************************************************

    KillAnts

    Turn the selection rectangle animation off. We would want to do this, for
    example, if the application is put into the background.

********************************************************************************/

void KillAnts(MyWindowPtr whichWindow)
{
    Rect theRect;

    whichWindow->antsOnTheMarch = FALSE;
    theRect = whichWindow->selectionRect;
    RefreshWindow(whichWindow, &theRect);
}

/********************************************************************************

    StartAnts

    Turn the ants on. We would want to call this routine whenever our
    application moves from the background into the foreground.

********************************************************************************/

void StartAnts(MyWindowPtr whichWindow)
{
    whichWindow->antsOnTheMarch = TRUE;
}

/********************************************************************************

    DoOpenWindow

    Open a MacPaint file and show it in a window. Call Standard File to let
    the user select a MacPaint file (such files have the file type of 'PNTG').
    If the user doesn't select Cancel, we create a window for the picture,
    read the file into an offscreen grafport, attach the offscreen buffer to
    the window, and finally show the window. 

********************************************************************************/

void DoOpenWindow(void)
{
    MyWindowPtr newWindow;
    SFTypeList types = {'PNTG'};
    StandardFileReply reply;

    StandardGetFile ((FileFilterProcPtr) NIL, 1, types, &reply);
    if (reply.sfGood){
        newWindow = CreateBufferedWindow(72*8, 720, 1);
        if (newWindow != NIL) {
            ReadMacPaint(&reply.sfFile, newWindow->offWorld);
            ShowWindow((WindowPtr) newWindow);
        }
    }
}

/********************************************************************************

    DoCloseWindow

    Called when the user selects the close menu item or clicks on the GoAway
    box. Dispose of the offscreen grafport used to buffer the picture, and 
    then closes the window and dispose of the data structures.

********************************************************************************/

void DoCloseWindow(WindowPtr theWindow)
{
    DisposeGWorld(((MyWindowPtr) theWindow)-> offWorld);
    DisposeWindow(theWindow);
}

/********************************************************************************

    DoDrawGrowIcon

    Draw the grow icon. We do this in such a way that the sroll bar lines are
    not drawn. Normally, when the Toolbox routine DrawGrowIcon is called,
    vertical and horizontal liens are also drawn indicating where the 
    scroll bars will be drawn. We don't have scroll bars, so we don't want those
    lines drawn. We avoid them by setting the clipping region of the window to
    include the grow box only. We then call DrawGrowIcon, and restore the old
    clipping region before returning. 

********************************************************************************/

void DoDrawGrowIcon(WindowPtr theWindow)
{
    Rect iconRect;
    RgnHandle oldClip;

    SetPort(theWindow);
    oldClip = NewRgn();
    GetClip(oldClip);

    iconRect = theWindow->portRect;
    iconRect.top = iconRect.bottom - 15;
    iconRect.left = iconRect.right - 15;
    ClipRect(&iconRect);

    PenNormal();
    DrawGrowIcon(theWindow);

    SetClip(oldClip);
    DisposeRgn(oldClip);
}

/********************************************************************************

    DoActivateWindow

    Take care of a window that needs to be activated and deactivated. Normally,
    this means removing the highlighting used for the current selection. Since
    we do highlight the current selection with Marching Ants, we need to turn
    them on and off. 

********************************************************************************/

void DoActivateWindow(WindowPtr theWindow, Boolean activating)
{
    if (activating)
        StartAnts((MyWindowPtr) theWindow);
    else
        KillAnts((MyWindowPtr) theWindow);
}

/********************************************************************************

    DoUpdateWindow

    Called to update any area of the window that may have bene clobbered by
    other windows. We get the bounding rectangle of the update region (which
    has been incorporated into the visRgn of the window by a previous call to
    BeginUpdate), and pass that into a RefreshWindow routine that does the
    actual drawing. 

********************************************************************************/

void DoUpdateWindow(EventRecord *theEvent)
{
    MyWindowPtr theWindow;
    Rect rectToCopy;

    theWindow = (MyWindowPtr) theEvent->message;
    rectToCopy = (**(theWindow->windowRecord.port.visRgn)).rgnBBox;

    RefreshWindow(theWindow, &rectToCopy);
}

/********************************************************************************

    DoContentClick

    Called when the user clicks in the content area of our window.  Clicks on
    the grow box are detected and handled elsewhere, so we don't have to worry
    about them.

    See if the user clicked within the current selection rectangle. If so,
    drag the contents of that rectangle around the window. If not, get rid of
    the old selection and drag out a new one.

********************************************************************************/

Rect DoContentClick(EventRecord *theEvent, WindowPtr whichWindow)
{
    Point location;
    Rect newSelection;

    location = theEvent->where;
    GlobalToLocal(&location);
    if (PointInSelection(whichWindow, location)){
        DragSelection(whichWindow, location);
    } else {
        KillAnts((MyWindowPtr) whichWindow);
        newSelection = SketchNewRect(FALSE);
        ((MyWindowPtr) whichWindow)->selectionRect = newSelection;
        StartAnts((MyWindowPtr)whichWindow);
    }
}

/********************************************************************************

    PointInSelection

    Utility used to determine if a given point (in window's local
    coordinates) is within the window's selection rectangle. This routine is
    called from DoContentClick. 

********************************************************************************/
Boolean PointInSelection(WindowPtr whichWindow, Point location)
{
    Rect selectionRect;

    selectionRect = ((MyWindowPtr)whichWindow)->selectionRect;
    return PtInRect(location, &selectionRect);
}

/********************************************************************************

    RefreshWindow

    Workhorse that copies a section of the offscreen buffer to the window.
    This routine is called from a couple of places, including the update
    routines, and Marching Ant routines.

    The pointer to our offscreen buffer is stored after the normal window 
    record. We retrieve it, get a reference to its PixMap, and lock the bits
    so that they don't move during the call to CopyBits. We then make sure
    that the foreground color is set to black, and that the background color
    is set to white. If they are set to any other values, then "colorization"
    occurs according to Inside Macintosh, Volume V, page 71 and Technical Note #163.

********************************************************************************/

void RefreshWindow(MyWindowPtr theWindow, Rect *theRect)
{
    GrafPtr oldPort;
    Rect rectToCopy;
    GWorldPtr offWorld;
    PixMapHandle offPixMap;

    GetPort(&oldPort);
    SetPort((GrafPtr) theWindow);

    offWorld = theWindow->offWorld;
    offPixMap = GetGWorldPixMap(offWorld);
    SectRect(theRect, &(**offPixMap).bounds, &rectToCopy);

    (void) LockPixels(offPixMap);
    RGBForeColor(&kRGBBlack);
    RGBBackColor(&kRGBWhite);

    CopyBits(&(( (GrafPtr) offWorld)->portBits), &theWindow->windowRecord.port.portBits, &rectToCopy, &rectToCopy, srcCopy, NIL);

    UnlockPixels(offPixMap);
    SetPort(oldPort);
}

/********************************************************************************

    CreateBufferedWindow

    Creates a window that's backed up by an offscreen buffer. We do this by
    first defining an extended window record. In the same way that a window
    record contains an embedded grafport, we define a record with an embedded
    WindowRecord. This record includes a pointer to an offscreen buffer and a 
    rectangle for keeping track of the current selection. 

    We start by creating an offscreen buffer using NewGWorld (available with
    32-bit Color QuickDraw under 6.0.x, and built into 7.0). If that succeeds,
    we clear out the buffer with a call to EraseRect, and then start to create
    the window. dynamically allocate the extended window record with NewPtr
    and pass the result to GetNewWindow. If we successfully create the window,
    we copy in the pointer to the offscreen buffer and initialise the
    selection rectangle.

********************************************************************************/

MyWindowPtr CreateBufferedWindow(short width, short height, short depth)
{
    Rect boundsRect;
    CTabHandle aColorTable = NIL;
    GDHandle aGDevice = NIL;
    GWorldFlags flags = 0;
   
    MyWindowPtr newWindow;
    GWorldPtr offscreenWorld;
    CGrafPtr oldPort;
    GDHandle oldDevice;

    SetRect(&boundsRect, 0, 0, width, height);

    if (NewGWorld(&offscreenWorld, depth, &boundsRect, aColorTable, aGDevice, flags) != noErr)
        return NIL;

    GetGWorld (&oldPort, &oldDevice);
    SetGWorld(offscreenWorld, NIL);
    EraseRect(&qd.thePort->portRect);
    SetGWorld(oldPort, oldDevice);

    newWindow = (MyWindowPtr) NewPtr(sizeof(MyWindowRecord));
    if (newWindow != NIL)
        newWindow = (MyWindowPtr) GetNewWindow(kNewWindowID, (Ptr) newWindow, (WindowPtr) -1);

    if (newWindow == NIL) {
        DisposeGWorld(offscreenWorld);
    } else {
        newWindow->offWorld = offscreenWorld;
        newWindow->selectionRect.top = 10;
        newWindow->selectionRect.left = 10;
        newWindow->selectionRect.bottom = 90;
        newWindow->selectionRect.right = 90;
        newWindow->antsOnTheMarch = FALSE;
    }
    return(newWindow);
}

/********************************************************************************

    ReadMacPaint

    Read in a MacPaint file according to Technical Note #86. We open the file,
    ship. over the first 512 bytes, and read the rest of the file into a buffer.
    This buffer now contains the packed MacPaint image. By making successive
    calls to UnpackBits, we unpack the image into our offscreen buffer.

********************************************************************************/

void ReadMacPaint(FSSpec* theFile, GWorldPtr buffer)
{
    short refNum;
    long fileSize;
    short scanline;
    Ptr sourceBuffer;
    Ptr sourcePtr;
    Ptr destPtr;
    short rowBytes;
    char mmuMode;

    if (FSpOpenDF(theFile, fsRdPerm, &refNum) != noErr)
        return;

    if (GetEOF(refNum, &fileSize) != noErr) {
        FSClose(refNum);
        return;
    }

    if (SetFPos(refNum, fsFromStart, 512) != noErr) {
        FSClose(refNum);
        return;
    }

    fileSize -= 512;
    sourceBuffer = NewPtr(fileSize);
    if (sourceBuffer == NIL) {

        FSClose(refNum);
        return;
    }

    if (FSRead(refNum, &fileSize, sourceBuffer) != noErr) {
        DisposePtr(sourceBuffer);
        FSClose(refNum);
        return;
    }
    FSClose(refNum);

    sourcePtr = sourceBuffer;
    destPtr = GetPixBaseAddr(GetGWorldPixMap(buffer));
    rowBytes = (**(GetGWorldPixMap(buffer))).rowBytes & 0x7FFF;

    //mmuMode = TRUE32b;
    mmuMode = true32b;
    SwapMMUMode(&mmuMode);
    for(scanline=0; scanline < 720; ++scanline) {
        UnpackBits(&sourcePtr, &destPtr, 72);
        destPtr += (rowBytes - 72);
    }
    SwapMMUMode(&mmuMode);

    DisposePtr(sourceBuffer);
}


/********************************************************************************

    Drag variables--these are used by the routine that drags around the
    currently selected part of the picture.

********************************************************************************/


Point       startPoint;     /* The point where the user mouse-downed.        */
Rect        startRect;      /* The initial selection rectangle.              */
Rect        previousRect;   /* The last location of dragged selection.       */
Rect        currentRect;    /* The current location fo the same.             */
GWorldPtr   sourceBuffer;   /* Buffer that holds the original image.         */
GWorldPtr   destBuffer;     /* Buffer holding the modified image.            */
WindowPtr   gTheWindow;     /* Reference to the window we are munging.       */




/********************************************************************************

    DragSelection

    Called to drag the current selection. This function is really just 
    a front end to the real workhorse, TrackMouse(). It sets up some variable
    that we'll need later, and then calls TrackMouse(). TrackMouse() tracks
    the mouse, and calls a callback routine we passed to it when needed. This
    callback merely has to take care of drawing the selected image in the 
    right place. When TrackMouse() returns, we make note of the final location
    of the dragged image, dispose of our temporary buffer, and return.

********************************************************************************/

void DragSelection(WindowPtr whichWindow, Point mouse)
{
    Point dummyStart;
    Point dummyEnd;

    gTheWindow = whichWindow;
    startPoint = mouse;
    startRect = ((MyWindowPtr) whichWindow)->selectionRect;
    previousRect = startRect;
    destBuffer = ((MyWindowPtr) whichWindow)->offWorld;
    sourceBuffer = CloneGWorld(destBuffer);

    if (sourceBuffer != NIL) {
        TrackMouse(nil,  DragSelectionFeedback, nil, &dummyStart, &dummyEnd);
        ((MyWindowPtr) whichWindow) -> selectionRect = currentRect;
        DisposeGWorld(sourceBuffer);
    }
}



/********************************************************************************

    DragSelectionFeedback

    This routine is called by TrackMouse() to provide dragging feedback. Here,
    we check to see if the mouse has moved. If so, we calculate where the
    selection rectangle should be given the current location of the mouse. If
    the new selection rectangle is different from the previous rectangle, we
    update our working buffer from information stored in our original,
    pristine ("I'm not that pristine!"--Molly Ringwald) buffer.

********************************************************************************/

void DragSelectionFeedback(Point anchorPoint,
                           Point nextPoint,
                           Boolean turnItOn,
                           Boolean mouseDidMove)
{
#pragma unused (turnItOn)
    Rect tempRect;
    
    if (mouseDidMove) {
        PointToCurrentRect(nextPoint);
        if (!EqualRect(&currentRect, &previousRect)) {
            UpdateBuffers();
            UnionRect(&previousRect, &currentRect, &tempRect);
            RefreshWindow((MyWindowPtr) gTheWindow, &tempRect);
            previousRect = currentRect;
        }
    }
}




/********************************************************************************

    PointToCurrentRect

    This function is used to figure out the where the selected image should be
    positioned. When the mouse down occurred, we noted the location of the 
    mouse click and the original location of the selection rectangle. Using
    the current location of the mouse, we figure out the new location of the 
    selection rectangle. This is done by first figuring out the offset of the 
    current mouse position to the starting position. We then offset the
    selection rectangle by the same amount. Once that is done, we make sure
    that it's pinned to the window's bounds.

********************************************************************************/

void PointToCurrentRect(Point currentPoint)
{
    Point delta;
    Rect portRect;

    delta = currentPoint;
    SubPt(startPoint, &delta);
    currentRect = startRect;
    portRect = gTheWindow->portRect;
    OffsetRect(&currentRect, delta.h, delta.v);

    SetPt(&delta, 0, 0);
    if (currentRect.left < portRect.left)
        delta.h = portRect.left - currentRect.left;

    if (currentRect.right < portRect.right)
        delta.h = portRect.right - currentRect.right;

    if (currentRect.top < portRect.top)
        delta.v = portRect.top - currentRect.top;

    if (currentRect.bottom < portRect.bottom)
        delta.v =  portRect.bottom - currentRect.bottom;

    OffsetRect(&currentRect, delta.h, delta.v);
}

/********************************************************************************

    UpdateBuffers

    During our dragging, we maintain two offscreen buffers. The first holds
    the picture as it was before we started dragging. The second holds the
    image as we would like it to appear in the window. This second buffer is
    created from the first one. When we first start dragging, the second 
    buffer is identical to the first. When we need to show the image being
    dragged around, we perform the following three stepsL:

        1. Using the first buffer, restore the background int he second
           buffer where the dragged image used to be. This location is
           stored in previousRect.
        2. Erase the rectangle that shows where the image started from.
           This location is stored in startRect.
        3. Using the first buffer, draw the dragged image where it should
           currently be shown. This location is stored in currentRect.

********************************************************************************/
void UpdateBuffers(void)
{
    PixMapHandle sourceMap;
    PixMapHandle destMap;

    CGrafPtr oldPort;
    GDHandle oldDevice;

    sourceMap = GetGWorldPixMap(sourceBuffer);
    destMap = GetGWorldPixMap(destBuffer);

    (void) LockPixels(sourceMap);
    (void) LockPixels(destMap);


    GetGWorld(&oldPort, &oldDevice);
    SetGWorld (destBuffer, NIL);

    RGBForeColor(&kRGBBlack);
    RGBBackColor(&kRGBWhite);

    CopyBits(&(((GrafPtr) sourceBuffer)->portBits), &(((GrafPtr) destBuffer)->portBits), &previousRect, &previousRect, srcCopy, NIL);

    EraseRect(&startRect);

    CopyBits(&(((GrafPtr) sourceBuffer)->portBits), &(((GrafPtr) destBuffer)->portBits), &startRect, &currentRect, srcCopy, NIL);

    SetGWorld (oldPort, oldDevice);

    UnlockPixels(sourceMap);
    UnlockPixels(destMap);
}

/********************************************************************************

    CloneGWorld

    Given a GWorldPtr, clone it entirely and return the pointer to the new
    GWorld. This routine is called when we still dragging and we need to 
    manipulate two buffers that are initially identical.

********************************************************************************/

GWorldPtr CloneGWorld(GWorldPtr sourceGWorld)
{
    GDHandle aGDevice;
    short depth;
    CTabHandle aColorTable;
    GWorldFlags flags;
    Rect bounds;
    GWorldPtr newGWorld;

    PixMapHandle sourceMap;
    PixMapHandle destMap;

    CGrafPtr oldPort;
    GDHandle oldDevice;

    aGDevice = GetGWorldDevice(sourceGWorld);
    depth = (**(**aGDevice).gdPMap).pixelSize;
    aColorTable = (**(**aGDevice).gdPMap).pmTable;
    HandToHand((Handle*) &aColorTable);
    flags=(**aGDevice).gdFlags;
    bounds = (**aGDevice).gdRect;

    if (NewGWorld(&newGWorld, depth, &bounds, aColorTable, aGDevice, flags) != noErr)
        return NIL;

    sourceMap = GetGWorldPixMap(sourceGWorld);
    destMap = GetGWorldPixMap(newGWorld);

    (void) LockPixels(sourceMap);
    (void) LockPixels(destMap);


    GetGWorld(&oldPort, &oldDevice);
    SetGWorld (newGWorld, NIL);

    RGBForeColor(&kRGBBlack);
    RGBBackColor(&kRGBWhite);

    CopyBits(&(((GrafPtr) sourceGWorld)->portBits), &(((GrafPtr) newGWorld)->portBits), &bounds, &bounds, srcCopy, NIL);

    SetGWorld (oldPort, oldDevice);

    UnlockPixels(sourceMap);
    UnlockPixels(destMap);

    return newGWorld;
}
  

