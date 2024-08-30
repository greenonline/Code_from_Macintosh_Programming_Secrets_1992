/* Listing 9-3. Windoid.c */

#include "GestaltEqu.h"

/*******************************************************************************

    Function prototypes. Normally, these would be put in an .h file so that any 
    clients of this module could access the exported routines. However, this
    file contains the code for an entirely self-contained WDEF; there are no
    exported routines.

*******************************************************************************/

void DoDraw(WindowPeek theWindow, long param);
long DoHit(WindowPeek theWindow, long param); 
void DoCalcRgns(WindowPeek theWindow);

pascal void DoDrawAll(short depth, short deviceFlags, GDHandle targetDevice, WindowPeek theWindow);
pascal void ToggleGoAway(short depth, short deviceFlags, GDHandle targetDevice, WindowPeek theWindow);

void DrawGoAwayBox(WindowPeek theWindow, Boolean tracked); 

void SyncPorts(void);

void GetContentRect(WindowPeek theWindow, Rect *theRect); 
void GetStrucRect(WindowPeek theWindow, Rect *theRect); 
void GetGoAwayRect(WindowPeek theWindow, Rect *theRect);

/*******************************************************************************

    main

    Entry point for the WDEF. Note that, because the WDEF is called by the Mac 
    Toolbox, main() is declared as "pascal." This indicates that the Pascal 
    calling convention should be used.

    Our entry point doesn't do much. First, it saves the current port and
    calls SyncPorts(), which switches over to the color Window Manager port if
    we are running on a color Mac. As a courtesy to all our drawing
    routines, we then save the pen state. Next, we call a subroutine
    appropriate to the type of operation we are asked to perform. Note that we 
    support only wDraw, wHit, and wCalcRgns. We do not support wNew, wDispose, 
    wGrow, wDrawGIcon, wZoomIn, or wZoomOut. Before leaving, we restore the 
    initial Penn State and grafport.

*******************************************************************************/



pascal long main( short varCode,         // variation code of the window
                  WindowPeek theWindow,  // window to operate on 
                  short message,         // operation to perform 
                  long param)            // extra information
{
    long result;
    GrafPtr oldPort;
    PenState oldPenState;
 

    GetPort(&oldPort),
    SyncPorts(),

    GetPenState(&oldPenState),

    result = 0;
    switch (message) {
        case wDraw:
            DoDraw(theWindow, param);
            break;
        case wHit:
            result = DoHit(theWindow, param);
            break;
        case wCalcRgns: 
            DoCalcRgns(theWindow);
            break;
    }

    SetPenState(&oldPenState),

    SetPort(oldPort);
    return result;
}

/******************************************************************************* 

    DoDraw

    The function that does all of the drawing. What we draw depends on the 
    value of param, which is passed to us by the Window Manager. If param is
    zero, we draw the entire frame in its standard state. If param is
    wInGoAway, wInZoomIn, or wInZoomOut, we have to toggle the state of those
    boxes. We don't have a zoom box in our window, so we don't respond to
    wInZoomIn or wInZoomOut.

    Our drawing is done through the System 7.0 DeviceLoop() function. This
    function calls a drawing routine for each set of graphics devices that
    have different drawing characteristics. For instance, if we're running
    on a Mac with two monitors, one in i-bit mode and the other in 8-bit mode,
    our drawing routine is called once for each device. On the other hand, if
    both monitors are both in 8-bit mode and are also identical in every other
    way, our drawing routine is called only once.

*******************************************************************************/
void DoDraw(WindowPeek theWindow, long param)
{
    DeviceLoopDrawingProcPtr procToCall;

    if (theWindow->visible) {

        switch (param) { 
            case 0:
                procToCall = (DeviceLoopDrawingProcPtr) DoDrawAll;
                break;
            case wInGoAway:
                procToCall = (DeviceLoopDrawingProcPtr) ToggleGoAway;
                break;
            default:
                return;         /* Blowout on anything we don't recognize. */
        }
        DeviceLoop( theWindow->strucRgn,   /* Region we'll be drawing in. */
                procToCall,            /* Function to do the drawing. */
                (long) theWindow,      /* Data passed to our routine. */
                0);                    /* Flags - none used here. */
    }
}

/*******************************************************************************

    DoHit

    Given the point passed to us in param, determine what part of the window
    was hit. The only values we return here are wInContent, wInGoAway,
    wInDrag, and wNoHit.

    Take special note of the way we check to see if the user clicked in the
    close box or not. We only check to see if the window has its gOAwayFlag
    set or not before we check the mouse click location. In most WDEFs, you'd
    also have to check to see if the window is active or not by checking the 
    window's highlighted flag. However, we don't do so here because this WDEF is
    for floating windows. These windows have a special flavor of always
    acting and looking active. For this reason, we allow clicks on the close
    box even if the window is not the frontmost.

*******************************************************************************/

