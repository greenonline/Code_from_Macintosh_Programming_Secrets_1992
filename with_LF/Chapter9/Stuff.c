/* Listing 9-5. Stuff.c */



//#include "Neat Stuff.h"
#include "Stuff.h"
#include "GestaltEqu.h"
#include "Palettes.h" // For GetGray call.

/*******************************************************************************

    Global variables, type definitions, and function prototypes.

*******************************************************************************/

const RGBColor kBlack = {0x0000, 0x0000, 0x0000}; 
const RGBColor kWhite = {0xFFFF, 0xFFFF, 0xFFFF}; 
const RGBColor kGray  = {0xB000, 0xB000, 0xB000};

ListHandle gMyList;
Rect gNextWindowRect = {40, 10, 180, 110};
const  Point gWindowStagger = {20, 20};
const  Point gWindowStart = {40, 10};

// Structure used in our list-drawing routines 
typedef struct {
    Boolean selected; 
    Rect* bounds; 
    ListItemHandle data;
    ListHandle lHandle;
} DrawItemBundle, *DrawItemBundlePtr;

// Function prototypes.Some routines are declared as using the Pascal 
// calling convention because they are called by the Toolbox.

pascal Boolean ListFilter(DialogPtr dlg, EventRecord* event, short* itemHit);
void FillList(ListHandle theList);
void FlashDialogItem(DialogPtr dlg, short itemToFlash);
pascal void MyLDEF( short lMessage, Boolean lSelect, Rect* lRect,
Cell lCell, short lDataOffset, short lDataLen,
ListHandle lHandle);
void DrawItem(Boolean selected, Rect* bounds, ListItemHandle data,
ListHandle lHandle);
void DrawNewWay(Boolean selected, Rect* bounds, ListItemHandle data,
ListHandle lHandle);
pascal void DrawIconNewWay(short depth, short deviceFlags, GDHandle targetDevice,
long userData);
void DrawOldWay(Boolean selected, Rect* bounds, ListItemHandle data,
ListHandle lHandle);
void DrawTheIcon(Rect *bounds, ListHandle lHandle, ListItemHandle data); 
void DrawTheText(Rect *bounds, ListHandle lHandle, ListItemHandle data);
void GrayOut(Rect* bounds);
void DrawSmallIcon(short id, short index, Rect* where);



/*******************************************************************************

    DoShowCustomList

    Brings up a modal dialog containing a list and an OK button. The list is created with a custom LDEF that shows items with an icon drawn to their left.
After we create the dialog with a call to GetNewDialog, we prepare our list. This list is implemented as a userltem. First, we get the bounds of the userltem. We treat the bounds as a maximum size and shrink it as necessary so that an integral number of lines will appear in the list. The size of each line is determined by the height of the font the dialog uses. However, since we'll be drawing icons along with each text item, and since the icons are 16 pixels high, we make sure that each line is at least 16 pixels high, too.

    Once we've determined and set the height for the userltem, we call the List Manager to create the list we draw inside it. Because we're using
our stub LDEF that can't handle the LlnitMsg, we must call the list definition code ourselves so that it can perform any initialization. After that, we set the list flags so that only one item is selected at a time. Then we fill up the list with some items and finally turn on the flag
that allows the list to be drawn.

    All that needs to be done now is call ModalDialog. After the user clicks OK, we dispose of the list and the dialog and leave. A real program would probably do something with the item that the user selected, but we'll leave that up to you.
*******************************************************************************/
void DoShowCustomList()
{
    DialogPtr dlg;
    short itemHit;
    short iKind;
    Handle iHandle;
    Rect iRect;
    FontInfo info;
    Point cSize;
    Rect dataBounds = {0, 0, 0, 1};



    dlg = GetNewDialog(130, nil, (WindowPtr) -1); 
    SetPort(dlg);

    GetDItem(dlg, 2, &iKind, &iHandle, &iRect);

    GetFontInfo(&info);
    cSize.h = iRect.right - iRect.left - 15;
    cSize.v = info.ascent + info.descent + info.leading;
    if (cSize.v < 16)
        cSize.v =16;



    iRect.bottom = iRect.top + ((iRect.bottom - iRect.top) / cSize.v) * cSize.v; 
    SetDItem(dlg, 2, iKind, iHandle, &iRect);
    iRect.right -= 15; // To allow for the list's scroll bar.

    gMyList = LNew(&iRect, &dataBounds, cSize, kGenericLDEF, dlg,
                   FALSE,                   // delay drawing list
                   FALSE,                   // no grow box
                   FALSE,                   // no horizontal scroll bar
                   TRUE);                   // has vertical scroll bar

    MyLDEF(lInitMsg,
            FALSE, nil, cSize, 0, 0,        // these are all dummy values
            gMyList) ; 

    (**gMyList).selFlags = lOnlyOne;          // "There can be only one!"

    FillList(gMyList);
    LDoDraw(TRUE, gMyList);

    do {
        ModalDialog(ListFilter, &itemHit);
    } while (itemHit != ok);

    LDispose(gMyList);
    DisposDialog(dlg);
}

