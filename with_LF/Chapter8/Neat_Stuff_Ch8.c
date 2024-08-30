/* Listing 8-2 NeatStuff.c */

#include "NeatStuff.h"
#include "Script.h"

#define NIL 0L

const short kData =1;
const short kResource = 1;

const short kGetDirectoryDLOGID = 6042;
const short kDirectorySelectButton = 10;

Ptr gBufferPtr = NIL;
long gAmountInBuffer = 0;
const long kCopyBufferSize = 20 * 1024;


/********************************************************************************

 CopyAFile

 Queries the user for a single file and a destination for a copy of that
 file. The file is then copied to that new location. The copy is performed
 in small chunks that will handle any file, no matter how large. First, the
 data fork is copied. If that goes OK, the resource fork is copied. If
 there is no error, the directory information for that file (including the 
 Finder information) is transferred. However, any Finder comments
 associated with the file are not transferred, as they are kept in the
 Finder's private data file.

********************************************************************************/

OSErr CopyAFile()
{
    OSErr err;
    FSSpec source;
    FSSpec destination;

    //
    // Get the name of a file to copy and a destination for the copy.
    //

    if (!GetAFile(&source))
        return userCanceledErr;

    if (!GetAFile(&destination))
        return userCanceledErr;

    // 
    // If the destination is the same as the source, do nothing.
    //

    if ((source.vRefNum == destination.vRefNum) && (source.parID == destination.parID))
        return noErr;

    BlockMove(&source.name, &destination.name, source.name[0] + 1);

    //
    // Get a little buffer or reading and writing. Abort if there
    // are any errors.
    //

    gBufferPtr = NewPtr(kCopyBufferSize);
    if (MemError() != noErr) return MemError();
    if (gBufferPtr == NIL) return memFullErr;

    //
    // Copy the data and resource forks. Transfer the directory information.
    //

    err = CopyFork(&source, &destination, kData);
    if (err == noErr)
        err = CopyFork(&source, &destination, kResource);
    if (err == noErr)
        err = TransferCatInfo(&source, &destination);
    if (err != noErr)
        err = FSpDelete(&destination);

    DisposPtr (gBufferPtr);
    return err;
}


/********************************************************************************

    CopyFork

    Given a source file and a destination specification, copy the fork
    designated by whichFork. Open the source and destination files, creating 
    the destination if necessary. Set the size of the destination fork to the
    size of the source fork (we can quickly catch any "Out of disk space"
    errors this way). Then copy the fork in small chunks, read a little, then 
    write a little until the whole fork is copied. Close both files when we're 
    done. Leave it up to the caller to delete the destination on any errors.

********************************************************************************/

OSErr CopyFork(FSSpec* source, FSSpec* destination, short whichFork)
{
    OSErr err;
    Boolean done;
    short sourceRefNum;
    short destinationRefNum;

    err = OpenFiles(source, destination, &sourceRefNum, &destinationRefNum, whichFork);
    if (err != noErr) return err;

    err = SetDestinationFileSize(sourceRefNum, destinationRefNum);
    if (err != noErr) return err;

    done = FALSE;
    do {
        gAmountInBuffer = kCopyBufferSize;
        err = FSRead(sourceRefNum, &gAmountInBuffer, gBufferPtr);
        if (err == eofErr) {
            err = noErr;
            done = TRUE;
        }
        if (err == noErr)
            err = FSWrite(destinationRefNum, &gAmountInBuffer, gBufferPtr);
    } while (!done && (err ==noErr));
    FSClose(sourceRefNum);
    FSClose(destinationRefNum);

    return err;
}


/********************************************************************************

    OpenFiles

    Given FSSpecs for two files, open the designated for for both files. The
    source is assumed to exist. If the destination does not exist, it is 
    created. The refNums for both open files are returned. If there are any 
    errors, the refNums are not valid, and both files are closed.

    The code for opening the resource fork of  file is almost identical to the code 
    for opening a data fork. Therefore, at the start of the function,
    we set a procedure pointer to either a routine that opens the data fork or
    a routine that opens a resource fork, whichever is appropriate. Whatever
    routine is pointed to is called in the heart of the logic of the rest of
    the function as needed.

********************************************************************************/

OSErr OpenFiles(FSSpec* source, FSSpec* destination, short* sourceRefNum, short* destRefNum, short whichFork)
{
    typedef OSErr (*OpenProcPtr) (const FSSpec *spec, char permissions, short *refNum);

    OpenProcPtr openProc;
    OSErr err;

    if (whichFork == kData) {
        openProc = DoOpenDF;
    } else {
        openProc = DoOpenRF;
    }

    err = openProc(source, fsRdPerm, sourceRefNum);
    if (err != noErr)
        return err;

    err = openProc(destination, fsRdPerm, destRefNum);
    if (err == fnfErr) {
        err = FSpCreate(destination, 'Fo', 'Trot', smSystemScript);
        if (err == noErr)
            err = openProc(destination, fsRdWrPerm, destRefNum);
    }

    if (err != noErr)
        FSClose(*sourceRefNum);

    return err;
}

//
// Wrapper for FSpOpenDF and FSpOpenRF. we genericise the above routine by
// using a pointer to a function that opens either a data fork or a resource
// fork. However, FSpOpenDF and FSpOpenRF are implemented with inline instructions,
// so there is no function that we can point to. Neither can we use
// GetTrapAddress to get the address of these routines; they share a single
// trap, and hence, jump to the same address.The system knows which function
// you really want by a selector" value that is pushed on the stack by the 
// the inline instructions.
//
OSErr DoOpenDF(const FSSpec *spec, char permission, short *refNum)
{
    return FSpOpenDF((FSSpec*) spec, permission, refNum);
}

