#pragma once
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <windows.h>

class ConsoleCanvas
{

public:

	ConsoleCanvas(int w, int h);

	void DrawLine(int x1, int y1, int x2, int y2, COLORREF color);

	void DrawLine2(int xi, int yi, int xf, int yf, COLORREF color);

	void Clip(int &x, int &y);

	void Fill(int xi, int yi, int xf, int yf, COLORREF color);

	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color);

	void Clear();

	int ScreenWidth();

	int ScreenHeight();

	const COLORREF red = RGB(255, 0, 0);
	const COLORREF yellow = RGB(255, 255, 0);
	const COLORREF orange = RGB(255, 128, 0);
	const COLORREF blue = RGB(0, 0, 255);
	const COLORREF green = RGB(0, 255, 0);
	const COLORREF black = RGB(0, 0, 0);

private:

	HWND console;

	HDC context;

	int width;

	int height;

	RECT consoleRect;

	HBRUSH solidBlackBrush;

	class Graphics* pGraphics;

};

