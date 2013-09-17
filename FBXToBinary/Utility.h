///////////////////////////////////////////////////////////////////////////////////////
//
//	Utility.h
//	Various utlity functions some from the FBX samples.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

template< typename type >
void ConvetToStl( std::vector<type>& container , KFbxLayerElementArrayTemplate<type>* FbxContainter)
{
	int numberofObjects = FbxContainter->GetCount();
	void * pData = FbxContainter->GetLocked();

	container.resize( numberofObjects );
	memcpy( &container[0] , pData , sizeof(type)*numberofObjects );

	FbxContainter->Release( &pData );
}

template< typename type >
void FillStl( std::vector<type>& container , std::size_t size)
{
	container.resize( size );
	for( std::size_t i=0;i<size;++i)
		container[i] = i;
}

template< typename type>
static inline void DeleteIt( type* pointer ){delete pointer;}

template<typename keytype,typename type>
static inline void DeleteIt(std::pair<const keytype,type>& entry ){delete entry.second;}

template<typename contype>
void DeleteObjectsInContainer( contype& container )
{
	contype::iterator it = container.begin();
	contype::iterator end = container.end();
	for (;it!=end;++it) DeleteIt( *it );
	container.clear();
}

KFbxVector4 Transform(const KFbxXMatrix& pXMatrix, const KFbxVector4& point);
void SetXMatrix(KFbxXMatrix& pXMatrix, const KFbxMatrix& pMatrix);
void SetMatrix(KFbxMatrix& pXMatrix, const KFbxXMatrix& pMatrix);
bool IsBoneNode(KFbxNode* pNode);
KFbxXMatrix GetGeometry(KFbxNode* pNode);
KFbxXMatrix GetGlobalDefaultPosition(KFbxNode* node);

//File Utility
void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene);
void DestroySdkObjects(KFbxSdkManager* pSdkManager);
bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename);

KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex);
KFbxXMatrix GetPoseMatrix(KFbxNode* pNode,KFbxPose* pPose);

KFbxNodeAttribute::EAttributeType GetNodeAttributeType(KFbxNode* pNode);

//Check to see if scaling is uniform
bool CheckScaling(KFbxVector4 scale);
//Check to see if scaling if positive
bool CheckPositive(KFbxVector4 scale);
