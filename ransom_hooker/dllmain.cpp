// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "hookRansom.h"

#include <psapi.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    char image_name[256] = "";
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugStringA("DLL_PROCESS_ATTACH!\n");
        
        GetProcessImageFileNameA(GetCurrentProcess(), image_name, sizeof(image_name));
        OutputDebugStringA(image_name);
        hookRansom();
        break;
     
    case DLL_THREAD_ATTACH:
        OutputDebugStringA("DLL_THREAD_ATTACH!\n");
        break;
    case DLL_THREAD_DETACH:
        OutputDebugStringA("DLL_THREAD_DETACH!\n");
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugStringA("DLL_PROCESS_DETACH!\n");
        break;
    }
    return TRUE;
}

