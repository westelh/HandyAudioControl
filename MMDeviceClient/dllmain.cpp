// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include <combaseapi.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Using multi thread model because this library does not have main message queue
        // Implementation have to be considered about async call from other threads
        // COINIT_DISABLE_OLE1DDE disables old technology and its overheads
    {
        const auto result = CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
        if (result != S_OK) {
            CoUninitialize();
            return false;
        }
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        CoUninitialize();
        break;
    }
    return TRUE;
}

