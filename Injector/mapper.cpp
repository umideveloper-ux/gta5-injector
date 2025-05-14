#include "include/mapper.h"
#include <winternl.h>

ManualMapper::ManualMapper(HANDLE hProc, const PEImage& pe) : m_hProc(hProc), m_pe(pe), m_remoteBase(nullptr) {}

bool ManualMapper::Map() {
    if (!AllocateMemory()) return false;
    if (!WriteHeaders()) return false;
    if (!WriteSections()) return false;
    if (!PerformRelocations()) return false;
    if (!ResolveImports()) return false;
    if (!HandleTLS()) return false;
    CleanPEHeader();
    UnlinkModule();
    ExceptionHandler();
    if (!CallEntryPoint()) return false;
    return true;
}

LPVOID ManualMapper::GetRemoteBase() const {
    return m_remoteBase;
}

bool ManualMapper::AllocateMemory() {
    m_remoteBase = VirtualAllocEx(m_hProc, m_pe.GetImageBase(), m_pe.GetImageSize(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!m_remoteBase) m_remoteBase = VirtualAllocEx(m_hProc, nullptr, m_pe.GetImageSize(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return m_remoteBase != nullptr;
}

bool ManualMapper::WriteHeaders() {
    return WriteProcessMemory(m_hProc, m_remoteBase, m_pe.GetData().data(), m_pe.GetNTHeaders()->OptionalHeader.SizeOfHeaders, nullptr);
}

bool ManualMapper::WriteSections() {
    auto nt = m_pe.GetNTHeaders();
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        WriteProcessMemory(m_hProc, (LPVOID)((uintptr_t)m_remoteBase + sec[i].VirtualAddress), m_pe.GetData().data() + sec[i].PointerToRawData, sec[i].SizeOfRawData, nullptr);
    }
    return true;
}

bool ManualMapper::PerformRelocations() {
    auto nt = m_pe.GetNTHeaders();
    DWORD delta = (DWORD)((uintptr_t)m_remoteBase - nt->OptionalHeader.ImageBase);
    if (!delta) return true;
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (!dir.Size) return true;
    auto base = m_pe.GetData().data();
    DWORD offset = dir.VirtualAddress;
    while (offset < dir.VirtualAddress + dir.Size) {
        auto block = (IMAGE_BASE_RELOCATION*)(base + offset);
        if (!block->SizeOfBlock) break;
        DWORD count = (block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
        WORD* list = (WORD*)(block + 1);
        for (DWORD i = 0; i < count; ++i) {
            if ((list[i] >> 12) == IMAGE_REL_BASED_HIGHLOW) {
                DWORD rva = block->VirtualAddress + (list[i] & 0xFFF);
                DWORD val = 0;
                ReadProcessMemory(m_hProc, (LPVOID)((uintptr_t)m_remoteBase + rva), &val, sizeof(DWORD), nullptr);
                val += delta;
                WriteProcessMemory(m_hProc, (LPVOID)((uintptr_t)m_remoteBase + rva), &val, sizeof(DWORD), nullptr);
            }
        }
        offset += block->SizeOfBlock;
    }
    return true;
}

bool ManualMapper::ResolveImports() {
    auto nt = m_pe.GetNTHeaders();
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.Size) return true;
    auto base = m_pe.GetData().data();
    auto imp = (IMAGE_IMPORT_DESCRIPTOR*)(base + dir.VirtualAddress);
    while (imp->Name) {
        HMODULE mod = LoadLibraryA((char*)(base + imp->Name));
        auto thunk = (IMAGE_THUNK_DATA*)(m_remoteBase ? (uintptr_t)m_remoteBase + imp->FirstThunk : 0);
        auto orig = (IMAGE_THUNK_DATA*)(base + imp->OriginalFirstThunk);
        for (; orig->u1.AddressOfData; ++orig, ++thunk) {
            FARPROC fn = nullptr;
            if (orig->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                fn = GetProcAddress(mod, (LPCSTR)(orig->u1.Ordinal & 0xFFFF));
            } else {
                auto info = (IMAGE_IMPORT_BY_NAME*)(base + orig->u1.AddressOfData);
                fn = GetProcAddress(mod, (char*)info->Name);
            }
            WriteProcessMemory(m_hProc, thunk, &fn, sizeof(fn), nullptr);
        }
        ++imp;
    }
    return true;
}

bool ManualMapper::HandleTLS() {
    auto nt = m_pe.GetNTHeaders();
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
    if (!dir.Size) return true;
    auto base = m_pe.GetData().data();
    auto tls = (IMAGE_TLS_DIRECTORY*)(base + dir.VirtualAddress);
    auto cb = (PIMAGE_TLS_CALLBACK*)(tls->AddressOfCallBacks - nt->OptionalHeader.ImageBase + (uintptr_t)m_remoteBase);
    for (; *cb; ++cb) {
        HANDLE hThread = CreateRemoteThread(m_hProc, nullptr, 0, (LPTHREAD_START_ROUTINE)*cb, m_remoteBase, 0, nullptr);
        if (hThread) CloseHandle(hThread);
    }
    return true;
}

bool ManualMapper::CallEntryPoint() {
    auto nt = m_pe.GetNTHeaders();
    LPTHREAD_START_ROUTINE entry = (LPTHREAD_START_ROUTINE)((uintptr_t)m_remoteBase + nt->OptionalHeader.AddressOfEntryPoint);
    HANDLE hThread = CreateRemoteThread(m_hProc, nullptr, 0, entry, m_remoteBase, 0, nullptr);
    if (!hThread) return false;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    return true;
}

void ManualMapper::CleanPEHeader() {
    DWORD old;
    VirtualProtectEx(m_hProc, m_remoteBase, 0x1000, PAGE_READWRITE, &old);
    std::vector<char> zero(0x1000, 0);
    WriteProcessMemory(m_hProc, m_remoteBase, zero.data(), zero.size(), nullptr);
    VirtualProtectEx(m_hProc, m_remoteBase, 0x1000, old, &old);
}

void ManualMapper::UnlinkModule() {
    PROCESS_BASIC_INFORMATION pbi;
    ULONG retLen;
    NtQueryInformationProcess(m_hProc, 0, &pbi, sizeof(pbi), &retLen);
    PEB peb;
    ReadProcessMemory(m_hProc, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr);
    LIST_ENTRY ldrList;
    ReadProcessMemory(m_hProc, (PBYTE)peb.Ldr + 0x10, &ldrList, sizeof(ldrList), nullptr);
    LIST_ENTRY* flink = ldrList.Flink;
    while (flink != &ldrList) {
        LDR_DATA_TABLE_ENTRY entry;
        ReadProcessMemory(m_hProc, CONTAINING_RECORD(flink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks), &entry, sizeof(entry), nullptr);
        if (entry.DllBase == m_remoteBase) {
            LIST_ENTRY prev, next;
            ReadProcessMemory(m_hProc, entry.InLoadOrderLinks.Blink, &prev, sizeof(prev), nullptr);
            ReadProcessMemory(m_hProc, entry.InLoadOrderLinks.Flink, &next, sizeof(next), nullptr);
            prev.Flink = entry.InLoadOrderLinks.Flink;
            next.Blink = entry.InLoadOrderLinks.Blink;
            WriteProcessMemory(m_hProc, entry.InLoadOrderLinks.Blink, &prev, sizeof(prev), nullptr);
            WriteProcessMemory(m_hProc, entry.InLoadOrderLinks.Flink, &next, sizeof(next), nullptr);
            break;
        }
        flink = entry.InLoadOrderLinks.Flink;
    }
}

void ManualMapper::ExceptionHandler() {
    // Exception handling ve hata yönetimi burada yapılacak (detaylı kod eklenecek)
}
