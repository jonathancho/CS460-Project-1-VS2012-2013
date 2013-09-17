///////////////////////////////////////////////////////////////////////////////
//
//	Model.cpp
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "DXUT.h"
#include "Model.hpp"
#include "ChunkReader.hpp"
#include "FileElements.hpp"
#include "VertexTypes.hpp"//only needed for software skinning
//#include <xnamath.h>

Model * LoadModel(std::string filename,ID3D11Device * device, ID3DX11EffectTechnique** technique, ID3D11InputLayout** inputLayout)
{
	ChunkReader file;
	file.Open( filename );

	//TODO: Expand loading code to be more generic

	GChunk fileChunk = file.ReadChunkHeader();

	if( fileChunk.Type != FileMark )
		return NULL;

	Model * newModel = new Model();
	newModel->modelType = Static;
			
	fileChunk = file.PeekChunk();

	while( fileChunk.Type != 0 )
	{
		switch( fileChunk.Type )
		{
			case MarkMesh:
			{
				newModel->mesh = new Mesh();
				// Load from file into vertexbufferdata and indexbufferdata here
				newModel->mesh->ReadFrom( file );
				// Load into our actual DX11 index and vertex buffers
				if (newModel->mesh->VertexType == VertexTypeDefault)
					newModel->mesh->Initialize( device, technique[0], inputLayout );
				else
					newModel->mesh->Initialize( device, technique[1], inputLayout );
			}
			break;
			case MarkSkel:
			{			
				newModel->modelType = Skinned;
				newModel->Controller = new AnimationController();

				Skel * newSkel = new Skel();
				newSkel->ReadFrom( file );
				newSkel->Initialize();
				newModel->Controller->SetSkel( newSkel );
				newSkel->Release();
			}
			break;
			case MarkAnimation:
			{
				Animation * newAnimation = new Animation();
				newAnimation->ReadFrom( file );
				newModel->Controller->AddAnimation( newAnimation );
				newAnimation->Release();
			}
			break;
			default:
				file.SkipChunk(fileChunk);
			break;
		}

		fileChunk = file.PeekChunk();
	}

	return newModel;
}



