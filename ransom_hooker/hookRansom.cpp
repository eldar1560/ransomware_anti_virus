#include "hookRansom.h"

#include "HookerX64.h"
#include "HookerX86.h"
#include "randomness.h"

#include <memory>
#include <string>
#include <fstream>
#include <Psapi.h>

std::unique_ptr<hooker::Hooker> ransom_hooker = nullptr;
unsigned int encryptCounter = 0;

BOOL (WINAPI *old_CryptEncrypt)(
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
	if (is_random(pbData, *pdwDataLen)) {
		OutputDebugStringA("Found random key encryption, incrementing the counter");
		encryptCounter++;
	}	

	if (encryptCounter > 5) {
		OutputDebugStringA("Found ransomware, killing it");

		SYSTEMTIME current_time = { 0 };
		GetLocalTime(&current_time);
		char data_to_write[1024] = { 0 };
		char process_path[MAX_PATH * 2] = "";
		GetProcessImageFileNameA(GetCurrentProcess(), process_path, sizeof(process_path));
		snprintf(data_to_write, sizeof(data_to_write), "Ransom Hooker - %hu/%hu/%hu %hu:%hu:%hu - %s\n",
			current_time.wDay,
			current_time.wMonth,
			current_time.wYear,
			current_time.wHour,
			current_time.wMinute,
			current_time.wSecond,
			process_path);

		std::ofstream log_file("c:\\windows\\temp\\ransomav.log", std::ios_base::app);
		log_file << data_to_write;
		log_file.close();
	
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
#if defined( _WIN64 )

	ransom_hooker = std::make_unique<hooker::HookerX64>();
#elif defined( _WIN32 )
	ransom_hooker = std::make_unique<hooker::HookerX86>();
#endif
	

	ransom_hooker->hook(reinterpret_cast<void*>(CryptEncrypt), reinterpret_cast<void*>(my_CryptEncrypt), reinterpret_cast<void**>(&old_CryptEncrypt));
	OutputDebugStringA("hook succesfully!\n");
}