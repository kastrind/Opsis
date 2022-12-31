#include "ConsoleCanvas.h"

ConsoleCanvas::ConsoleCanvas(int w, int h) {
    width = w;
    height = h;
    console = ::GetConsoleWindow();
    RECT ConsoleRect;
    GetWindowRect(console, &ConsoleRect);
    MoveWindow(console, ConsoleRect.left, ConsoleRect.top, ScreenWidth(), ScreenHeight(), TRUE);
    context = ::GetDC(console);
    consoleRect = { 0, 0, ScreenWidth(), ScreenHeight() };
    solidBlackBrush = CreateSolidBrush(black);
    Sleep(100);
}


void ConsoleCanvas::DrawLine2(int x1, int y1, int x2, int y2, COLORREF color) {
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = abs(dx);
		dy1 = abs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			::SetPixel(context, x, y, color);
			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						y = y + 1;
					else
						y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				::SetPixel(context, x, y, color);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			::SetPixel(context, x, y, color);
			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						x = x + 1;
					else
						x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				::SetPixel(context, x, y, color);
			}
		}
}

void ConsoleCanvas::DrawLine(int xi, int yi, int xf, int yf, COLORREF color) {
    int x1 = xi < xf ? xi : xf;
    int x2 = x1 == xi ? xf : xi;
    int y1 = x1 == xi ? yi : yf;
    int y2 = x1 == xi ? yf : yi;
	//std::cout << "x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << " ";
    int x = x1;
    int y = y1;
    int xrange = x2 - x1;
    // d = inf.
    if (xrange == 0) {
        for (y; y <= y2; y++) {
            ::SetPixel(context, x, y, color);
        }
        return;
    }
    int yrange = y2 - y1;
    float d = (float)yrange / (float)xrange;
	float absd = abs(d);
	//std::cout << "!d=" << d;
    if (d == 0) {
        for (x; x <= x2; x++) {
            ::SetPixel(context, x, y, color);
        }
        return;
    }
    if (absd == 1) {
        for (x; x <= x2; x++) {
            ::SetPixel(context, x, y, color);
			y += d < 0 ? -1 : 1;
        }
        return;
    }
    if (absd < 1) {
        int stepsize = ceil(abs(float(xrange) / float(yrange + 1)));
        for (x; x <= x2; x++) {
            if (x - x1 > 0 && (x - x1) % stepsize == 0 && y < y2) {
				y += d < 0 ? -1 : 1;
            }
            ::SetPixel(context, x, y, color);
        }
        return;
    }
    // if d > 1 OR d < -1
	y1 = yi < yf ? yi : yf;
	y2 = y1 == yi ? yf : yi;
	x1 = y1 == yi ? xi : xf;
	x2 = y1 == yi ? xf : xi;
	x = x1;
	y = y1;
	xrange = x2 - x1;
	yrange = y2 - y1;
	float stepsize = abs(float(yrange) / float(xrange+1));
	int istepsize = floor(stepsize);
	int iremstepsize = ceil(1 / (stepsize - istepsize));
	//std::cout << " d=" << d << ", step size=" << stepsize << ", iremstepsize=" << iremstepsize << std::endl;
	//std::cout << "x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << std::endl;
	int i = 0;
    for (y; y <= y2; y++) {
		::SetPixel(context, x, y, color);
		i++;
		if (i >= istepsize) {
			x += d < 0 ? -1 : 1;
			i = 0;
		}
		if (y - y1 > 0 && (y-y1)%(iremstepsize+1)==0) {
			x += d < 0 ? 1 : -1;
		}
    }
}

void ConsoleCanvas::Clip(int &x, int &y)
{
    if (x < 0) x = 0;
    if (x >= width) x = width;
    if (y < 0) y = 0;
    if (y >= height) y = height;
}

void ConsoleCanvas::Fill(int xi, int yi, int xf, int yf, COLORREF color)
{
    RECT consoleRect = {xi, yi, xf, yf};
    ::FillRect(context, &consoleRect, CreateSolidBrush(color));
}

void ConsoleCanvas::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color)
{
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
}

void ConsoleCanvas::Clear()
{
    ::FillRect(context, &consoleRect, solidBlackBrush);
}

int ConsoleCanvas::ScreenWidth() {
    return width;
}

int ConsoleCanvas::ScreenHeight() {
    return height;
}
