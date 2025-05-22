#pragma once
#ifndef DXUT_APP_H  
#define DXUT_APP_H


#include "Graphics.h"
#include "Window.h"
#include "Input.h"

class App {

protected:
    static Graphics*& graphics;
    static Window*& window;
    static Input*& input;
    static double& frameTime;

public:
    App();
    virtual ~App();

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Finalize() = 0;


    virtual void Draw() {}
    virtual void Display() {}
    virtual void OnPause() { Sleep(10); }
};




#endif