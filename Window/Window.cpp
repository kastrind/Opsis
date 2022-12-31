#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h> // Or: remove this
#include <chrono>
#include <gdiplus.h>

#define SCREENWIDTH  800
#define SCREENHEIGHT 600

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
        static const TCHAR* HelloWorld = L"Hello, World!!!!";
        wchar_t s[256];
        swprintf_s(s, 256, L"%s %3.2f", HelloWorld, 1.0f / fElapsedTime);
        // or: const WCHAR* HelloWorld = L"Hello, World!";

        PAINTSTRUCT pntStruct = { 0 };
        HDC hdc = BeginPaint(hWnd, &pntStruct);
        //RECT rc = RECT{ 0, 0, 800, 600 };
        //HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 0));
        //FillRect(hdc, &rc, hbr);
        //TextOut(hdc, 5, 5, s, _tcslen(s));
        // or: TextOutW(hdc, 5, 5, HelloWorld, lstrlenW(HelloWorld));

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
        PostQuitMessage(0);
        break;
    }

    case WM_CLOSE:
    {
        //...
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
        TEXT("NAME OF WINDOW"), // Or: L"NAME OF WINDOW"
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        SCREENWIDTH,
        SCREENHEIGHT,
        //CW_USEDEFAULT,//WIDTH:[TODO]->Make custom width to fit window
        //CW_USEDEFAULT,//HEIGHT:[TODO]->Make custom width to fit window
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
        RedrawWindow(hCreateWin, NULL, NULL, RDW_INVALIDATE);
    };

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
};

void draw(HDC hdc) {
    Gdiplus::Graphics gf(hdc);
    Gdiplus::Pen pen(Gdiplus::Color(255, 128+rand() % 100, 128+rand() % 100, 0));
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 255, 0));
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color(255,0,0,0));
    //gf.FillRectangle(&brushBlack, 0, 0, 800, 600);

    Gdiplus::Bitmap bmp(SCREENWIDTH, SCREENHEIGHT);
    Gdiplus::Graphics* gf2 = Gdiplus::Graphics::FromImage(&bmp);

    //gf.Clear(RGB(0,0,0));
    gf2->FillRectangle(&brushBlack, 0, 0, 800, 600);
    gf2->DrawLine(&pen, 0 + rand() % 2, 0 + rand() % 20, 500, 500);
    // Create an array of Point objects that define the lines to draw.
    Gdiplus::Point point1(50 + rand() % 100, 50+rand()%100);
    Gdiplus::Point point2(100 + rand() % 100, 200 + rand() % 100);
    Gdiplus::Point point3(200 + rand() % 100, 250 + rand() % 100);

    Gdiplus::Point points[4] = { point1, point2, point3, point1 };
    Gdiplus::Point* pPoints = points;
    gf2->DrawLines(&pen, pPoints, 4);
    gf2->DrawLines(&pen, pPoints, 2);

    wchar_t s[256];
    swprintf_s(s, 256, L"FPS: %3.2f", 1.0f / fElapsedTime);

    Gdiplus::FontFamily   fontFamily(L"Arial");
    Gdiplus::Font         font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::PointF      pointF(30.0f, 10.0f);
    Gdiplus::SolidBrush   solidBrush(Gdiplus::Color(255, 0, 0, 255));

    gf2->DrawString(s, -1, &font, pointF, &brush);
    gf.DrawImage(&bmp, 0, 0, SCREENWIDTH, SCREENHEIGHT);

}