/*******************************************************************************

     DoShowCustomWindow

    Simply show a window. This window uses the custom Windoid WDEF that we've
    written. Don't be fooled, though I This window may look like a floating
    window, but it doesn't act like one. It takes more than a custom WDEF to
    make a window float.

*******************************************************************************/
void DoShowCustomWindow()
{
    Ptr p = nil;
    Str255 title = "\pNeat Window"; // Title not shown in this WDEF TRUE;
    Boolean visible = TRUE;
    short procID = (1000 * 16);     // WDEF #1000
    WindowPtr     behind = (WindowPtr) -1;
    Boolean goAwayFlag = TRUE;
    long refCon = 0;
    WindowPtr myWindow;


    myWindow = NewCWindow(p, &gNextWindowRect, title, visible, procID, behind, goAwayFlag, refCon);



    //
    // We stagger our windows. We update gNextWindowRect so that the next 
    // window we create is moved down and to the right of the one we
    // just created.
    //

    OffsetRect(&gNextWindowRect, gWindowStagger.h, gWindowStagger.v); 
    if (gNextWindowRect.bottom > qd.screenBits.bounds.bottom)
OffsetRect(&gNextWindowRect, 0, gWindowStart.v - gNextWindowRect.top); 
    if (gNextWindowRect.right > qd.screenBits.bounds.right)
OffsetRect(&gNextWindowRect, gWindowStart.h - gNextWindowRect.left, 0);
}

/******************************************************************************* 

    ListFilter

    This is the ModalDialog filter we use for the dialog that contains our 
    list. It handles updates and mouse clicks for our list, as well as some 
    special handling of the OK button.

    When the user clicks the mouse in our dialog, we need to see if the click 
    was on our list. This is done by getting the bounds of the list and
    using it and the mouseDown location in a call to PtInRect. If PtInRect 
    returns TRUE, the user clicked in our list, and we need to call LClick to
    handle it. LClick is a List Manager routine that takes care of clicks on 
    the list items or in the scroll bar(s) attached to the list. If LClick
    returns TRUE, it's reporting that this click was the second part of a 
    double-click. If it was, and if the item wasn't disabled, we simulate a
    click on the OK button.

    If the user pressed a key, we check to see if that key was either the
    Return or Enter key. If so, we simulate a click on the OK button. This is
    standard action for dialogs, and is normally handled by the Dialog
    Manager. However, if an application implements a custom dialog filter, it
    is up to that application to re-implement that standard behavior.
    Fortunately, it only takes a few lines, as you can see.

    Finally, if there is an update event for the dialog, we need to do two
    things. First, we put a bold outline around the OK button, signifying that
    this is the button that will be selected if the user presses Return or
    Enter. Second, we call LUpdate to draw our list.

*******************************************************************************/
pascal Boolean ListFilter(DialogPtr dlg, EventRecord* event, short* itemHit) 
{
    short iKind;
    Handle iHandle;
    Rect iRect;
    short radius;

    char          key;
    Point         localMouse;

    switch (event->what) {
        case mouseDown:
            localMouse = event->where;
            GlobalToLocal(&localMouse);
            GetDItem(dlg, 2, &iKind, &iHandle, &iRect);
            if (PtInRect(localMouse, &iRect)) {
                if (LClick(localMouse, event->modifiers, gMyList)) {
                    *itemHit = ok;
                    FlashDialogItem(dlg, *itemHit);
                }
                return TRUE;
            }
            return FALSE;

        case keyDown:
            key = event->message & charCodeMask;
            if ((key == kReturn) || (key == kEnter)) {
                *itemHit = ok; 
                FlashDialogItem(dlg, *itemHit); 
                return TRUE;
            }
            return FALSE;

        case updateEvt:
            SetPort(dlg);
            GetDItem(dlg, ok, &iKind, &iHandle, &iRect);
            InsetRect(&iRect, -4, -4);
            radius = (iRect.bottom - iRect.top) / 2;
            if (radius < 16)
                radius =16; 
            PenNormal ();
            PenSize(3,3);
            FrameRoundRect(&iRect, radius, radius);

            GetDItem(dlg, 2, &iKind, &iHandle, &iRect); 
            InsetRect(&iRect, -1, -1);
            PenNormal ();
            FrameRect(&iRect);

            LUpdate(dlg->visRgn, gMyList);

            return FALSE;

        default:
            return FALSE;
    }
}





