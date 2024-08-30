/* Listing 9-4. Generic LDEF.c */
/*******************************************************************************

    Generic LDEF

    This short code segment is the front end for an LDEF. Normally, LDEFs are 
    standalone pieces of code developed separately from the rest of the
    application. While this is not really a bad idea, there are several
    disadvantages with it:

    - You don't have access to your application's global variables. Your
      compiled code can only access a common set of global variables if all
      the parts are linked at the same time. Since the LDEF is linked
      separately from the application, you won't be able to access the
      application's globals. On the other hand, some would say that a module
      such as an LDEF should be independent of an application's
      implementation. If so, then an LDEF shouldn't need to access the
      application's variables.

    - If you program in a THINK development system and are developing your
      LDEF and application in parallel, you will have to jump continually back
      and forth between projects. You will also have to make sure continually
      that your LDEF is incorporated into the right resource file.

    - Source-level debugging is more difficult under MPW and impossible
      under THINK.

    To solve these problems, we use a front end for the code used to display
    the list. All this LDEF does is call the real code back in the
    application. When the list record is created, the application will be 
    expected to store the address of the real code in the list record's refCon 
    field. Our front end will fetch that address and jump to it.

    The disadvantage of this approach is that the application is not able to 
    set the refCon before the List Manager calls the LDEF with the LInitMsg. 
    This means that any initialization normally done at that time will have 
    to be done later. In our sample application, we show how to do that.

    Note that when we call the real code in the application, we duplicate the 
    parameters passed in to us. This isn't wrong or bad or anything. But it
    is inefficient. Ideally, we would like just to jump to the real routine, 
    letting it use the same parameters passed to us. If we were writing in 
    assembly, this would be simple to do. However, when writing in C, we have 
    to deal with some of the little extras a high-level language gives us. 
    Although THINK C has an inline assembly and we could generate the 
    instructions to jump to the application code, we have to contend with any 
    LINK statements or register-saving instructions the compiler generates for 
    our routine. These alter the stack pointer and would have to be


    compensated for before jumping back into the application. By examining the
    output of the compiler, we could do this. In fact, early versions of this
    LDEF did do this. However, this approach is highly dependent on the
    compiler you are using. Since, in the Galactic Order of Things, we don't
    really gain a whole lot by doing a JMP back into the application, we opted
    for compatibility and easy maintenance instead. The result is that we end
    up with a 64-byte routine instead of a 30-byte routine. Big deal.

*******************************************************************************/

typedef pascal void (*LDEFProc)(short lMessage, Boolean lSelect, Rect* lRect, Cell lCell, short lDataOffset, short lDataLen,
ListHandle lHandle);

pascal void main(short lMessage, Boolean lSelect, Rect* lRect, Cell lCell,
short lDataOffset, short lDataLen, ListHandle lHandle)
{
    long refCon;

    refCon = (**lHandle).refCon;
    if (refCon != 0L) {
        ((LDEFProc) refCon) (lMessage, lSelect, lRect, lCell, lDataOffset, lDataLen, lHandle);
    }
}


