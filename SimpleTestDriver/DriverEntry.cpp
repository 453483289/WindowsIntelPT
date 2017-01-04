/**********************************************************************
 *  Intel Processor Trace Stupid Test Driver
 *	Filename: DriverEntry.cpp
 *	This is a very BUGGED stupid driver implemented ONLY for TESTING purposes
 *  Last revision: 11/29/2016
 *
 *  Copyright© 2016 Andrea Allievi
 *  All right reserved
 **********************************************************************/
#include "stdafx.h"
#include "DriverEntry.h"
#include "Debug.h"

// The driver entry point. You have to insert a 0x9A opcode in the GsDriverEntry original EP
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) {
	UNICODE_STRING devNameString = { 0 };					// The I/O device name
	UNICODE_STRING dosDevNameString = { 0 };				// The DOS device name (User-mode access)
	PDEVICE_OBJECT pDevObj = NULL;							// The device object
	UNREFERENCED_PARAMETER(pRegPath);
	NTSTATUS ntStatus = STATUS_SUCCESS;

	// Enable special Interrupt for catching from the Intel PT driver
	EnableDebugOutput();
	DrvDbgPrint("TestPtDrv - DriverEntry has been called!");

	// No do the things, register a TEST device
	// Build the controller device
	RtlInitUnicodeString(&devNameString, L"\\Device\\IntelPtTest");
	RtlInitUnicodeString(&dosDevNameString, L"\\DosDevices\\IntelPtTest");

	// XXX: require admin to prevent side channel attacks on 3rd party programs (IoCreateDeviceSecure)
	ntStatus = IoCreateDevice(pDriverObject, 0, &devNameString, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, &pDevObj);

	if (!NT_SUCCESS(ntStatus)) {
		if (pDevObj) IoDeleteDevice(pDevObj);
		return ntStatus;
	}
	ntStatus = IoCreateSymbolicLink(&dosDevNameString, &devNameString);

	// Initialize Driver dispatch routine
	for (DWORD i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObject->MajorFunction[i] = DeviceUnsupported;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DevicePassThrough;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DevicePassThrough;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = DevicePassThrough;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl;
	pDriverObject->MajorFunction[IRP_MJ_READ] = DeviceRead;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DeviceWrite;

	// Driver entry point!
	pDriverObject->DriverUnload = DriverUnload;
	pDevObj->Flags |= DO_DIRECT_IO;
	pDevObj->Flags &= (~DO_DEVICE_INITIALIZING);

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDrvObj) {
	PDEVICE_OBJECT pDevObj = pDrvObj->DeviceObject;
	UNICODE_STRING dosDevNameString = { 0 };

	// Driver unload routine
	DrvDbgPrint("TestPtDrv - Driver Unload routine has been called!");
	RtlInitUnicodeString(&dosDevNameString, L"\\DosDevices\\IntelPtTest");

	if (pDevObj) {
		IoDeleteSymbolicLink(&dosDevNameString);
		IoDeleteDevice(pDevObj);
	}
	RevertToDefaultDbgSettings();
}