///////////////////////////////////////////////////////////////////////////////////////
//
//	VertexTypes.hpp
//	Defines the different vertex types used to display models and their
//  vertex declarations.
//	
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "FileElements.hpp"

using namespace DirectX;

struct ModelVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 norm;
	XMFLOAT2 tex;
	const static D3DVERTEXELEMENT9 Decl[6];
};

struct SkinnedModelVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 norm;
	XMFLOAT2 tex;
	float w[4]; // bones affecting vertex
	byte i[4];
	const static D3DVERTEXELEMENT9 Decl[6];
};


struct VertexDescription
{
	UINT VertexType;
	UINT SizeOfVertex;
	UINT SizeOfInputElementDesc;
	const D3D11_INPUT_ELEMENT_DESC * Declartion;
	const static VertexDescription Desc[2];
};



