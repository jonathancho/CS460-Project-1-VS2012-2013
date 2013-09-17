///////////////////////////////////////////////////////////////////////////////
//
//	Skeleton.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Skeleton.h"
#include "Utility.h"
#include "FileElements.h"

struct NodeAndParent
{
	NodeAndParent(KFbxNode* n,int i)
		: node(n) , ParentId( i ) {}
	KFbxNode* node;
	int ParentId;
};


// Collect the bones in breadth first order
void ExtractSkeleton(KFbxNode* root,Skeleton& skel,std::vector<KFbxNode*>& boneNodes)
{
	std::queue<NodeAndParent> nodeStack;
	nodeStack.push( NodeAndParent(root,InvalidIndex) );

	while( nodeStack.size() != 0 )
	{
		NodeAndParent curNode = nodeStack.front();
		nodeStack.pop();

		KFbxNode * pNode = curNode.node;
		int parentIndex = curNode.ParentId;
		int nodeIndex = InvalidIndex;

		if( GetNodeAttributeType(pNode) == KFbxNodeAttribute::eSKELETON  )
		{
			Bone * bone = new Bone;
			nodeIndex= skel.Bones.size();
			bone->ParentIndex = parentIndex;
			bone->BoneIndex = nodeIndex;
			bone->Name = pNode->GetName();
			bone->BoneNode = pNode;

			skel.Bones.push_back( bone );
			boneNodes.push_back( pNode );

			Logger::Log( MsgDetail , "Bone: '%s' Id %d\n" , bone->Name.c_str() , nodeIndex  );
		}

		for(int i = 0; i < pNode->GetChildCount(); i++)
		{
			nodeStack.push( NodeAndParent(pNode->GetChild(i),nodeIndex) );
		}
	}

	Logger::Log( MsgResult , "Skeleton Extracted, Number Of Bones: %d\n" , skel.Bones.size() );

}


void ExtractBindPose(KFbxNode* pNode,KFbxPose* pPose ,Bone& bone ,Converter& converter)
{
	int lNodeIndex = pPose->Find(pNode);
	KFbxXMatrix localMatrix;
	if( lNodeIndex == -1 )
	{
		//Bone does not have a bind pose export identity
		localMatrix.SetIdentity();
	}
	else
	{
		localMatrix = GetPoseMatrix(pPose,lNodeIndex);
	}
	ExtractBindMatrix(pNode,bone,localMatrix,converter);
}


void ExtractBindMatrix(KFbxNode* pNode,Bone& bone ,KFbxXMatrix& localMatrix,Converter& converter)
{
	converter.ConvertMatrix( localMatrix );

	KFbxVector4 scale = localMatrix.GetS();
	if( !CheckScaling(scale) || !CheckPositive(scale) )
	{
		//If there is negative scaling the matrix inverse might have problems
		Logger::Log( MsgError , "Non-uniform scaling on node '%s'.\n", pNode->GetName()  );	
		localMatrix.SetS( KFbxVector4(1,1,1) );
	}

	KFbxXMatrix Inverse = localMatrix.Inverse();

	//TODO: You could also store scaling if you want to have scaling on your mesh but
	//it should always be uniform scaling

	bone.BindPos = localMatrix.GetT();
	bone.BindRot = localMatrix.GetQ();

	bone.BoneSpacePos = Inverse.GetT();
	bone.BoneSpaceRot = Inverse.GetQ();
}

int GetBoneIndex(KFbxNode* pNode,Skeleton& skel)
{
	for( int i=0;i<skel.Bones.size();++i)
	{
		if( pNode == skel.Bones[i]->BoneNode )
			return i;
	}
	return InvalidIndex;
}

Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{
	DeleteObjectsInContainer(Bones);
}

void Skeleton::Write(ChunkFileWriter& writer)
{
	u32 skelStartChunk = writer.StartChunk( MarkSkel );
	writer.Write( u32( Bones.size() ) );
	for ( uint j=0;j<Bones.size();++j)
	{
		Bone& curBone = *Bones[j];

		writer.Write( curBone.Name );
		writer.Write( curBone.ParentIndex );

		writer.Write( f32( curBone.BindPos[0] ) );
		writer.Write( f32( curBone.BindPos[1] ) );
		writer.Write( f32( curBone.BindPos[2] ) );

		writer.Write( f32( curBone.BindRot[0] ) );
		writer.Write( f32( curBone.BindRot[1] ) );
		writer.Write( f32( curBone.BindRot[2] ) );
		writer.Write( f32( curBone.BindRot[3] ) );


		writer.Write( f32( curBone.BoneSpacePos[0] ) );
		writer.Write( f32( curBone.BoneSpacePos[1] ) );
		writer.Write( f32( curBone.BoneSpacePos[2] ) );

		writer.Write( f32( curBone.BoneSpaceRot[0] ) );
		writer.Write( f32( curBone.BoneSpaceRot[1] ) );
		writer.Write( f32( curBone.BoneSpaceRot[2] ) );
		writer.Write( f32( curBone.BoneSpaceRot[3] ) );

	}
	writer.EndChunk( skelStartChunk );
}