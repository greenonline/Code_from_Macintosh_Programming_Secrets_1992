/* Listing 8 -1 NeatStuff.h */

OSErr CopyAFile();
OSErr CopyFork(FSSpec* source, FSSpec* destination, short whichFork);
OSErr OpenFiles(FSSpec* source, FSSpec* destination, short* sourceRefNum, short* destinationRefNum, short whichFork);
OSErr DoOpenDF(const FSSpec *spec, char permission, short *refNum);
OSErr DoOpenRF(const FSSpec *spec, char permission, short *refNum);
OSErr TransferCatInfo(FSSpec* source, FSSpec* destination);
OSErr SetDestinationFileSize(short sourceRefNum, short destinationRefNum);

Boolean GetAFile(FSSpec* file);
Boolean GetADirectory (FSSpec* directory);
pascal Boolean GetDirFileFilter(ParmBlkPtr PB, StandardFileReply *replyPtr);
pascal short GetDirDlgHook(short item, DialogPtr theDialog, Boolean *reallyPressedSelect);
