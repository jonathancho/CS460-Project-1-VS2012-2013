///////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh.cpp
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "DXUT.h"
#include "Mesh.hpp"
#include "ChunkReader.hpp"
#include "FileElements.hpp"
#include "VertexTypes.hpp"

Mesh::Mesh()
{
	vertexBufferData = NULL;
	indexBufferData = NULL;
	//d3dVertexBuffer = NULL;
	//d3dIndexBuffer = NULL;
	//vertexDecl = NULL;
	NumVertices = 0;
	NumPrimitives = 0;
}

Mesh::~Mesh()
{
	//SafeRelease( vertexDecl );
	//SafeRelease( d3dVertexBuffer );
	//SafeRelease( d3dIndexBuffer );
	SafeDelete( vertexBufferData );
	SafeDelete( indexBufferData );
}


void Mesh::ReadFrom( ChunkReader& file )
{
	GChunk meshChunk = file.ReadChunkHeader();

	file.Read( VertexType );
	VertexSize = VertexDescription::Desc[ VertexType ].SizeOfVertex;

	file.Read( NumIndices );
	indexBufferData = new uint[ NumIndices ];
	file.ReadArray( indexBufferData , NumIndices );

	file.Read( NumVertices );
	vertexBufferData = new byte[ NumVertices * VertexSize ];
	file.ReadArraySize( vertexBufferData , NumVertices * VertexSize );

	NumPrimitives = NumIndices/3;
}

void Mesh::Initialize(ID3D11Device * device, ID3DX11EffectTechnique* technique, ID3D11InputLayout** inputLayout)
{
	// Describes the buffer we are going to create
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth =  NumVertices * VertexSize;	// Size in bytes of the buffer we are going to create
	vbd.Usage = D3D11_USAGE_IMMUTABLE;			// The contents will not change after creation
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// Making a vertex buffer
	vbd.CPUAccessFlags = 0;                     // CPU does not require read or write access after the buffer has been created
	vbd.MiscFlags = 0;                          // We need nothing here
	vbd.StructureByteStride = 0;                // For structured buffers only

	// Specifies the data we want to initialize the buffer with
	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = vertexBufferData;

	// Create the vertex buffer
	HRESULT success = device->CreateBuffer(&vbd, &vertexBufferInitData, &vertexBuffer);

	// Describes the buffer we are going to create
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = NumIndices * sizeof(uint);  // Size in bytes of the buffer we are going to create
	ibd.Usage = D3D11_USAGE_IMMUTABLE;			// The contents will not change after creation
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// Making a index buffer
	ibd.CPUAccessFlags = 0;                     // CPU does not require read or write access after the buffer has been created
	ibd.MiscFlags = 0;                          // We need nothing here
	ibd.StructureByteStride = 0;                // For structured buffers only

	// Specifies the data we want to initialize the buffer with
	D3D11_SUBRESOURCE_DATA indexBufferInitData;
	indexBufferInitData.pSysMem = indexBufferData;

	// Create the index buffer
	success = device->CreateBuffer(&ibd, &indexBufferInitData, &indexBuffer);

	//D3D11_INPUT_ELEMENT_DESC vertexDescription[] = 
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"ONE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TWO",  0, DXGI_FORMAT_R8G8B8A8_UINT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//	//DXGI_FORMAT_R8G8B8A8_TYPELESS
	//};

	// Get the amount of elements in the D3D11_INPUT_ELEMENT_DESC struct that was chosen
	int vertexSize = VertexDescription::Desc[VertexType].SizeOfInputElementDesc;
	int elementSize = sizeof(D3D11_INPUT_ELEMENT_DESC);
	int numElements = vertexSize / elementSize;

	D3DX11_PASS_DESC passDescription;

	technique->GetPassByIndex(0)->GetDesc(&passDescription);
	success = device->CreateInputLayout(VertexDescription::Desc[VertexType].Declartion, numElements, passDescription.pIAInputSignature, passDescription.IAInputSignatureSize, inputLayout);

	if (success != S_OK)
		printf("Create Input layout failed!\n");

	//Load into vertex Buffer
	/*device->CreateVertexBuffer(NumVertices*VertexSize,D3DUSAGE_WRITEONLY, 0,D3DPOOL_MANAGED, &d3dVertexBuffer , NULL );
	void *pVertices = NULL;
	d3dVertexBuffer->Lock( 0, NumVertices*VertexSize, (void**)&pVertices, 0 );
	memcpy( pVertices, vertexBufferData , NumVertices * VertexSize );
	d3dVertexBuffer->Unlock();

	device->CreateVertexDeclaration( VertexDescription::Desc[VertexType].Declartion , &vertexDecl  );

	device->CreateIndexBuffer( NumIndices * sizeof(uint) , D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED , &d3dIndexBuffer  , NULL );
	d3dIndexBuffer->Lock( 0 , 0 , (void**)&pVertices , 0 );
	memcpy( pVertices, indexBufferData , NumIndices * sizeof(uint) );
	d3dIndexBuffer->Unlock();*/

}

void Mesh::SetAndDraw(ID3D11DeviceContext* deviceContext)
{
	deviceContext->DrawIndexed(NumIndices, 0, 0);
	/*device->SetVertexDeclaration( vertexDecl );
	device->SetStreamSource( 0, d3dVertexBuffer, 0, VertexSize );
	device->SetIndices( d3dIndexBuffer );
	device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0 , 0 , NumVertices , 0 , NumPrimitives );*/
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer* Mesh::GetIndexBuffer()
{
	return indexBuffer;
}