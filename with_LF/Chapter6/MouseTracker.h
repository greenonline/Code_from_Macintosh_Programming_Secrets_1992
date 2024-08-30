/* Mousetracker.h */


/* Values used in mouse tracking */


#define trackPress 1
#define trackMove 2
#define trackRelease 3

#define kMouseMoved TRUE
#define kTurnItOn TRUE
#define kTurnItOff !kTurnItOn

typedef void (*ConstrainProcPtr)(Point anchorPoint, Point previousPoint, Point *nextPoint);
typedef void (*FeedbackProcPtr)(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove);
typedef void (*TrackMouseProcPtr)(short aTrackPhase, Point *anchorPoint, Point *previousPoint, Point *nextPoint, Boolean mouseDidMove);


/* Function declarations */

Rect SketchNewRect(Boolean fromCenter);
void RectFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove);

Rect SketchNewLine(Boolean fromCenter);
void LineFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove);

Rect SketchNewOval(Boolean fromCenter);
void OvalFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove);

Rect SketchNewRoundRect(Boolean fromCenter);
void RoundRectFeedback(Point anchorPoint, Point currentPoint, Boolean turnItOn, Boolean mouseDidMove);

/*
#define CONSTRAINONCE() \
        if (constrainProc) \
            (*constrainProc)(*anchorPoint, *endPoint, &theMouse);

#define FEEDBACKONCE(turnItOn, mouseMoved) \
        if (feedbackProc) { \
            PenNormal(); \
            PenPat (gd.gray); \
            PenMode(patXor); \
            (*feedbackProc)(*anchorPoint, *endPoint, turnItOn, mouseMoved); \
}

#define TRACKONCE(phase, mouseMoved) \
        if (trackMouseProc) \
            (*trackMouseProc)(phase, anchorPoint, endPoint, &theMouse, mouseMoved);
*/

void TrackMouse(ConstrainProcPtr constrainProc,
                FeedbackProcPtr feedbackProc,
                TrackMouseProcPtr trackMouseProc,
                Point *anchorPoint,
                Point *endPoint);



void AdjustForTrackingFromCenter(Point *anchorPoint, Point endPoint);