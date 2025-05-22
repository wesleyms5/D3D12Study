#include "Window.h"



void (*Window::inFocus)() = nullptr;                       
void (*Window::lostFocus)() = nullptr;                     



Window::Window()
{
    windowId = 0;                                      
    windowWidth = GetSystemMetrics(SM_CXSCREEN);         
    windowHeight = GetSystemMetrics(SM_CYSCREEN);          
    windowIcon = LoadIcon(NULL, IDI_APPLICATION);       
    windowCursor = LoadCursor(NULL, IDC_ARROW);            
    windowColor = RGB(0, 0, 0);                             
    windowTitle = string("Windows App");              
    windowStyle = WS_POPUP | WS_VISIBLE;                 
    windowMode = FULLSCREEN;                            
    windowPosX = 0;                                     
    windowPosY = 0;                                     
    windowCenterX = windowWidth / 2;                         
    windowCenterY = windowHeight / 2;                         
    windowHdc = { 0 };                                  
    windowRect = { 0, 0, 0, 0 };                        
}



Window::~Window()
{
 
    if (windowHdc) ReleaseDC(windowId, windowHdc);
}



void Window::Mode(int mode)
{
    windowMode = mode;

    if (mode == WINDOWED)
    {
      
        windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;
    }
    else
    {
       
        windowStyle = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
    }
}



void Window::Size(int width, int height)
{
  
    windowWidth = width;
    windowHeight = height;

   
    windowCenterX = windowWidth / 2;
    windowCenterY = windowHeight / 2;

    
    windowPosX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (windowWidth / 2);
    windowPosY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (windowHeight / 2);
}



bool Window::Create()
{

    HINSTANCE appId = GetModuleHandle(NULL);

   
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = Window::WinProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = appId;
    wndClass.hIcon = windowIcon;
    wndClass.hCursor = windowCursor;
    wndClass.hbrBackground = (HBRUSH)CreateSolidBrush(windowColor);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = "AppWindow";
    wndClass.hIconSm = windowIcon;

 
    if (!RegisterClassEx(&wndClass))
        return false;

   
    windowId = CreateWindowEx(
        NULL,                            
        "AppWindow",                   
        windowTitle.c_str(),              
        windowStyle,                      
        windowPosX, windowPosY,            
        windowWidth, windowHeight,         
        NULL,                              
        NULL,                              
        appId,                             
        NULL);                           

    

    if (windowMode == WINDOWED)
    {
      
        RECT rect = { 0, 0, windowWidth, windowHeight };

        
        AdjustWindowRectEx(&rect,
            GetWindowStyle(windowId),
            GetMenu(windowId) != NULL,
            GetWindowExStyle(windowId));

      
        windowPosX = (GetSystemMetrics(SM_CXSCREEN) / 2) - ((rect.right - rect.left) / 2);
        windowPosY = (GetSystemMetrics(SM_CYSCREEN) / 2) - ((rect.bottom - rect.top) / 2);

        
        MoveWindow(
            windowId,                     
            windowPosX,                   
            windowPosY,                    
            rect.right - rect.left,        
            rect.bottom - rect.top,       
            TRUE);                         
    }


    windowHdc = GetDC(windowId);

  
    GetClientRect(windowId, &windowRect);

  
    return (windowId ? true : false);
}


LRESULT CALLBACK Window::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
     
    case WM_KILLFOCUS:
        if (lostFocus)
            lostFocus();
        return 0;

    
    case WM_SETFOCUS:
        if (inFocus)
            inFocus();
        return 0;

  
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

