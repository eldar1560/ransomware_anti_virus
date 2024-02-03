#include "randomness.h"

#include "logging.h"

ULONG _fltused = 0;

__declspec(noinline)
BOOLEAN DoFloatingPointCalculation(PUCHAR buffer, ULONG len)
{
    BOOLEAN result = FALSE;
    ULONG observedCounts[256] = { 0 };

    for (ULONG i = 0; i < len; ++i)
    {
        observedCounts[buffer[i]]++;
    }

    double chiSquare = 0.0;
    double expectedFrequency = (double)len / 256;

    for (int i = 0; i < 256; ++i)
    {
        double deviation = observedCounts[i] - expectedFrequency;
        chiSquare += (deviation * deviation) / expectedFrequency;
    }

    if (chiSquare < ENCRPYTION_CHI_SQUARE_TRESHOLD) {
        PT_DBG_PRINT(PTDBG_TRACE_PROC_OPERATIONS_STATUS,
            ("****** chiSquare under new: %d\n", ENCRPYTION_CHI_SQUARE_TRESHOLD));
        result = TRUE;
    }
    return result;
}


BOOLEAN isRandom(PUCHAR buffer, ULONG len)
{
    XSTATE_SAVE SaveState;
    NTSTATUS Status;
    BOOLEAN result = FALSE;

    Status = KeSaveExtendedProcessorState(XSTATE_MASK_LEGACY, &SaveState);
    if (!NT_SUCCESS(Status)) {
        goto exit;
    }

    __try {
        result = DoFloatingPointCalculation(buffer, len);       
    }
    __finally {
        KeRestoreExtendedProcessorState(&SaveState);
    }

exit:
    return result;

}