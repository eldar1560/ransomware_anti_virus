#ifndef C_11_HOOKERX86_H
#define C_11_HOOKERX86_H


#include "Hooker.h"

namespace hooker {
    class HookerX86 : public Hooker {
    public:
        void doHook(void* func, void* newAddr, void** origFunc) const;
        ~HookerX86() {}
    };
}


#endif //C_11_HOOKERX86_H