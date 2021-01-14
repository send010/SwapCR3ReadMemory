
#ifndef _TEST_H_    //——1
#define _TEST_H_    //——2
//加入各种变量，函数声明
#include <ntifs.h>

#define READCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ALL_ACCESS)
#define BASEDLL CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ALL_ACCESS)
//符号名
#define DRIVER_NAME L"\\Device\\fuck"
//设备名
#define SYM_NAME L"\\??\\fuck"
//数据传输结构
typedef struct DATA
{
	ULONG newpid;//记事本pid
	ULONG primarypid;//游戏pid
	ULONG size;//大小
}Data, *pData;
extern Data data ;
extern PDEVICE_OBJECT DeviceObject ;
extern UNICODE_STRING symName ;
extern ULONG64 cr3;
extern ULONG64 OldCr3;
extern char OldVadRot[0x45];
extern ULONG64 cr3win10;
extern ULONG64 OldCr3win10;
extern ULONG64 OldVadRotwin10;
extern ULONG64 OldCr3win102;
//初始化设备
extern ULONG initialization(PDRIVER_OBJECT pDriver);
extern ULONG Createcallback();
extern ULONG ReadMemoryPid(pData pdata, _Inout_ struct _IRP *Irp);
extern ULONG ReadMemoryNameDll( _Inout_ struct _IRP *Irp);
extern ULONG PsResumeProcess(PEPROCESS Process);
extern NTSTATUS Disaptch(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
	);

#endif              //——3