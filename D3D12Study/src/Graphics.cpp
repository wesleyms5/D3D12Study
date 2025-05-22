#include "Graphics.h"
#include "Error.h"
#include <sstream>
using std::wstringstream;

Graphics::Graphics()
{
    backBufferCount = 2;
    backBufferIndex = 0;
    antialiasing = 1;
    quality = 0;
    vSync = false;

    bgColor[0] = 0.0f;
    bgColor[1] = 0.0f;
    bgColor[2] = 0.0f;
    bgColor[3] = 0.0f;

    factory = nullptr;
    device = nullptr;
    swapChain = nullptr;
    commandQueue = nullptr;
    commandList = nullptr;
    commandListAlloc = nullptr;

    renderTargets = new ID3D12Resource * [backBufferCount] {nullptr};
    depthStencil = nullptr;
    renderTargetHeap = nullptr;
    depthStencilHeap = nullptr;
    rtDescriptorSize = 0;
    ZeroMemory(&viewport, sizeof(viewport));
    ZeroMemory(&scissorRect, sizeof(scissorRect));

    fence = nullptr;
    currentFence = 0;
}

Graphics::~Graphics()
{
    WaitCommandQueue();

    if (depthStencil)
        depthStencil->Release();

    if (renderTargets)
    {
        for (uint i = 0; i < backBufferCount; ++i)
        {
            if (renderTargets[i])
                renderTargets[i]->Release();
        }
        delete[] renderTargets;
    }

    if (fence)
        fence->Release();

    if (depthStencilHeap)
        depthStencilHeap->Release();

    if (renderTargetHeap)
        renderTargetHeap->Release();

    if (swapChain)
    {
        swapChain->SetFullscreenState(false, NULL);
        swapChain->Release();
    }

    if (commandList)
        commandList->Release();

    if (commandListAlloc)
        commandListAlloc->Release();

    if (commandQueue)
        commandQueue->Release();

    if (device)
        device->Release();

    if (factory)
        factory->Release();
}

void Graphics::LogHardwareInfo()
{
    const uint BytesinMegaByte = 1048576U;

    IDXGIAdapter* adapter = nullptr;
    if (factory->EnumAdapters(0, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        wstringstream text;
        text << L"---> Placa de vídeo: " << desc.Description << L"\n";
        OutputDebugStringW(text.str().c_str());
    }

    IDXGIAdapter4* adapter4 = nullptr;
    if (SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter4))))
    {
        DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
        adapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);

        wstringstream text;
        text << L"---> Memória de vídeo (livre): " << memInfo.Budget / BytesinMegaByte << L"MB\n";
        text << L"---> Memória de vídeo (usada): " << memInfo.CurrentUsage / BytesinMegaByte << L"MB\n";
        OutputDebugStringW(text.str().c_str());

        adapter4->Release();
    }

    D3D_FEATURE_LEVEL featureLevels[9] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;
    featureLevelsInfo.NumFeatureLevels = 9;
    featureLevelsInfo.pFeatureLevelsRequested = featureLevels;

    device->CheckFeatureSupport(
        D3D12_FEATURE_FEATURE_LEVELS,
        &featureLevelsInfo,
        sizeof(featureLevelsInfo));

    {
        wstringstream text;
        text << L"---> Feature Level: ";
        switch (featureLevelsInfo.MaxSupportedFeatureLevel)
        {
        case D3D_FEATURE_LEVEL_12_1: text << L"12_1\n"; break;
        case D3D_FEATURE_LEVEL_12_0: text << L"12_0\n"; break;
        case D3D_FEATURE_LEVEL_11_1: text << L"11_1\n"; break;
        case D3D_FEATURE_LEVEL_11_0: text << L"11_0\n"; break;
        case D3D_FEATURE_LEVEL_10_1: text << L"10_1\n"; break;
        case D3D_FEATURE_LEVEL_10_0: text << L"10_0\n"; break;
        case D3D_FEATURE_LEVEL_9_3:  text << L"9_3\n";  break;
        case D3D_FEATURE_LEVEL_9_2:  text << L"9_2\n";  break;
        case D3D_FEATURE_LEVEL_9_1:  text << L"9_1\n";  break;
        }
        OutputDebugStringW(text.str().c_str());
    }

    IDXGIOutput* output = nullptr;
    if (adapter->EnumOutputs(0, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        wstringstream text;
        text << L"---> Monitor: " << desc.DeviceName << L"\n";
        OutputDebugStringW(text.str().c_str());
    }

    uint dpi = GetDpiForSystem();
    uint screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
    uint screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

    DEVMODE devMode = { 0 };
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    uint refresh = devMode.dmDisplayFrequency;

    wstringstream text;
    text << L"---> Resolução: " << screenWidth << L"x" << screenHeight << L" " << refresh << L" Hz\n";
    OutputDebugStringW(text.str().c_str());

    if (adapter) adapter->Release();
    if (output) output->Release();
}

