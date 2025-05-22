#pragma once
#ifndef DXUT_INPUT_H
#define DXUT_INPUT_H

#include "Window.h"

class Input {

private:
    static bool keys[256];
    static bool ctrl[256];
    static string text;

    static int mouseX;
    static int mouseY;
    static short mouseWheel;

public:
    Input();
    ~Input();

    bool KeyDown(int vkcode);
    bool KeyUp(int vkcode);
    bool KeyPress(int vkcode);

    int MouseX();
    int MouseY();
    short MouseWheel();

    void Read();
    static const char* Text();

    static LRESULT CALLBACK Reader(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

inline bool Input::KeyDown(int vkcode)
{
    return keys[vkcode];
}

inline bool Input::KeyUp(int vkcode)
{
    return !keys[vkcode];
}

inline int Input::MouseX()
{
    return mouseX;
}

inline int Input::MouseY()
{
    return mouseY;
}

inline const char* Input::Text()
{
    return text.c_str();
}



#endif