/*******************************************************************************

    FillList

    Fills up our list with some data. We've defined a structure that contains
    data for a single item in the list. It looks like this:

              StringHandle
              short
              short
              Boolean

    This structure is used to contain the text to be drawn, the icon to draw
    with it, and a flag indicating if the item is enabled (drawn in black) or 
    disabled (drawn in gray).

    To squeeze the most out of memory that we can, we don't actually store
    this structure in our ListRec, even though we could if we wanted. The
    reason we don't is because we can only store 32K of data in our
    ListRec. If we stored the entire structure (which takes about 10
    bytes), we would be able to insert only 3,276 items into the list.
    Instead, we create our record dynamically with NewHandle and store the
    handle (which is only 4 bytes long) into the list. This allows us to 
    insert up to 8,192 items.

    The data for our list come from the root directory of whatever volume
    we're running on. We make repeated calls to PBGetCatInfo to get
    information on all the files at the root level. For each item, we record
    its name and determine what icon should be used to represent it. To
    simulate Standard File's PutFile dialog, we disable all files and enable
    only directories.

*******************************************************************************/ 
void FillList(ListHandle theList)
{
    short vRefNum;
    long dirID;
    short index;
    OSErr err;
    CInfoPBRec pb;
    ListItemHandle listData;
    Str255  itsName;
    StringHandle  itsNameHandle;
    Cell  cell = {0, 0};

    HGetVol(nil, &vRefNum, &dirID);
    dirID = fsRtDirID;



    index =0;
    do {
        ++index; 
        pb.hFileInfo.ioNamePtr = itsName; 
        pb.hFileInfo.ioVRefNum = vRefNum;
        pb.hFileInfo.ioFDirIndex = index;
        pb.hFileInfo.ioDirID = dirID;
        err =PBGetCatInfoSync(&pb);

        if (err ==noErr) {
            listData = (ListItemHandle) NewHandle(sizeof(ListItemRecord));
            (**listData).theString = NewString(itsName);
            (**listData).iconID = kOurSICNs;
            if(pb.hFileInfo.ioFlAttrib &ioDirMask) {    // we have a folder 
                (**listData).iconIndex = 1;
                (**listData).enabled = TRUE;
            } else {                                    // check for app or doc
                if (pb.hFileInfo.ioFlFndrInfo.fdType == 'APPL')
                    (**listData).iconIndex = 4;
                else
                    (**listData).iconIndex = 0;
                (**listData).enabled = FALSE; 
            }
            cell.v = LAddRow(1, 32767, theList); 
            LSetCell(&listData, sizeof(ListItemHandle), cell, theList);
        }
    } while (err == noErr);
}

/*******************************************************************************

    FlashDialogItem

    Utility that quickly flashes a button in the dialog. We highlight the button
    by calling HiliteControl, waiting a little bit so that the user can see
    the button highlighted, and then turn off the button by calling
    HiliteControl again.

    This routine assumes that the item we want to flash is backed up by a 
    Control Manager control and really works best if the item is a simple 
    button control.

*************.*****************************************************************/
void FlashDialogItem(DialogPtr dlg, short itemToFlash)
{
    short iKind;
    Handle iHandle;
    Rect iRect;
    long ignored;

    GetDItem(dlg, itemToFlash, &iKind, &iHandle, &iRect); 
    HiliteControl((ControlHandle) iHandle, 1);
    Delay(8L, &ignored);
    HiliteControl((ControlHandle) iHandle, 0);
}

