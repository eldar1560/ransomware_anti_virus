/*++

Module Name:

    ransomdetector.c

Abstract:

    This is the main module of the ransom_detector miniFilter driver.

Environment:

    Kernel mode

--*/

#include "process_monitor.h"
#include "logging.h"
#include "randomness.h"

#include <fltKernel.h>
#include <dontuse.h>
#include <ntifs.h>
#include <ntstrsafe.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;


/*************************************************************************
    Prototypes
*************************************************************************/

EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );

NTSTATUS
ransomdetectorInstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

VOID
ransomdetectorInstanceTeardownStart (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    );

VOID
ransomdetectorInstanceTeardownComplete (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    );

NTSTATUS
ransomdetectorUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    );

NTSTATUS
ransomdetectorInstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
ransomdetectorPreOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
ransomdetectorPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
    );

EXTERN_C_END

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, ransomdetectorUnload)
#pragma alloc_text(PAGE, ransomdetectorInstanceQueryTeardown)
#pragma alloc_text(PAGE, ransomdetectorInstanceSetup)
#pragma alloc_text(PAGE, ransomdetectorInstanceTeardownStart)
#pragma alloc_text(PAGE, ransomdetectorInstanceTeardownComplete)
#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_WRITE,
      0,
      ransomdetectorPreOperation,
      ransomdetectorPostOperation },

    { IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    Callbacks,                          //  Operation callbacks

    ransomdetectorUnload,                           //  MiniFilterUnload

    ransomdetectorInstanceSetup,                    //  InstanceSetup
    ransomdetectorInstanceQueryTeardown,            //  InstanceQueryTeardown
    ransomdetectorInstanceTeardownStart,            //  InstanceTeardownStart
    ransomdetectorInstanceTeardownComplete,         //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};



NTSTATUS
ransomdetectorInstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++

Routine Description:

    This routine is called whenever a new instance is created on a volume. This
    gives us a chance to decide if we need to attach to this volume or not.

    If this routine is not defined in the registration structure, automatic
    instances are always created.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Flags describing the reason for this attach request.

Return Value:

    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("ransomdetector!ransomdetectorInstanceSetup: Entered\n") );

    return STATUS_SUCCESS;
}


NTSTATUS
ransomdetectorInstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This is called when an instance is being manually deleted by a
    call to FltDetachVolume or FilterDetach thereby giving us a
    chance to fail that detach request.

    If this routine is not defined in the registration structure, explicit
    detach requests via FltDetachVolume or FilterDetach will always be
    failed.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Indicating where this detach request came from.

Return Value:

    Returns the status of this operation.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("ransomdetector!ransomdetectorInstanceQueryTeardown: Entered\n") );

    return STATUS_SUCCESS;
}


VOID
ransomdetectorInstanceTeardownStart (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This routine is called at the start of instance teardown.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Reason why this instance is being deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("ransomdetector!ransomdetectorInstanceTeardownStart: Entered\n") );
}


VOID
ransomdetectorInstanceTeardownComplete (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This routine is called at the end of instance teardown.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Reason why this instance is being deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("ransomdetector!ransomdetectorInstanceTeardownComplete: Entered\n") );
}


/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine for this miniFilter driver.  This
    registers with FltMgr and initializes all global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.

    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Routine can return non success error codes.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( RegistryPath );

    PT_DBG_PRINT(PTDBG_TRACE_LOAD_UNLOAD_OPERATION_STATUS,
                  ("ransomdetector!DriverEntry: Entered\n") );
    ULONG reserverdValue = 0;
    KeInitializeMutex(&g_procMutex, reserverdValue);

    //
    //  Register with FltMgr to tell it our callback routines
    //

    status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &gFilterHandle );

    FLT_ASSERT( NT_SUCCESS( status ) );

    if (NT_SUCCESS( status )) {

        //
        //  Start filtering i/o
        //

        status = FltStartFiltering( gFilterHandle );

        if (!NT_SUCCESS( status )) {

            FltUnregisterFilter( gFilterHandle );
        }
    }

    return status;
}

