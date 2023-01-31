#include <Windows.h>
#include <Windowsx.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h> // Or: remove this
#include <chrono>
#include <gdiplus.h>
#include "OpsisEngine3D.h"

#define SCREENWIDTH  800
#define SCREENHEIGHT 600
#define WINDOWNAME "Opsis"

OpsisEngine3D* opsisEng3D;

static float fElapsedTime;

static int nMouseLastX = 0;
static int nMouseLastY = 0;
static int nMouseX = SCREENWIDTH / 2;
static int nMouseY = SCREENHEIGHT / 2;

static TCHAR WindowClass[] = TEXT("Window");
// or: static WCHAR WindowClass[] = L"Window";

void draw(HDC hdc);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    wchar_t msg[32];
    nMouseLastX = nMouseX;
    nMouseLastY = nMouseY;

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

    case WM_KEYDOWN:
        //swprintf_s(msg, L"WM_KEYDOWN: 0x%c\n", wParam);
        //OutputDebugString(msg);
        if ((char)wParam == 'W') {
            opsisEng3D->bWKeyHeld = true;
        }
        else if ((char)wParam == 'S')
        {
            opsisEng3D->bSKeyHeld = true;
        }
        else if ((char)wParam == 'A')
        {
            opsisEng3D->bAKeyHeld = true;
        }
        else if ((char)wParam == 'D')
        {
            opsisEng3D->bDKeyHeld = true;
        }
        break;

    case WM_KEYUP:
        //swprintf_s(msg, L"WM_KEYUP: 0x%c\n", wParam);
        //OutputDebugString(msg);
        if ((char)wParam == 'W') {
            opsisEng3D->bWKeyHeld = false;
        }
        else if ((char)wParam == 'S')
        {
            opsisEng3D->bSKeyHeld = false;
        }
        else if ((char)wParam == 'A')
        {
            opsisEng3D->bAKeyHeld = false;
        }
        else if ((char)wParam == 'D')
        {
            opsisEng3D->bDKeyHeld = false;
        }
        else if (wParam == VK_ESCAPE)
        {
            opsisEng3D->bAtomActive = false;
            PostQuitMessage(0);
        }
        break;

    case WM_MOUSEMOVE:
        nMouseX = GET_X_LPARAM(lParam);
        nMouseY = GET_Y_LPARAM(lParam);

        if (nMouseX > nMouseLastX)
        {
            opsisEng3D->bMouseRight = true;
        }
        else if (nMouseX < nMouseLastX)
        {
            opsisEng3D->bMouseLeft = true;
        }
        if (nMouseY > nMouseLastY)
        {
            opsisEng3D->bMouseDown = true;
        }
        else if (nMouseY < nMouseLastY)
        {
            opsisEng3D->bMouseUp = true;
        }
        break;

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
    if (opsisEng3D)
    {
        if (nMouseX == nMouseLastX)
        {
            opsisEng3D->bMouseRight = false;
            opsisEng3D->bMouseLeft = false;
        }
        if (nMouseX == 0) {
            opsisEng3D->bMouseLeft = true;
            SetCursorPos(SCREENWIDTH/2, nMouseY);

        }else if (nMouseX > SCREENWIDTH - 50) {
            opsisEng3D->bMouseRight = true;
            SetCursorPos(SCREENWIDTH / 2, nMouseY);
        }
        if (nMouseY == nMouseLastY)
        {
            opsisEng3D->bMouseUp = false;
            opsisEng3D->bMouseDown = false;
        }
        if (nMouseY == 0) {
            opsisEng3D->bMouseUp = true;
            SetCursorPos(nMouseX, SCREENHEIGHT / 2);

        }
        else if (nMouseY > SCREENHEIGHT - 50) {
            opsisEng3D->bMouseDown = true;
            SetCursorPos(nMouseX, SCREENHEIGHT/2);
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

    // Get the window client area.
    RECT rc;
    GetClientRect(hCreateWin, &rc);

    // Convert the client area to screen coordinates.
    POINT pt = { rc.left, rc.top };
    POINT pt2 = { rc.right, rc.bottom };
    ClientToScreen(hCreateWin, &pt);
    ClientToScreen(hCreateWin, &pt2);
    SetRect(&rc, pt.x, pt.y, pt2.x, pt2.y);

    // Confine the cursor.
    ClipCursor(&rc);
    SetCursorPos(SCREENWIDTH / 2, SCREENHEIGHT / 2);
    ShowCursor(false);

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
    Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0));
    Gdiplus::SolidBrush brushGreen(Gdiplus::Color(192, 0, 192, 0));
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color(255,0,0,0));

    Gdiplus::Bitmap bmp(SCREENWIDTH, SCREENHEIGHT);
    Gdiplus::Graphics* gf2 = Gdiplus::Graphics::FromImage(&bmp);

    gf2->FillRectangle(&brushBlack, 0, 0, SCREENWIDTH, SCREENHEIGHT);

    wchar_t s[256];
    unsigned char r = 0; unsigned char g = 0; unsigned char b = 0;

    Gdiplus::Color bmpColor;

    if (opsisEng3D != nullptr) {
        opsisEng3D->bLockRaster = true;

        //for (int i = 0; i < 1000; i++) {

            int modelWidth  = opsisEng3D->mdl.modelTexture->GetWidth();
            int modelHeight = opsisEng3D->mdl.modelTexture->GetHeight();


            opsisEng3D->mtx.lock();
            std::vector<triangle> tris = opsisEng3D->finalTrianglesToRaster;
            opsisEng3D->mtx.unlock();

            for (triangle& tri : tris)
            {
                Gdiplus::PointF point1(tri.p[0].x, tri.p[0].y);
                Gdiplus::PointF point2(tri.p[1].x, tri.p[1].y);
                Gdiplus::PointF point3(tri.p[2].x, tri.p[2].y);
                Gdiplus::PointF points[4] = { point1, point2, point3, point1 };

                r = tri.R; g = tri.G; b = tri.B;

                Gdiplus::SolidBrush brushShaded(Gdiplus::Color(255, r * tri.luminance, g * tri.luminance, b * tri.luminance));
                //gf2->DrawLines(&pen, points, 4);
                //gf2->FillPolygon(&brushShaded, points, 4);

                if (opsisEng3D->mdl.modelTexture == nullptr) continue;
                
                std::vector<texturePoint> tps = tri.texturePoints;
                for (texturePoint& tp : tps)
                {
                    // texture coordinates
                    int u = (tp.t.u / tp.t.w) * modelWidth;
                    int v = (tp.t.v / tp.t.w) * modelHeight;

                    // screen coordinates
                    int x = tp.p.u;
                    int y = tp.p.v;

                    // get texture pixel value
                    opsisEng3D->mdl.modelTexture->GetPixel(u, v, &bmpColor);

                    byte red = bmpColor.GetRed() * tri.luminance;
                    byte green = bmpColor.GetGreen() * tri.luminance;
                    byte blue = bmpColor.GetBlue() * tri.luminance;
                    bmpColor.SetValue(Gdiplus::Color::MakeARGB(255, red, green, blue));
                    Gdiplus::SolidBrush brushCustom(bmpColor);

                    gf2->FillRectangle(&brushCustom, x, y, 1, 1);
                }

            }
            opsisEng3D->mtx.lock();
            opsisEng3D->trianglesToRaster.clear();
            opsisEng3D->mtx.unlock();

        //}
            opsisEng3D->bLockRaster = false;
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
