#pragma once

inline std::wstring getExeDir()
{
    WCHAR exedir[MAX_PATH];
    GetModuleFileName(NULL, exedir, MAX_PATH);
    PathRemoveFileSpec(exedir);
    return std::wstring(exedir);
}