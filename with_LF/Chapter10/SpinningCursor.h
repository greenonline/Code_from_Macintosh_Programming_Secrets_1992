/* Listing 10-7. SpinningCursor.h */

void InitCursorCtl(short resID);
void SpinCursor(short increment);
void LockCursorData(void);
void UnlockCursorData(void);
void StartAsyncSpinning(short period);
void StopAsyncSpinning(void);
void MySpinner(void);