void Graphics::Initialize(Window* window)
{
    uint factoryFlags = 0;

#ifdef _DEBUG
    factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    ID3D12Debug* debugController;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
#endif

    ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)));

    if FAILED(D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)))
    {
        IDXGIAdapter* warp;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));

        ThrowIfFailed(D3D12CreateDevice(
            warp,
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device)));

        warp->Release();
        OutputDebugString("---> Usando Adaptador WARP: não há suporte ao D3D12\n");
    }

#ifdef _DEBUG
    LogHardwareInfo();
#endif 

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandListAlloc)));

    ThrowIfFailed(device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandListAlloc,
        nullptr,
        IID_PPV_ARGS(&commandList)));

    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = window->Width();
    swapChainDesc.Height = window->Height();
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = antialiasing;
    swapChainDesc.SampleDesc.Quality = quality;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = backBufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        commandQueue,
        window->Id(),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain));

    D3D12_DESCRIPTOR_HEAP_DESC renderTargetHeapDesc = {};
    renderTargetHeapDesc.NumDescriptors = backBufferCount;
    renderTargetHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    renderTargetHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&renderTargetHeapDesc, IID_PPV_ARGS(&renderTargetHeap)));

    D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
    rtDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (uint i = 0; i < backBufferCount; ++i)
    {
        swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        device->CreateRenderTargetView(renderTargets[i], nullptr, rtHandle);
        rtHandle.ptr += rtDescriptorSize;
    }

    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = window->Width();
    depthStencilDesc.Height = window->Height();
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = antialiasing;
    depthStencilDesc.SampleDesc.Quality = quality;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES dsHeapProperties = {};
    dsHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    dsHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    dsHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    dsHeapProperties.CreationNodeMask = 1;
    dsHeapProperties.VisibleNodeMask = 1;

    D3D12_CLEAR_VALUE optmizedClear = {};
    optmizedClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    optmizedClear.DepthStencil.Depth = 1.0f;
    optmizedClear.DepthStencil.Stencil = 0;

    ThrowIfFailed(device->CreateCommittedResource(
        &dsHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optmizedClear,
        IID_PPV_ARGS(&depthStencil)));

    D3D12_DESCRIPTOR_HEAP_DESC depthstencilHeapDesc = {};
    depthstencilHeapDesc.NumDescriptors = 1;
    depthstencilHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    depthstencilHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&depthstencilHeapDesc, IID_PPV_ARGS(&depthStencilHeap)));

    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateDepthStencilView(depthStencil, nullptr, dsHandle);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = depthStencil;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    SubmitCommands();

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(window->Width());
    viewport.Height = static_cast<float>(window->Height());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    scissorRect = { 0, 0, window->Width(), window->Height() };

    COLORREF color = window->Color();
    bgColor[0] = GetRValue(color) / 255.0f;
    bgColor[1] = GetGValue(color) / 255.0f;
    bgColor[2] = GetBValue(color) / 255.0f;
    bgColor[3] = 1.0f;
}

