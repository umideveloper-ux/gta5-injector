#include <windows.h>
#include <intrin.h>

void AntiDebug() {
    if (IsDebuggerPresent() || __debugbreak) ExitProcess(0);
    __try { __asm { int 3 } } __except (EXCEPTION_EXECUTE_HANDLER) { }
}

void CleanPEHeader(HMODULE hMod) {
    DWORD old;
    VirtualProtect(hMod, 0x1000, PAGE_READWRITE, &old);
    ZeroMemory(hMod, 0x1000);
    VirtualProtect(hMod, 0x1000, old, &old);
}

void UnlinkModule(HMODULE hMod) {
    ULONG_PTR peb = __readgsqword(0x60);
    ULONG_PTR ldr = *(ULONG_PTR*)(peb + 0x18);
    ULONG_PTR list = *(ULONG_PTR*)(ldr + 0x10);
    ULONG_PTR flink = list;
    do {
        ULONG_PTR entry = flink - 0x10;
        if (*(HMODULE*)(entry + 0x30) == hMod) {
            *(ULONG_PTR*)(*(ULONG_PTR*)entry) = *(ULONG_PTR*)(entry + 0x8);
            *(ULONG_PTR*)(*(ULONG_PTR*)(entry + 0x8) + 0x8) = *(ULONG_PTR*)entry;
            break;
        }
        flink = *(ULONG_PTR*)flink;
    } while (flink != list);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        AntiDebug();
        CleanPEHeader(hModule);
        UnlinkModule(hModule);
    }
    return TRUE;
}
