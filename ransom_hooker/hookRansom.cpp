#include "hookRansom.h"

#include "HookerX64.h"

#include <memory>

std::unique_ptr<hooker::Hooker> ransom_hooker = nullptr;
unsigned int encryptCounter = 0;

BOOL (*old_CryptEncrypt)(
	HCRYPTKEY,
	HCRYPTHASH,
	BOOL      ,
	DWORD     ,
	BYTE* ,
	DWORD*,
	DWORD 
);

BOOL WINAPI my_CryptEncrypt(
	HCRYPTKEY  hKey,
	HCRYPTHASH hHash,
	BOOL       Final,
	DWORD      dwFlags,
	BYTE* pbData,
	DWORD* pdwDataLen,
	DWORD      dwBufLen
)
{
	OutputDebugStringA("cryptEncrypt hook data:\n");
	OutputDebugStringA((LPCSTR)pbData);
	if (encryptCounter > 5) {
		ExitProcess(-1);
	}
	return old_CryptEncrypt(hKey,
		hHash,
		Final,
		dwFlags,
		pbData,
		pdwDataLen,
		dwBufLen);
}

void hookRansom()
{
	ransom_hooker = std::make_unique<hooker::HookerX64>();

	ransom_hooker->hook(reinterpret_cast<void*>(CryptEncrypt), reinterpret_cast<void*>(my_CryptEncrypt), reinterpret_cast<void**>(&old_CryptEncrypt));
	OutputDebugStringA("hook succesfully!\n");
}