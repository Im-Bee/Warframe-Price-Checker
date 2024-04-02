#include "Core.hpp"

#include <future>
#include <filesystem>
#include <fstream>
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>

using namespace std;
using namespace WarframeSnail;

// -----------------------------------------------------------------------------
const wchar_t* WarframeSnail::GetCurrentPath()
{
    static bool init = false;
    static LPWSTR current = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * MAX_PATH);
    if (init)
        return current;

    if (!current)
        throw bad_alloc();
    GetModuleFileName(NULL, current, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(current, L'\\');
    *(lastSlash + 1) = L'\0';

    init = true;
    return current;
}

//  ----------------------------------------------------------------------------
void WarframeSnail::ExeOCROnPNG(wchar_t const* const pngPath)
{
    string cmd = {};
    cmd += "start ";
    wstring wstrCurrent = GetCurrentPath();
    string convertedStrCurrent(wstrCurrent.begin(), wstrCurrent.end());
    cmd += convertedStrCurrent;
    cmd += "\\External\\Tesseract-OCR\\tesseract.exe ";
    wstring wstrPngPath = pngPath;
    string convertedStrPngPath(wstrPngPath.begin(), wstrPngPath.end());
    cmd += convertedStrPngPath;
    cmd += " ";
    cmd += convertedStrCurrent;
    cmd += "\\Temp\\o";

    async(system, cmd.c_str());
}

// -----------------------------------------------------------------------------
WarframeSnail::ItemEntry CreateEntryFromStr(wchar_t* str, const int& strSize)
{
    ItemEntry ie;

    int i;
    for (i = 0;
        i < strSize &&
        str[i] != L':';
        ++i)
    {
    }

    str[i++] = L'\0';
    ie.ItemName = str;
    try
    {
        ie.Price = stoi(&str[i]);
    }
    catch (...)
    {
        return ItemEntry::Empty();
    }

    return ie;
}

// -----------------------------------------------------------------------------
std::vector<WarframeSnail::ItemEntry> WarframeSnail::ReadOCRResults(wchar_t const* const resultsPath)
{
    vector<ItemEntry> result = {};
    wifstream file(resultsPath, ios_base::in);
    if (!file.is_open())
        throw invalid_argument("ReadOCRResults couldn't open the file");

    constexpr int maxReadBuff = 256;
    wchar_t* readBuff = new wchar_t[maxReadBuff];
    while (!file.eof())
    {
        file.getline(readBuff, maxReadBuff);
        if (readBuff[0] == L'\0')
            continue;
        
        auto ie = CreateEntryFromStr(readBuff, maxReadBuff);
        if (ie == ItemEntry::Empty())
            result.push_back(std::move(ie));
    }

    file.close();
    // filesystem::remove(resultsPath);

    return result;
}


