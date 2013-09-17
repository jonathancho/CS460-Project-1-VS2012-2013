///////////////////////////////////////////////////////////////////////////////
//
//	Scene.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Scene.h"

Scene::Scene()
{
	BindPose = NULL;
}


void Scene::CollectMeshes(KFbxNode* pRootNode,bool onlySkinned)
{
	for(int i = 0; i < pRootNode->GetChildCount(); i++)
	{
		KFbxNode* pNode = pRootNode->GetChild(i);

		//If the node invisible it is usually a construction or dummy mesh
		//either way ignore it
		if( pNode->GetVisibility() )
		{
			//The node must be a mesh
			if( GetNodeAttributeType(pNode) == KFbxNodeAttribute::eMESH )
			{
				KFbxMesh * pMesh = pNode->GetMesh();

				//If looking for only skinned models ignore meshes without deformers
				if( onlySkinned && pMesh->GetDeformer(0, KFbxDeformer::eSKIN) == NULL )
					continue;

				//MeshNodes.push_back( pRootNode->GetChild(i) );
				Meshes.push_back( new MeshData(pRootNode->GetChild(i)) );
			}

			//TODO: You can collect all the meshes in the scene but then you must
			//support multiple meshes or combine them for simplicity this sample does not
			//search past the root level
			if( GetNodeAttributeType(pNode) == KFbxNodeAttribute::eNULL  )
			{
				CollectMeshes( pNode , onlySkinned );
			}


		}
	}
}


bool Scene::ExtractSceneData(KFbxScene* gFBXScene)
{
	//Determine conversion
	Converter converter(gFBXScene);

	KFbxNode* rootNode = gFBXScene->GetRootNode();
	
	// Get the list of all the takes.
	KArrayTemplate<KString*> TakeNameArray;
	gFBXScene->FillTakeNameArray(TakeNameArray);
	int poseCount = gFBXScene->GetPoseCount();//Need a Bind Pose
	int takeCount = TakeNameArray.GetCount();//Need default animation and one other animation

	//TODO: Is this the logic you want for your exporter?
	if( poseCount > 0 && gFBXScene->GetPose(0)->IsBindPose() && TakeNameArray.GetCount() > 1 )
	{
		Logger::Log(MsgStd,"Exporting As Skinned Model\n" );	
		ExtractAnimations = true;
		ExtractMesh = true;
		ExtractSkinData = true;
		ExtractSkeletonData = true;
		BindPose = gFBXScene->GetPose(0);
		Logger::Log(MsgStd,"Using Bind Pose: %s\n" , BindPose->GetName() );	
	}
	else
	{
		Logger::Log(MsgStd,"Exporting As Static Model\n"  );	
		ExtractMesh = true;
		ExtractAnimations = false;
		ExtractSkinData = false;
		ExtractSkeletonData = false;
	}

	//What about Mesh Node Animation?
	//If you want node based animation the best way to accomplish this is the
	//same way skinning is done. Export the meshes in model space and generate
	//a skeleton that uses mesh nodes instead of skeleton nodes. Set all the
	//vertices to be fully weighted to their corresponding node and use the
	//first frame as a bind pose. This prevents having to make multiple draw
	//calls and having to code a separate system for skinned models.


	if( ExtractMesh )
	{
		CollectMeshes( gFBXScene->GetRootNode() , ExtractSkinData );

		if( Meshes.size() == 0 )
		{
			Logger::Log(MsgError,"No valid meshes found in scene");
			return false;
		}

		Logger::Log(MsgStd,"Exporting Geometry...\n");
		for(uint i = 0; i < Meshes.size(); i++)
			ExtractGeometry(Meshes[i]->MeshNode,BindPose,Meshes[i]->Mesh,converter);
	}

	//If we are extracting the skin data we still need the bones from the scene
	if( ExtractSkeletonData || ExtractSkinData )
	{	
		Logger::Log(MsgStd,"Reading Skeleton Data...\n");
		ExtractSkeleton(rootNode,Skel,BoneNodes);
	}

	if( ExtractMesh && ExtractSkinData )
	{		
		for(uint i = 0; i < Meshes.size(); i++)
		{
			if( ExtractSkinWeights( BindPose, Meshes[i]->MeshNode , Meshes[i]->Skin, Skel, converter  ) )
				Meshes[i]->MeshVertexType = VertexTypeSkin;
		}
	}

	if( ExtractSkeletonData || ExtractSkinData  )
	{
		for(uint i=0;i<BoneNodes.size();++i)
			ExtractBindPose( BoneNodes[i] , BindPose , *Skel.Bones[i] , converter );
	}

	if( ExtractAnimations )
	{
		Logger::Log(MsgStd,"Exporting Animations...\n");

		for(int i = 0; i < TakeNameArray.GetCount(); i++)
		{
			//Skip the default take
			if( TakeNameArray[i]->Compare( KFBXTAKENODE_DEFAULT_NAME ) == 0 )
				continue;

			Animation * animation = new Animation();
			Animations.push_back( animation );

			ExtractAnimationTracksFromTake( gFBXScene , *animation , converter  , BoneNodes , TakeNameArray[i]->Buffer() );
		}

	}

	return true;
}


void Scene::WriteScene(std::string outputFileName)
{
	Logger::Log(MsgStd,"Writing Binary File '%s'\n" , outputFileName.c_str() );

	ChunkFileWriter writer;
	writer.Open( outputFileName );

	u32 fileChunkStart = writer.StartChunk( FileMark );

	if( ExtractMesh )
	{
		//TODO: You could combine all the meshes or support multiple meshes per file
		if( Meshes.size() > 1 )
		{
			Logger::Log(MsgStd,"Combing Meshes in scene.\n" );

			for( uint i=1;i<Meshes.size();++i)
				Meshes[0]->CombineInto( *Meshes[i] );
		}
	

		WriteMesh( *Meshes[0], writer );
	}

	if( ExtractSkeletonData )
		Skel.Write( writer );

	if( ExtractAnimations )
	{
		if( Animations.size() > 1 )
			Logger::Log(MsgError,"More than one animation in file. Only one will be exported.\n" );

		if( Animations.size() > 0 )
			WriteAnimation( *Animations[0], writer );
	}

	writer.EndChunk( fileChunkStart );

	Logger::Log(MsgResult,"File Completed, Size: %.2f KB\n" , float(writer.GetPosition())  / 1024.0f  );
}

Scene::~Scene()
{
	DeleteObjectsInContainer(Meshes);
	DeleteObjectsInContainer(Animations);
}