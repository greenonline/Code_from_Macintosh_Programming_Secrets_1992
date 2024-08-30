/* Listing 6-1. NeatStuff.h */

#include "QDOffscreen.h"

typedef struct {
    WindowRecord windowRecord;
    GWorldPtr offWorld;
    Rect selectionRect;
    Boolean antsOnTheMarch;
} MyWindowRecord, *MyWindowPtr;

void DoMarchingAnts(void);
void KillAnts(MyWindowPtr whichWindow);
void StartAnts(MyWindowPtr whichWindow);
void DoOpenWindow(void);
void DoCloseWindow(WindowPtr theWindow);
void DoDrawGrowIcon(WindowPtr theWindow);
void DoActivateWindow(WindowPtr theWindow, Boolean activating);
void DoUpdateWindow(EventRecord *theEvent);
Rect DoContentClick(EventRecord *theEvent, WindowPtr whichWindow);
Boolean PointInSelection(WindowPtr whichWindow, Point location);
void RefreshWindow(MyWindowPtr theWindow, Rect *theRect);
MyWindowPtr CreateBufferedWindow(short width, short height, short depth);
void ReadMacPaint(FSSpec* theFile, GWorldPtr buffer);


void DragSelectionFeedback(Point anchorPoint,
                           Point nextPoint,
                           Boolean turnItOn,
                           Boolean mouseOldMove);


void DragSelection(WindowPtr whichWindow, Point mouse);
void PointToCurrentRect(Point currentPoint);
void UpdateBuffers(void);
GWorldPtr CloneGWorld(GWorldPtr sourceGWorld);

#define kNewWindowID 128