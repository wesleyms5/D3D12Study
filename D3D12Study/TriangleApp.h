#pragma once
#include "D3D12App.h"

class TriangleApp : public D3D12App {
public:
    TriangleApp(HINSTANCE hInstance);
    ~TriangleApp();

    bool Init() override;
    void Update() override;
    void Draw() override;

private:
    void BuildPipeline();
    void BuildVertexBuffer();

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;
    Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
};
