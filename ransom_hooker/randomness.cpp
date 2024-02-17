#include "randomness.h"

#include <math.h>
#include <string>

BOOL is_random(PBYTE buf, UINT length)
{
    ULONG histogram[UCHAR_MAX + 1] = { 0 };
    fill_histogram(histogram, buf, length);

    double buf_entropy = entropy(histogram);
    OutputDebugStringA("Found entropy:");
    OutputDebugStringA(std::to_string(buf_entropy).c_str());
    if (buf_entropy >= ENTROPY_RANDOM_THRESHOLD) {
        return TRUE;
    }

    return FALSE;
}

void fill_histogram(ULONG histogram[], PBYTE buf, UINT length)
{
    for (UINT i = 0; i < length; i++) {
        ++histogram[buf[i]];
    }
}

/* Histogram array length must be UCHAR_MAX+1 */
ULONG total(const ULONG histogram[])
{
    ULONG sum = 0;
    for (ULONG i = 0; i <= UCHAR_MAX; ++i) {
        sum += histogram[i];
    }
    return sum;
}

double entropy(const ULONG histogram[])
{
    ULONG len = total(histogram);

    double result = 0.0;
    for (ULONG i = 0; i <= UCHAR_MAX; ++i) {
        if (!histogram[i]) {
            continue;
        }
        double freq = (double)histogram[i] / (double)len;
        result -= freq * log10(freq) / log10(2.0);
    }

    return result;
}