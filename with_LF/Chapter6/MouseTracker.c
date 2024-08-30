/* Mouse Tracker.h Listing 6-3 */

#include "MouseTracker.h"

// Private global used for tracking. When FALSE, the mouse down location
// is used as one of the corners or endpoints of whatever we draw. If
// set to TRUE, the mouse down location is used as the center of what we draw.

Boolean pFromCenter;

/********************************************************************************

    SketchNewRect

    Called when the mouse is clicked in the context area of the window. Using
    The generic TrackMouse routine, sketches the outline of a rectangle.
    Returns the resulting rectangle.

********************************************************************************/
Rect SketchNewRect(Boolean fromCenter)
{
    Point startPoint;
    Point endPoint;
    Rect tempRect;

    pFromCenter = fromCenter;
    TrackMouse(nil, RectFeedback, nil, &startPoint, &endPoint);
    AdjustForTrackingFromCenter(&startPoint, endPoint);
    Pt2Rect(startPoint, endPoint, &tempRect);
    return tempRect;
}

void RectFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove)
{
   Rect tempRect;

   if(mouseDidMove){
        AdjustForTrackingFromCenter(&anchorPoint, currentPoint);
        Pt2Rect(anchorPoint, currentPoint, &tempRect);
        FrameRect(&tempRect);
    }
}

/********************************************************************************

    SketchNewLine

    Called when the mouse is clicked in the content area of the window. Using
    the generic TrackMouse routine, sketches a line. Returns a rectangle
    indicating the endpoints of the line: the TopLeft of the rectangle is one
    endpoint, the BottomRight is the other.

********************************************************************************/

Rect SketchNewLine(Boolean fromCenter)
{
    Point startPoint;
    Point endPoint;
    Rect tempRect;

    pFromCenter = fromCenter;
    TrackMouse(nil, LineFeedback, nil, &startPoint, &endPoint);
    AdjustForTrackingFromCenter(&startPoint, endPoint);
    topLeft(tempRect) = startPoint;
    botRight (tempRect) = endPoint;
    return tempRect;
}

void LineFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove)
{
   Rect tempRect;

   if(mouseDidMove){
        AdjustForTrackingFromCenter(&anchorPoint, currentPoint);
        MoveTo(anchorPoint.h, anchorPoint.v);
        LineTo(currentPoint.h, currentPoint.v);
    }
}

/********************************************************************************

    SketchNewOval

    Called when the mouse is clicked in the content area of the window. Using
    the generic TrackMouse routine, sketches the outline of an oval. Returns
    the bounding rectangle of the resulting oval.

********************************************************************************/
Rect SketchNewOval(Boolean fromCenter)
{
    Point startPoint;
    Point endPoint;
    Rect tempRect;

    pFromCenter = fromCenter;
    TrackMouse(nil, OvalFeedback, nil, &startPoint, &endPoint);
    AdjustForTrackingFromCenter(&startPoint, endPoint);
    Pt2Rect(startPoint,  endPoint, &tempRect);
    return tempRect;
}

void OvalFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove)
{
    Rect tempRect;

    if(mouseDidMove){
        AdjustForTrackingFromCenter(&anchorPoint, currentPoint);
        Pt2Rect(anchorPoint, currentPoint, &tempRect);
        FrameOval(&tempRect);
    }
}


/********************************************************************************

    SketchNewRoundRect

    Called when the mouse is clicked in the content area of the window. Using
    the generic TrackMouse routine, sketches the outline of an roundRect. Returns
    the bounding rectangle of the resulting roundRect.

********************************************************************************/
Rect SketchNewRoundRect(Boolean fromCenter)
{
    Point startPoint;
    Point endPoint;
    Rect tempRect;

    pFromCenter = fromCenter;
    TrackMouse(nil, RoundRectFeedback, nil, &startPoint, &endPoint);
    AdjustForTrackingFromCenter(&startPoint, endPoint);
    Pt2Rect(startPoint,  endPoint, &tempRect);
    return tempRect;
}

void RoundRectFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove)
{
    Rect tempRect;

    if(mouseDidMove){
        AdjustForTrackingFromCenter(&anchorPoint, currentPoint);
        Pt2Rect(anchorPoint, currentPoint, &tempRect);
        FrameOval(&tempRect);
    }
}


/********************************************************************************

    Macros that are used in the generic TrackMouse routine. We use macros to
    help make the code clearer.

    When TrackMouse is called, it is passed the addresses of three callback
    routines. These routines are called in the main loop of TrackMouse.
    However, the pointers can also be NIL, indicating that there is no
    corresponding routine to be called, These macros check to see if the
    callback address is NIL or not. If not, the callback is called back.

    In the case of the feedback callback routine, we also set up the 
    appropriate drawing environment.

********************************************************************************/

#define CONSTRAINONCE() \
        if (constrainProc) \
            (*constrainProc)(*anchorPoint, *endPoint, &theMouse);