/******************************************************************************* 

    MyLDEF

    This is the entry point for the custom list definition. Normally, this
    would be the entry point of an LDEF resource. However, we're using a stub
    LDEF resource that simply calls back into this routine.

    We handle three of the messages that can be sent to us: lInitMsg,
    lDrawMsg, and lHiliteMsg. We don't handle lCloseMsg because we don't have
    any special disposing needs.

    When called with the lInitMsg, we do two things. First, we calculate and
    cache some values we'll need for positioning our text and icons. We cache
    these values in our ListRec in the indent field, which is ours to use.
    Second, we set up our list's refCon to point to our custom definition's
    entry point. This is so that our stub LDEF knows the address of the REAL
    custom definition and can call it for us in the future.

    For lDrawMsg and lHiliteMsg, we first need to find the data needed for the
    cell we are told to handle. There is a field in the ListRec called cells
    which contains a handle to all the data we've inserted into the list.
    These data are the array of handles we inserted in FillList. We also give an 
    offset into this array. Using these two pieces of information, we are able
    to retrieve the correct handle.

    We defer drawing to a subroutine called DrawItem. However, we deal with 
    highlighting right here. If the item is enabled, we call InvertRect on it. If 
    the item is disabled, we leave it as it is to show the user that a
    disabled item can't be selected.

*******************************************************************************/
pascal void MyLDEF( short       lMessage,       // what operation to do
                    Boolean     lSelect,        // draw it selected?
                    Rect*       lRect,          // where to draw the item
                    Cell        lCell,          // which cell to draw
                    short       lDataOffset,    // offset to data for drawing
                    short       lDataLen,       // length of that data
                    ListHandle  lHandle)        // handle to list record
{
    FontInfo  info;
    ListPtr  listPtr;

    ListItemHandle myDataHandle;
    OSErr err;
    long systemVersion;

    listPtr = *lHandle;
    switch (lMessage) {
        case lInitMsg:

            // Cache the vertical offset for the icon in indent.h 
            GetFontInfo(&info);
            listPtr->indent.h = (listPtr->cellSize.v - 16) / 2;

            // Cache the vertical offset for the text in indent.v 
           listPtr->indent.v =
               (listPtr->cellSize.v - (info.ascent + info.descent)) / 2 + info.ascent;
            // Point refCon to this entry point so that stub LDEF knows where to go.
            listPtr->refCon = (long) MyLDEF;

            // Check and remember if we're running under 7.0. If so, then 
            // we can use its new, nifty grayish drawing routines.
            err = Gestalt(gestaltSystemVersion, &systemVersion);
            listPtr->userHandle = (Handle) (systemVersion >= 0x0700);

            break; 

         case lDrawMsg:

            myDataHandle = *(ListItemHandle *)((*listPtr->cells) + lDataOffset);
            if (myDataHandle != nil)
                DrawItem(lSelect, lRect, myDataHandle, lHandle);
            break;

        case lHiliteMsg:

            myDataHandle = *(ListItemHandle *)((*listPtr->cells) + lDataOffset);
            if ((myDataHandle != nil) && ((**myDataHandle).enabled))
                InvertRect(lRect);
            break;
    }
}



/*******************************************************************************

    DrawItem

    This is the routine used to draw each item in the list. We draw items in
    one of two ways, depending on the capabilities of our machine. If we're
    running System 7.0, then we have this way cool new QuickDraw transfer mode 
    called grayishTextOr that we can use for drawing text. Depending on the
    depth of the monitor, it draws text either as true gray or as dithered
    black and white. If we're running under 6.0.x, which doesn't have this
    mode, then we draw in dithered black and white by hand.

*******************************************************************************/
void DrawItem(Boolean selected, Rect* bounds, ListItemHandle data, ListHandle lHandle)
{
    PenState ps;
    short oldTextMode;
    Boolean hasGrayishText;

    GetPenState(&ps);
    oldTextMode = qd.thePort->txMode;

    hasGrayishText = (**lHandle).userHandle != 0;

    if (hasGrayishText) {
        DrawNewWay(selected, bounds, data, lHandle);
    } else {
        DrawOldWay(selected, bounds, data, lHandle);
    }

    TextMode(oldTextMode);
    SetPenState(&ps);
}

