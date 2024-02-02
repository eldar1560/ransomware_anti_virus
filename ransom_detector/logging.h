#pragma once

#include <ntifs.h>

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002
#define PTDBG_TRACE_WRITE_OPERATION_STATUS    0x00000004
#define PTDBG_TRACE_LOAD_UNLOAD_OPERATION_STATUS    0x00000008
#define PTDBG_TRACE_PROC_OPERATIONS_STATUS    0x000000010

extern ULONG gTraceFlags;


#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))