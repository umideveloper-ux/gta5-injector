#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "pe.h"

class ManualMapper {
public:
    ManualMapper(HANDLE hProc, const PEImage& pe);
    bool Map();
    LPVOID GetRemoteBase() const;
private:
    HANDLE m_hProc;
    const PEImage& m_pe;
    LPVOID m_remoteBase;
    bool AllocateMemory();
    bool WriteHeaders();
    bool WriteSections();
    bool PerformRelocations();
    bool ResolveImports();
    bool HandleTLS();
    bool CallEntryPoint();
    void CleanPEHeader();
    void UnlinkModule();
    void ExceptionHandler();
};
