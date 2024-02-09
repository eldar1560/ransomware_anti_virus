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

	// find the CC instruction between jmps
	int index = HOOK_HEAD_SIZE * 2;
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

	if (origFunc) {
		memcpy((void*)((size_t)old + HOOK_HEAD_SIZE), func, index - HOOK_HEAD_SIZE);

		char* old_func = (char*)old;
		*(uint16_t*)&old_func[0] = 0x25ff;
		*(int*)&old_func[2] = 0x00000000;
		*(size_t*)&old_func[6] = *original_addr;


		*origFunc = old;
	}

	gHookedMap[func] = old;

	*(uint16_t*)&f[0] = 0x25ff;
	*(int*)&f[2] = 0x00000000;
	*(size_t*)&f[6] = (size_t)newAddr;
}

