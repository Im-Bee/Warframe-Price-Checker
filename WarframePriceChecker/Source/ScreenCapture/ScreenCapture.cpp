#include "ScreenCapture.hpp"

#include "Exceptions/Exceptions.hpp"
#include "Core/Core.hpp"

#include "LUL_Profiler.h"
#include "../../External/LodePNG/lodepng.h"

// ScreenCapture ---------------------------------------------------------------
// Public ----------------------------------------------------------------------

// -----------------------------------------------------------------------------
WarframeSnail::ScreenCapture::~ScreenCapture() noexcept
{
    CleanUpAfterLastCapture();
}

// -----------------------------------------------------------------------------
bool WarframeSnail::ScreenCapture::Capture() noexcept
{ 
    LUL_PROFILER_TIMER_START();

    CleanUpAfterLastCapture();

    try
    {
        CaptureScreenInternal();
    }
    catch (...)
    {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
bool WarframeSnail::ScreenCapture::SaveImageBMP(LPTSTR path) noexcept
{
    LUL_PROFILER_TIMER_START();

    // Check if there is even any image to save
    if (m_hBitmap == nullptr ||
        m_hScreenDC == nullptr)
        return false;

    try
    {
        if (!m_pBmi)
            m_pBmi = CreateBitmapInfoStruct(m_hBitmap);
    }
    catch (...)
    {
        return false;
    }

    try
    {
        if (path == nullptr)
        {
            path = (LPTSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * MAX_PATH);
            if (!path)
                throw std::bad_alloc();
            wcscpy_s(path, MAX_PATH, (LPWSTR)WarframeSnail::GetCurrentPath());
            wcscat_s(path, MAX_PATH, L"\\Temp.bmp");
        }

        CreateBMPFile(
            path,
            m_pBmi,
            m_hBitmap,
            m_hScreenDC);
    }
    catch (...)
    {
        return false;
    }

	return true;
}

// -----------------------------------------------------------------------------
bool WarframeSnail::ScreenCapture::SaveImagePNG(LPTSTR path) noexcept
{
    LUL_PROFILER_TIMER_START();

    try
    {
        if (!m_pBmi)
            m_pBmi = CreateBitmapInfoStruct(m_hBitmap);
    }
    catch (...)
    {
        return false;
    }
    
    try
    {
        if (path == nullptr)
        {
            path = (LPTSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * MAX_PATH);
            if (!path)
                throw std::bad_alloc();
            wcscpy_s(path, MAX_PATH, (LPWSTR)WarframeSnail::GetCurrentPath());
            wcscat_s(path, MAX_PATH, L"\\Temp.png");
        }

        CreatePNGFile(
            path,
            m_pBmi,
            m_hBitmap,
            m_hScreenDC);
    }
    catch (...)
    {
        return false;
    }

    return true;
}
// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void WarframeSnail::ScreenCapture::CaptureScreenInternal()
{
    LUL_PROFILER_TIMER_START();

    m_hScreenDC = CreateDC(
        L"DISPLAY",
        nullptr,
        nullptr,
        nullptr);
    m_hMemoryDC = CreateCompatibleDC(m_hScreenDC);

    // Get width and height
    int width = GetDeviceCaps(m_hScreenDC, HORZRES);
    int height = GetDeviceCaps(m_hScreenDC, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(m_hScreenDC, width, height);
    if (!hBitmap)
        throw WarframeSnail::Excepts::CapturingScreen();

    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(m_hMemoryDC, hBitmap));
    if (hOldBitmap == NULL ||
        hOldBitmap == HGDI_ERROR)
        throw WarframeSnail::Excepts::CapturingScreen();

    // BitBlt() takes most of the time in this method
    if (!BitBlt(
        m_hMemoryDC,
        0, // Starting width, x-coordinate
        0, // Starting height, y-coordinate
        width,
        height,
        m_hScreenDC,
        0, // The y-coordinate, in logical units, of the upper-left corner of the source rectangle
        0, // The x-coordinate, in logical units, of the upper-left corner of the source rectangle
        SRCCOPY))
    {
        throw WarframeSnail::Excepts::CapturingScreen(std::string("BitBlt() failed"), WARFRAMESNAIL_EXECPT_HELPER());
    }

    m_hBitmap = static_cast<HBITMAP>(SelectObject(m_hMemoryDC, hOldBitmap));
    if (m_hBitmap == NULL ||
        m_hBitmap == HGDI_ERROR)
        throw WarframeSnail::Excepts::CapturingScreen();
}

// -----------------------------------------------------------------------------
PBITMAPINFO WarframeSnail::ScreenCapture::CreateBitmapInfoStruct(HBITMAP hBmp)
{
    LUL_PROFILER_TIMER_START();

    BITMAP bmp;
    PBITMAPINFO pBmi;
    WORD cClrBits;

    // Retrieve the bitmap color format, width, and height.  
    if (!GetObject(hBmp, sizeof(BITMAP), (LPVOID)&bmp))
        throw WarframeSnail::Excepts::CapturingScreen();

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

    if (cClrBits < 24)
    {
        pBmi = (PBITMAPINFO)LocalAlloc(
            LPTR,
            sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
    }
    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 
    else
    {
        pBmi = (PBITMAPINFO)LocalAlloc(
            LPTR,
            sizeof(BITMAPINFOHEADER));
    }

    // Initialize the fields in the BITMAPINFO structure.  
    pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pBmi->bmiHeader.biWidth = bmp.bmWidth;
    pBmi->bmiHeader.biHeight = bmp.bmHeight;
    pBmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pBmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pBmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pBmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pBmi->bmiHeader.biSizeImage = ((pBmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * pBmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pBmi->bmiHeader.biClrImportant = 0;
    return pBmi;
}

// -----------------------------------------------------------------------------
void WarframeSnail::ScreenCapture::CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
    LUL_PROFILER_TIMER_START();

    HANDLE hf;                  // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE* hp;                   // byte pointer  
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, pbih->biSizeImage);

    if (!lpBits)
        throw std::bad_alloc();

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    if (!GetDIBits(
        hDC, 
        hBMP, 
        0, 
        (WORD)pbih->biHeight, 
        lpBits, 
        pbi,
        DIB_RGB_COLORS))
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::CapturingScreen();
    }

    // Create the .BMP file.  
    hf = CreateFile(
        pszFile,
        GENERIC_READ | GENERIC_WRITE,
        (DWORD)0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);
    if (hf == INVALID_HANDLE_VALUE)
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::HandlingFile();
    }

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +  pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if (!WriteFile(
        hf, 
        (LPVOID)&hdr, 
        sizeof(BITMAPFILEHEADER),
        (LPDWORD)&dwTmp, 
        NULL))
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::HandlingFile();
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    if (!WriteFile(
        hf, 
        (LPVOID)pbih, 
        sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD),
        (LPDWORD)&dwTmp, 
        (NULL)))
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::HandlingFile();
    }

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(
        hf, 
        (LPSTR)hp, 
        (int)cb, 
        (LPDWORD)&dwTmp, 
        NULL))
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::HandlingFile();
    }

    // Close the .BMP file.  
    if (!CloseHandle(hf))
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::HandlingFile();
    }

    HeapFree(GetProcessHeap(), 0, lpBits);
}

