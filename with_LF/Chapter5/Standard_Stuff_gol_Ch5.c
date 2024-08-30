
/* template skeleton - Macintosh Programming Secrets */
/* Standard_stuff.c */

/* As the book listing has the headers: */
/*#include "Standard_stuff.h"*/
/*#include "Traps.h"*/
/* I think the order should be reversed */
#include "Traps.h"
//#include "Standard_stuff.h"

/* Headers required for compiling */

#include "types.h"
#include "Events.h"
//#include "Traps.h"
//#include <Memory.h>/* for THz heap zone ptr */
#include <OSUtils.h> /* for GetTrapAddress() */
#include "Windows.h"
#include "Menus.h"
#include "Values.h"
#include "AppleEvents.h"
//#include "MacTypes.h"

/* Headers required for building */
#include "TextEdit.h"
#include "Dialogs.h"
#include "DiskInit.h"
//#include "Disks.h"
#include "Files.h"
#include "Fonts.h"
#include "Desk.h"
//#include "Windows.h"
//#include "StandardFile.h"
//#include "OSEvents.h"
//#include "OSUtils.h"
//#include "Packages.h"
//#include "Processes.h"
//#include "QuickDraw.h"
#include "ToolUtils.h"

/* This needs to be at the end */
#include "Standard_Stuff_Ch5.h"

#define __chapter5__

/********************************************************************************

    The "g" prefix is used to emphasise that a variable is global.

********************************************************************************/


SysEnvRec gMac;                    /* gMac is used to hold the result of a
                                      SysEnvirons call, This makes it convenient
                                      for any routine to check the environment. */

Boolean gQuit;                     /* We set this to TRUE when the user selects
                                      Quit from the File menu. Out main event
                                      loop exists when gQuit is TRUE. */

Boolean gInBackground;             /* gInBackground is maintained by our OSEvent
                                      handling routines. Any part of the program
                                      can check it to find out if it is currently
                                      in the background. */

/* Note that we *can* actually handle this better! (from Creation.p) */
Boolean gHasWaitNextEvent;

/********************************************************************************

    Define HiWrd and LoWrd for efficiency.

********************************************************************************/

#define HiWrd(aLong) (((aLong) >> 16) & 0xFFFF)
#define LoWrd(aLong) ((aLong) & 0xFFFF)

/********************************************************************************

    main

    Entry point for our program. We initialise the Toolbox, make sure we are
    running on a sufficiently brawl machine, and put up the menu bar. Finally,
    we start polling for events  and handling them by entering our main event
    loop.

********************************************************************************/


//#define TRUE true
//#define FALSE false 
#define NIL nil

main()
{
    /* If you have stack requirements that differ from the default,
       then you could use SetAppLimit to increase StackSpace at 
       this point, before calling MaxApplZone. */

  MaxApplZone();                   /* Expand the heap so code segments load
                                      at the top. */
  InitToolbox();                   /* Initialise the program */
  MainEventLoop();                 /* Call the main event loop */
}

/********************************************************************************

    InitToolBox

    Set up the whole world, including global variables, Toolbox managers,
    and menus.

********************************************************************************/


