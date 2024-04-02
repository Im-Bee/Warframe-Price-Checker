#include "ScreenCapture/ScreenCapture.hpp"
#include "Core/Core.hpp"

#include "LUL_Profiler.h"

#include <filesystem>

#define CURL_STATICLIB
#include <curl/curl.h>

int main()
{
	 WarframeSnail::ScreenCapture sc;

	 std::wstring wcur = WarframeSnail::GetCurrentPath();
	 std::filesystem::create_directory(wcur + L"\\Temp\\");
	 if (!sc.Capture()) throw;
	 if (!sc.SaveImagePNG((LPTSTR)(wcur + L"\\Temp\\Temp.png").c_str())) throw;
	 
	 WarframeSnail::ExeOCROnPNG((wcur + L"\\Temp\\Temp.png").c_str());
	 
	 // CURL* myCurl;
	 // myCurl = curl_easy_init();
	 // curl_easy_cleanup(myCurl);

	 wcur += L"\\Results\\";
	 std::filesystem::create_directory(wcur);
	 LUL_PROFILER_TIMER_SET_OUTPUT_DIR(wcur.c_str());
	 LUL_PROFILER_TIMER_RESULTS();
	 
	 return 0;
}	
