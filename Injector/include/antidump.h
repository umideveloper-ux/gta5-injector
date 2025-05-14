#pragma once
#include <windows.h>

class AntiDump {
public:
    static void PatchPEHeader(HANDLE hProc, LPVOID base);
    static void PatchSections(HANDLE hProc, LPVOID base);
    static void HandleHijack(HANDLE hProc);
};