long DoHit(WindowPeek theWindow, long param)
{
    Point where; 
    Rect tempRect;

    where.v = HiWord(param) ;
    where.h = LoWord(param) ;

    if (PtInRgn(where, theWindow->contRgn)) {
        return wInContent;
    } else if (PtInRgn(where, theWindow->strucRgn)){

        if (theWindow->goAwayFlag) {
            GetGoAwayRect(theWindow, &tempRect);
            if (PtInRect(where, &tempRect)){
                return  wInGoAway;
            }
        }

        return  wInDrag;
    }
    return  wNoHit;
}

/*******************************************************************************

    DoCalcRgns

    Calculate the content and structure regions for our window. When we are
    called, the window's contRgn and strucRgn fields have already been
    initialized with handles to empty regions. All we have to do is fill in 
    the regions with the appropriate data.

    This is easy. For the content region, we just get the content rectangle
    and call QuickDraw's RectRgn() routine on it. The structure region is a
    little trickier because of the shadow. What we do is get the rectangle for
    the frame, not including the shadow. We turn that into a region and
    save it. We then take the rectangle we just used and offset it down and to
    the right a single pixel. Again, we turn that rectangle into a region. The
    final step involves merging those two regions together with a call to
    UnionRgn().

********* ••********************************************************************/




 void DoCalcRgns(WindowPeek theWindow)
{
     Rect          tempRect;
     RgnHandle     tempRgn;

    GetContentRect(theWindow, &tempRect);
    RectRgn(theWindow->contRgn, &tempRect);

    GetStrucRect(theWindow, &tempRect);
    RectRgn(theWindow->strucRgn, &tempRect);

    OffsetRect(&tempRect, 1, 1);

    tempRgn =NewRgn();
    RectRgn(tempRgn, &tempRect);
    UnionRgn(tempRgn, theWindow->strucRgn, theWindow->strucRgn); 
    DisposeRgn(tempRgn);
}

/******************************************************************************* 

    DoDrawAll

    This function is responsible for drawing the entire frame in its standard
    state. Since our window is not very complicated, there isn't much to do.

    First, we draw the outline of the window. This is quickly done with a
    FrameRect and a few MoveTo's and LineTo's.

    Next comes the tricky part: drawing the dotted pattern in the drag
    area. The problem here is that we are drawing in the Window Manager's
    port, which is in global coordinates. This means that any patterns drawn
    will be aligned to the global origin. However, when drawing the pattern in
    the drag area, we want the pattern to be aligned to the window's origin,
    not the global origin.

    Here's an example to show the problem. Assume that when we first draw the
    window, the drag area looks like this:

+---------------------------------------------------+
| * +-+ * * * * * * * * * * * * * * * * * * * * * * |
| * +-+ * * * * * * * * * * * * * * * * * * * * * * :
+---------------------------------------------------+ 
|                                                   |
|/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|




 However, the drag area of a window drawn just one pixel over to the right
 would look like this:

+---------------------------------------------------+
|* *+-+* * * * * * * * * * * * * * * * * * * * * * *|
|* *+-+* * * * * * * * * * * * * * * * * * * * * * *|
+---------------------------------------------------+
|                                                   |
|/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|

 See the difference? In the first window, the dotted pattern leaves a white
 one pixel border on the left and right side of the drag area. In the
 second window, the dots are drawn right next to the edges of the drag area.

 To take care of this problem, we have to alter our pattern based on our
 window's location. This is the same thing that QuickDraw does when it
 draws patterns in the content area of your window. Once the pattern is
 determined, we draw the drag area with a call to FillRect().

 Finally, we call a subroutine to draw the GoAway box.

 Note that if our WDEF supported appearances that differed according to the 
 highlighted state of the window, we would have to check for that here and draw
 accordingly. We can do this check by looking at the hilited field of the
 window record.

*******************************************************************************/ pascal void DoDrawAll(short depth, short deviceFlags, GDHandle targetDevice, WindowPeek theWindow)
{
    Rect tempRect;
    FontInfo fontInfo;
    Point titleLocation;
    Pattern framePattern;
    long patternSeed;


    PenNormal();
    GetStrucRect(theWindow, &tempRect);

    //
    // Draw the frame 
    //

    FrameRect(&tempRect);

    //
    // Draw the shadow 
    //



    MoveTo(tempRect.left+1, tempRect.bottom);
    LineTo(tempRect.right, tempRect.bottom);
    LineTo(tempRect.right, tempRect.top+1);

    //
    // Draw the line at bottom of title bar 
    //

    tempRect.bottom = tempRect.top + 11; 
    InsetRect(&tempRect, 1, 1); 
    MoveTo(tempRect.left, tempRect.bottom); 
    LineTo(tempRect.right, tempRect.bottom);

    //
    // Choose a pattern seed that is properly aligned horizontally. 
    //

    if (tempRect.left & 1) 
        patternSeed = 0x00550055;
    else
        patternSeed = 0x00AA00AA;

    //
    // Align the seed vertically if needed. Shifting the
    // pattern left by 8 bits moves each row of the pattern I
    // up a single row. The top row is discarded, which is OK 
    // for our purposes. The bottom row is filled with zeros, 
    // which is OK, since that matches the discarded row.
    //

    if (tempRect.top & 1) 
        patternSeed <<= 8;

    //
    // A pattern is 8 bytes long, and we've just finished creating 
    // a value that represents the first 4. Jam that 4-byte value 
    // into the full 8 bytes of our pattern.
    //

    *(long *) &framePattern[4] = *(long *) &framePattern[0] = patternSeed; 
    FillRect(&tempRect, framePattern);
    DrawGoAwayBox(theWindow, FALSE);
}


