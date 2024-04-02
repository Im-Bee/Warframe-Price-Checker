#include "Exceptions.hpp"

#include <fstream>
#include <string>

#include "Core/Core.hpp"

// -----------------------------------------------------------------------------
size_t FindSourceSubpath(char const* const file)
{
    int k = 0;
    char search[] = "Source";
    size_t fileStrSize = strlen(file),
        searchStrSize = strlen(search);

    // If the path in some magical way starts with the search then it's going to
    // return 0 anyway
    for (size_t i = (fileStrSize - 1); i > 0; --i)
    {
        if (file[i] != search[k])
            continue;

        // Compare possible substring with the search
        for (size_t l = 0; l < (fileStrSize - i); ++l)
        {
            // Search has been found
            // and it starts at 'i'
            if (k == searchStrSize)
                return i;

            if (file[i + l] != search[k])
            {
                k = 0;
                break;
            }

            ++k;
        }
    }
    return 0;
}

// Exception -------------------------------------------------------------------
// Public ----------------------------------------------------------------------

// -----------------------------------------------------------------------------
WarframeSnail::Excepts::Exception::Exception(char const* const msg, char const* const file, const int& line) noexcept
{
    LogError(msg, file, line);
}

// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void WarframeSnail::Excepts::Exception::LogError(char const* const msg, char const* const file, const int& line)
{
    std::wstring wLogPath = WarframeSnail::GetCurrentPath();
    std::string logPath(wLogPath.begin(), wLogPath.end());
    std::ofstream log(logPath + "\\Error.log", std::ios_base::out | std::ios_base::app);
    if (!log.is_open())
        return;

    std::string output = std::string();
    output += msg;
    if (file)
    {
        output += " in file ";
        output += file;
    }
    if (line > -1)
    {
        output += " at line ";
        output += std::to_string(line);
    }
    log << output << '\n';
}