void InitToolbox(){
  Handle menuBar;
  EventRecord event;
  short count;
  
  /*gInBackground = FALSE;*/
  gInBackground = false;
  /*gQuit = FALSE;*/
  gQuit = false;

  InitGraf((Ptr) &qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(NIL);
  InitCursor();

  /* This next bit of code waits until MultiFinder brings our application
     to the front.  The gives us a better effect if we open a window at
     startup. */

  for (count =1; count <= 3; ++count)
  EventAvail(everyEvent, &event);

  SysEnvirons(curSysEnvVers, &gMac);

  if (gMac.machineType < 0)
    DeathAlert(errWimpyROMs);

  if (gMac.systemVersion < 0x0600)
    DeathAlert(errWimpySystem);

  if (!TrapExists(_WaitNextEvent))
    DeathAlert(errWeirdSystem);

  /* Note that we *can* actually handle this better! (from Creation.p) */
  /* gHasWaitNextEvent := TrapAvailable (_WaitNextEvent, ToolTrap);    */

  /* gHasWaitNextEvent = TrapAvailable (_WaitNextEvent, ToolTrap);    */
  /* or */
  /* gHasWaitNextEvent = TrapExists(_WaitNextEvent);    */

  menuBar = GetNewMBar(rMenuBar);           /* Read menus into menu bar */
  if (menuBar == NIL)
    DeathAlert(errNoMenuBar);
  SetMenuBar(menuBar);                      /* Install menus */
  DisposHandle(menuBar);
  AddResMenu(GetMHandle(mApple), 'DRVR');   /* Add DA names to Apple menu */
  DrawMenuBar();
}

/********************************************************************************

    MainEventLoop

    Get events forever and handle them by calling HandleEvent. First, call
    DoAdjustCursor to set our cursor shape and to set the cursor region. We
    then call WaitNextEvent() to get the event. This is OK, because we know
    we're running on System 6.0 or later by this time. If we got an event, we
    handle it by calling HandleEvent().  But before doing that, we call 
    DoAdjustCursor again in case our application had fallen asleep under
    MultiFinder.

********************************************************************************/


void MainEventLoop(){
  RgnHandle cursorRgn;
  Boolean gotEvent;
  EventRecord event;
  Point mouse;

  cursorRgn = NIL;
  while (!gQuit) {
    gotEvent = WaitNextEvent(everyEvent, &event, MAXLONG, cursorRgn);
    if (gotEvent){
      HandleEvent(&event);
    }

    /* Note that we *can* actually handle this better! (from Creation.p) */
    /* Alternative (from Creation.p):
      IF gHasWaitNextEvent THEN
        ignoreResult := WaitNextEvent (everyEvent, myEvent, GetSleep,cursorRgn)
      ELSE 
        BEGIN
          SystemTask;
          gotEvent := GetNextEvent(everyEvent, myEvent);
         END;
    */
    /*
    if  (gHasWaitNewEvent)
      gotEvent = WaitNextEvent(everyEvent, &event, MAXLONG, cursorRgn);
    else {
      SystemTask;
      gotEvent = GetNextEvent(everyEvent, &event);
    }
    if (gotEvent){
      HandleEvent(&event);
    }
    */

  }
}

/********************************************************************************

    HandleEvent

    Do the right thing for an event. Determine what kind of event it is and
    call the appropriate routines

********************************************************************************/


void HandleEvent(EventRecord *event){
  switch (event->what){
    case mouseDown:
      HandleMouseDown(event);
      break;
    case keyDown:
    case autoKey:
      HandleKeyPress(event);
      break;
    case activateEvt:
      HandleActivate(event);
      break;
    case updateEvt:
      HandleUpdate(event);
      break;
    case diskEvt:
      HandleDiskInsert(event);
      break;
    case osEvt:
      HandleMultiFinder(event);
      break;
  }
}

/********************************************************************************

    HandleActivate

    This is called when a window is activated or deactivated. In this sample,
    the Window MAnager's  handing of activate and deactivate events is
    sufficient. Other applications may have TextEdit records, controls, lists,
    etc., to activate/deactivate.

********************************************************************************/


void HandleActivate(EventRecord *event){
  WindowPtr theWindow;
  Boolean becomingActive;

  theWindow = (WindowPtr) event->message;
  becomingActive = (event->modifiers &activeFlag) != 0;
  if (IsAppWindow(theWindow)) {
    DrawGrowIcon(theWindow);
    /* DoActivateWindow(theWindow, becomingActive) */
  }
}

/********************************************************************************

    HandleDiskInsert 

    Called when we get a disk-inserted event. Check the upper word of the
    event message; if it's nonzero, then a bad disk was inserted, and it
    needs to be formatted.

********************************************************************************/


void HandleDiskInsert(EventRecord *event){
  Point aPoint ={100, 100};
 if (HiWrd(event->message) != noErr){
    (void) DIBadMount(aPoint, event->message);
  }
}

/********************************************************************************

    HandleKeyPress

    The user pressed a key. What are you going to do about it?

********************************************************************************/

void HandleKeyPress(EventRecord *event){
  char key;

  key = event->message & charCodeMask;
  if (event->modifiers &cmdKey){             /* Command key down? */
    AdjustMenus();                           /* Set up menu items properly */
    HandleMenuCommand(MenuKey(key));
  } else{
  /* Do key event */;
  }
}

/********************************************************************************

    HandleMouseDown 

    Called to handle mouse clicks. The user could have clicked anywhere, so
    let's first find out where by calling FindWindow. That returns a number
    indicating where in the screen the mouse was clicked. "Switch" on that
    number and call the appropriate routine.

********************************************************************************/


void HandleMouseDown(EventRecord *event){
  long newSize;
  Rect growRect;
  WindowPtr theWindow; /*= (WindowPtr);*/ /* ??? */
  short part = FindWindow(event->where, &theWindow);

  switch (part){
    case inMenuBar:        /* Process a mouse menu command (if any) */
      AdjustMenus();
      HandleMenuCommand(MenuSelect(event->where));
      break;

    case inSysWindow:      /* Let the system handle the mouse down */
      SystemClick(event, theWindow);
      break;

    case inContent:
      if (theWindow != FrontWindow())
        SelectWindow(theWindow);
      else
        /* DoContentClick(event)  */;
      break;

    case inDrag:             /* Pass screenBits.bounds to get all gDevices */
      DragWindow(theWindow, event->where, &qd.screenBits.bounds);
      break;

    case inGrow:
      growRect = qd.screenBits.bounds;
      growRect.top = growRect.left = 80;   /* Arbitrary minimum size */
      newSize=GrowWindow(theWindow, event->where, &growRect);
      if (newSize != 0){
        InvalidateScrollbars(theWindow);
        /*SizeWindow(theWindow, LoWrd(newSize), HiWrd(newSize), TRUE);*/
        SizeWindow(theWindow, LoWrd(newSize), HiWrd(newSize), true);
        InvalidateScrollbars(theWindow);
      }

      break;

    case inGoAway:
      if (TrackGoAway(theWindow, event->where)){
        CloseAnyWindow(FrontWindow());
      }
      break;
    case inZoomIn:
    case inZoomOut:
      if (TrackBox(theWindow, event->where, part)){
        SetPort(theWindow);
        EraseRect(&theWindow->portRect);
        /*ZoomWindow(theWindow, part, TRUE);*/
        ZoomWindow(theWindow, part, true);
        InvalRect(&theWindow->portRect);
      }
      break;
  }
}

/********************************************************************************

    HandleOSEvent 

    Deal with OSEvents (formerly, app4Events). These are messages that
    MultiFinder--known as the Process Manger under System 7.0--sends to
    us. Here, we deal with the suspend and resume message.

********************************************************************************/



void HandleMultiFinder(EventRecord *event){
  switch((event->message>>24)&0x00FF){            /* High byte of message */
    case suspendResumeMessage:

        /* In our SIZE resource, we say that we are MultiFinder aware.
           This means that we take on the responsibility of activating
           and deactivating our own windows on suspend/resume events. */

      gInBackground = (event->message & resumeFlag) == 0;
      if (FrontWindow()){
        DrawGrowIcon(FrontWindow());
        /*
        DoActivateWindow(FrontWindow(), !gInBackground); 
        */
    }
    break;
    case mouseMovedMessage:
      break;
  }
}

/********************************************************************************

    HandleUpdate 

    This is called when an update event is received for a window. It calls
    DoUpdateWindow to draw the contents of an application window. As an
    efficiency measure that does not have to be followed, it draws the drawing
    routine only if the visRgn is nonempty. This will handle situations where
    calculations for drawing or drawing itself is very time consuming

********************************************************************************/


void HandleUpdate(EventRecord *event){
  WindowPtr theWindow = (WindowPtr) event->message;
  if (IsAppWindow(theWindow)){
    BeginUpdate(theWindow);             /* This sets up the visRgn */
    if (!EmptyRgn(theWindow->visRgn)){  /* Draw if updating needs to be done */
      SetPort(theWindow);
      EraseRgn(theWindow->visRgn);
      /* DoUpdateWindow(event). */;
      DrawGrowIcon(theWindow);
    }
    EndUpdate(theWindow);
  }
}

/********************************************************************************

    AdjustMenus 

    Enable and disable menus based on the current state. The user can only
    select enabled menu items. We set up al the menu items before calling
    MenuSelect or MenuKey, since these are the only times that a menu item can
    be selected. Note that MenuSelect is the only time the user will see
    menu items. Tis approach to deciding what enable/disable state a menu
    item has has the advantage of concentrating all the decision making in one
    routine, as opposed to being spread throughout the application. Other
    application designs may take different approach that is just as valid.

********************************************************************************/


void AdjustMenus(){

  WindowPtr window;
  MenuHandle menu;

  window= FrontWindow();
  menu=GetMHandle(mFile);
  //if (IsDAWindow(window))
  if (window != NIL)
    EnableItem(menu, iClose);
  else
    DisableItem(menu, iClose);

  menu = GetMHandle(mEdit);
  if (IsDAWindow(window)) {        /* A desk accessory might need the edit menu... */
    EnableItem(menu, iUndo);
    EnableItem(menu, iCut);
    EnableItem(menu, iCopy);
    EnableItem(menu, iClear);
    EnableItem(menu, iPaste);
  } else {                         /* but we don't use it. */
    DisableItem(menu, iUndo);
    DisableItem(menu, iCut);
    DisableItem(menu, iCopy);
    DisableItem(menu, iClear);
    DisableItem(menu, iPaste);
  }
}

/********************************************************************************

    HandleMenuCommand

    This is called when an item is chosen from the menu bar (after calling
    MenuSelect or MenuKey). It performs the right operation for each command.
    It is good to have both the result of MenuSelect and MenuKey go to one
    routine like this to keep everything organised.

********************************************************************************/


void HandleMenuCommand(long menuResult){
  /*long menuResult;*/

  short menuID;                      /* The resource ID of the selected menu */
  short menuItem;                    /* The item number of the selected menu */
  Str255 daName;
  menuID = HiWrd(menuResult);
  menuItem = LoWrd(menuResult);

  switch (menuID){
    case mApple:
      switch (menuItem){
        case iAbout:
          /*(void) Alert(rAboutAlert, NIL);*/
          (void) Alert(rAboutAlert, nil);
          break;
        default:                    /* All non-About items in this menu are DAs */
          GetItem(GetMHandle(mApple), menuItem, daName);
          (void) OpenDeskAcc(daName);
         break;
      }
      break;

    case mFile:
      switch (menuItem){
        case iNew:

#ifdef __chapter5__

  /* BEGIN: Chapter 5 changes START */
          DoNewWindow(); /* Added for Chapter 5 */
  /* END: Chapter 5 changes END */

#endif

          break;
        case iClose:
          if (IsDAWindow(FrontWindow())){
            CloseDeskAcc(((WindowPeek)FrontWindow())->windowKind);
          } else {
            DisposeWindow(FrontWindow());
          }
          break;
        case iQuit:
          /*gQuit = TRUE;*/
          gQuit = true;
         break;
      }
      break;

    case mEdit:
      switch (menuItem){
        /* Call SystemEdit for DA editing and MultiFinder */
        /* since we don't do any editing */
        case iUndo:
        case iCut:
        case iCopy:
        case iPaste:
        case iClear:
          (void) SystemEdit(menuItem-1);
          break;
      }
      break;

#ifdef __chapter5__

  /* BEGIN: Chapter 5 changes START */
    case mDialog:
      switch (menuItem){
          case iShowDialog:
              ShowDialog();
              break;
          case iShowSavingDialog:
              ShowSavingDialog();
              break;
      }
      break;
  /* END: Chapter 5 changes END */

#endif

  }
  HiliteMenu(0);    /* Unhilite what MenuSelect and MenuKey hilited */
}

/********************************************************************************

    CloseAnyWindow

    Close the given window in an manner appropriate for that window. If the
    window belongs to a DA, we call CloseDeskAcc. For dialogs, we simply hide
    the window. If we had any document windows, we would probably call either
    DisposeWindow or CloseWindow after disposing of any document data and/or
    controls.

********************************************************************************/

void CloseAnyWindow(WindowPtr window)
{
    if (IsDAWindow(window)) {
        CloseDeskAcc(((WindowPeek) window) ->windowKind);
    } else if (IsDialogWindow(window)) {
        HideWindow(window);
    } else if (IsAppWindow(window)) {
        /* Do something significant for document windows. */
    }
}

/********************************************************************************

    DeathAlert

    Display an alert that tells the user an error occurred, then exit the
    program. The routine is used as a n ultimate bail-out for serious errors
    that prohibit the continuation of the application. The error number is
    used to index an 'STR#' resource so that a relevant message can be
    displayed.

********************************************************************************/

void DeathAlert(short errNumber)
{
  short itemHit;
  Str255 theMessage;

  SetCursor(&qd.arrow);
  GetIndString(theMessage, rErrorStrings, errNumber);
  /*ParamText(theMessage, nil, nil, nil);*/
  ParamText(theMessage, NIL, NIL, NIL);
  /*itemHit = StopAlert(rErrorAlert, NIL);*/
  itemHit = StopAlert(rErrorAlert, nil);
  ExitToShell();
}

/********************************************************************************

    IsAppWindow

    Check to see if a window belongs to the application. If the window pointer
    passed was NIL, then it could not be an application window. WindowKinds
    that are negative belong to the system and windowKinds less that userKind
    are reserved by Apple except from windowKinds equal to dialogKind, which
    mean it is a dialog

********************************************************************************/


Boolean IsAppWindow(WindowPtr window){
  short windowKind;

  /*if (window == NIL)*/
  if (window == nil)
    /*return FALSE;*/
    return false;
  else {
    windowKind = ((WindowPeek) window) ->windowKind;
    return ((windowKind >= userKind) || (windowKind == dialogKind));
  }
}

/********************************************************************************

    IsDAWindow

    Check to see if a window belongs to a desk accessory. It belongs to a DA
    if the windowKind field of the window record is negative.

********************************************************************************/


Boolean IsDAWindow(WindowPtr window){

  if (window==NIL)
    return FALSE;
  else
    return (((WindowPeek) window) ->windowKind < 0);
}

/********************************************************************************

    IsDialogWindow

    Check to see if a window is a dialog window. We can determine this by
    checking to see if the windowKind field is equal to dialogKind.

********************************************************************************/
Boolean IsDialogWindow(WindowPtr window)
{
    if (window==NIL)
        return FALSE;
    else
        return (((WindowPeek)window)->windowKind == dialogKind);
}

/********************************************************************************

    InvalidateScrollBars

    Call InvalRect on the right and bottom edges of a window. This routine is
    called during the resizing of a window to take care of the scroll bar lines
    and grow icon.

********************************************************************************/


void InvalidateScrollbars(WindowPtr theWindow){
  Rect tempRect;

  SetPort(theWindow);

  tempRect=theWindow->portRect;
  tempRect.left = tempRect.right-15;
  InvalRect(&tempRect);
  EraseRect(&tempRect);

  tempRect=theWindow->portRect;
  tempRect.top = tempRect.bottom-15;
  InvalRect(&tempRect);
  EraseRect(&tempRect);

}

/********************************************************************************

    TrapExists

    Check to see if a given trap is implemented. The recommended approach to
    see if a trap is implemented is to see if the address of the trap routine
    is the same as the address of the the unimplemented trap. However, we must
    also makes sure the the trap is contained in the trap table on the machine
    we're running on. Not all Macintoshes have the same size trap tables. We
    call NumToolboxTraps to finds out the size of the table. If the trap we are
    examining falls off the end, then we treat it automatically as not being
    implemented.

********************************************************************************/


Boolean TrapExists(short theTrap){

  TrapType theTrapType;

  theTrapType = GetTrapType(theTrap);
  if ((theTrapType == ToolTrap) && ((theTrap &= 0x07FF) >= NumToolboxTraps()))
    return FALSE;
  else
    return (NGetTrapAddress(_Unimplemented, ToolTrap) != NGetTrapAddress(theTrap, theTrapType));
}

/********************************************************************************

    GetTrapType

    Check the bits of a trap number to determine its type. If bit 11 is set,
    it's a Toolbox trap. Otherwise, it's an OS trap.

********************************************************************************/


TrapType GetTrapType(short theTrap) {
  if ((theTrap & 0x0800 )==0)                /* Per D.A. */
    return(OSTrap);
  else
    return(ToolTrap);
}

/********************************************************************************

    NumToolboxTraps

    Find the size of the Toolbox trap table. This can be either 0x0200 or
    0x0400 bytes, depending on which Macintosh we're running on. We determine
    the size by taking advantage of an anomaly of the smaller trap table: any
    entries that fall beyond the end of the table are mirrored back down into
    the lower part. For example, on a large table, trap numbers A86E and AA6E
    correspond to different routines. However, on a small table, they
    correspond to the same routine. By checking the addresses of these
    routines, we can determine the size of the table.

********************************************************************************/


short NumToolboxTraps(void)
{
  if (NGetTrapAddress(0xA86E, ToolTrap) == NGetTrapAddress(0xAA6E, ToolTrap))
    return (0x200);
  else
    return(0x400);
}










