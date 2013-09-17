///////////////////////////////////////////////////////////////////////////////////////
//
//	VertexTypes.cpp
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "DXUT.h"
#include "VertexTypes.hpp"

// DX9 Version
const D3DVERTEXELEMENT9 ModelVertex::Decl[] =
{
	{ 0, offsetof(ModelVertex,pos),  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, offsetof(ModelVertex,norm), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0 },
	{ 0, offsetof(ModelVertex,tex), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

const D3DVERTEXELEMENT9 SkinnedModelVertex::Decl[] =
{
	{ 0, offsetof(SkinnedModelVertex,pos),  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, offsetof(SkinnedModelVertex,norm), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0 },
	{ 0, offsetof(SkinnedModelVertex,tex), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, offsetof(SkinnedModelVertex,w),  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
	{ 0, offsetof(SkinnedModelVertex,i),  D3DDECLTYPE_UBYTE4 , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
	D3DDECL_END()
};

// DX11 Version
D3D11_INPUT_ELEMENT_DESC regularModel[] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//DXGI_FORMAT_R8G8B8A8_TYPELESS
};

D3D11_INPUT_ELEMENT_DESC skinnedModel[] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BLENDINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//DXGI_FORMAT_R8G8B8A8_UINT
};

const VertexDescription VertexDescription::Desc[] =
{
	{ VertexTypeDefault, sizeof(ModelVertex), sizeof(regularModel) , regularModel },
	{ VertexTypeSkin, sizeof(SkinnedModelVertex), sizeof(skinnedModel) , skinnedModel },
};

