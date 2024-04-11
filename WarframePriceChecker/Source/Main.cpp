#include "Core/Core.hpp"
#include "ScreenCapture/ScreenCapture.hpp"

#include "LUL_Profiler.h"

#include <filesystem>

// CURL reminder
// #define CURL_STATICLIB
// #include <curl/curl.h>

int main()
{
	 WarframeSnail::ScreenCapture sc;
	 
	 std::wstring wcur = WarframeSnail::GetCurrentPath();
	 std::filesystem::create_directory(wcur + L"\\Temp\\");
	 if (!sc.Capture()) throw;
	 if (!sc.SaveImagePNG((LPTSTR)(wcur + L"\\Temp\\Temp.png").c_str())) throw;
	 
	 WarframeSnail::ExeOCROnPNG((wcur + L"\\Temp\\Temp.png").c_str());
	 auto ocrResults = WarframeSnail::ReadOCRResultsForPython((wcur + L"\\Temp\\o.txt").c_str());
	 WarframeSnail::ExePy((wcur + L"\\Temp\\o.txt").c_str(), ocrResults.c_str());
	 auto pythonResults = WarframeSnail::ReadPythonResults((wcur + L"\\Temp\\o.txt").c_str());

	 wcur += L"\\Results\\";
	 std::filesystem::create_directory(wcur);
	 LUL_PROFILER_TIMER_SET_OUTPUT_DIR(wcur.c_str());
	 LUL_PROFILER_TIMER_RESULTS();
	 
	 return 0;
}	
