#pragma once

#include <Ntifs.h>

#define PROC_TAG 'corP' // Proc
#define MAX_TIME_BETWEEN_ENCRYPTIONS_SECS 60
#define HUNDERED_NANO_TO_SEC 10000000

typedef struct _PROC_NODE {
	struct _PROC_NODE * next;
	ULONG pid;
	ULONG encryptedFilesCounter;
	LARGE_INTEGER lastFileEncryptionTime;
} PROC_NODE, *PPROC_NODE;

PPROC_NODE findProc(ULONG pid);
void freeProcs();
PPROC_NODE createNewProc(ULONG pid);

extern PPROC_NODE g_encryptorsHead;
extern KMUTEX g_procMutex;