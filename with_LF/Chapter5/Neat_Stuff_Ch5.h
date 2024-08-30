/* Listing 5-1 NeatStuff.h */

typedef struct {
    RgnHandle coveredArea;
    GrafPtr offWorld;
} SavedBGRecord, *SavedBGPtr, **SavedBGHandle;

void DoNewWindow(void);
PicHandle GetRandomPicture(Rect* bounds);

void ShowDialog(void);
void ShowSavingDialog(void);
void SaveBackground(WindowPtr w, SavedBGHandle* savedData);
void RestoreBackground(SavedBGHandle savedData);

GrafPtr GetDesktopPort();
void SetDesktopDevice();

RgnHandle GetCoveredArea(DialogPtr w);
GrafPtr CreateOffWorld ( Rect globalRect);
void UseOffWorld (GrafPtr offWorlder);
void DoneWithOffWorld(GrafPtr offWorlder);
Boolean IsColorPort(GrafPtr offWorlder);
void DisposeOffWorld(GrafPtr offWorlder);
void ValidateWindows(RgnHandle globalRgn);
Rect GetWindowStructureRect(WindowPtr window);
Point GetGlobalTopLeft(WindowPtr window);
