#ifndef __TOOLS_H__H_
#define __TOOLS_H__H_
#include <ntifs.h>

#define WINXP					51
#define WINXP2600				512600

#define WIN7					61
#define WIN77600				617600
#define WIN77601				617601

#define	WIN8					62
#define	WIN89200				629200

#define WIN81					63
#define	WIN819600				639600


#define WIN10					100
#define WIN1010240				10010240
#define WIN1010586				10010586
#define WIN1014393				10014393


typedef struct _FindCode
{
	UCHAR code[200];
	ULONG64 len;
	int offset;
	ULONG64 lastAddressOffset;
}FindCode, *PFindCode;

void KernelSleep(ULONG64 ms, BOOLEAN alert);


UCHAR charToHex(UCHAR * ch);

void initFindCodeStruct(PFindCode findCode,PCHAR code,ULONG64 offset,ULONG64 lastAddrOffset);
ULONG64 findAddressByCode(ULONG64 beginAddr,ULONG64 endAddr,PFindCode  findCode,ULONG64 size);

#define FindAddressByCode(PFIND_CODE,SIZE) findAddressByCode(0xfffff80000000000,0xffffffffffffffff,PFIND_CODE,SIZE)

void ProbeForWriteHandle(PHANDLE handle);
#define ProbeForReadSmallStructure(p,size,testSize) ProbeForRead(p,size,testSize);

PKPCR  GetCurrentKpcr();


//ULONG64calcE8OrE9(ULONG64 oldAddr, ULONG64 newAddr);

#define  CALCJMPMACHIMECODE(XX,OO) calcE8OrE9(XX - 1,OO)


//链表操作
//ULONG64GetFunctionAddressByCodeAddress(ULONG64 addr);
//获取操作系统版本号
ULONG windowsDDK();
void writeProbOff();
void writeProbNo();
#endif