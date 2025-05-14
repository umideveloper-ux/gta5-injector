#pragma once
#include <windows.h>
#include <vector>
#include <string>

class Mapper {
public:
    Mapper(HANDLE proc, const std::vector<char>& dll);
    bool Map();
private:
    HANDLE hProc;
    const char* pSrc;
    size_t size;
    LPVOID remoteBase;
    bool WriteSections();
    bool Relocate();
    bool ResolveImports();
    bool CallEntry();
    void CleanPEHeader();
    void UnlinkModule();
};
