#include "FacilitateDll.h"


ULONG ObtainDll(ULONG64 eprocess, wchar_t* Lrstring)
{
	ULONG64 baseaddr;
	PPEB_LDR_DATA pLdr;
	PLDR_DATA_TABLE_ENTRY pLdrData;
	ULONG64 peb;
	//初始化模块名
	UNICODE_STRING rstring;
	RtlInitUnicodeString(&rstring, Lrstring);
	//判断版本号
	RTL_OSVERSIONINFOEXW OSVersion = { 0 };
	OSVersion.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	RtlGetVersion((PRTL_OSVERSIONINFOW)&OSVersion);
	if (OSVersion.dwMajorVersion==6)
	{
		peb = *(ULONG64*)((ULONG64)eprocess + 0x338);
	}
	if (OSVersion.dwMajorVersion == 10)
	{
		peb = *(ULONG64*)((ULONG64)eprocess + 0x3f8);
	}
	KAPC_STATE prkapc = { 0 };
	KeStackAttachProcess(eprocess, &prkapc);
	pLdr = *(ULONG64*)(peb + 0x18);
	pLdrData = (PLDR_DATA_TABLE_ENTRY)pLdr->InLoadOrderModuleList.Flink;
	while ((pLdr->InLoadOrderModuleList.Flink) != (pLdrData->InLoadOrderLinks.Flink))
	{
		if (!RtlCompareUnicodeString(&pLdrData->BaseDllName,&rstring, 1))
		{
			KdPrint(("锁定修复模块名字 %s\r\n", pLdrData->BaseDllName.Buffer));
			baseaddr = pLdrData->DllBase;
			KeUnstackDetachProcess(&prkapc);
			return baseaddr;
		}
		pLdrData = (PLDR_DATA_TABLE_ENTRY)(pLdrData->InLoadOrderLinks.Flink);
	}
	KeUnstackDetachProcess(&prkapc);
	return 0;
}