// -----------------------------------------------------------------------------
void WarframeSnail::ScreenCapture::CreatePNGFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
    LUL_PROFILER_TIMER_START();

    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, pbih->biSizeImage);

    if (!lpBits)
        throw std::bad_alloc();

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    if (!GetDIBits(
        hDC,
        hBMP,
        0,
        (WORD)pbih->biHeight,
        lpBits,
        pbi,
        DIB_RGB_COLORS))
    {
        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::CapturingScreen();
    }

    // Convert wide string
    std::wstring fileWStr = pszFile;
    std::string fileStr(fileWStr.begin(), fileWStr.end());

    ReverseBytes(lpBits, pbi);

    // Encode the image
    unsigned error = lodepng::encode(
        fileStr,
        lpBits,
        m_pBmi->bmiHeader.biWidth,
        m_pBmi->bmiHeader.biHeight);

    if (error)
    {
        std::string msg = "Png conversion ";
        msg += lodepng_error_text(error);
        msg += ", with code: ";
        msg += std::to_string(error);

        HeapFree(GetProcessHeap(), 0, lpBits);
        throw WarframeSnail::Excepts::CapturingScreen(msg, WARFRAMESNAIL_EXECPT_HELPER());
    } 

    HeapFree(GetProcessHeap(), 0, lpBits);
}

// -----------------------------------------------------------------------------
void WarframeSnail::ScreenCapture::CleanUpAfterLastCapture()
{
    LUL_PROFILER_TIMER_START();

    if (m_hScreenDC)
    {
        DeleteDC(m_hScreenDC);
        m_hScreenDC = nullptr;
    }
    if (m_hMemoryDC)
    {
        DeleteDC(m_hMemoryDC);
        m_hMemoryDC = nullptr;
    }
    if (m_hBitmap)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
    if (m_pBmi)
    {
        DeleteObject(m_pBmi);
        m_pBmi = nullptr;
    }
}

// -----------------------------------------------------------------------------
void WarframeSnail::ScreenCapture::ReverseBytes(LPBYTE& bytes, PBITMAPINFO pbi)
{
    LUL_PROFILER_TIMER_START();

    LPBYTE bytesCopy;
    PBITMAPINFOHEADER pbih;     // bitmap info-header  

    pbih = (PBITMAPINFOHEADER)pbi;
    bytesCopy = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!bytesCopy)
        throw std::bad_alloc();

    DWORD width = ((pbih->biWidth * 32 + 31) & ~31) / 8;

    for (DWORD i = 0; i < pbih->biHeight; ++i)
    {
        CopyMemory(
            &bytesCopy[i * width], 
            &bytes[(pbih->biHeight - i - 1) * width], 
            width);
    }

    LPBYTE tmp = bytes;
    bytes = bytesCopy;
    GlobalFree((HGLOBAL)tmp);
}
