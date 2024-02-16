#include "Hooker.h"
#include "HookerError.h"

#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <assert.h>

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

void hooker::Hooker::hook(void *func, void *newAddr, void **origFunc) const {
    changeCodeAttrs(func, PAGE_EXECUTE_READWRITE);

    int index = HOOK_HEAD_SIZE * 2;

    // TODO: use this to validate the size until 0xCC.
    // find the CC instruction between jmps
    //while (true) {
    //	if (static_cast<uint8_t>(f[index++]) == 0xcc || index >= 1024) {
    //		break;
    //	}
    //}
    //index += HOOK_HEAD_SIZE;


    void* old = VirtualAlloc(NULL, index, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (old == nullptr) {
        return;
    }

    memcpy((void*)((size_t)old + HOOK_HEAD_SIZE), func, index - HOOK_HEAD_SIZE);

    gHookedMap[func] = old;

    doHook(func, newAddr, origFunc);

    if (origFunc) {
        *origFunc = gHookedMap[func];
    }

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