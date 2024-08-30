/* Listing 4-4 Progress Indicator.c */

#include "Progress_Indicator.h"
#include "Standard_Stuff_Ch4.h" // for "gMac"

/********************************************************************************

    Progress indicator routines

********************************************************************************/


/********************************************************************************

    Private routines for the process indicator routines. You'll never have to
    set or look at these values. The first three variables are initialised by
    calling InitProgressIndicator. The remaining variables are used 
    internally.

********************************************************************************/

GrafPtr pProgressPort;   /* Port to be used for drawing. When updating the
                            indicator, we temporarily switch this port,
                            restoring the old port when we are done. */

Rect pRect;              /* Rectangle used for the progress indicator. The entire indicator is drawn within this rectangle. pRect is used to frame the indicator, and is then inset by a pixel to draw the insides. */

long pMax;               /* Maximum value for the process. This maximum can be any value. The indicator is drawn such that the ratio of the completed part to the entire part is the same as the current value to the max value. */

long pCurrent;           /* Current value. Set to zero when you call InitProgressIndicator. It is updated by calling SetProgress or SetProgressDelta. */

long pLastCurrent;       /* The last value of pCurrent that was used to draw the indicator. This is used to check if we need to do any drawing; if pLastCurrent == pCurrent, we don't need to draw anything. */

short pLastBorder;       /* The location of the division between "completed" part and the "to be completed" part of our indicator the last time we drew it. We use this internally (along with pLastCurrent) for determining if the indicator needs to be updated. */


/********************************************************************************

    InitProgressIndicator

    Call this just before you start your periodic task. All you have to do is
    tell the progress indicator routines what port you want to draw in, the
    rectangle you want to use, and the maximum value of your indicator.
    Later, you'll make repeated calls to update your progress value (which can
    be anything from zero to the maximum value), which will update the
    "thermometer" in the window.

********************************************************************************/

void InitProgressIndicator (GrafPtr ourPort, Rect r, long max)
{
    pProgressPort= ourPort;
    pRect = r;
    pMax = max;
    pCurrent = 0;
    pLastCurrent = 0;
    pLastBorder = 0;

}


/********************************************************************************

    SetProgress

    Call this periodically to update the indicator. Pass in the new value that
    the indicator is supposed to represent. The value passed in will be
    clipped between 0 and the maximum value.

********************************************************************************/

void SetProgress(long absoluteAmount)
{
    pCurrent = absoluteAmount;
    DrawProgressBar();
}

/********************************************************************************

    SetProgressDelta

    Call this periodically to update the indicator. Pass in the new value to
    be added to the current value. The result will be clipped between 0 and
    the maximum value. The indicator will then be updated  to indicate the new
    current value.

    This routine returns TRUE if we hit or passed the maximum value. This
    could be handy to determine if we are done doing whatever periodic task we
    are representing.

********************************************************************************/



Boolean SetProgressDelta(long delta)
{
    pCurrent += delta;
    DrawProgressBar();
    return(pCurrent >= pMax);
}

/********************************************************************************

    DrawProgressBar

    Internal routine to update internal variables after the client has called
    SetProgress() or SetProgressDelta(). It calculates the width of the new
    rectangle; if it's changed since the last time, we call out routine.

********************************************************************************/


void DrawProgressBar()
{
    short border;
    short rectWidth;

    if ((pLastCurrent != pCurrent) && (pCurrent <= pMax)){
        pLastCurrent = pCurrent;
        rectWidth = pRect.right - pRect.left -2;
        border = pRect.left +1 + rectWidth * pCurrent / pMax;
        if (pLastBorder != border) {
            pLastBorder = border;
            UpdateProgressBar();
        }
    }
}

/********************************************************************************

    UpdateProgressBar

    Performs the actual drawing of the indicator. This routine is called
    internally from other progress indicator routines to draw the new state of
    the indicator. However, it can also be called in response to update events
    by outside clients.

********************************************************************************/


void UpdateProgressBar()
{
    GrafPtr oldPort;
    RGBColor oldForeColor;
    RGBColor oldBackColor;
 
    Rect doneRect;
    Rect toDoRect;

    doneRect = pRect;
    InsetRect(&doneRect, 1, 1);
    toDoRect = doneRect;

    doneRect.right = toDoRect.left = pLastBorder;
    GetPort(&oldPort);
    SetPort(pProgressPort);
    GetForeColor(&oldForeColor);
    GetBackColor(&oldBackColor);
    PenNormal();

    SetFrameColor();
    FrameRect(&pRect);
    SetDoneColor();
    PaintRect(&doneRect);
    
    SetToDoColor();
    PaintRect(&toDoRect);

    RGBForeColor(&oldForeColor);
    RGBBackColor(&oldBackColor);
    SetPort(oldPort);
}

/********************************************************************************

    Progress indicator routines

    These are called to set the colors for the three different parts of the
    progress indicator: the frame, the "done" part (the stuff of the left side
    of the indicator), and the "to do" part( the stuff on the right).

    Note that we check to see only if we have Color QuickDraw or not; we don't
    make a check to see how deep our monitor is. Instead, we simply let
    QuickDraw match the color we specify to one most closely matching it on
    whatever screen we're drawing.

    However, if we set only the foreground color, we run headfirst into
    QuickDraw as it tries to help us out behind our backs. The the case of
    string the "to do" color. For that colour, we use red = 0xCCCC, green =
    0xCCCC, and blue = 0xFFFF. This gives us a nice steel blue color in 8-bit
    mode, and a nice grey in 4- and 2-bit modes. However, it draws as
    black in 1-bit mode, when we expect and need it to draw in white.

    What's happening is this: when QuickDrawtakes the color you specify and
    tries to match it with one of the colors in the screen device's color table,
    it makes a special check to see if it maps your color into the background
    color. If so, it inverts the color and tries again. In our case, our steel
    blue gets mapped into white, which is the background color. QuickDraw says
    "Oops! I'm about to draw something that won't show up, so I'll use a
    different color, instead!" So it changes (0xCCCC, 0xCCCC, 0xFFFF) into
    (0x3333, 0x3333, 0x0000) and remaps it. This time, the color maps into
    black, which is what QuickDraw ends up using.

    To avoid this feature, we take color of the background color. When we
    want our light colors to map into white, we change the background color to
    black, avoiding any conflict. Alternatively, we could attach a palette to
    our window containing the two colors we want as "courteous" entries. This
    is sufficient to get QuickDraw to stop being so helpful.

********************************************************************************/



const RGBColor kBlack      = {0x0000, 0x0000, 0x0000};
const RGBColor kWhite      = {0xFFFF, 0xFFFF, 0xFFFF};
const RGBColor kDarkGrey   = {0x4000, 0x4000, 0x4000};
const RGBColor kSteelBlue  = {0xCCCC, 0xCCCC, 0xFFFF};

void SetFrameColor()
{
    if (gMac.hasColorQD) {
        RGBForeColor(&kBlack);
        RGBBackColor(&kWhite);
    } else {
        PenPat(qd.black);
    }
}


void SetDoneColor()
{
    if (gMac.hasColorQD) {
        RGBForeColor(&kDarkGrey);
        RGBBackColor(&kWhite);
    } else {
        PenPat(qd.black);
    }
}


void SetToDoColor()
{
    if (gMac.hasColorQD) {
        RGBForeColor(&kSteelBlue);
        RGBBackColor(&kBlack);
    } else {
        PenPat(qd.white);
    }
}





