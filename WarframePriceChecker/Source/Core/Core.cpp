#include "Core.hpp"

#include "LUL_Profiler.h"

#include <algorithm>
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

#define WS_IN_OUT_STR_SPACER ':'

// -----------------------------------------------------------------------------
const wchar_t* WarframeSnail::GetCurrentPath()
{
    LUL_PROFILER_TIMER_START();

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
    LUL_PROFILER_TIMER_START();

    string cmd = {};
    // cmd += "start "; // For async execution
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

    system(cmd.c_str());
}

// -----------------------------------------------------------------------------
void WarframeSnail::ExePy(wchar_t const* const pyPath, char const* const args)
{
    LUL_PROFILER_TIMER_START();

    string cmd = {};
    cmd += "python3 ";
    wstring wstrPyPath = pyPath;
    string convertedStrPyPath(wstrPyPath.begin(), wstrPyPath.end());
    cmd += convertedStrPyPath;
    cmd += " ";
    cmd += args;

    system(cmd.c_str());
}

const std::vector<std::pair<std::string, std::string>>& WarframeSnail::CreateDict()
{
    static std::vector<std::pair<std::string, std::string>> r = {};
    if (!r.empty())
        return r;

    return r;
}

// -----------------------------------------------------------------------------
WarframeSnail::ItemEntry CreateEntryFromStr(wchar_t* str, const int& strSize)
{
    LUL_PROFILER_TIMER_START();

    ItemEntry ie;

    constexpr int minVal = 0;
    constexpr int maxVal = 10000;

    wchar_t* i = wcsrchr(str, L':');
    if (!i)
    {
        return ItemEntry::Empty();
    }

    *i = L'\0';
    ie.ItemName = str;
    try
    {
        ie.Price = stoi(i + 1);
    }
    catch (...)
    {
        return ItemEntry::Empty();
    }

    if (ie.Price > maxVal ||
        ie.Price < minVal)
    {
        ie.Price = 0;
    }

    return ie;
}

// -----------------------------------------------------------------------------
std::vector<WarframeSnail::ItemEntry> WarframeSnail::ReadPythonResults(wchar_t const* const resultsPath)
{
    LUL_PROFILER_TIMER_START();

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
            continue;

        result.push_back(move(ie));
    }

    file.close();

#ifndef _DEBUG
    filesystem::remove(resultsPath);
#endif // !_DEBUG

    return result;
}

// -----------------------------------------------------------------------------
std::string CreateStrFromStr(char* str, const int& strSize, bool& shouldBeCombined)
{
    LUL_PROFILER_TIMER_START();

    string r = str;
    // A vector with a pair of itmes.
    // First item represents a string normalized for search
    // Second a string normalized for output
    const vector<pair<string, string>>& possibleItems = WarframeSnail::CreateDict(); 

    r.erase(remove(r.begin(), r.end(), ' '), r.end());
    for_each(r.begin(), r.end(), [](char& c)
        {
            return tolower(c);
        });

    int diff;
    int smallestDiff = INT_MAX;
    int iPossibleMatch = -1;
    for (int i = 0; i < possibleItems.size(); ++i)
    {
        diff = r.compare(possibleItems[i].first);

        if (diff == 0)
        {
            r = possibleItems[i].second;
            break;
        }
        if (diff < smallestDiff)
        {
            smallestDiff = diff;
            iPossibleMatch = i;
        }
    }

    if (iPossibleMatch >= 0)
        r = possibleItems[iPossibleMatch].second;

    return r;
}

// -----------------------------------------------------------------------------
std::string WarframeSnail::ReadOCRResultsForPython(wchar_t const* const resultsPath)
{
    LUL_PROFILER_TIMER_START();

    string result = string();
    ifstream file(resultsPath, ios_base::in);
    if (!file.is_open())
        throw invalid_argument("ReadOCRResults couldn't open the file");

    constexpr int maxReadBuff = 512;
    char* readBuff = new char[maxReadBuff];
    string storeForLater = string();
    bool shouldBeCombined = false;
    while (!file.eof())
    {
        file.getline(readBuff, maxReadBuff);
        if (readBuff[0] == L'\0')
            continue;

        auto rStr = CreateStrFromStr(readBuff, maxReadBuff, shouldBeCombined);
        if (rStr.empty())
        {
            continue;
        }
        // Combine two lines together
        if (shouldBeCombined)
        {
            storeForLater = rStr;
        }
        if (!storeForLater.empty())
        {
            rStr = storeForLater + '_' + rStr;
            storeForLater = "";
        }

        // The "for python" part
        result += move(rStr);
        result += WS_IN_OUT_STR_SPACER;
    }
    delete[] readBuff;

    file.close();

#ifndef _DEBUG
    filesystem::remove(resultsPath);
#endif // !_DEBUG

    return result;
}


