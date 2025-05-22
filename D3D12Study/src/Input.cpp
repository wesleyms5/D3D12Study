#include "Input.h"



bool Input::keys[256] = { 0 };              
bool Input::ctrl[256] = { 0 };              
string Input::text;                      

int Input::mouseX = 0;                   
int Input::mouseY = 0;                    
short Input::mouseWheel = 0;                



Input::Input()
{
    
    SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::InputProc);
}



Input::~Input()
{
  
    SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Window::WinProc);
}



bool Input::KeyPress(int vkcode)
{
    if (ctrl[vkcode])
    {
        if (KeyDown(vkcode))
        {
            ctrl[vkcode] = false;
            return true;
        }
    }
    else if (KeyUp(vkcode))
    {
        ctrl[vkcode] = true;
    }

    return false;
}



short Input::MouseWheel()
{
    short val = mouseWheel;
    mouseWheel = 0;
    return val;
}



void Input::Read()
{
    
    text.clear();

   
    SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::Reader);
}



LRESULT CALLBACK Input::Reader(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  
    if (msg == WM_CHAR)
    {
        switch (wParam)
        {
           
        case 0x08:
            if (!text.empty())
                text.erase(text.size() - 1);
            break;

         
        case 0x09:
        case 0x0D:
        
            SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::InputProc);
            break;

            
        default:
            text += char(wParam);
            break;
        }

        return 0;
    }

    return CallWindowProc(Input::InputProc, hWnd, msg, wParam, lParam);
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Input::InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        
    case WM_KEYDOWN:
        keys[wParam] = true;
        return 0;

        
    case WM_KEYUP:
        keys[wParam] = false;
        return 0;

        
    case WM_MOUSEMOVE:
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
        return 0;

       
    case WM_MOUSEWHEEL:
        mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
        return 0;

        
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        keys[VK_LBUTTON] = true;
        return 0;

      
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        keys[VK_MBUTTON] = true;
        return 0;

        
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        keys[VK_RBUTTON] = true;
        return 0;

        
    case WM_LBUTTONUP:
        keys[VK_LBUTTON] = false;
        return 0;

    case WM_MBUTTONUP:
        keys[VK_MBUTTON] = false;
        return 0;

       
    case WM_RBUTTONUP:
        keys[VK_RBUTTON] = false;
        return 0;
    }

    return CallWindowProc(Window::WinProc, hWnd, msg, wParam, lParam);
}


