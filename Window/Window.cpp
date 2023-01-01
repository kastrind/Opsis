#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h> // Or: remove this
#include <chrono>
#include <gdiplus.h>
#include "Engine3D.h"

#define SCREENWIDTH  240
#define SCREENHEIGHT 240
#define WINDOWNAME "Opsis"

std::vector<triangle> trianglesToProject;

class myEngine3D : public Engine3D {
public:
    myEngine3D(HWND hWnd, int width, int height)
        : Engine3D(hWnd, width, height) {
    }
    bool OnUserCreate() override
    {
        meshCube.tris = {
            // SOUTH
            {0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f},

            // EAST
            {1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f},

            // NORTH
            {1.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f},

            // WEST
            {0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 0.0f,       0.0f, 0.0f, 0.0f},

            // TOP
            {0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 0.0f},

            // BOTTOM
            {1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f},
        };

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        fTheta += 1.0f * fElapsedTime;

        std::vector<triangle> newTrianglesToProject;
        // Project Triangles
        for (auto tri : meshCube.tris)
        {
            triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

            // Rotate Z
            mat4x4 matRotZ = getRotMatrixZ(fTheta);
            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

            // Rotate X
            mat4x4 matRotX = getRotMatrixX(fTheta);
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            // Translate further along Z
            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= 0.5f * (float)width;
            triProjected.p[0].y *= 0.5f * (float)height;
            triProjected.p[1].x *= 0.5f * (float)width;
            triProjected.p[1].y *= 0.5f * (float)height;
            triProjected.p[2].x *= 0.5f * (float)width;
            triProjected.p[2].y *= 0.5f * (float)height;

            newTrianglesToProject.push_back(triProjected);

        }
        trianglesToProject = newTrianglesToProject;

        return true;
    }

private:

    mesh meshCube;

    float fTheta = 0;

};

myEngine3D* myEng3D;

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
        myEng3D->bAtomActive = false;
        PostQuitMessage(0);
        break;
    }

    case WM_CLOSE:
    {
        myEng3D->bAtomActive = false;
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

    myEng3D = new myEngine3D(hCreateWin, SCREENWIDTH, SCREENHEIGHT);
    std::thread t = myEng3D->Start();

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
    Gdiplus::SolidBrush brushGreen(Gdiplus::Color(255, 0, 255, 0));
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color(255,0,0,0));

    Gdiplus::Bitmap bmp(SCREENWIDTH, SCREENHEIGHT);
    Gdiplus::Graphics* gf2 = Gdiplus::Graphics::FromImage(&bmp);

    gf2->FillRectangle(&brushBlack, 0, 0, SCREENWIDTH, SCREENHEIGHT);

    wchar_t s[256];

    for (auto tri : trianglesToProject)
    {
        Gdiplus::PointF point1(tri.p[0].x, tri.p[0].y);
        Gdiplus::PointF point2(tri.p[1].x, tri.p[1].y);
        Gdiplus::PointF point3(tri.p[2].x, tri.p[2].y);
        Gdiplus::PointF points[4] = {point1, point2, point3, point1};
        
        gf2->DrawLines(&pen, points, 4);
    }

    swprintf_s(s, 256, L"FPS: %3.2f", 1.0f / fElapsedTime);

    Gdiplus::FontFamily fontFamily(L"Arial");
    Gdiplus::Font font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::PointF pointF(30.0f, 10.0f);
    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, 0, 0, 255));

    gf2->DrawString(s, -1, &font, pointF, &brushGreen);
    gf.DrawImage(&bmp, 0, 0, SCREENWIDTH, SCREENHEIGHT);

    delete gf2;

}
