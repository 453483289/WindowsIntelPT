/**********************************************************************
 *  Intel Processor Trace Stupid Test Driver
 *	Filename: TestDriverIo.cpp
 *	Implements a damn BUGGED insecure test I/O functions
 *  Last revision: 11/29/2016
 *
 *  Copyright© 2016 Andrea Allievi
 *  All right reserved
 **********************************************************************/
#include "stdafx.h"
#include "DriverEntry.h"
#include "TestDriverIo.h"

// The kernel module currently selected
PMODULE_ENTRY g_pTargetModule = NULL;

// Driver generic pass-through routine
NTSTATUS DevicePassThrough(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Driver unsupported routine
NTSTATUS DeviceUnsupported(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_NOT_SUPPORTED;
}


// Very BUGGED Driver read routine:
NTSTATUS DeviceRead(PDEVICE_OBJECT pDevObj, PIRP pIrp) {
	NTSTATUS ntStatus = 0;
	PIO_STACK_LOCATION pIoStack = NULL;
	LPBYTE pReadBuff = NULL;
	DWORD dwSize = 0;
	DWORD dwOffset = 0;

	pIoStack = IoGetCurrentIrpStackLocation(pIrp);
	pReadBuff = (LPBYTE)MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	dwSize = pIoStack->Parameters.Read.Length;
	dwOffset = pIoStack->Parameters.Read.ByteOffset.LowPart;

	while (1) {
		if (!pReadBuff || dwSize < 1) { ntStatus = STATUS_INVALID_PARAMETER; break; }
		if (!g_pTargetModule) { ntStatus = STATUS_DEVICE_NOT_READY; break; }
		if (dwOffset + dwSize > g_pTargetModule->SizeOfImage)
			dwSize = g_pTargetModule->SizeOfImage - dwOffset;
		if (!dwSize) { ntStatus = STATUS_NO_MEMORY; break; }

		__try {
			RtlCopyMemory(pReadBuff, (LPBYTE)g_pTargetModule->pDrvBase + dwOffset, dwSize);
			pIrp->IoStatus.Information = dwSize;
			ntStatus = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			ntStatus = GetExceptionCode();
		}
		break;
	}

	pIrp->IoStatus.Status = ntStatus;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return pIrp->IoStatus.Status;
}

// Very bugged driver write routine:
NTSTATUS DeviceWrite(PDEVICE_OBJECT pDevObj, PIRP pIrp) {
	NTSTATUS ntStatus = 0;
	PIO_STACK_LOCATION pIoStack = NULL;
	LPBYTE pWriteBuff = NULL;
	DWORD dwSize = 0;
	DWORD dwOffset = 0;

	pIoStack = IoGetCurrentIrpStackLocation(pIrp);
	pWriteBuff = (LPBYTE)MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	dwSize = pIoStack->Parameters.Write.Length;
	dwOffset = pIoStack->Parameters.Write.ByteOffset.LowPart;

	while (1) {
		if (!pWriteBuff || dwSize < 1) { ntStatus = STATUS_INVALID_PARAMETER; break; }
		if (!g_pTargetModule) { ntStatus = STATUS_DEVICE_NOT_READY; break; }
		if (dwOffset + dwSize > g_pTargetModule->SizeOfImage)
			dwSize = g_pTargetModule->SizeOfImage - dwOffset;
		if (!dwSize) { ntStatus = STATUS_NO_MEMORY; break; }

		LPBYTE lpTarget = (LPBYTE)g_pTargetModule->pDrvBase + dwOffset;
		dwOffset = (DWORD)((ULONG_PTR)lpTarget % PAGE_SIZE);
		if (dwOffset) 
			lpTarget = lpTarget - dwOffset;
		PMDL pMdl = IoAllocateMdl(lpTarget, dwSize + dwOffset, NULL, FALSE, NULL);

		__try {
			MmProbeAndLockPages(pMdl, KernelMode, IoWriteAccess);
			LPBYTE pMappedAddr = (LPBYTE)MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
			RtlCopyMemory(pMappedAddr + dwOffset, pWriteBuff, dwSize);
			pIrp->IoStatus.Information = dwSize;
			MmUnlockPages(pMdl);
			ntStatus = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			ntStatus = GetExceptionCode();
		}
		
		IoFreeMdl(pMdl);
		break;
	}

	pIrp->IoStatus.Status = ntStatus;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return pIrp->IoStatus.Status;
}

// Very bugged Driver Device IO Control dispatch routine
NTSTATUS DeviceIoControl(PDEVICE_OBJECT pDevObj, PIRP pIrp) {
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PDRIVER_OBJECT pDrvObj = NULL;						// The base driver object
	PIO_STACK_LOCATION pIoStack = NULL;					// The I/O stack location
	DWORD dwInBuffSize = 0, dwOutBuffSize = 0;			// Input and output buffer size
	LPVOID lpOutBuff = NULL, lpInBuff = NULL;			// Input and output buffer
	DWORD dwIoCtlCode = 0;

	pIoStack = IoGetCurrentIrpStackLocation(pIrp);
	dwInBuffSize = pIoStack->Parameters.DeviceIoControl.InputBufferLength;
	dwOutBuffSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
	dwIoCtlCode = pIoStack->Parameters.DeviceIoControl.IoControlCode;
	pDrvObj = pDevObj->DriverObject;

	if (dwIoCtlCode == IOCTL_PTBUG_SEARCHKERNELMODULE) {
		UNICODE_STRING moduleToFind = { 0 };

		// Read the module to find
		if (dwInBuffSize < sizeof(TCHAR) || dwInBuffSize > sizeof(TCHAR) * 260)
			return STATUS_INVALID_BUFFER_SIZE;

		LPWSTR lpModuleName = (LPWSTR)pIrp->AssociatedIrp.SystemBuffer;
		// Method buffered - I can do this kind of horrible cagate:
		RtlInitUnicodeString(&moduleToFind, lpModuleName);

		PMODULE_ENTRY pCurModule = (PMODULE_ENTRY)pDrvObj->DriverSection;
		PMODULE_ENTRY pNextModule = (PMODULE_ENTRY)pCurModule->curEntry.Flink;

		ntStatus = STATUS_NOT_FOUND;
		while (pNextModule != pCurModule) {
			if (RtlCompareUnicodeString(&pNextModule->BaseDrvName, &moduleToFind, TRUE) == 0) {
				// Module found
				KERNEL_MODULE mod = { 0 };
				mod.lpStartAddr = pNextModule->pDrvBase;
				mod.dwSize = pNextModule->SizeOfImage;
				RtlCopyMemory(mod.modName, pNextModule->BaseDrvName.Buffer, pNextModule->BaseDrvName.Length);
				
				if (dwOutBuffSize >= (DWORD)(sizeof(KERNEL_MODULE))) {
					RtlZeroMemory(pIrp->AssociatedIrp.SystemBuffer, dwOutBuffSize);
					RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, &mod, sizeof(KERNEL_MODULE));
					pIrp->IoStatus.Information = sizeof(KERNEL_MODULE);
					// Set the current module
					g_pTargetModule = pNextModule;
					ntStatus = STATUS_SUCCESS;
					break;
				}
			}
			// Go to next module
			pNextModule = (PMODULE_ENTRY)pNextModule->curEntry.Flink;
		}
		
	} else {
		ntStatus = STATUS_NOT_SUPPORTED;
	}
	pIrp->IoStatus.Status = ntStatus;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return ntStatus;

}

