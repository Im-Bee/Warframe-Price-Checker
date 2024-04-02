#pragma once

#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif // !WIN32_LEAN_AND_MEAN 

#include <Windows.h>

namespace WarframeSnail
{
	class ScreenCapture
	{
		HBITMAP m_hBitmap = nullptr;

	public:
		ScreenCapture() noexcept = default;
		
		~ScreenCapture() noexcept;

	public:
		/*
		* Takes a screenshot.
		* @returns True if succeeded */
		bool Capture() noexcept;

		/* 
		* Saves the last captured image to provided path
		* @param path - If nullptr then the path is set to "./Temp.bmp"
		* @returns True if succeeded */
		bool SaveImageBMP(LPTSTR path = nullptr) noexcept;

		/*
		* Saves the last captured image to provided path
		* @param path - If nullptr then the path is set to "./Temp.bmp"
		* @returns True if succeeded */
		bool SaveImagePNG(LPTSTR path = nullptr) noexcept;

	private:
		void CaptureScreenInternal();

		PBITMAPINFO CreateBitmapInfoStruct(
			HBITMAP hBmp);

		void CreateBMPFile(
			LPTSTR pszFile,
			PBITMAPINFO pbi,
			HBITMAP hBMP,
			HDC hDC);

		void CreatePNGFile(
			LPTSTR pszFile,
			PBITMAPINFO pbi,
			HBITMAP hBMP,
			HDC hDC);

		void CleanUpAfterLastCapture();

		void ReverseBytes(LPBYTE& bytes, PBITMAPINFO pbi);

	private:
		HDC m_hScreenDC = nullptr,
			m_hMemoryDC = nullptr;

		PBITMAPINFO m_pBmi = nullptr;
	};
}