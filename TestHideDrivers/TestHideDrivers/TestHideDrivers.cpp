#include "pch.h"
#include <stdio.h>
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib,"ntdll.lib")

// These structures are copied from Process Hacker source code (ntldr.h)

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

int main()
{
	NTSTATUS status;
	ULONG i;

	PRTL_PROCESS_MODULES ModuleInfo;

	ModuleInfo = (PRTL_PROCESS_MODULES)VirtualAlloc(NULL, 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Allocate memory for the module list

	if (!ModuleInfo)
	{
		printf("\nUnable to allocate memory for module list (%d)\n", GetLastError());
		return -1;
	}

	if (!NT_SUCCESS(status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)11, ModuleInfo, 1024 * 1024, NULL))) // 11 = SystemModuleInformation
	{
		printf("\nError: Unable to query module list (%#x)\n", status);

		VirtualFree(ModuleInfo, 0, MEM_RELEASE);
		return -1;
	}

	WCHAR name[200] = {};
	for (i = 0; i < ModuleInfo->NumberOfModules; i++)
	{
		char * xx = (char*)ModuleInfo->Modules[i].FullPathName;
		//printf("\nImage base: %s\n", xx);
		auto x = strstr(xx, "Ndu");
		if (x != NULL)
		{
			//printf("\n*****************************************************\n");
			//printf("\nImage base: %#x\n", ModuleInfo->Modules[i].ImageBase);
			//printf("\nImage name: %s\n", ModuleInfo->Modules[i].FullPathName + ModuleInfo->Modules[i].OffsetToFileName);
			printf("\nImage full path: %s\n", ModuleInfo->Modules[i].FullPathName);
			//printf("\nImage size: %d\n", ModuleInfo->Modules[i].ImageSize);
			//printf("\n*****************************************************\n");
		}
	}

	VirtualFree(ModuleInfo, 0, MEM_RELEASE);
	return 0;
}