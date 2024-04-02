#pragma once

#include <exception>
#include <string>

size_t FindSourceSubpath(char const* const file);

#define WARFRAMESNAIL_EXECPT_HELPER()               \
std::string(__FILE__).substr(FindSourceSubpath(__FILE__)).c_str(), __LINE__

namespace WarframeSnail::Excepts
{
    class Exception
        : public std::exception
    {
    public:
        Exception() noexcept = default;

        explicit Exception(
            char const* const msg,
            char const* const file = 0,
            const int& line = -1) noexcept;

        ~Exception() noexcept = default;

    private:
        void LogError(
            char const* const msg, 
            char const* const file, 
            const int& line);
    };

    class CapturingScreen : public Exception
    {
    public:
        CapturingScreen(
            char const* const file = 0,
            const int& line = -1) noexcept
            : Exception(
                "WarframeSnail - CapturingScreen - Unknown error",
                file,
                line)
        {
        }

        explicit CapturingScreen(
            std::string customMsg,
            char const* const file = 0,
            const int& line = -1) noexcept
            : Exception(
                ("WarframeSnail - CapturingScreen - " + customMsg).c_str(),
                file,
                line)
        {
        }

        ~CapturingScreen() noexcept = default;
    };

    class HandlingFile : public Exception
    {
    public:
        HandlingFile(
            char const* const file = 0,
            const int& line = -1) noexcept
            : Exception(
                "WarframeSnail - HandlingFile - Unknown error",
                file,
                line)
        {
        }

        ~HandlingFile() noexcept = default;
    };
}
