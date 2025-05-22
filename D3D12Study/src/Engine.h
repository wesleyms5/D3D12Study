#pragma once
#ifndef DXUT_ENGINE_H
#define DXUT_ENGINE_H

#include "Graphics.h"
#include "Window.h"
#include "Input.h"
#include "Timer.h"
#include "App.h"


class Engine
{

private:

    static Timer timer;
    static bool  paused;

    double FrameTime();
    int Loop();

public:
    static Graphics* graphics;
    static Window* window;
    static Input* input;
    static App* app;
    static double frameTime;

    Engine();
    ~Engine();

    int Start(App* application);

    static void Pause();
    static void Resume();

    static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};


inline void Engine::Pause() {

    paused = true; timer.Stop();
}

inline void Engine::Resume() {
    paused = false; timer.Start();
}

#endif