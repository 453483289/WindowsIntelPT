/**********************************************************************
 *  Intel Processor Trace Stupid Test Driver
 *	Filename: TestDriverIo.h
 *	Test BUGGED Driver I/O functions and data structures definitions
 *  Last revision: 11/29/2016
 *
 *  Copyright© 2016 Andrea Allievi
 *  All right reserved
 **********************************************************************/
#pragma once

typedef struct _KERNEL_MODULE {
	LPVOID lpStartAddr;
	DWORD dwSize;
	TCHAR modName[100];
}KERNEL_MODULE, *PKERNEL_MODULE;

// Search a particular kernel module in memory and return the associated structure
#define IOCTL_PTBUG_SEARCHKERNELMODULE CTL_CODE(FILE_DEVICE_UNKNOWN, 0xB01, METHOD_BUFFERED, FILE_READ_DATA)
