///////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh.hpp
//	3d traiagular mesh container which consists of a 
//	vertex buffer, index buffer, and a vertex declaration.
//	
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include "Asset.hpp"

class ChunkReader;

class Mesh : public Asset
{
public:
	Mesh();
	~Mesh();

	void * vertexBufferData;
	UINT VertexSize;
	UINT * indexBufferData;
	UINT NumVertices;
	UINT NumPrimitives;
	UINT NumIndices;
	UINT VertexType;

	void Initialize(ID3D11Device * device, ID3DX11EffectTechnique* technique, ID3D11InputLayout** inputLayout);
	void SetAndDraw(ID3D11DeviceContext* deviceContext);
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();

	//IDirect3DVertexBuffer9 * d3dVertexBuffer;
	//IDirect3DIndexBuffer9 * d3dIndexBuffer;
	//IDirect3DVertexDeclaration9* vertexDecl;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	void ReadFrom( ChunkReader& reader );
};