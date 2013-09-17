///////////////////////////////////////////////////////////////////////////////
//
//	Skeleton.h
//	Extracts skeleton data (bone graph and bind pose) from FBX scene and write
//	them in binary format.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Conversion.h"

struct Bone
{
	std::string Name;
	int ParentIndex;
	int BoneIndex;
	KFbxNode * BoneNode;

	KFbxVector4 BindPos;
	KFbxQuaternion BindRot;

	KFbxVector4 BoneSpacePos;
	KFbxQuaternion BoneSpaceRot;
};

struct Skeleton
{
	Skeleton();
	~Skeleton();
	std::vector<Bone*> Bones;
	void Write(ChunkFileWriter& writer);
};

int GetBoneIndex(KFbxNode* pNode,Skeleton& skel);
void ExtractSkeleton(KFbxNode* root,Skeleton& skel,std::vector<KFbxNode*>& boneNodes);
void ExtractBindPose(KFbxNode* pNode,KFbxPose* pPose ,Bone& bone ,Converter& converter);
void ExtractBindMatrix(KFbxNode* pNode,Bone& bone ,KFbxXMatrix& matrix,Converter& converter);



