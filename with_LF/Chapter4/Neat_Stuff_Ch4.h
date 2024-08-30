/* Listing 4-1 Neat stuff.h */

extern Boolean gPeriodicTask;

void DoDialogHit(DialogPtr dlg, short item); 
void DoMainLoopLoopTasks (void);
void DoMusicMaker(void);
pascal Boolean OurFilter(DialogPtr dlg, EventRecord *event, short *itemHit);
void ShowModeless(void);
void StartMovable(void);
void FlashDialogItem(DialogPtr dlg, short itemToFlash);
void SetDialogItemState(DialogPtr dlg, short controlNumber, short value);
short GetDialogItemState(DialogPtr dlg, short controlNumber);
void SetRadioButton(DialogPtr dlg, short buttonNumber);
void ToggleCheckBox (DialogPtr dlg, short buttonNumber);
void SetDialogItemTitle(DialogPtr dlg, short item, Str255 *newTitle);
void FlipItems(DialogPtr dlg, Boolean flippedOn);
long cstrlen (char *s);
long cstrlen (register char *s);
char UprChar(char c);

#define kEnter (char) 0x03
#define kReturn (char) 0x0D
#define kEscape (char) 0x1B
#define kPeriod '.'

#define kMusicMakerDialog 256
#define kFirstRadio 8
#define kLastRadio 14
#define kFirstCheckBox 15
#define kLastCheckBox 16
#define kIconID 26

#define kModelessDialog 257

#define kMovableModalDialog 258
#define kMMOKButton 1
#define kMMText 2
#define kMMProgressBar 3

#define kSourceStrings 129
#define kFlippedStrings 130

#define kDialogStrings 131
#define kErasingDisk 1






