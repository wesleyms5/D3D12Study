#pragma once

#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

#include <dxgi1_6.h>
#include <d3d12.h>
#include "Window.h"
#include "Types.h"
#include <D3DCompiler.h>

enum AllocationType { GPU, UPLOAD };

class Graphics
{
private:
    uint                         backBufferCount;
    uint                         antialiasing;
    uint                         quality;
    bool                         vSync;
    float                        bgColor[4];

    ID3D12Device4* device;
    IDXGIFactory6* factory;
    IDXGISwapChain1* swapChain;
    uint                         backBufferIndex;

    ID3D12CommandQueue* commandQueue;
    ID3D12GraphicsCommandList* commandList;
    ID3D12CommandAllocator* commandListAlloc;

    ID3D12Resource** renderTargets;
    ID3D12Resource* depthStencil;
    ID3D12DescriptorHeap* renderTargetHeap;
    ID3D12DescriptorHeap* depthStencilHeap;
    uint                         rtDescriptorSize;
    D3D12_VIEWPORT               viewport;
    D3D12_RECT                   scissorRect;

    ID3D12Fence* fence;
    ullong                       currentFence;

    void LogHardwareInfo();
    bool WaitCommandQueue();

public:
    Graphics();
    ~Graphics();

    void VSync(bool state);
    void Initialize(Window* window);
    void Clear(ID3D12PipelineState* pso);
    void Present();

    void ResetCommands();
    void SubmitCommands();

    void Allocate(uint sizeInBytes, ID3DBlob** resource);
    void Allocate(uint type, uint sizeInBytes, ID3D12Resource** resource);
    void Copy(const void* vertices, uint sizeInBytes, ID3DBlob* bufferCPU);
    void Copy(const void* vertices, uint sizeInBytes, ID3D12Resource* bufferUpload, ID3D12Resource* bufferGPU);

    ID3D12Device4* Device();
    ID3D12GraphicsCommandList* CommandList();
    uint Antialiasing();
    uint Quality();
};

inline void Graphics::VSync(bool state) { vSync = state; }
inline ID3D12Device4* Graphics::Device() { return device; }
inline ID3D12GraphicsCommandList* Graphics::CommandList() { return commandList; }
inline uint Graphics::Antialiasing() { return antialiasing; }
inline uint Graphics::Quality() { return quality; }

#endif