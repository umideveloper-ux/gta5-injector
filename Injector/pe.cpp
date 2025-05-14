#include "include/pe.h"

PEImage::PEImage(const std::vector<char>& data) : m_data(data) {
    m_dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(m_data.data());
    m_nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(m_data.data() + m_dos->e_lfanew);
}

bool PEImage::IsValid() const {
    return m_dos->e_magic == IMAGE_DOS_SIGNATURE && m_nt->Signature == IMAGE_NT_SIGNATURE;
}

LPVOID PEImage::GetImageBase() const {
    return reinterpret_cast<LPVOID>(m_nt->OptionalHeader.ImageBase);
}

DWORD PEImage::GetImageSize() const {
    return m_nt->OptionalHeader.SizeOfImage;
}

DWORD PEImage::GetEntryPointRVA() const {
    return m_nt->OptionalHeader.AddressOfEntryPoint;
}

bool PEImage::HasRelocations() const {
    return m_nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress != 0;
}

bool PEImage::HasImportTable() const {
    return m_nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress != 0;
}

bool PEImage::HasTLS() const {
    return m_nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress != 0;
}

const IMAGE_NT_HEADERS* PEImage::GetNTHeaders() const {
    return m_nt;
}

const IMAGE_DOS_HEADER* PEImage::GetDOSHeader() const {
    return m_dos;
}

const std::vector<char>& PEImage::GetData() const {
    return m_data;
}
