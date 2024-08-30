/* Listing 9-5. Stuff.h */

#define kEnter ((char) 0x03) 
#define kReturn ((char) 0x00)
#define kOurSICNs  1964
#define kGenericLDEF 1000

typedef struct { 
    StringHandle theString;
     short iconID;
     short iconIndex;
     Boolean enabled;
} ListItemRecord, *ListItemPtr, **ListItemHandle;

void DoShowCustomList(void); 
void DOShowCustomWindow(void);