#define FEEDBACKONCE(turnItOn, mouseMoved) \
        if (feedbackProc) { \
            PenNormal(); \
            PenPat (qd.gray); \
            PenMode(patXor); \
            (*feedbackProc)(*anchorPoint, *endPoint, turnItOn, mouseMoved); \
}

#define TRACKONCE(phase, mouseMoved) \
        if (trackMouseProc) \
            (*trackMouseProc)(phase anchorPoint, endPoint, &theMouse, mouseMoved);

/********************************************************************************

    TrackMouse

    Generic mouse tracking routine. This is a stripped-down version of MacApp's
    own core tracking routine. The main difference in our version is that we
    don't support automatic scrolling of the window if the mouse moves beyond
    its bounds.

    TrackMouse is called with the addresses of three callback routines. The
    first callback is the Constrain callback. This function is called to
    perform any constraining or gridding of the mouse location. It is called
    with the current location of the mouse, and is expected to return a
    modified location. Note that the cursor itself is NOT constrained (i.e.,
    the mouse doesn't jump from grid point to grid point as it does when
    moving a HyperCard window horizontally).

    The second callback routine is the FeedBack routine. This routine is
    called to perform any drawing that indicates to the user that something
    important is going on. For instance, if you wanted to sketch a rectangle
    in the window the way you can in the Finder, you would provide a feedback
    routine that would draw the rectangle. Note that this routine is called
    both to draw the feedback and to erase it later.

    The third callback is the Track routine. This callback is used to perform
    any other processing that needs to be done in the main tracking loop. For
    instance, something like the Finder could use this routine to highlight
    any icons that are within the currently sketched rectangle.

    The basic algorithm of the routine is this:

        1. The initial mouse location(the anchor point) is determined
           and constrained
        2. The track routine is called with a message indicating that
           tracking is about to start.
        3. The FeedBack routine is called to draw the initial feedback.
        4. The next mouse location is determined and constrained.
        5. We determine if the mouse moved since the last time we
           looked at it. A Boolean indicating whether or not the
           mouse moved will be passed to the Track and FeedBack routines
           to help them decide what to do.
        6. Delay a single tick. This is to give the last-drawn feedback
           a chance to show on the screen. 
        7. Call the FeedBack routine to erase the previous feedback.
        8. Call the Track routine to allow it to do any other processing.
        9. Call the FeedBack routine again to draw the new feedback
       10. Repeat steps 4-9 until the mouse button is lifted.
       11. Call the FeedBack routine to erase the final feedback.
       12. Call the Track routine, telling it that tracking is complete.

********************************************************************************/
void TrackMouse(ConstrainProcPtr constrainProc,
                FeedbackProcPtr feedbackProc,
                TrackMouseProcPtr trackMouseProc,
                Point *anchorPoint,
                Point *endPoint)
{
    Point theMouse;
    Boolean didMove;
    Rect finalRect;
    long dummy;

    GetMouse(&theMouse);
    *anchorPoint = theMouse;
    *endPoint = theMouse;
    CONSTRAINONCE();

    TRACKONCE(trackPress, kMouseMoved);
    FEEDBACKONCE(kTurnItOn, kMouseMoved);

    while (StillDown()) {
        GetMouse(&theMouse);
        CONSTRAINONCE();
        didMove = !EqualPt(*endPoint, theMouse);
        FEEDBACKONCE(kTurnItOff, didMove);
        TRACKONCE(trackPress, didMove);
        *endPoint = theMouse;
        FEEDBACKONCE(kTurnItOn, didMove);
    }

    FEEDBACKONCE(kTurnItOff, didMove);
    TRACKONCE(trackPress, didMove);
}

/********************************************************************************

    AdjustForTrackingFromCenter

    Handy routines that adjusts the anchor point if we want track-from-center
    tracking. Tracking from the center means that we would lie the initial
    mouse-down location to be at the center of whatever shape we are sketching.
    For example, if we had a routine that sketched out a circle origin and the
    current mouse location determine its radius.

    This routine will take the initial and final mouse locations, and convert
    the initial mouse location such that the two points will describe the
    bounding box of the circle, roundRect, or whatever.

    +....................+
    .(p2)                .  Here's a picture to illustrate what we're
    .                    .  doing. Assume the initial mouse click is
    .                    .  at p0 and that the user drags the mouse to
    .                    .  p1. The anchor and final points now describe
    .         +----------+  the dark bordered rectangle. However, we want
    .         | (p0)     |  p0 to be the enter of the rectangle. Calling
    .         |          |  AdjustForTrackingFromCenter will convert
    .         |          |  p0 into p2. Now our two points describe the
    .         |          |  larger rectangle.
    +.........+----------+
                          (p1)

********************************************************************************/
void AdjustForTrackingFromCenter(Point *anchorPoint, Point endPoint)
{
    if (pFromCenter) {
        anchorPoint->v -= (endPoint.v - anchorPoint->v);
        anchorPoint->h -= (endPoint.h - anchorPoint->h);
    }
}


        
