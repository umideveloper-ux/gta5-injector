#include "include/antidebug.h"
#include <intrin.h>
#include <winternl.h>
#include <chrono>

bool AntiDebug::IsDebuggerPresentRemote(HANDLE hProc) {
    BOOL dbg = FALSE;
    ReadProcessMemory(hProc, (LPCVOID)((ULONG_PTR)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtGlobalFlag")), &dbg, sizeof(dbg), nullptr);
    return dbg;
}

bool AntiDebug::HasHardwareBreakpoints() {
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE hThread = GetCurrentThread();
    if (GetThreadContext(hThread, &ctx)) {
        if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3) return true;
    }
    return false;
}

bool AntiDebug::NtQueryInfoCheck(HANDLE hProc) {
    typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    pNtQueryInformationProcess NtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
    ULONG debugPort = 0;
    NTSTATUS status = NtQueryInformationProcess(hProc, 7, &debugPort, sizeof(debugPort), nullptr);
    return (status == 0 && debugPort != 0);
}

bool AntiDebug::TimingCheck() {
    auto t1 = std::chrono::high_resolution_clock::now();
    for (volatile int i = 0; i < 1000000; ++i);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    return diff > 10000;
}
