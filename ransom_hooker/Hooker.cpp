#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "Hooker.h"
#include "HookerError.h"

hooker::Hooker::~Hooker() {
    for (auto& it : gHookedMap) {
        doUnHook(it.first);
    }

    gHookedMap.clear();
}

void hooker::Hooker::changeCodeAttrs(void *func, DWORD attr) const {    
    constexpr size_t PAGESIZE = 4096;
    size_t start = PAGE_START((size_t)func, PAGESIZE);

    DWORD old_protect = 0;

    if (!VirtualProtect(reinterpret_cast<LPVOID>(start),
        PAGESIZE,
        attr,
        &old_protect)) {
        throw hooker::error::HookerError("VirtualProtect error");
    }
}

void hooker::Hooker::hook(void *func, void *newAddr, void **origFunc, bool saveOrig) const {
    changeCodeAttrs(func, PAGE_EXECUTE_READWRITE);

    doHook(func, newAddr, origFunc);

    changeCodeAttrs(func, PAGE_EXECUTE_READ);
}

void hooker::Hooker::doUnHook(void *func) const {

    LPVOID addr = gHookedMap[func];
    if (addr == 0)
        throw hooker::error::HookerError("it must be hooked before");

    changeCodeAttrs(func, PAGE_EXECUTE_READWRITE);
    memcpy(func, (void*)((size_t)addr + HOOK_HEAD_SIZE), HOOK_HEAD_SIZE);
    changeCodeAttrs(func, PAGE_EXECUTE_READ);

    constexpr SIZE_T ALL_REGION = 0;
    VirtualFree(addr, ALL_REGION, MEM_RELEASE);   
}