#include "process_monitor.h"

PPROC_NODE findProc(ULONG pid)
{
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
        return NULL;
    }

    PPROC_NODE current = g_encryptorsHead;
    for (; current != NULL; current = current->next) {
        if (current->pid == pid) {
            break;
        }

        // last one
        if (current->next == NULL) {
            PPROC_NODE newProc = (PPROC_NODE)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(PROC_NODE), PROC_TAG);
            if (!newProc) {
                goto Exit;
            }

            newProc->pid = pid;
            current->next = newProc;
            current = newProc;
            break;
        }
    }

    current->encryptedFilesCounter += 1;
    current->lastFileEncryptionTime = NOW;
Exit:

	BOOLEAN restoreToOriginalIrql = FALSE;
	KeReleaseMutex(&g_procMutex, restoreToOriginalIrql);
}

void freeProcs() {    
    for (PPROC_NODE current = g_encryptorsHead; current != NULL;) {
        PPROC_NODE tempNext = current->next;
        ExFreePoolWithTag(current, PROC_TAG);
        current = tempNext;
    }
}