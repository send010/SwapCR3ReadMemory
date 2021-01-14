#pragma once
#include <ntifs.h>

ULONG64 cr3 ;
ULONG64 OldCr3 ;
char OldVadRot[0x45] ;
ULONG64 cr3win10 ;
ULONG64 OldCr3win10 ;
ULONG64 OldVadRotwin10 ;
ULONG64 OldCr3win102 ;
UNICODE_STRING symName;
PDEVICE_OBJECT DeviceObject;

extern ULONG Win7Cr3date(PEPROCESS processnew, PEPROCESS processprimary);
extern ULONG Win10Cr3date(PEPROCESS processnew, PEPROCESS processprimary);
extern int PsSuspendProcess(PEPROCESS Process);
