#include "Readingwriting.h"
#include "Comm.h"
//切换cr3
ULONG Win7Cr3date(PEPROCESS processnew, PEPROCESS processprimary)
{
	//挂起记事本进程
	NTSTATUS arr = PsSuspendProcess(processnew);
	cr3 = (ULONG64)processnew + 0x28;
	if (NT_SUCCESS(arr))
	{
		//保存原有的变量
		OldCr3 = *(ULONG64*)((ULONG64)processnew + 0x28);
		memcpy(OldVadRot, (void*)((ULONG64)processnew + 0x448), 0x40);
		//傀儡进程
		//cr3
		*(ULONG64*)((ULONG64)processnew + 0x28) = *(ULONG64*)((ULONG64)processprimary + 0x28);
		//处理vad
		memcpy((void*)((ULONG64)processnew + 0x448), (void*)((ULONG64)processprimary + 0x448), 0x40);
		return 1;
	}
	return 0;
}
ULONG Win10Cr3date(PEPROCESS processnew, PEPROCESS processprimary)
{
	//挂起记事本进程
	NTSTATUS arr = PsSuspendProcess(processnew);
	cr3win10 = (ULONG64)processnew + 0x28;
	if (NT_SUCCESS(arr))
	{
		//保存原有的变量
		OldCr3win10 = *(ULONG64*)((ULONG64)processnew + 0x28);
		OldCr3win102 = *(ULONG64*)((ULONG64)processnew + 0x280);
		OldVadRotwin10 = *(ULONG64*)((ULONG64)processnew + 0x658);
		//傀儡进程
		//cr3
		*(ULONG64*)((ULONG64)processnew + 0x28) = *(ULONG64*)((ULONG64)processprimary + 0x28);
		//处理内核层cr3
		*(ULONG64*)((ULONG64)processnew + 0x280) = *(ULONG64*)((ULONG64)processprimary + 0x280);
		//vad
		*(ULONG64*)((ULONG64)processnew + 0x658) = *(ULONG64*)((ULONG64)processprimary + 0x658);
		return 1;
	}
	return 0;
}