/**********************************************************************
 *  Intel Processor Trace Stupid Test Driver
 *	Filename: DriverEntry.h
 *	This is a very BUGGED stupid driver implemented ONLY for TESTING purposes
 *  Last revision: 11/29/2016
 *
 *  Copyright© 2016 Andrea Allievi
 *  All right reserved
 **********************************************************************/
#pragma once

struct _DRVOBJ_SECTION_X64 {
	LIST_ENTRY curEntry;					// + 0x00 - Current driver entry in the linked list
	LPVOID pExceptionTable;					// + 0x10
	ULONG ExceptionTableSize;				// + 0x18
	QWORD qwReserved[2];					// + 0x20
	LPVOID pDrvBase;						// + 0x30
	LPVOID pDrvEp;							// + 0x38
	ULONG SizeOfImage;						// + 0x40
	UNICODE_STRING FullDrvPath;				// + 0x48
	UNICODE_STRING BaseDrvName;				// + 0x58
	ULONG Flags;							// + 0x68
};

typedef struct _DRVOBJ_SECTION_X64 MODULE_ENTRY, *PMODULE_ENTRY;

// Driver entry point
DDKBUILD NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath);

// Driver unload routine
VOID DriverUnload(PDRIVER_OBJECT pDrvObj);

// Driver generic pass-through routine
NTSTATUS DevicePassThrough(PDEVICE_OBJECT pDevObj, PIRP pIrp);

// Very BUGGED Driver read routine:
NTSTATUS DeviceRead(PDEVICE_OBJECT pDevObj, PIRP pIrp);

// Very bugged driver write routine:
NTSTATUS DeviceWrite(PDEVICE_OBJECT pDevObj, PIRP pIrp);

// Very bugged Driver Device IO Control dispatch routine
NTSTATUS DeviceIoControl(PDEVICE_OBJECT pDevObj, PIRP pIrp);

// Driver unsupported routine
NTSTATUS DeviceUnsupported(PDEVICE_OBJECT pDevObj, PIRP pIrp);

