#include "process_monitor.h"

#include "logging.h"

PPROC_NODE g_encryptorsHead = NULL;
KMUTEX g_procMutex = { 0 };

PPROC_NODE findProc(ULONG pid)
{
    PPROC_NODE resultNode = NULL;
    BOOLEAN notAlertable = FALSE;
    PLARGE_INTEGER waitIndifinitely = NULL;
    NTSTATUS status =
        KeWaitForSingleObject(
            &g_procMutex,
            Executive,
            KernelMode,
            notAlertable,
            waitIndifinitely);

    if (NT_ERROR(status))  {
        PT_DBG_PRINT(PTDBG_TRACE_PROC_OPERATIONS_STATUS,
            ("****** KeWaitForSingleObject failed\n"));
        return NULL;
    }

    PPROC_NODE current = g_encryptorsHead;
    if (current == NULL) {
        g_encryptorsHead = createNewProc(pid);
        if (!g_encryptorsHead) {
            goto exit;
        }
        current = g_encryptorsHead;
    }

    PT_DBG_PRINT(PTDBG_TRACE_PROC_OPERATIONS_STATUS,
        ("****** Got the lock, checking for counter through the list\n"));
    for (; current != NULL; current = current->next) {
        if (current->pid == pid) {
            break;
        }

        // last one
        if (current->next == NULL) {
            PPROC_NODE newProc = createNewProc(pid);
            if (!newProc) {
                goto exit;
            }

            current->next = newProc;
            current = newProc;
            break;
        }
    }

    LARGE_INTEGER currentTime = { 0 };
    KeQuerySystemTime(&currentTime);

    // We past the maximum interval between encryptions, probably a new process, clearing the counter.
    if (currentTime.QuadPart - current->lastFileEncryptionTime.QuadPart > MAX_TIME_BETWEEN_ENCRYPTIONS_SECS * HUNDERED_NANO_TO_SEC) {
        current->encryptedFilesCounter = 0;
    }

    current->encryptedFilesCounter += 1;
    current->lastFileEncryptionTime = currentTime;
    resultNode = current;

exit:    	
	KeReleaseMutex(&g_procMutex, FALSE);
    return resultNode;
}

void freeProcs() 
{    
    for (PPROC_NODE current = g_encryptorsHead; current != NULL;) {
        PPROC_NODE tempNext = current->next;
        ExFreePoolWithTag(current, PROC_TAG);
        current = tempNext;
    }
}

PPROC_NODE createNewProc(ULONG pid)
{
    PPROC_NODE newProc = (PPROC_NODE)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(PROC_NODE), PROC_TAG);
    if (!newProc) {
        return NULL;
    }

    newProc->pid = pid;   
    newProc->encryptedFilesCounter = 0;
    newProc->next = NULL;

    LARGE_INTEGER currentTime = { 0 };
    KeQuerySystemTime(&currentTime);
    newProc->lastFileEncryptionTime = currentTime;

    return newProc;
}