/*******************************************************************************

    ToggleGoAway

    Called when the Toolbox is tracking the mouse after the user clicks on the 
    GoAway box. Nothing fancy here: we just invert the box.

*******************************************************************************/
pascal void ToggleGoAway(short depth, short deviceFlags, GDHandle targetDevice, WindowPeek theWindow)
{
    Rect tempRect;

    GetGoAwayRect(theWindow, &tempRect);
    InsetRect(&tempRect, 1, 1); 
    InvertRect(&tempRect);
}

/*******************************************************************************
    DrawGoAwayBox

    Draw the go away box. It is up to us to check if the window actually
    has a go away box. If we supported a zoom box or grow box, we would have to  
    check for those as well in their corresponding draw routines.

    Note that we don't check to see if the window is active before
    drawing the close box. Normally, we would, as inactive windows don't draw
    their close boxes or allow clicks on them. However, this WDEF is for
    floating windows. These windows have a special flavor--they're always acting
    and looking active. For this reason, we draw the close box even if the window
    is not the frontmost.

*******************************************************************************/
void DrawGoAwayBox(WindowPeek theWindow, Boolean tracked)
{
    short index;
    Rect tempRect;

    if (theWindow->goAwayFlag) {
        PenNormal();
        GetGoAwayRect(theWindow, &tempRect);
        EraseRect(&tempRect);
        InsetRect(&tempRect, 1, 1);
        FrameRect(&tempRect);
    }
}


/*******************************************************************************

    SyncPorts

    When our WDEF is entered, the port is set to the WMgrPort. This is an
    old-fashioned grafport that allows us to draw only in black and white. If
    we want to draw in color, we have to switch over to the CWMgrPort. When
    doing so, we have to make sure that the two ports are synchronized (the
    Window Manager doesn't normally keep all of the fields of both ports in
    sync with each other--probably for performance reasons).

*******************************************************************************/ void SyncPorts()
{
    GrafPtr bwPort;
    CGrafPtr colorPort;
    long value;
    OSErr err;

    err = Gestalt(gestaltQuickdrawVersion, &value);

    if ((err == noErr) && (value >= gestalt8BitQD)){

        GetWMgrPort(&bwPort);
        GetCWMgrPort(&colorPort);
        SetPort((GrafPtr) colorPort);

        BlockMove (&bwPort->pnLoc, &colorPort->pnLoc, 10);    // pnLoc[4),
                                                              // pnSize[4),
                                                              // pnMode[2)
        BlockMove (&bwPort->pnVis, &colorPort->pnVis, 14);    // pnVis[2),
                                                              // txFont[2),
                                                              // txFace[2),
                                                              // txMode[2),
                                                              // txSize[2),
                                                              // spExtra[4)
        PenPat(bwPort->pnPat);
        BackPat(bwPort->bkPat);
   }
}

/*******************************************~*********************************** 

    GetContentRect

    Get the rectangle (in global coordinates) for the window's content area.
    We get this rectangle by starting with the window's portRect. We then
    convert the rectangle to global coordinates by temporarily setting the
    port to the window and calling LocalToGlobal on the TopLeft and
    BottomRight corners of the rectangle.

*******************************************************************************/
void GetContentRect(WindowPeek theWindow, Rect *theRect) {
    GrafPtr oldPort;

    *theRect =theWindow->port.portRect;
    GetPort(&oldPort);
    SetPort((GrafPtr) theWindow);
    LocalToGlobal((Point *) &(theRect->top));  
    LocalToGlobal((Point *) &(theRect->bottom));
    SetPort(oldPort);
}

/******************************************************************************* 

    GetStrucRect

    Get the rectangle used for the frame (structure) of our window. We base
    this rectangle on the content rectangle, so we get that first. We then
    tweak it into the right shape for the frame rectangle. Note that the
    rectangle we return does not include the window's shadow. There's no
    good reason for doing this except that it makes the drawing and region
    calculating routines a little easier to implement.

*******************************************************************************/
void GetStrucRect(WindowPeek theWindow, Rect *theRect)
{
    GetContentRect(theWindow, theRect); 
    theRect->top -= 10; 
    InsetRect(theRect, -1, -1);
}

/*******************************************************************************

    GetGoAwayRect

    Return the rectangle to be used for the go away box.

*******************************************************************************/ 
void GetGoAwayRect(WindowPeek theWindow, Rect *theRect)
{
    GetStrucRect(theWindow, theRect);
    theRect->top += 1;
    theRect->left += 7;
    theRect->bottom =theRect->top + 9;
    theRect->right = theRect->left + 9;
}

