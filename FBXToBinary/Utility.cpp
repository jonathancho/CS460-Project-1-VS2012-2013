///////////////////////////////////////////////////////////////////////////////////////
//
//	Utility.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Common.h"

bool IsBoneNode(KFbxNode* pNode)
{
	if( pNode->GetNodeAttribute() != NULL )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
		if( lAttributeType == KFbxNodeAttribute::eSKELETON )
			return true;
	}
	return false;
}


//Does homogeneous transform
KFbxVector4 Transform(const KFbxXMatrix& pXMatrix, const KFbxVector4& point)
{
	KFbxMatrix * m = (KFbxMatrix*)&pXMatrix;
	return m->MultNormalize( point );
}

//FBX SDK has two different types of matrices XMatrix and Matrix
//XMatrix is affine and has functions for extracting pos,trans and rot
//Matrix has all the matrix operations and is returned by several SDK functions
//because of this the matrices need to be converted between each other
void SetXMatrix(KFbxXMatrix& pXMatrix, const KFbxMatrix& pMatrix)
{
	memcpy((double*)pXMatrix.mData, &pMatrix.mData, sizeof(pMatrix.mData));
}

void SetMatrix(KFbxMatrix& pXMatrix, const KFbxXMatrix& pMatrix)
{
	memcpy((double*)&pXMatrix.mData, &pMatrix.mData, sizeof(pMatrix.mData));
}

// Get the matrix of the node in the given pose
KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex)
{
	KFbxXMatrix lPoseMatrix;
	KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
	SetXMatrix(lPoseMatrix,lMatrix);
	return lPoseMatrix;
}

// Get the matrix of the node in the given pose
KFbxXMatrix GetPoseMatrixInverse(KFbxPose* pPose, int pNodeIndex)
{
	KFbxXMatrix lPoseMatrix;
	KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
	lMatrix = lMatrix.Inverse();
	SetXMatrix(lPoseMatrix,lMatrix);
	return lPoseMatrix;
}


// Recursive function to get a node's global default position 
// As a prerequisite, parent node's default local position must be already set 
KFbxXMatrix GetGlobalDefaultPosition(KFbxNode* node) 
{
	// Stores the local position, global position, and parent's global position
	KFbxXMatrix local_position;
	KFbxXMatrix global_position;
	KFbxXMatrix parent_global_position;

	// Get the translation, rotation, and scaling
	KFbxVector4 T, R, S;
	local_position.SetT(node->GetDefaultT(T));
	local_position.SetR(node->GetDefaultR(R));
	local_position.SetS(node->GetDefaultS(S));

	// If the node has a parent
	if (node->GetParent())
	{
		parent_global_position = GetGlobalDefaultPosition(node->GetParent());
		global_position = parent_global_position * local_position;
	}
	// Otherwise, we've reached the end of the recursion,
	// so the global position is the local position
	else
	{
		global_position = local_position;
	}

	// Return the global position
	return global_position;
}



KFbxXMatrix GetPoseMatrix(KFbxNode* pNode,KFbxPose* pPose)
{
	int lNodeIndex = pPose->Find(pNode);
	return GetPoseMatrix( pPose , lNodeIndex );
}

// Get the geometry deformation local to a node. It is never inherited by the
// children.
KFbxXMatrix GetGeometry(KFbxNode* pNode) {
	KFbxVector4 lT, lR, lS;
	KFbxXMatrix lGeometry;

	lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
	lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
	lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

	lGeometry.SetT(lT);
	lGeometry.SetR(lR);
	lGeometry.SetS(lS);

	return lGeometry;
}

void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene)
{
	// The first thing to do is to create the FBX SDK manager which is the 
	// object allocator for almost all the classes in the SDK.
	pSdkManager = KFbxSdkManager::Create();

	if (!pSdkManager)
	{
		Logger::Log( MsgError ,"Unable to create the FBX SDK manager\n");
		exit(0);
	}

	// Create the entity that will hold the scene.
	pScene = KFbxScene::Create(pSdkManager,"");

}

void DestroySdkObjects(KFbxSdkManager* pSdkManager)
{
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	if (pSdkManager) pSdkManager->Destroy();
	pSdkManager = NULL;
}

bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	int lFileFormat = -1;
	bool lStatus;

	// Get the file version number generate by the FBX SDK.
	KFbxIO::GetCurrentVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

	if (!pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat))
	{
		// Unrecognizable file format. Try to fall back to native format.
		lFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeReaderFormat();
	}
	lImporter->SetFileFormat(lFileFormat);

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename);
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		Logger::Log(MsgError,"Call to KFbxImporter::Initialize() failed.\n");
		Logger::Log(MsgError,"Error returned: %s\n\n", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
			lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
		{
			Logger::Log(MsgError,"FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			Logger::Log(MsgError,"FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	Logger::Log(MsgStd,"FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		Logger::Log(MsgStd,"FBX version number for file %s is %d.%d.%d\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOSREF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		IOSREF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		IOSREF.SetBoolProp(IMP_FBX_LINK,            true);
		IOSREF.SetBoolProp(IMP_FBX_SHAPE,           true);
		IOSREF.SetBoolProp(IMP_FBX_GOBO,            true);
		IOSREF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		IOSREF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	Logger::Log(MsgStd,"Importing the Scene...\n");
	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if(lStatus == false)
	{		
		Logger::Log(MsgError,"Failed to import scene.\n");
		Logger::Log(MsgError,"Error returned: %s\n\n", lImporter->GetLastErrorString());
	}
	else
	{
		Logger::Log(MsgResult,"Scene Successfully Imported.\n");
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}



KFbxNodeAttribute::EAttributeType GetNodeAttributeType(KFbxNode* pNode)
{
	KFbxNodeAttribute* nodeAtt = pNode->GetNodeAttribute();
	if( nodeAtt != NULL )
		return nodeAtt->GetAttributeType();
	return KFbxNodeAttribute::eNULL;
}

inline bool IsEqualEpsilon( float A, float B )
{
	return fabs( A - B ) <= 1e-5f;
}

bool CheckPositive(KFbxVector4 scale)
{
	if( scale[0] < 0.0f || 
		scale[1] < 0.0f ||
		scale[2] < 0.0f )
	{
		return false;
	}

	return true;
}

bool CheckScaling(KFbxVector4 scale)
{
	if( !IsEqualEpsilon( scale[0] , scale[1] ) ||
		!IsEqualEpsilon( scale[1], scale[2] ) ||
		!IsEqualEpsilon( scale[0], scale[2] ) )
	{
		return false;
	}

	return true;
}