/*******************************************************************************

    DrawNewWay

    Called to draw our list of items when we have grayishTextOr mode available
    to us. While this makes drawing the text pretty easy, we have a bit of a
    problem with our icon: grayishTextOr mode affects only text; it won't
    affect the icon we draw with CopyBits. Therefore, if we want our icon
    to match the way the text is drawn, we have to do the same thing that 
    grayishTextOr does.

    Basically, we want to draw our icon in one of two ways. If the monitor
    supports it, we want to draw the icon in gray. If the monitor doesn't
    support gray (say, for instance, it's in I-bit mode), then we'd like to
    draw the icon using a dithered black and white pattern. To take into
    account the possibility that our icon could actually span two monitors,
    one of which supports gray and one which doesn't, we call DeviceLoop to
    manage the drawing. Thus the actual drawing of the icon is performed in 
    DrawIconNewWay, which is the procedure whose address we pass to DeviceLoop.

    After we've drawn the icon, we proceed with the text. This is much 
    simpler. If the text is enabled, we draw it in black using srcOr mode. If 
    the text is disabled, we draw it using grayishTextOr mode. This will draw 
    the text either in gray, if gray is available, or dithered gray if not.

*******************************************************************************/ 
void DrawNewWay(Boolean selected, Rect* bounds, ListItemHandle data, ListHandle lHandle)
{
    Boolean itemEnabled;
    DrawItemBundle dataBundle;

    itemEnabled =(**data).enabled;
    dataBundle.selected = selected;
    dataBundle.bounds = bounds;
    dataBundle.data = data;
    dataBundle.lHandle = lHandle;

    DeviceLoop( qd.thePort->clipRgn,    /* Region we'll be drawing in. */
                DrawIconNewWay,         /* Function to do the drawing. */
                (long)  &dataBundle,    /* Data passed to our routine. */
                0) ;                    /* Flags - none used here. */

    RGBForeColor(&kBlack);
    if (itemEnabled) {
        TextMode (srcOr) ; 
    } else {
        TextMode(grayishTextOr);
    }

    DrawTheText(bounds, lHandle, data);

    if (selected && itemEnabled)
        InvertRect(bounds);  
}

// From Page 441
/*
void DrawNewWay(Boolean selected, Rect* bounds, ListItemHandle data, ListHandle lHandle)
{
    IconTransformType iconTransform;
    Boolean itemEnabled =(**data).enabled;
    Rect iconRect;

    iconTransform - (itemEnabled ? ttNone : ttDisabled);

    iconRect.left = bounds->left;
    iconRect.top = bounds->top + (**lHandle).indent.h;
    iconRect.bottom = iconRect.top +16;
    iconRect.right = iconRect.left +16;  

    PlotIconID(&iconRect, atNone, iconTransform, (**data).iconIndex +128);  

    TextMode(itemEnabled ? srcOr: grayishTextOr);
    DrawTheText(bounds, lHandle, data);

    if (selected && itemEnabled)
        InvertRect(bounds);
}
*/

/*******************************************************************************

    DrawIconNewWay

    This routine is called by DeviceLoop to draw our icon. If the monitor
    supports it, we want to draw the icon in gray. If the monitor doesn't
    support gray (say, for instance, it's in I-bit mode), then we'd like to
    draw the icon using a dithered black and white pattern.

    First, we check to see if the item is disabled or not. If not, then we
    just draw in black and we're finished. If the icon is disabled, we see if
    we can get an appropriate shade of gray. This is done by calling System
    7.0's GetGray routine, which is the same routine called by the text
    drawing routines when grayishTextOr mode is used. GetGray takes two RGB 
    parameters and tries to find an appropriate level of gray that falls
    between these two colors. If GetGray returns TRUE, then we can draw in
    gray. If not, we have to dither the icon. In this routine, we feed GetGray
    black and white as the two RGB parameters.

    We draw the icon in gray using the little-known trick of CopyBits. You are 
    warned by Inside Mac that, unless you want "colorization to occur," you
    should set the foreground color to black and the background color to
    white. Well, colorization is exactly what we want to happen, since we want
    to draw a gray icon. To do this, all we have to do is set the foreground
    color to the gray that GetGray gave us, and that color will be applied to
    all the black bits of the icon. If we couldn't get a gray from GetGray, we
    just set the foreground color to black.

    After drawing the icon, we have to deal with the case in which we drew it
    in black when we wanted gray. In this case, we want to have the icon appear
    in the old-fashioned dithered gray that we've known for years. This is
    done by using a speckled pattern to erase some of the bits. We use our
    utility routine GrayOut to do this for us.

****************************.**************************************************/
pascal void DrawIconNewWay(short depth, short deviceFlags, GDHandle targetDevice, long userData)
{
    RGBColor grayColor;
    Boolean drawIconDithered;
    Boolean itemEnabled;
    DrawItemBundlePtr myDataPtr = (DrawItemBundlePtr) userData;

    itemEnabled = (**(myDataPtr->data)).enabled;

    RGBBackColor(&kWhite);
    drawIconDithered = FALSE;

    if (itemEnabled) {
        RGBForeColor(&kBlack);
    } else {
        grayColor = kBlack;
        if (!GetGray(targetDevice, &kWhite, &grayColor)) {
            // grayColor stays black if GetGray returns FALSE
            drawIconDithered = TRUE; 
        }
        RGBForeColor(&grayColor);
    }



    DrawTheIcon(myDataPtr->bounds, myDataPtr->lHandle, myDataPtr->data); 

    if (!itemEnabled && drawIconDithered) {
        GrayOut(myDataPtr->bounds);
    }
}

