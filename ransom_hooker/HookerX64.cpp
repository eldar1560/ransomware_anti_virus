#include "Hooker.h"
#include "HookerX64.h"
#include "HookerError.h"

#include <cstdlib>
#include <string>
#include <iostream>

void hooker::HookerX64::doHook(void* func, void* newAddr, void** origFunc) const {

	char* f = (char*)func;
	if (f[0] != '\x48' || f[1] != '\xff' || f[2] != '\x25') {
		throw hooker::error::HookerError("unsupported hook type");
	}

	unsigned int offset = *(unsigned int*)&f[3];

	size_t* original_addr = (size_t*)((size_t)func + offset + 7);

	void* old = gHookedMap[func];
	char* old_func = (char*)old;
	*(uint16_t*)&old_func[0] = 0x25ff;
	*(int*)&old_func[2] = 0x00000000;
	*(size_t*)&old_func[6] = *original_addr;

	*(uint16_t*)&f[0] = 0x25ff;
	*(int*)&f[2] = 0x00000000;
	*(size_t*)&f[6] = (size_t)newAddr;
}

