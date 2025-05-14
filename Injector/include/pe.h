#pragma once
#include <windows.h>
#include <vector>
#include <string>

class PEImage {
public:
    PEImage(const std::vector<char>& data);
    bool IsValid() const;
    LPVOID GetImageBase() const;
    DWORD GetImageSize() const;
    DWORD GetEntryPointRVA() const;
    bool HasRelocations() const;
    bool HasImportTable() const;
    bool HasTLS() const;
    const IMAGE_NT_HEADERS* GetNTHeaders() const;
    const IMAGE_DOS_HEADER* GetDOSHeader() const;
    const std::vector<char>& GetData() const;
private:
    std::vector<char> m_data;
    const IMAGE_DOS_HEADER* m_dos;
    const IMAGE_NT_HEADERS* m_nt;
};
