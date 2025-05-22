#include "Engine.h"
#include <windows.h>
#include <sstream>
using std::stringstream;

Graphics* Engine::graphics = nullptr;
Window* Engine::window = nullptr;
Input* Engine::input = nullptr;
App* Engine::app = nullptr;
double Engine::frameTime = 0.0;
bool Engine::paused = false;
Timer Engine::timer;

Engine::Engine()
{
    window = new Window();
    graphics = new Graphics();
}

Engine::~Engine()
{
    delete app;
    delete graphics;
    delete input;
    delete window;
}

int Engine::Start(App* application)
{
    app = application;
    window->Create();
    input = new Input();
    graphics->Initialize(window);
    SetWindowLongPtr(window->Id(), GWLP_WNDPROC, (LONG_PTR)EngineProc);
    timeBeginPeriod(1);
    int exitCode = Loop();
    timeEndPeriod(1);
    return exitCode;
}

double Engine::FrameTime()
{
#ifdef _DEBUG
    static double totalTime = 0.0;
    static uint frameCount = 0;
#endif

    frameTime = timer.Reset();

#ifdef _DEBUG
    totalTime += frameTime;
    frameCount++;

    if (totalTime >= 1.0)
    {
        stringstream text;
        text << std::fixed;
        text.precision(3);

        text << window->Title().c_str() << "    "
            << "FPS: " << frameCount << "    "
            << "Frame Time: " << frameTime * 1000 << " (ms)";

        SetWindowText(window->Id(), text.str().c_str());

        frameCount = 0;
        totalTime -= 1.0;
    }
#endif

    return frameTime;
}

int Engine::Loop()
{
    timer.Start();
    MSG msg = { 0 };
    app->Init();

    do
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (input->KeyPress(VK_PAUSE))
            {
                if (paused)
                    Resume();
                else
                    Pause();
            }

            if (!paused)
            {
                frameTime = FrameTime();
                app->Update();
                app->Draw();
            }
            else
            {
                app->OnPause();
            }
        }

    } while (msg.message != WM_QUIT);

    app->Finalize();
    return int(msg.wParam);
}

LRESULT CALLBACK Engine::EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT)
        app->Display();

    return CallWindowProc(Input::InputProc, hWnd, msg, wParam, lParam);
}