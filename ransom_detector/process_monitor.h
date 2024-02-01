#pragma once

#include <Wdm.h>

#define PROC_TAG 'corP' // Proc

typedef struct _PROC_NODE {
	struct _PROC_NODE * next;
	ULONG pid;
	ULONG encryptedFilesCounter;
	LARGE_INTEGER lastFileEncryptionTime;
} PROC_NODE, *PPROC_NODE;

PPROC_NODE findProc(ULONG pid);
void freeProcs();
PPROC_NODE addNewProc(ULONG pid);

PPROC_NODE g_encryptorsHead = NULL;
KMUTEX g_procMutex = { 0 };