void Graphics::Clear(ID3D12PipelineState* pso)
{
    commandListAlloc->Reset();
    commandList->Reset(commandListAlloc, pso);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets[backBufferIndex];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
    rtHandle.ptr += SIZE_T(backBufferIndex) * SIZE_T(rtDescriptorSize);
    commandList->ClearRenderTargetView(rtHandle, bgColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(1, &rtHandle, true, &dsHandle);
}

bool Graphics::WaitCommandQueue()
{
    currentFence++;

    if (FAILED(commandQueue->Signal(fence, currentFence)))
        return false;

    if (fence->GetCompletedValue() < currentFence)
    {
        HANDLE eventHandle = CreateEventEx(NULL, NULL, NULL, EVENT_ALL_ACCESS);

        if (eventHandle)
        {
            if (FAILED(fence->SetEventOnCompletion(currentFence, eventHandle)))
                return false;

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    return true;
}

void Graphics::ResetCommands()
{
    commandList->Reset(commandListAlloc, nullptr);
}

void Graphics::SubmitCommands()
{
    commandList->Close();
    ID3D12CommandList* cmdsLists[] = { commandList };
    commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    WaitCommandQueue();
}

void Graphics::Allocate(uint sizeInBytes, ID3DBlob** resource)
{
    D3DCreateBlob(sizeInBytes, resource);
}

void Graphics::Allocate(uint type, uint sizeInBytes, ID3D12Resource** resource)
{
    D3D12_HEAP_PROPERTIES bufferProp = {};
    bufferProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    bufferProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    bufferProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    bufferProp.CreationNodeMask = 1;
    bufferProp.VisibleNodeMask = 1;

    if (type == UPLOAD)
        bufferProp.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Alignment = 0;
    bufferDesc.Width = sizeInBytes;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.SampleDesc.Quality = 0;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON;

    if (type == UPLOAD)
        initState = D3D12_RESOURCE_STATE_GENERIC_READ;

    ThrowIfFailed(device->CreateCommittedResource(
        &bufferProp,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        initState,
        nullptr,
        IID_PPV_ARGS(resource)));
}

void Graphics::Copy(const void* vertices, uint sizeInBytes, ID3DBlob* bufferCPU)
{
    CopyMemory(bufferCPU->GetBufferPointer(), vertices, sizeInBytes);
}

void Graphics::Copy(const void* vertices, uint sizeInBytes, ID3D12Resource* bufferUpload, ID3D12Resource* bufferGPU)
{
    D3D12_SUBRESOURCE_DATA vertexSubResourceData = {};
    vertexSubResourceData.pData = vertices;
    vertexSubResourceData.RowPitch = sizeInBytes;
    vertexSubResourceData.SlicePitch = sizeInBytes;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts;
    uint numRows;
    ullong rowSizesInBytes;
    ullong requiredSize = 0;

    D3D12_RESOURCE_DESC bufferGPUDesc = bufferGPU->GetDesc();

    device->GetCopyableFootprints(
        &bufferGPUDesc,
        0, 1, 0, &layouts, &numRows,
        &rowSizesInBytes, &requiredSize);

    BYTE* pData;
    bufferUpload->Map(0, nullptr, (void**)&pData);

    D3D12_MEMCPY_DEST DestData =
    {
        pData + layouts.Offset,
        layouts.Footprint.RowPitch,
        layouts.Footprint.RowPitch * ullong(numRows)
    };

    for (uint z = 0; z < layouts.Footprint.Depth; ++z)
    {
        BYTE* destSlice = (BYTE*)(DestData.pData) + DestData.SlicePitch * z;
        const BYTE* srcSlice = (const BYTE*)(vertexSubResourceData.pData) + vertexSubResourceData.SlicePitch * z;

        for (uint y = 0; y < numRows; ++y)
            memcpy(destSlice + DestData.RowPitch * y,
                srcSlice + vertexSubResourceData.RowPitch * y,
                (size_t)rowSizesInBytes);
    }

    bufferUpload->Unmap(0, nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = bufferGPU;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    commandList->CopyBufferRegion(
        bufferGPU,
        0,
        bufferUpload,
        layouts.Offset,
        layouts.Footprint.Width);

    barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = bufferGPU;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);
}

void Graphics::Present()
{
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets[backBufferIndex];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    SubmitCommands();

    swapChain->Present(vSync, 0);
    backBufferIndex = (backBufferIndex + 1) % backBufferCount;
}
