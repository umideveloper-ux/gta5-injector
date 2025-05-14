#pragma once
#include <windows.h>
#include <string>

class Stealth {
public:
    static bool HollowProcess(const std::wstring& targetPath, const std::vector<char>& payload, LPVOID& remoteBase, HANDLE& hProc);
    static bool GhostProcess(const std::wstring& targetPath, const std::vector<char>& payload, LPVOID& remoteBase, HANDLE& hProc);
};