void Model::Draw(ID3D11Device * device, ID3DX11EffectTechnique** technique, ID3D11DeviceContext* deviceContext, ID3DX11Effect* effect)
{	
	D3DX11_TECHNIQUE_DESC techniqueDesc;

	if (modelType == Skinned)
	{
		std::vector<XMFLOAT4X4> boneMatrices;
		boneMatrices.resize(Controller->skel->bones.size());
		for (unsigned i = 0; i < boneMatrices.size(); ++i)
		{
			XMMATRIX one = XMLoadFloat4x4(&Controller->skel->GetModelToBoneSpaceTransform(i));
			XMMATRIX two = XMLoadFloat4x4(&Controller->BoneMatrixBuffer[i]);
			XMMATRIX three = one * two;
			XMStoreFloat4x4(&boneMatrices[i], three);
		}

		const bool HardwareSkinned = true;
		//Why is this not #defined out? That way compile errors will not be missed.
		//Both methods are provided as examples.

		if( HardwareSkinned )
		{
			//Hardware skinning using vertex shaders
			ID3DX11EffectMatrixVariable* worldMatrixArray = effect->GetVariableByName("WorldMatrixArray")->AsMatrix();
			HRESULT hresult = worldMatrixArray->SetMatrixArray(reinterpret_cast<float*>(&boneMatrices[0]), 0, boneMatrices.size());
			technique[1]->GetDesc(&techniqueDesc);
			for (unsigned int i = 0; i < techniqueDesc.Passes; ++i)
			{
				technique[1]->GetPassByIndex(i)->Apply(0, deviceContext);
				mesh->SetAndDraw(deviceContext);
			}
		}
		else
		{
			////Software skinning used to debug problems in the pipeline
			//SkinnedModelVertex *pDest = new SkinnedModelVertex[ mesh->NumVertices ];
			//SkinnedModelVertex *pSource = (SkinnedModelVertex *)mesh->vertexBufferData;

			//for( uint i=0;i<mesh->NumVertices;++i)
			//{
			//	Vec3 v = Vec3(0,0,0);
			//	Vec3 n = Vec3(0,0,0);
			//	for( int w=0;w<4;++w)
			//	{		
			//		Vec3 tn,tp;
			//		uint boneIndex = pSource[i].i[w];
			//		Matrix4 boneMatrix = boneMatrices[ boneIndex ] ;
			//		////(x,y,z,1) Homogeneous transform
			//		D3DXVec3TransformCoord( &tp , &pSource[i].pos , &boneMatrix  );
			//		//(x,y,z,0) Normal transform remove translation(technically the matrix should be MatrixTranspose(MatrixInverse(tn)) but
			//		//since we are using only uniform scaling this is equivalent)
			//		D3DXVec3TransformNormal( &tn , &pSource[i].norm , &boneMatrix );
			//		tn *= pSource[i].w[w];
			//		tp *= pSource[i].w[w];
			//		v+= tp;
			//		n+= tn;
			//	}
			//	pDest[i].pos = v;
			//	pDest[i].norm = n;
			//	pDest[i].tex = pSource[i].tex;
			//}

			//device->SetVertexDeclaration( mesh->vertexDecl );

			//pEffect->SetTechnique( "Technique0" );
			//pEffect->Begin( &passes, 0 );
			//for( UINT uPass = 0; uPass < passes; ++uPass )
			//{
			//	pEffect->BeginPass( uPass );
			//	device->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST , 0 , mesh->NumVertices , mesh->NumPrimitives , mesh->indexBufferData, D3DFMT_INDEX32 ,  pDest , mesh->VertexSize );
			//	pEffect->EndPass();
			//}
			//pEffect->End();

			//delete pDest;
		}

	}
	else
	{
		technique[0]->GetDesc(&techniqueDesc);
		for (unsigned int i = 0; i < techniqueDesc.Passes; ++i)
		{
			technique[0]->GetPassByIndex(i)->Apply(0, deviceContext);
			mesh->SetAndDraw(deviceContext);
		}
	}

	//UINT passes = 0;

	//if( modelType == Skinned )
	//{
	//	std::vector< Matrix4 > boneMatrices;
	//	boneMatrices.resize( Controller->skel->bones.size() );
	//	for(uint i=0;i<boneMatrices.size();++i)
	//	{
	//		//The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
	//		boneMatrices[i] = Controller->skel->GetModelToBoneSpaceTransform( i ) * Controller->BoneMatrixBuffer[i];
	//	}

	//	const bool HardwareSkinned = true;
	//	//Why is this not #defined out? That way compile errors will not be missed.
	//	//Both methods are provided as examples.

	//	if( HardwareSkinned )
	//	{
	//		//Hardware skinning using vertex shaders
	//		pEffect->SetMatrixArray( "WorldMatrixArray" , &boneMatrices[0] ,  boneMatrices.size() );
	//		pEffect->SetTechnique( "TechniqueSkin" );
	//		pEffect->Begin( &passes, 0 );
	//		for( UINT uPass = 0; uPass < passes; ++uPass )
	//		{
	//			pEffect->BeginPass( uPass );
	//			mesh->SetAndDraw( device );
	//			pEffect->EndPass();
	//		}
	//		pEffect->End();
	//	}
	//	else
	//	{
	//		//Software skinning used to debug problems in the pipeline
	//		SkinnedModelVertex *pDest = new SkinnedModelVertex[ mesh->NumVertices ];
	//		SkinnedModelVertex *pSource = (SkinnedModelVertex *)mesh->vertexBufferData;

	//		for( uint i=0;i<mesh->NumVertices;++i)
	//		{
	//			Vec3 v = Vec3(0,0,0);
	//			Vec3 n = Vec3(0,0,0);
	//			for( int w=0;w<4;++w)
	//			{		
	//				Vec3 tn,tp;
	//				uint boneIndex = pSource[i].i[w];
	//				Matrix4 boneMatrix = boneMatrices[ boneIndex ] ;
	//				////(x,y,z,1) Homogeneous transform
	//				D3DXVec3TransformCoord( &tp , &pSource[i].pos , &boneMatrix  );
	//				//(x,y,z,0) Normal transform remove translation(technically the matrix should be MatrixTranspose(MatrixInverse(tn)) but
	//				//since we are using only uniform scaling this is equivalent)
	//				D3DXVec3TransformNormal( &tn , &pSource[i].norm , &boneMatrix );
	//				tn *= pSource[i].w[w];
	//				tp *= pSource[i].w[w];
	//				v+= tp;
	//				n+= tn;
	//			}
	//			pDest[i].pos = v;
	//			pDest[i].norm = n;
	//			pDest[i].tex = pSource[i].tex;
	//		}

	//		device->SetVertexDeclaration( mesh->vertexDecl );

	//		pEffect->SetTechnique( "Technique0" );
	//		pEffect->Begin( &passes, 0 );
	//		for( UINT uPass = 0; uPass < passes; ++uPass )
	//		{
	//			pEffect->BeginPass( uPass );
	//			device->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST , 0 , mesh->NumVertices , mesh->NumPrimitives , mesh->indexBufferData, D3DFMT_INDEX32 ,  pDest , mesh->VertexSize );
	//			pEffect->EndPass();
	//		}
	//		pEffect->End();

	//		delete pDest;
	//	}

	//}
	//else
	//{
	//	pEffect->SetTechnique( "Technique0" );
	//	pEffect->Begin( &passes, 0 );
	//	for( UINT uPass = 0; uPass < passes; ++uPass )
	//	{
	//		pEffect->BeginPass( uPass );
	//		mesh->SetAndDraw( device );
	//		pEffect->EndPass();
	//	}
	//	pEffect->End();
	//}

}

Model::Model()
{
	mesh = NULL;
	Controller = NULL;
}

Model::~Model()
{
	SafeRelease( mesh );
	SafeDelete( Controller );
}
