///////////////////////////////////////////////////////////////////////////////
//
//	Conversion.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Conversion.h"

//Compute the final matrix MD that transform the vectors
//this may look strange so here is an explanation
//Remember that matrix math is not commutative (order matters)

// D = Destination Space
// F = Whatever space the FBX file is in
// MD = Matrix in Destination Space
// MF = Matrix in FBX Space
// VD = Vector in Destination Space
// VF = Vector in FBX Space
// F->D = Matrix that converts from FBX to Dest
// D->F = Matrix that converts from Dest to FBX
// V' = Transformed vector

//We want MD but we have MF so to convert

//In vector transformation terms these are all straightforward
//1. VD' = VD * MD
//2. VF = VD * D->F
//3. VF' = VF * MF	
//4. VD' = VF' * F->D

//Expanding
//VD' = VD * MD <-(What we want) From 1
//VD' = VF' * F->D From 4
//VD' = VF * MF * F->D From 3
//VD' = VD * D->F * MF * F->D From 2

//Therefore
//MD = D->F * MF * F->D

Converter::Converter(KFbxScene* pScene)
{

	// Convert Axis System to what is used in this example, if needed
	KFbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();

	KFbxAxisSystem DxTransform(KFbxAxisSystem::eDirectX);
	KFbxAxisSystem MaxTransform(KFbxAxisSystem::eMax);
	KFbxAxisSystem MayaTransform(KFbxAxisSystem::eMayaYUp);

	//construct the conversion matrix that takes a vector and converts 
	//it from one vector space to the other (invert the Z axis)
	ConversionMatrix.SetIdentity();
	NeedToChangedWinding = false;

	if( SceneAxisSystem == MaxTransform )
	{
		Logger::Log(MsgStd,"Converting from Max Coordinate System to DirectX\n");
		ConversionMatrix.SetRow( 0 , KFbxVector4(1,0,0,0)  );
		ConversionMatrix.SetRow( 1 , KFbxVector4(0,0,1,0)  );
		ConversionMatrix.SetRow( 2 , KFbxVector4(0,1,0,0)  );
		NeedToChangedWinding = true;
	}
	else if( SceneAxisSystem == MayaTransform)
	{	
		Logger::Log(MsgStd,"Converting from Maya/OpenGL Coordinate System to DirectX\n");
		ConversionMatrix.SetRow( 0 , KFbxVector4(1,0,0,0)  );
		ConversionMatrix.SetRow( 1 , KFbxVector4(0,1,0,0)  );
		ConversionMatrix.SetRow( 2 , KFbxVector4(0,0,-1,0)  );
		NeedToChangedWinding = true;
	}
	else
	{
		Logger::Log( MsgError ,"Unsupported Axis System, no conversion available\n");
	}

	//What we might want to do
	//if( SceneAxisSystem != DxTransform )
	//{
	//	DxTransform.ConvertScene(gScene);
	//}

}

//Convert a matrix in FBX format (right handed,y up) to DirectX format (left handed, y up)
void Converter::ConvertMatrix(KFbxXMatrix& sourceMatX)
{		
	//KFbxMatrix does not have a matrix multiplication
	KFbxMatrix& sourceMat = *(KFbxMatrix*)&sourceMatX;
	sourceMat = ConversionMatrix * sourceMat * ConversionMatrix;
}

//Same as convert expect do not apply the second conversion matrix because we want the vector
//in converted space
void Converter::ConvertMeshMatrix(KFbxXMatrix& sourceMatX)
{			
	//KFbxMatrix does not have a matrix multiplication
	KFbxMatrix& sourceMat = *(KFbxMatrix*)&sourceMatX;
	sourceMat = ConversionMatrix * sourceMat;
}