NTSTATUS
ransomdetectorUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    This is the unload routine for this miniFilter driver. This is called
    when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unload indicated by the Flags
    parameter.

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT(PTDBG_TRACE_LOAD_UNLOAD_OPERATION_STATUS,
                  ("ransomdetector!ransomdetectorUnload: Entered\n") );

    FltUnregisterFilter( gFilterHandle );

    freeProcs();

    return STATUS_SUCCESS;
}

NTSTATUS drvTerminateProcess(ULONG processID)
{
    NTSTATUS          ntStatus = STATUS_SUCCESS;
    HANDLE            hProcess;
    OBJECT_ATTRIBUTES ObjectAttributes;
    CLIENT_ID         ClientId;

    DbgPrint("drvTerminateProcess( %u )", processID);

    InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_INHERIT, NULL, NULL);

    ClientId.UniqueProcess = (HANDLE)processID;
    ClientId.UniqueThread = NULL;

    __try
    {
        ntStatus = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &ObjectAttributes, &ClientId);
        if (NT_SUCCESS(ntStatus))
        {
            ntStatus = ZwTerminateProcess(hProcess, 0);
            if (!NT_SUCCESS(ntStatus))
                DbgPrint("ZwTerminateProcess failed with status : %08X\n", ntStatus);

            ZwClose(hProcess);
        }
        else
            DbgPrint("ZwOpenProcess failed with status : %08X\n", ntStatus);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        ntStatus = STATUS_UNSUCCESSFUL;
        DbgPrint("Exception caught in drvTerminateProcess()");
    }

    return ntStatus;
}

NTSTATUS logDetection(ULONG processID)
{

    UNICODE_STRING      filePath;
    HANDLE              hFile;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    IO_STATUS_BLOCK     IoStatusBlock;

    LARGE_INTEGER systemTime = { 0 };
    LARGE_INTEGER currentTime = { 0 };
    TIME_FIELDS currentTimeFields;

    KeQuerySystemTime(&systemTime);
    ExSystemTimeToLocalTime(&systemTime, &currentTime);
    RtlTimeToTimeFields(&currentTime, &currentTimeFields);

    CHAR pszDest[1024];
    size_t cbDest = sizeof(pszDest);

    PEPROCESS process = NULL;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)processID, &process);
    if (!NT_SUCCESS(status))
    {
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** PsLookupProcessByProcessId error\n"));
        return status;
    }

    PUNICODE_STRING ransomName = NULL;
    status = SeLocateProcessImageName(process, &ransomName);
    if (!NT_SUCCESS(status))
    {
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** PsLookupProcessByProcessId error\n"));
        return status;
    }

    status = RtlStringCbPrintfA(pszDest, cbDest, "Ransom Driver Detector - %hu/%hu/%hu %hu:%hu:%hu - %wZ\n",
        currentTimeFields.Day,
        currentTimeFields.Month,
        currentTimeFields.Year,
        currentTimeFields.Hour, 
        currentTimeFields.Minute, 
        currentTimeFields.Second, 
        *ransomName);
    if (!NT_SUCCESS(status))
    {
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** RtlStringCbPrintfA error\n"));
        return status;
    }

    size_t realSize = 0;
    status = RtlStringCbLengthA(pszDest, cbDest, &realSize);

    if (!NT_SUCCESS(status))
    {
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** RtlStringCbLengthA error\n"));
        return status;
    }

    RtlInitUnicodeString(&filePath, L"\\??\\C:\\Windows\\Temp\\ransomav.log");
    InitializeObjectAttributes(&ObjectAttributes, &filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwCreateFile(&hFile, FILE_APPEND_DATA, &ObjectAttributes,
        &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

    if (!NT_SUCCESS(status))
    {
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** Creating log file error\n"));
        return status;
    }

    status = ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatusBlock, (PVOID)pszDest, (ULONG)realSize, NULL, NULL);

    if (!NT_SUCCESS(status))
    {        
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("***********  Writing log file error\n"));

        ZwClose(hFile);
        return status;
    }

    ZwClose(hFile);
    return STATUS_SUCCESS;
}


