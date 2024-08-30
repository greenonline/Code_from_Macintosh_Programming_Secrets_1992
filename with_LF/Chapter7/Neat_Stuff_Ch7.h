/* NeatStuff.h */


#define MIN(a,b) ((a) < (b) ? (a) : (b))

extern WindowPtr gFirstWindow;

#define kNewWindowID 128
#define rMiscStrings 129
#define sUntitledTitle 1


typedef void  (*SetUpProc)(void);
typedef void  (*WindowActionProc) (WindowPtr theWindow);
typedef void (*FinishUpProc)(void);


void DoNewWindow(void);
void DoCloseWindow(WindowPtr theWindow);
void DoActivateWindow(WindowPtr theWindow, Boolean becomingActive);
void DoZoomWindow(WindowPtr window, short zoomDir, Boolean front);

void DoTileWindows(void);
void TileSetup(void);
void TileTheWindow(WindowPtr theWindow);

void DoStackWindows(void);
void StackSetup(void);
void StackTheWindow(WindowPtr theWindow);

void DoSelectFromWindowsMenu(short menuItem);
void LookForSelectedWindow(WindowPtr theWindow);

void AddWindowToMenu(WindowPtr theWindow);
void RemoveWindowFromMenu(WindowPtr theWindow);
void TweakLowerGuys(WindowPtr theWindow);  

void ForEachWindowDo(SetUpProc theStarter,
WindowActionProc theDoer,
FinishUpProc theEnder);
void ForEachWindowPerScreenDo(SetUpProc theStarter,
WindowActionProc theDoer,
FinishUpProc theEnder);
void CountWindows (WindowPtr theWindow);

WindowPtr GetPreviouslyCreatedWindow(WindowPtr theWindow);
void LookForPreviousWindow (WindowPtr theWindow);

short GetMenuItemForWindow (WindowPtr theWindow);
void CountSomeWindow(WindowPtr theWindow);

void SetWindowBounds(WindowPtr then, Rect newBounds);

Rect GetWindowContentRect(WindowPtr window);
Rect GetWindowStructureRect (WindowPtr window);
Rect GetWindowDeviceRectNMB (WindowPtr window);
Rect GetWindowDeviceRect (WindowPtr window);
GDHandle GetWindowDevice(WindowPtr window);
GDHandle GetRectDevice(Rect globalRect);
void LocalToGlobalRect(Rect *aRect);
Point GetGlobalTopLeft(WindowPtr window);
Rect GetMainScreenRect(void);

void CatenatePStrings(Str255 targetStr, Str255 appendStr);

























