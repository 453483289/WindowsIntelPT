// PagingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int timeToSleep = 120;
	wprintf(L"This is a paging test Application.\r\n\r\n");
	wprintf(L"Going to sleep for %i seconds. Put a breakpoint on my CR3 pyhisical memory...\r\n", timeToSleep);
	wprintf(L"... and also remember to load a tons of applications...\r\n\r\n");
	DbgBreak();

	while (TRUE) {
		Sleep(timeToSleep * 1000);
		wprintf(L"The process has been resurrected. Check the CR3 now.\r\n");
		DbgBreak();
	}

	wprintf(L"Exiting....\r\n");
	return 0;
}

