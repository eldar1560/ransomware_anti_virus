#pragma once

#include <ntifs.h>

BOOLEAN isRandom(PVOID buffer, ULONG len);

#define ENCRPYTION_CHI_SQUARE_TRESHOLD 250