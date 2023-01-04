#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h> // Or: remove this
#include <chrono>
#include <gdiplus.h>
#include "OpsisEngine3D.h"

#define SCREENWIDTH  600
#define SCREENHEIGHT 600
#define WINDOWNAME "Opsis"

OpsisEngine3D* opsisEng3D;

static float fElapsedTime;

static TCHAR WindowClass[] = TEXT("Window");
// or: static WCHAR WindowClass[] = L"Window";

void draw(HDC hdc);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT pntStruct = { 0 };
        HDC hdc = BeginPaint(hWnd, &pntStruct);

        draw(hdc);

        EndPaint(hWnd, &pntStruct);
        break;
    }

    case WM_SIZE:
    {
        //...
        break;
    }

    case WM_MOVE:
    {
        
        break;
    }

    case WM_DESTROY:
    {
        opsisEng3D->bAtomActive = false;
        PostQuitMessage(0);
        break;
    }

    case WM_CLOSE:
    {
        opsisEng3D->bAtomActive = false;
        PostQuitMessage(0);
        break;
    }

    case WM_ACTIVATEAPP:
    {
        if (WM_ACTIVATEAPP)
        {
            OutputDebugString(TEXT("WM_ACTIVEAPP->TRUE"));
            // or: OutputDebugStringW(L"WM_ACTIVEAPP->TRUE");
        }
        else
        {
            OutputDebugString(TEXT("WM_ACTIVEAPP->FALSE"));
            // or: OutputDebugStringW(L"WM_ACTIVEAPP->FALSE");
        }

        break;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASSEX wclass = { 0 }; // Or: WNDCLASSEXW
    wclass.cbSize = sizeof(wclass);
    wclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wclass.lpfnWndProc = &WindowProc;
    wclass.cbClsExtra = 0;
    wclass.cbWndExtra = 0;
    wclass.hInstance = hInstance;
    wclass.hIcon = NULL; // TODO: CREATE ICON
    wclass.hCursor = NULL;
    wclass.hbrBackground = CreateSolidBrush(RGB(0,0,0)); //(HBRUSH)(COLOR_WINDOW+1);
    wclass.lpszMenuName = NULL;
    wclass.lpszClassName = WindowClass;
    wclass.hIconSm = NULL;

    if (!RegisterClassEx(&wclass)) // Or: RegisterClassExW()
    {
        // error! Use GetLastError() to find out why...
        return 0;
    }

    HWND hCreateWin = CreateWindow( // Or: CreateWindowW()
        WindowClass,
        TEXT(WINDOWNAME), // Or: L"NAME OF WINDOW"
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        SCREENWIDTH,
        SCREENHEIGHT,
        0,
        0,
        hInstance,
        0
    );

    if (!hCreateWin)
    {
        // error! Use GetLastError() to find out why...
        return 0;
    }

    ShowWindow(hCreateWin, nCmdShow);
    UpdateWindow(hCreateWin);

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    opsisEng3D = new OpsisEngine3D(hCreateWin, SCREENWIDTH, SCREENHEIGHT);
    std::thread t = opsisEng3D->Start();

    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        // Handle Timing
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        fElapsedTime = elapsedTime.count();

        TranslateMessage(&message);
        DispatchMessage(&message); 
    };
    
    t.join();
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
};

void draw(HDC hdc) {
    Gdiplus::Graphics gf(hdc);
    Gdiplus::Pen pen(Gdiplus::Color(0, 0, 0));
    Gdiplus::SolidBrush brushGreen(Gdiplus::Color(192, 0, 192, 0));
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color(255,0,0,0));

    Gdiplus::Bitmap bmp(SCREENWIDTH, SCREENHEIGHT);
    Gdiplus::Graphics* gf2 = Gdiplus::Graphics::FromImage(&bmp);

    gf2->FillRectangle(&brushBlack, 0, 0, SCREENWIDTH, SCREENHEIGHT);

    wchar_t s[256];

    if (opsisEng3D != nullptr) {
        //for (int i = 0; i < 1000; i++) {
            for (auto tri : opsisEng3D->trianglesToProject)
            {
                Gdiplus::PointF point1(tri.p[0].x, tri.p[0].y);
                Gdiplus::PointF point2(tri.p[1].x, tri.p[1].y);
                Gdiplus::PointF point3(tri.p[2].x, tri.p[2].y);
                Gdiplus::PointF points[4] = { point1, point2, point3, point1 };

                Gdiplus::SolidBrush brushShaded(Gdiplus::Color(255, 255 * tri.luminance, 255 * tri.luminance, 255 * tri.luminance));

                gf2->DrawLines(&pen, points, 4);
                gf2->FillPolygon(&brushShaded, points, 4);
            }
        //}

    swprintf_s(s, 256, L"EngineLoops: %3.2f / FPS: %3.2f", 1.0f / opsisEng3D->fElapsedTime, 1.0f / fElapsedTime);
    }

    Gdiplus::FontFamily fontFamily(L"Arial");
    Gdiplus::Font font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::PointF pointF(30.0f, 10.0f);
    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, 0, 0, 255));

    gf2->DrawString(s, -1, &font, pointF, &brushGreen);
    gf.DrawImage(&bmp, 0, 0, SCREENWIDTH, SCREENHEIGHT);

    delete gf2;

}
