/* Listing 7-4 ZoomRect.c */

#include "ZoomRect.h"
#include "Math.h"      // for pow().
/********************************************************************************

    ZoomRect

    Draws a series of rectangles that give the appearance of a source rectangle
    being transformed into a destination rectangle. Drawing takes place in a
    grafport that covers all monitors.

    The basic idea i8 this: a transformation in progress can be said to be
    partially complete. The amount by which the transformation is complete can
    be expressed as a percentage. Given this percentage, we can calculate the
    rectangle that needs to be drawn. for example, if we were animating the
    transformation of rectangle (0, 0, 32, 32) into (40, 4, 300, 500),
    then the rectangle drawn at the 501 point would be (20, 2, 166, 266)

    Using mathematical formulas determined by the node parameter, 12
    intermediate percentages are determined, and the rectangles corresponding
    to these percentages are generated. These 12 rectangles are then drawn
    four at a time. First, rectangles 1, 2, 3, and 4 are drawn. Then,
    rectangle 1 is erased and rectangle 5 is drawn. This process continues
    until all 12 rectangles have had their turn.

The caller can choose one of three different formulas determine the
percentages used to generate the rectangles.

    kZoomOut - This effect can be used for zooming a small rectangle into
               a larger one. Each percentage i8 7/5 as large as the
               previous percentage. The final generated value is 1001.
               This gives the effect of starting out slowly and finishing
               up quickly. It can also be perceived as seeing a rectangle
               move forward from a far distance.

    kZoomIn - This effect can be used for zooming a large rectangle into
              a smaller one. Each percentage is 5/7 as large as the
              previous percentage. The first value generated is 1001,
              and subsequent values are smaller. Because the values are
              generated backward from the way we'd like, we also
              swap the source and destination rectangles to get the
              desired effect, which is to start quickly and finish
              up slowly. It can also be perceived as a rectangle moving
              off into the distance.

    kLinear - This effect can be used for moving a rectangle from one
              place to another without changing its size. Each percentage
              is evenly spaced from O1 to 1001. This gives a nice even,
              measured effect. It gives the impression of a rectangle
              moving in single plane, rather than moving into the
              distance or vice-versa.


********************************************************************************/



void ZoomRect (Rect source, Rect dest, short mode) {

    GrafPtr oldPort;
    GrafPort zoomPort;
    Rect tempRect[4];
    Fixed deltas[4];
    int numberOfSteps;
    Fixed percentComplete;
    Fixed acceleration;
    long dasTicks;

    GetPort(&oldPort);
    OpenPort(&zoomPort);
    SetPort (&zoomPort);
    CopyRgn(GetGrayRgn(), zoomPort.visRgn);

    PenPat(qd.gray);
    PenMode (patXor);
    numberOfSteps = 12;


    switch (mode ) {
        case kZoomOut:
            acceleration = FixRatio(7, 5);
            percentComplete = FixRatio(32767/(pow((7./5.1), numberOfSteps)), 32767);
            break;
        case kZoomIn:
            tempRect[0] = source;
            source = dest;
            dest = tempRect[0];
            acceleration = FixRatio(5, 7);
            percentComplete = 65536; // shorthand for FixRatio(1, 1), or 100%
            break;
        case kLinear:
            acceleration = FixRatio(1, numberOfSteps);
            percentComplete = acceleration;
            break;
    }

    deltas[0] = FixRatio(dest.top - source.top, 1);
    deltas[1] = FixRatio(dest.left - source.left, 1);
    deltas[2] = FixRatio(dest.bottom - source.bottom, 1);
    deltas[3] = FixRatio(dest.right - source.right, 1);

    SetRect(&tempRect[1], 0, 0, 0, 0);
    tempRect[3] = tempRect[2] = tempRect[1];

    dasTicks = TickCount() - 1;
    do {
        tempRect[0] = source;
        tempRect[0].top    += FixRound (FixMul(deltas[0], percentComplete));
        tempRect[0].left   += FixRound (FixMul(deltas[1], percentComplete));
        tempRect[0].bottom += FixRound (FixMul(deltas[2], percentComplete));
        tempRect[0].right  += FixRound (FixMul(deltas[3], percentComplete));

        FrameRect(&tempRect[0]);   // Draw the latest rectangle

        while (dasTicks >= TickCount());  // Just wait for counter to change
        dasTicks = TickCount() + 1;

        if (!EmptyRect(&tempRect[3]))
            FrameRect(&tempRect[3]);      // Erase any guy who's been around
                                          // too long.
        tempRect[3] = tempRect[2];
        tempRect[2] = tempRect[1];
        tempRect[1] = tempRect[0];

        switch (mode) {
            case kZoomOut:
            case kZoomIn:
                percentComplete = FixMul(percentComplete, acceleration);
                break;
            case kLinear:
                percentComplete += acceleration;
                break;
        }

    } while (--numberOfSteps);

    FrameRect(&tempRect[3]);       // Erase anything left on screen
    FrameRect(&tempRect[2]);
    FrameRect(&tempRect[1]);

    SetPort(oldPort);
    ClosePort(&zoomPort);
}