/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS
ransomdetectorPreOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is a pre-operation dispatch routine for this miniFilter.

    This is non-pageable because it could be called on the paging path

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The context for the completion routine for this
        operation.

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("ransomdetector!ransomdetectorPreOperation: Entered\n") );

    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    PFLT_FILE_NAME_INFORMATION nameInfo = { 0 };
    LARGE_INTEGER byteOffset = iopb->Parameters.Write.ByteOffset;
    PVOID origBuf = NULL;

    /* We want the file offset writing, the file name, the pid that did the writing and the data and length */
    ULONG writeLen = iopb->Parameters.Write.Length;
    
    ULONG writingPid = FltGetRequestorProcessId(Data);

    FltGetFileNameInformation(Data, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &nameInfo);

    //
    //  If the users original buffer had a MDL, get a system address.
    //

    if (iopb->Parameters.Write.MdlAddress != NULL) {

        //
        //  This should be a simple MDL. We don't expect chained MDLs
        //  this high up the stack
        //

        FLT_ASSERT(((PMDL)iopb->Parameters.Write.MdlAddress)->Next == NULL);

        origBuf = MmGetSystemAddressForMdlSafe(iopb->Parameters.Write.MdlAddress,
            NormalPagePriority | MdlMappingNoExecute);

        if (origBuf == NULL) {

            PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
                ("%wZ Failed to get system address for MDL: %p\n",
                    &nameInfo->Name,
                    iopb->Parameters.Write.MdlAddress));

            //
            //  If we could not get a system address for the users buffer,
            //  then we are going to fail this operation.
            //

            Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Data->IoStatus.Information = 0;
            return FLT_PREOP_COMPLETE;
        }

    }
    else {

        //
        //  There was no MDL defined, use the given buffer address.
        //

        origBuf = iopb->Parameters.Write.WriteBuffer;
    }

    if (writeLen > 50 && byteOffset.QuadPart == 0 && isRandom(origBuf, writeLen)) {
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** I got a suspicious write opeartion from:\n"));
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("sus - writingPid: %d, Filename: %wZ, writeLen: %d, byteOffset: %lld, data: %s\n", writingPid, &nameInfo->Name, writeLen, byteOffset.QuadPart, origBuf));
        PPROC_NODE procNode = findProc(writingPid);
        if (!procNode) {
            PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
                ("*********** Couldn't load proc node for pid: %d\n", writingPid));
            return FLT_PREOP_SUCCESS_WITH_CALLBACK;
        }
        PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
            ("*********** The write counter: %d\n", procNode->encryptedFilesCounter));
        if (procNode->encryptedFilesCounter >= 10) {
            PT_DBG_PRINT(PTDBG_TRACE_WRITE_OPERATION_STATUS,
                ("*********** Found a ransomware: %d, killing it\n", writingPid));

            drvTerminateProcess(writingPid);
            logDetection(writingPid);
        }

    }

    // This template code does not do anything with the callbackData, but
    // rather returns FLT_PREOP_SUCCESS_WITH_CALLBACK.
    // This passes the request down to the next miniFilter in the chain.

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
ransomdetectorPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
    )
/*++

Routine Description:

    This routine is the post-operation completion routine for this
    miniFilter.

    This is non-pageable because it may be called at DPC level.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The completion context set in the pre-operation routine.

    Flags - Denotes whether the completion is successful or is being drained.

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
    UNREFERENCED_PARAMETER( Flags );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("ransomdetector!ransomdetectorPostOperation: Entered\n") );

    return FLT_POSTOP_FINISHED_PROCESSING;
}