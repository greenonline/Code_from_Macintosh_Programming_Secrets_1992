/* Listing 9-2. ZoomKey.c */

#include <Traps.h>
#include <SetupA4.h>
/*******************************************************************************

    Function prototypes.

*******************************************************************************/

void main();
pascal long MyMenuKey(short ch);

typedef pascal long (*MenuKeyProc) (short ch); 
MenuKeyProc oldMenuKeyAddress;

/*******************************************************************************

    main

    When INITs are loaded, the operating system jumps to the first byte in the
    resource to execute them. Under THINK C, the compiler adds a little
    snippet of code at the beginning of the INIT that jumps to main(), which 
    means that main() can appear anywhere in the source. Under MPW, this isn't
    done for you, so you have to make sure that main() is the first procedure
    in your source code.

    In order for INITs to stay resident, they must be loaded into the system 
    heap. Because they contain executable instructions that can be called at
    any time, the INIT resource should be locked. Since they'll remain locked
    during the entire time the computer is on, they should be loaded as low in
    the heap as possible to prevent fragmentation. Setting the resSysHeap and
    resLocked bits of our INIT resource will do this for us. However, none of
    this prevents our INIT resource from being removed from memory as soon as
    the file it's in is closed. Therefore, the first thing our main() routine
    does is detach the INIT from the resource file.

    Next, we plug ourselves into the MenuKey() routine. This is done in two
    steps. First, we get the address of the current MenuKey() routine and save
    it. Then we install the address of our own custom routine.

*******************************************************************************/
void main()
{
    Ptr ourAddress;

    RememberA0(); 
    SetUpA4();

    asm {
        move.l A0, ourAddress       ; AD points to the beginning of the
                                    ; INIT, courtesy of the THINK glue

        }
    DetachResource(RecoverHandle(ourAddress));

    oldMenuKeyAddress = (MenuKeyProc) GetToolTrapAddress(_MenuKey);
    SetToolTrapAddress((long) MyMenuKey, _MenuKey);
    RestoreA4 ();
}

/*******************************************************************************

    MyMenuKey

    This is the patch that gets called whenever any application calls MenuKey. 
    What we want to do here is detect whenever the user presses Command-space. 
    When they do, we zoom the frontmost window.

    Under the THINK environments, you are allowed to have global variables in 
    standalone code as long as you call SetUpA4() first. Since we've stored
    the address of the original MenuKey routine in a global, the first thing we 
    do is call SetUpA4(). Then we call the original MenuKey so that it can do 
    its processing.

    Next, it's time for our custom code to step in. When MenuKey returns, we 
    check for 3 conditions: 1) that the user pressed Command-space, 2) that
    Command-space doesn't correspond to anything in the current application's
    menus, and 3) that there is a front window. If we pass this battery of
    tests, we must check to see if that front window can be zoomed.
    Unfortunately, there isn't really a good way of doing this. While you can
    get zoomable windows by specifying a wDefProcID of 8 or 12, this is just a
    convention for the standard system WDEF. There's no guarantee that WDEFs
    written by Worldwide Washington windowsWare will use the same convention.
    Also, checking the window to see if its proc ID is 8 or 12 will fail if
    Apple defines proc IDs 9-11 to produce zoomable windows as well. On the
    other hand, they could as easily define proc IDs 9-11 to produce
    non-zoomable windows. Another solution is needed.

    The one we've chosen here is to call FindWindow at the location where we
    expect the zoom box to be. Obviously, this will fail if the WDEF
    programmer puts the zoom box in a different place, but it should at least
    work for all of Apple's WDEF designs.

    If FindWindow returns inZoomIn or inZoomOut, we know that the window has a
    zoom box. All that remains to be done is to zoom the window. Initially, you
    might think that a simple call to ZoomWindow will do the trick.
    Unfortunately, this is not so. Zooming a window involves more than just
    calling ZoomWindow. For instance, if there are any scroll bars in the
    window, they must be moved. No, a better solution is to fake a mouse click
    on the same spot that we used to test for a zoom box.

*******************************************************************************/

pascal long MyMenuKey(short ch)
{
    long result;
    WindowPtr theWindow;
    Rect *rectPtr;
    Point location;
    short part;
    EvQElPtr qElPtr;

    SetUpA4() ;

    result; oldMenuKeyAddress(ch);

    if ( ((char) ch == ' ')                        // If Command-Space ... 
      && (result == 0)                               // ... and not in menus ...
      && ((theWindow = FrontWindow()) != nil)) {      // ...and has front window ...

        rectPtr = &((**(((WindowPeek)theWindow)->strucRgn)).rgnBBox);
        location.h = rectPtr->right - 15;
        location.v = rectPtr->top + 10;

        part; FindWindow(location, &theWindow);

        if ((part == inZoomIn) || (part == inZoomOut)) {  // ... and it zooms.

            /*
                === Bad way ===

                GetPort(&oldPort);
                SetPort(theWindow);
                ZoomWindow(theWindow, part, TRUE);
                SetPort(oldPort);
            */

            PPostEvent(mouseDown, 0, &qElPtr);
            qElPtr->evtQWhere = location;
            PPostEvent(mouseUp, 0, &qElPtr);
            qElPtr->evtQWhere = location;
        }
    }

    RestoreA4(); 

    return result;
}