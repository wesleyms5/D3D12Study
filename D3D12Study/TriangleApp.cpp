#include "TriangleApp.h"
#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;

};

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const std::string& entry, const std::string& target);

void TriangleApp::BuildPipeline() {

	auto vsByteCode = CompileShader(L"shaders/Triangle.hlsl", "vs", "vs_5_0");
	auto psByteCode = CompileShader(L"shaders/Triangle.hlsl", "ps", "ps_5_0");

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig, &errorBlob);

    md3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));

    // Input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vsByteCode.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(psByteCode.Get());
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.NumRenderTargets = 1;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO));
}

// BuildVertexBuffer
void TriangleApp::BuildVertexBuffer() {
    Vertex vertices[] = {
        { { 0.0f,  0.25f, 0.0f }, {1.0f, 0.0f, 0.0f, 1.0f} },
        { { 0.25f, -0.25f, 0.0f }, {0.0f, 1.0f, 0.0f, 1.0f} },
        { { -0.25f, -0.25f, 0.0f }, {0.0f, 0.0f, 1.0f, 1.0f} },
    };

    const UINT vbSize = sizeof(vertices);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    md3dDevice->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&mVertexBuffer));

    void* mappedData = nullptr;
    mVertexBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, vertices, vbSize);
    mVertexBuffer->Unmap(0, nullptr);

    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.SizeInBytes = vbSize;
    mVertexBufferView.StrideInBytes = sizeof(Vertex);