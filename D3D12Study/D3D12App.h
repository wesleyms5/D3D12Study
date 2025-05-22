#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include <comdef.h>

class D3D12App
{

public:
	D3D12App(HINSTANCE hInstance, int nCmdShow);
	~D3D12App();
	int Run();

protected:
    virtual bool Init();
    virtual void OnResize();
    virtual void Update();
    virtual void Draw();
    virtual void CreateSwapChain();

    void InitWindow();
    void InitD3D();

protected:
    HWND mhMainWnd = nullptr;
    UINT mWidth = 1280;
    UINT mHeight = 720;
    std::wstring mWindowCaption = L"D3D12 App";

    HINSTANCE mhAppInst = nullptr;

    Microsoft::WRL::ComPtr<IDXGIFactory6> mdxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;

    static const int SwapChainBufferCount = 2;
    int mCurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    UINT mRtvDescriptorSize = 0;

    void FlushCommandQueue();
    void LogAdapters();


};