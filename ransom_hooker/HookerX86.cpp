#include "HookerX86.h"

void hooker::HookerX86::doHook(void* func, void* newAddr, void** origFunc) const {
    char* f = (char*)func;

    void* old = gHookedMap[func];

    memcpy(old, func, HOOK_HEAD_SIZE);

    f[0] = 0x68; // push addr
    *(long*)&f[1] = (long)newAddr;
    f[5] = 0xc3; // ret
}