OSErr DoOpenRF(const FSSpec *spec, char permission, short *refNum)
{
    return FSpOpenRF((FSSpec*) spec, permission, refNum);
}

/********************************************************************************

    TransferCatInfo

    Transfer the directory information of the source file to the destination
    file. Call PBGetCatInfo on the source file. If that succeeds, call
    PBSetCatInfo on the destination.

********************************************************************************/

OSErr TransferCatInfo(FSSpec* source, FSSpec* destination)
{
    CInfoPBRec pb;
    OSErr err;

    pb.hFileInfo.ioVRefNum = source->vRefNum;
    pb.hFileInfo.ioNamePtr = source->name;
    pb.hFileInfo.ioFDirIndex = 0;
    pb.hFileInfo.ioDirID = source->parID;
    err = PBGetCatInfoSync(&pb);

    if (err == noErr){
        pb.hFileInfo.ioVRefNum = destination->vRefNum;
        pb.hFileInfo.ioNamePtr = destination->name;
        pb.hFileInfo.ioFDirIndex = 0;
        pb.hFileInfo.ioDirID = destination->parID;
        err = PBSetCatInfoSync(&pb);
    }
    return err;
}

/********************************************************************************

    SetDestinationFileSize

    Given the refNums to two open file forks, set the size of the destination
    to be the same as the source. Call GetEOF on the source file. If that
    succeeds, we use the result in a call to SetEOF or the destination file.

********************************************************************************/

OSErr SetDestinationFileSize(short sourceRefNum, short destinationRefNum)
{
    OSErr err;
    long forkSize;

    err = GetEOF(sourceRefNum, &forkSize);
    if (err == noErr)
        err = SetEOF(destinationRefNum, forkSize);

    return err;
}

/********************************************************************************

    GetAFile

    Get the FSSpec of a file to open. Call StandardGetFile to allow the user
    to select any file. We pass -1 for the numTypes parameter to tell
    Standard File to display all files. Copy the FSSpec of the selected file
    to the FSSpec supplied by the caller. Returns a Boolean indicating whether
    or not the user pressed Cancel. If the user did press Cancel, the FSSpec
    is not valid.

********************************************************************************/

Boolean GetAFile(FSSpec* file)
{
    SFTypeList types;                      // dummy - not used
    StandardFileReply reply;

    StandardGetFile(NIL,                   // FileFilterProc       file filter
                    -1,                    // short                numTypes
                   types,                  // SFTypeList           typeList
                   &reply);                // StandardFileReply    *reply


    if (reply.sfGood)
        BlockMove(&reply.sfFile, file, sizeof(FSSpec));

    return reply.sfGood;
}

/********************************************************************************

    GetADirectory

    Return a specification for the directory to which the file should be copied.
    Call CustomGetFile to allow the user to select a directory. If the user
    selects a directory, the vRefNum and dirID are copied into the caller's
    FSSpec; the name field is not used. A Boolean is returned indicating 
    whether or not the user selected Cancel. If the user did press Cancel,
    the FSSpec is not valid.

    The interface for selecting a directory is similar to the standard GetFile
    dialog box. Two items have been added: a Select button and a line of
    help text. The idea is that the user moves into the destination directory
    and then presses the Select button.

    To implement the Select a directory dialog box, we need to use a custom
    file filter and a custom dialog hook. The file filter is used to display
    folders only and to filter out any files. The dialog hook is used to 
    handle clicks on the Select button. If the user clicks on that button, we
    dismiss the dialog by fooling Standard File into thinking the Cancel button
    was clicked. However, we use the yourDataPtr parameter in CustomGetFile
    to pass a pointer to a Boolean. We use this Boolean to keep track of
    whether Cancel was really clicked or if we are just faking a click on
    Cancel. When CustomGetFile returns, we look at the Boolean to decide
    whether a directory was clicked or not.

********************************************************************************/

Boolean GetADirectory (FSSpec* directory)
{
    SFTypeList types;                      
    StandardFileReply reply;
    Boolean reallyPressedSelect = FALSE;
    Point where = {-1, -1};   // center it on the main screen

    CustomGetFile((FileFilterYDProcPtr) GetDirFileFilter,  
                    -1,                    // short                numTypes
                   types,                  // SFTypeList           typeList
                   &reply,                  // StandardFileReply    *reply
                   kGetDirectoryDLOGID,    // short                dlgID
                   where,                  //Point                 where,
                   (DlgHookYDProcPtr) GetDirDlgHook,
                   NIL,                    // ModalFilterYDProcPtr filterProc
                   NIL,                    // short                *activeList,
                   NIL,                    // ActiveYDProcPtr      activateProc,
                   (void*) &reallyPressedSelect); // void          *YourDataPtr);

    if (reallyPressedSelect)
        BlockMove(&reply.sfFile, directory, sizeof(FSSpec));

    return reallyPressedSelect;
}

pascal Boolean GetDirFileFilter(ParmBlkPtr PB, StandardFileReply *replyPtr)
{
    if ((PB->fileParam.ioFlAttrib & ioDirMask) != 0)
        return FALSE;                       // It's a directory; show it
    else
        return TRUE;                        // It's a file; don't show it
}

pascal short GetDirDlgHook(short item, DialogPtr theDialog, Boolean *reallyPressedSelect){
    if (item == kDirectorySelectButton){
        *reallyPressedSelect = TRUE;        // Keep track of what really happened
        item = sfItemCancelButton;          // Fake a click on the Cancel button
    }
    return item;
}

