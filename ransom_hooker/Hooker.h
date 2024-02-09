//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKER_H
#define C_11_HOOKER_H

#include <Windows.h>
#include <cstdint>
#include <unordered_map>

#define HOOK_HEAD_SIZE 14

namespace hooker {
#define PAGE_START(x,pagesize)	((x) &~ ((pagesize) - 1))
    class Hooker {
    public:
        void changeCodeAttrs(void *func, DWORD attr) const;
        void hook(void *func, void *newAddr, void **origFunc, bool saveOrig=true) const;

		virtual ~Hooker() = 0;

    protected:
        virtual void doHook(void *func, void *newAddr, void **origFunc) const = 0;
        virtual void doUnHook(void *func) const;
        mutable std::unordered_map<LPVOID, LPVOID> gHookedMap;
    };
}


#endif //C_11_HOOKER_H