/******************************************************************************* 

    DrawOldWay

    This routine is called if we aren't running under 7.0. Here, we simply
    draw the text and the icon in black. If the item should be disabled, we
    gray it out by calling our GrayOut utility.

*******************************************************************************/ 
void DrawOldWay(Boolean selected, Rect* bounds, ListItemHandle data, ListHandle lHandle) 
{
    DrawTheIcon(bounds, lHandle, data);
    DrawTheText(bounds, lHandle, data);

    if (!(**data).enabled) {
        GrayOut(bounds);
    }

    if (selected && (**data).enabled)
         InvertRect(bounds);
}

/******************************************************************************* 

    DrawTheIcon

    This is the routine used by DrawNewWay and DrawOldWay to prepare the
    drawing of the icon. We do the actual drawing with a call to another
    utility routine called DrawSmallIcon. However, we first need to set up a 
    rectangle for DrawSmallIcon to draw in.

*******************************************************************************/ 
void DrawTheIcon(Rect *bounds, ListHandle lHandle, ListItemHandle data) 
{
    Rect iconRect;


    iconRect = *bounds;
    iconRect.top += (**lHandle).indent.h;
    iconRect.bottom = iconRect.top + 16;
    iconRect.right = iconRect.left + 16;
    DrawSmallIcon( (**data).iconID, (**data).iconIndex, &iconRect);
}

/*******************************************************************************  

    DrawTheText

    This is the routine used by DrawNewWay and DrawOldWay to draw the actual
    text. Nothing special here. We indent from the left 20 pixels, and we move
    down vertically by the amount we calculated in our initialization routine.
    Then we call DrawString to draw the text.

*******************************************************************************/
void DrawTheText(Rect *bounds, ListHandle lHandle, ListItemHandle data) 
{
    StringHandle theString;
    MoveTo(bounds->left + 20, bounds->top + (**lHandle).indent.v);
    theString = (**data).theString;
    HLock((Handle) theString);
    DrawString(*theString);
    HUnlock((Handle) theString);
}

/******************************************************************************* 

    GrayOut

    Small routine to take an area and make it appear grayed out. This is done
    by taking QuickDraw's global gray pattern and using it to erase every
    other bit in the specified rectangle. This erasing is done using
    QuickDraw's srcBic--or source bit clear--mode. Source bit clear means
    that each black bit in the source should result in the erasure of the 
    corresponding bit in the destination.

*******************************************************************************/ 
void GrayOut(Rect* bounds)
{
    PenNormal();
    PenPat(qd.gray);
    PenMode(srcBic); 
    PaintRect(bounds);
}

/*******************************************************************************

    DrawSmallIcon

    Short utility that draws a small icon. We pass in the resource ID of a
    SICN. Since this resource contains a series of indexed icons, we also pass
    in which icon to draw. Finally, we specify where we want the icon drawn.
    Since this location is specified as a rectangle, we can actually scale the
    icon if we want by specifying a rectangle that is not 16 by 16.

    Drawing the small icon is simply a matter of calling CopyBits. We fill out
    the fields of a bitmap and then get the resource. Each icon is 32 bits
    long, so we point to 32*index bytes into the resource to get the icon we
    want. This gives us the base address for our icon's bitmap. Once that
    field is filled out, we call CopyBits.

*******************************************************************************/ 
void DrawSmallIcon(short id, short index, Rect* where)
{
    Handle sicnData;
    BitMap bm={  nil,                // baseAddr 
                 2,                  // rowBytes 
                 {0, 0, 16, 16}};    // bounds


    sicnData = GetResource('SICN', id); 
    if (sicnData != nil) {
        bm.baseAddr = (*sicnData) + (index*32);
        CopyBits(&bm, &qd.thePort->portBits, &bm.bounds, where, srcCopy, nil);
        ReleaseResource(sicnData);
    }
}


