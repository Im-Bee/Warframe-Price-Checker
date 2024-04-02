#include "Core.hpp"

#include <future>
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>

// -----------------------------------------------------------------------------
const wchar_t* WarframeSnail::GetCurrentPath()
{
    static bool init = false;
    static LPWSTR current = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * MAX_PATH);
    if (init)
        return current;

    if (!current)
        throw std::bad_alloc();
    GetModuleFileName(NULL, current, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(current, L'\\');
    *(lastSlash + 1) = L'\0';

    init = true;
    return current;
}

//  ----------------------------------------------------------------------------
void WarframeSnail::ExeOCROnPNG(wchar_t const* const pngPath)
{
    std::string cmd = {};
    cmd += "start ";
    std::wstring wstrCurrent = GetCurrentPath();
    std::string convertedStrCurrent(wstrCurrent.begin(), wstrCurrent.end());
    cmd += convertedStrCurrent;
    cmd += "\\External\\Tesseract-OCR\\tesseract.exe ";
    std::wstring wstrPngPath = pngPath;
    std::string convertedStrPngPath(wstrPngPath.begin(), wstrPngPath.end());
    cmd += convertedStrPngPath;
    cmd += " ";
    cmd += convertedStrCurrent;
    cmd += "\\Temp\\o";

    std::async(system, cmd.c_str());
}
