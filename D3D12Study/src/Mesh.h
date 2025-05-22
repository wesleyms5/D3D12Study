#pragma once

#ifndef DXUT_MESH_H
#define DXUT_MESH_H
#include "Types.h"
#include <d3d12.h>
#include <string>
using std::string;

struct Mesh {

    string id;

    ID3DBlob* vertexBufferCPU;

    ID3D12Resource* vertexBufferUpload;
    
    ID3D12Resource* vertexBufferGPU;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

    uint vertexByteStride;
    uint vertexBufferSize;

    Mesh(string name);
    ~Mesh();

    D3D12_VERTEX_BUFFER_VIEW* VertexBufferView();

};

#endif