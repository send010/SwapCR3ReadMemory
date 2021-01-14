#include "Comm.h"
#include "tools.h"
#include "Readingwriting.h"
#include "FacilitateDll.h"
Data data = {0};
ULONG64 cr3=0;
ULONG64 OldCr3=0;
char OldVadRot[0x45] = {0};
ULONG64 cr3win10=0;
ULONG64 OldCr3win10=0;
ULONG64 OldVadRotwin10=0;
ULONG64 OldCr3win102=0;
UNICODE_STRING symName;
PDEVICE_OBJECT DeviceObject;

//进程关闭回调win7
void PcreateProcessNotifyRoutineWin7(
	HANDLE ParentId,
	HANDLE ProcessId,
	BOOLEAN Create
	)
{

	if (Create == 0)
	{

		if (data.primarypid == ProcessId)
		{
			PEPROCESS processnew = NULL;
			//获取记事本process
			PsLookupProcessByProcessId((HANDLE)data.newpid, &processnew);
			//cr3
			*(ULONG64*)((ULONG64)processnew + 0x28) = OldCr3;
			//处理vad
			memcpy((void*)((ULONG64)processnew + 0x448), OldVadRot, 0x40);
			KernelSleep(1000, 0);
			PsResumeProcess(processnew);
		}

	}
}
//进程关闭回调win10
void PcreateProcessNotifyRoutineWin10(
	HANDLE ParentId,
	HANDLE ProcessId,
	BOOLEAN Create
	)
{

	if (Create == 0)
	{

		if (data.primarypid == ProcessId)
		{
			PEPROCESS processnew = NULL;
			PEPROCESS processprimary = NULL;
			//获取游戏process
			PsLookupProcessByProcessId((HANDLE)data.primarypid, &processprimary);
			//获取记事本process
			PsLookupProcessByProcessId((HANDLE)data.newpid, &processnew);
			//cr3
			*(ULONG64*)((ULONG64)processnew + 0x28) = OldCr3win10;
			//cr3内核
			*(ULONG64*)((ULONG64)processnew + 0x280) = OldCr3win102;
			//处理vad
			*(ULONG64*)((ULONG64)processnew + 0x658) = OldVadRotwin10;
			PsResumeProcess(processnew);
		
		}

	}
}
//创建回调
ULONG Createcallback()
{
	ULONG ddk = windowsDDK();
	if (ddk == 6)
	{
		NTSTATUS routine = PsSetCreateProcessNotifyRoutine(PcreateProcessNotifyRoutineWin7, 0);
		if (!NT_SUCCESS(routine))
		{
			KdPrint(("PsSetCreateProcessNotifyRoutine\r\n"));
		}
	}
	if (ddk == 10)
	{
		NTSTATUS routine = PsSetCreateProcessNotifyRoutine(PcreateProcessNotifyRoutineWin10, 0);
		if (!NT_SUCCESS(routine))
		{
			KdPrint(("PsSetCreateProcessNotifyRoutine\r\n"));
		}
	}
}
//创建设备
ULONG initialization(PDRIVER_OBJECT pDriver)
{
	

	//符号名
	UNICODE_STRING deviceName = { 0 };
	//设备名
	
	RtlInitUnicodeString(&deviceName, DRIVER_NAME);
	DeviceObject = NULL;
	//创建设备对象
	NTSTATUS status = IoCreateDevice(pDriver, NULL, &deviceName, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	
	if (!NT_SUCCESS(status))
	{
		KdPrint(("创建对象失败\r\n"));
		return 0;
	}
	RtlInitUnicodeString(&symName, SYM_NAME);
	//符号与设备关联
	status = IoCreateSymbolicLink(&symName, &deviceName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DeviceObject);
		KdPrint(("创建符号失败\n"));
		return 0;
	}
	DeviceObject->Flags |= DO_BUFFERED_IO;
	//下一步
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		//创建设备函数
		pDriver->MajorFunction[i] = Disaptch;
	}
	//设备读写
	pDriver->MajorFunction[IRP_MJ_CREATE] = Disaptch;
	pDriver->MajorFunction[IRP_MJ_CLOSE] = Disaptch;
}
//通信函数
NTSTATUS Disaptch(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
	)
{
	PIO_STACK_LOCATION stack;

	stack = IoGetCurrentIrpStackLocation(Irp);

	switch (stack->MajorFunction)
	{

	case IRP_MJ_CREATE:
	{
		DbgPrint("设备已打开");
		break;
	}

	case IRP_MJ_CLOSE:
	{
		DbgPrint("设备已关闭");
		break;
	}
	case IRP_MJ_DEVICE_CONTROL:
	{

		switch (stack->Parameters.DeviceIoControl.IoControlCode)
		{
		case READCODE:
		{
			
			//获取pid
			ReadMemoryPid(&data, Irp);
		      
			break;
		}
		case BASEDLL:
			//获取dll基址
			ReadMemoryNameDll(Irp);
			break;
		}
		break;
	}


	}

	return 1;
}

//读pid函数
ULONG ReadMemoryPid(pData pdata, _Inout_ struct _IRP *Irp)
{
	PEPROCESS processnew= NULL;
	PEPROCESS processprimary = NULL;
	//读出数据
	memcpy(pdata, Irp->AssociatedIrp.SystemBuffer, sizeof(Data));
	//获取游戏process
	PsLookupProcessByProcessId((HANDLE)pdata->primarypid ,&processprimary);
	//获取记事本process
	PsLookupProcessByProcessId((HANDLE)pdata->newpid, &processnew);
	//判断是否win7-win10
	ULONG ddk = windowsDDK();
	//win7
	if (ddk==6)
	{
		Win7Cr3date(processnew, processprimary);
	}
	//win10
	if (ddk==10)
	{
	
		Win10Cr3date(processnew, processprimary);
	}
	// 指示完成此IRP
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return 1;
}
ULONG ReadMemoryNameDll( _Inout_ struct _IRP *Irp)
{
	
	PEPROCESS processprimary = NULL;
	//获取游戏process
	PsLookupProcessByProcessId(data.primarypid, &processprimary);
	wchar_t arr[20] = {0};
	memcpy(arr, Irp->AssociatedIrp.SystemBuffer, sizeof(arr));
	ULONG64 dllbese =ObtainDll(processprimary, arr);
	if (dllbese==0)
	{
		KdPrint(("获取模块基址失败\r\n"));
	}
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = sizeof(ULONG64);
	memcpy(Irp->AssociatedIrp.SystemBuffer, &dllbese, sizeof(ULONG64));
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return 1;
}