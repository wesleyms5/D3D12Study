#pragma once

#ifndef  DXUT_WINDOW_H
#define DXUT_WINDOW_H

#define WIN32_LEAN_AND_MEAN

#include "Types.h"
#include <windows.h>
#include <windowsx.h>
#include <string>
using std::string;

enum WindowModes { FULLSCREEN, WINDOWED };


class Window {

private:
	HDC windowHdc;
	RECT windowRect;
	HWND windowId;
	int windowWidth;
	int windowHeight;
	HICON windowIcon;
	HCURSOR windowCursor;
	COLORREF windowColor;
	string windowTitle;
	DWORD windowStyle;
	int windowMode;
	int windowPosX;
	int windowPosY;
	int windowCenterX;
	int windowCenterY;

	static void (*inFocus)();
	static void (*lostFocus)();

public:
	Window();
	~Window();
	
	HWND Id();
	int Width();
	int Height();
	int Mode() const;
	int CenterX() const;
	int CenterY() const;
	string Title() const;
	COLORREF Color();
	float AspectRatio() const;

	void Icon(const uint icon);
	void Cursor(const uint cursor);
	void Title(const string title);
	void Size(int width, int height);
	void Mode(int mode);
	void Color(int r, int g, int b);

	void HideCursor(bool hide);
	void Close();
	void Clear();
	bool Create();

	void InFocus(void(*func)());
	void LostFocus(void(*func)());

	static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


};

inline HWND Window::Id()
{
	return windowId;
}
inline int Window::Width()
{
	return windowWidth;
}
inline int Window::Height()
{
	return windowHeight;
}
inline int Window::Mode() const
{
	return windowMode;
}
inline int Window::CenterX() const
{
	return windowCenterX;
}
inline int Window::CenterY() const
{
	return windowCenterY;
}
inline string Window::Title() const
{
	return windowTitle;
}
inline COLORREF Window::Color()
{
	return windowColor;
}
inline float Window::AspectRatio() const
{
	return windowWidth / (float)windowHeight;
}

inline void Window::Icon(const uint icon)
{
	windowIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(icon));
}

inline void Window::Cursor(const uint cursor)
{
	windowCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(cursor));
}

inline void Window::Title(const string title)
{
	windowTitle = title;
}

inline void Window::Color(int r, int g, int b)
{
	windowColor = RGB(r, g, b);
}

inline void Window::HideCursor(bool hide)
{
	ShowCursor(!hide);
}

inline void Window::Close()
{
	PostMessage(windowId, WM_DESTROY, 0, 0);
}

inline void Window::Clear()
{
	FillRect(windowHdc, &windowRect, CreateSolidBrush(Color()));
}

inline void Window::InFocus(void(*func)())
{
	inFocus = func;
}

inline void Window::LostFocus(void(*func)())
{
	lostFocus = func;
}

#endif
