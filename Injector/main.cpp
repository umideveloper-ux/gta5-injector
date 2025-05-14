#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <random>
#include "include/pe.h"
#include "include/mapper.h"
#include "include/crypto.h"
#include "include/stealth.h"
#include "include/antidebug.h"
#include "include/antidump.h"

DWORD GetProcId(const std::wstring& procName) {
    DWORD procId = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(entry);
        if (Process32FirstW(snap, &entry)) {
            do {
                if (!_wcsicmp(entry.szExeFile, procName.c_str())) {
                    procId = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snap, &entry));
        }
    }
    CloseHandle(snap);
    return procId;
}

std::vector<char> ReadFileToMemory(const std::wstring& filePath) {
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return {};
    DWORD fileSize = GetFileSize(hFile, nullptr);
    std::vector<char> buffer(fileSize);
    DWORD read = 0;
    ReadFile(hFile, buffer.data(), fileSize, &read, nullptr);
    CloseHandle(hFile);
    return buffer;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc < 4) return 1;
    std::wstring procName = argv[1];
    std::wstring dllPath = argv[2];
    std::wstring key = argv[3];
    DWORD pid = GetProcId(procName);
    if (!pid) return 1;
    std::vector<char> dllBuffer = ReadFileToMemory(dllPath);
    if (dllBuffer.empty()) return 1;
    Crypto::Decrypt(dllBuffer, std::string(key.begin(), key.end()));
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProc) return 1;
    if (AntiDebug::IsDebuggerPresentRemote(hProc) || AntiDebug::HasHardwareBreakpoints() || AntiDebug::NtQueryInfoCheck(hProc) || AntiDebug::TimingCheck()) return 1;
    PEImage pe(dllBuffer);
    ManualMapper mapper(hProc, pe);
    if (!mapper.Map()) return 1;
    AntiDump::PatchPEHeader(hProc, mapper.GetRemoteBase());
    CloseHandle(hProc);
    return 0;
}
