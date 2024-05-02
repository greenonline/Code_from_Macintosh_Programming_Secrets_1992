/* Listing 4-3 Progress Indicator.h */

/* Progress indicator routines */

void InitProgressIndicator (GrafPtr ourPort, Rect r, long max);
void SetProgress(long absoluteAmount);
Boolean SetProgressDelta(long delta);
void DrawProgressBar(void);
void UpdateProgressBar(void);

void SetFrameColor(void);
void SetDoneColor(void);
void SetToDoColor(void);
