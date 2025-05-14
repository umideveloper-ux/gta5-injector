#pragma once
#include <windows.h>

class AntiDebug {
public:
    static bool IsDebuggerPresentRemote(HANDLE hProc);
    static bool HasHardwareBreakpoints();
    static bool NtQueryInfoCheck(HANDLE hProc);
    static bool TimingCheck();
};
