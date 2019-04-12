#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

PFLT_FILTER FilterHandle = NULL;
NTSTATUS MiniUnload(FLT_FILTER_UNLOAD_FLAGS	Flags);
FLT_POSTOP_CALLBACK_STATUS MiniPostCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_PREOP_CALLBACK_STATUS MiniPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
FLT_PREOP_CALLBACK_STATUS MiniPreWrite(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);

FLT_PREOP_CALLBACK_STATUS PreDirControl(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);


FLT_PREOP_CALLBACK_STATUS PreRead(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);

const FLT_OPERATION_REGISTRATION Callbacks[] = {
	{IRP_MJ_CREATE, 0, MiniPreCreate, MiniPostCreate},
	{IRP_MJ_WRITE,  0, MiniPreWrite,  NULL},
	{IRP_MJ_READ, 0, PreRead, NULL},
	{IRP_MJ_DIRECTORY_CONTROL, 0, PreDirControl, NULL},
	{IRP_MJ_OPERATION_END}
};

const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION), // size of this structure
	FLT_REGISTRATION_VERSION,
	0, //flag
	NULL, // context registration member
	Callbacks,
	MiniUnload,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL // https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/fltkernel/ns-fltkernel-_flt_registration
};

FLT_PREOP_CALLBACK_STATUS PreRead(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS PreDirControl(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	/*if (Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY)
	{
		// Not a query directory request, so let it through
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}*/

	PFILE_DIRECTORY_INFORMATION fileDirInfo, lastFileDirInfo, nextFileDirInfo;
	PFILE_FULL_DIR_INFORMATION fileFullDirInfo, lastFileFullDirInfo, nextFileFullDirInfo;
	PFILE_NAMES_INFORMATION fileNamesInfo, lastFileNamesInfo, nextFileNamesInfo;
	PFILE_BOTH_DIR_INFORMATION fileBothDirInfo, lastFileBothDirInfo, nextFileBothDirInfo;
	PFILE_ID_BOTH_DIR_INFORMATION fileIdBothDirInfo, lastFileIdBothDirInfo, nextFileIdBothDirInfo;
	PFILE_ID_FULL_DIR_INFORMATION fileIdFullDirInfo, lastFileIdFullDirInfo, nextFileIdFullDirInfo;
	UNICODE_STRING fileName;
	ULONG moveLength;

	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	WCHAR Name[260] = { 0 };
	auto status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);
	if (NT_SUCCESS(status)) {
		if (FileNameInfo->Name.MaximumLength < 260) {
			RtlCopyMemory(Name, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);

			KdPrint(("name : %ws \r\n", Name));
			if (wcsstr(Name, L"Project") != NULL) {
				KdPrint(("Sorry : %ws blocked \r\n", Name));
				Data->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Data->IoStatus.Information = 0;
				FltReleaseFileNameInformation(FileNameInfo);
				return FLT_PREOP_COMPLETE;
			}
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
	/*_wcsupr(Name);
	//if (wcsstr(Name, L"OLOLO") != NULL) 
	{
		KdPrint(("BLOCKED : %ws blocked \r\n", Name));
		Data->IoStatus.Status = STATUS_INVALID_PARAMETER;
		Data->IoStatus.Information = 0;
		return FLT_PREOP_COMPLETE;
	}
	//
	// Check for a directory enumeration
	//
	switch (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass)
	{
	case FileIdFullDirectoryInformation:
	case FileIdBothDirectoryInformation:
	case FileBothDirectoryInformation:
	case FileDirectoryInformation:
	case FileFullDirectoryInformation:
	case FileNamesInformation:
		//
		// These are all enumerations we want to intercept
		//
		break;

	default:
		//
		// Anything else is not
		//
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	//
	// For this one, we do want a post-operation callback so we can examine what
	// the directory enumeration is returning and make modifications if required.
	//
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;*/
}

NTSTATUS MiniUnload(FLT_FILTER_UNLOAD_FLAGS	Flags)
{
	//KdPrint(("driver unload\n"));
	//FltUnregisterFilter(FilterHandle);

	return STATUS_SUCCESS;
}
FLT_POSTOP_CALLBACK_STATUS MiniPostCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	//KdPrint(("post create is running \r\n"));
	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS MiniPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	WCHAR Name[260] = { 0 };

	/*status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);

	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(FileNameInfo);
		if (NT_SUCCESS(status)) {
			if (FileNameInfo->Name.MaximumLength < 260) {
				RtlCopyMemory(Name, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
				KdPrint(("create file: %ws \r\n", Name));
			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}
	*/
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS MiniPreWrite(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	WCHAR Name[200] = { 0 };

	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);

	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(FileNameInfo);
		if (NT_SUCCESS(status)) {
			if (FileNameInfo->Name.MaximumLength < 260) {
				RtlCopyMemory(Name, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
				//KdPrint(("create file: %ws \r\n", Name));
				
				_wcsupr(Name);
				if (wcsstr(Name, L"OPENME.TXT") != NULL || wcsstr(Name, L"SSS") != NULL) {
					KdPrint(("write file : %ws blocked \r\n", Name));
					Data->IoStatus.Status = STATUS_INVALID_PARAMETER;
					Data->IoStatus.Information = 0;
					FltReleaseFileNameInformation(FileNameInfo);
					return FLT_PREOP_COMPLETE;
				}
			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}

		return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	status = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);

	KdPrint(("Started! \r\n"));
	if (NT_SUCCESS(status)) {
		status = FltStartFiltering(FilterHandle);

		if (!NT_SUCCESS(status)) {
			FltUnregisterFilter(FilterHandle);
		}
	}

	return status;
}