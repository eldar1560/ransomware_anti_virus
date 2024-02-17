#pragma once

#include <Windows.h>

#define ENTROPY_RANDOM_THRESHOLD 4

BOOL is_random(PBYTE buf, UINT length);
void fill_histogram(ULONG histogram[], PBYTE buf, UINT length);
ULONG total(const ULONG histogram[]);
double entropy(const ULONG histogram[]);