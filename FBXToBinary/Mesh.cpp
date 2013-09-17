///////////////////////////////////////////////////////////////////////////////
//
//	Mesh.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Mesh.h"
#include "Skeleton.h"

void MeshVertices::ConvertTriWinding()
{
	for( uint i=0;i<ProcessedIndices.size();i+=3)
	{
		std::swap( ProcessedIndices[i] , ProcessedIndices[i+2] );
	}
}

void MeshVertices::Triangulate()
{
	std::vector<int> NewIndices;
	int c = 0;
	for( int i=0;i<PolygonSizeArray.size();++i)
	{
		int size = PolygonSizeArray[i];		
		//Simple Convex Polygon Triangulation: always n-2 tris
		const int NumberOfTris = size-2;

		for( int p=0;p<NumberOfTris;++p)
		{
			NewIndices.push_back( ProcessedIndices[c+0] );
			NewIndices.push_back( ProcessedIndices[c+1+p] );
			NewIndices.push_back( ProcessedIndices[c+2+p] );
		}		
		c+=size;
	}

	//Swap the new triangulated indices with the old vertices
	ProcessedIndices.swap( NewIndices );
}


void MeshVertices::Optimize()
{
	//TODO: Optimize triangle order for cache efficiency
}

int MeshVertices::AddNewVertex(IndexedVertex v)
{
	Vertex nv;
	nv.normal = Normals[v.normalIndex];
	nv.point = Points[v.posIndex];
	nv.texture = Uvs[v.uvIndex];
	ProcessedVertices.push_back( nv );
	SourceIndices.push_back( v );
	return ProcessedVertices.size() - 1;
}

bool MeshVertices::IsMatchingVertex( Vertex& vertex , IndexedVertex& cur , IndexedVertex& v  )
{
	//Need to check to see if the normals and uvs match if not generate a new vertex
	if( v.normalIndex != cur.normalIndex )
		return false;

	if( v.uvIndex != cur.uvIndex )
		return false;

	return true;
}


int MeshVertices::FindMatchingVertex(IndexedVertex& v)
{
	std::vector<int>& mappedVertices = ControlMap[v.posIndex];
	for( int i=0;i<mappedVertices.size();++i)
	{
		if( IsMatchingVertex( ProcessedVertices[ mappedVertices[i] ] , SourceIndices[ mappedVertices[i] ] , v ) )
			return mappedVertices[i];
	}

	//No matching vertices mapped for this index, just add one
	int newIndex = AddNewVertex( v );
	mappedVertices.push_back( newIndex );
	return newIndex;
}

void MeshVertices::GenerateVertices()
{
	if( UvIndices.empty() )
	{
		//Fill UvIndices with zeros
		UvIndices.resize( PositionIndices.size() );
		//Put in a dummy UV
		Uvs.push_back( KFbxVector2(0,0) );
	}

	//Create a vertex and an index buffer
	ControlMap.resize( PositionIndices.size() );
	for( int i=0;i<PositionIndices.size();++i)
	{
		IndexedVertex v = { PositionIndices[i] , NIndices[i] , UvIndices[i] };
		int index = FindMatchingVertex( v );
		ProcessedIndices.push_back( index );
	}	

	Triangulate();
	ConvertTriWinding();
}


void ExtractGeometry(KFbxNode* pNode,KFbxPose* pPose,MeshVertices& gen,Converter& converter)
{
	KString nodeName = pNode->GetName();

	if( pNode->GetNodeAttribute() != NULL )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();

		if( lAttributeType == KFbxNodeAttribute::eMESH )
		{
			Logger::Log(MsgStd,"Extracting Mesh '%s'\n" , nodeName.Buffer() );

			//When want to move the mesh into model space and in the bind pose if one is provided
			KFbxXMatrix  meshTransform;
			if( pPose )
			{
				//For skinned meshes get the pose matrix
				meshTransform = GetPoseMatrix( pNode , pPose );
			}
			else
			{
				//TODO: Adjust this code. What do you want it to do in the default case?

				//This will cause the mesh transform to be collapsed into the vertices.
				//Use this if you want exactly what is displayed in the model file 
				meshTransform = GetGlobalDefaultPosition(pNode);

				//This will make the mesh be exported at its local original ignore
				//its transformation in the scene.
				//meshTransform.SetIdentity();
			}

			//Meshes have a separate geometry transform that also needs to be applied
			KFbxXMatrix geoTransform = GetGeometry(pNode);
			meshTransform = meshTransform  * geoTransform;

			//Convert the matrix into the destination coordinate space
			converter.ConvertMeshMatrix( meshTransform );

			KFbxMesh * mesh = pNode->GetMesh();
			int polyCount =  mesh->GetPolygonCount();

			//Get the indices			
			int polyVertexCount = mesh->GetPolygonVertexCount();
			int * indices = mesh->GetPolygonVertices();

			gen.PositionIndices.resize( polyVertexCount );
			memcpy( &gen.PositionIndices[0] , indices , sizeof(int) *polyVertexCount );

			for(int i=0;i<polyCount;++i)
				gen.PolygonSizeArray.push_back( mesh->GetPolygonSize( i ) );

			//Get Points
			KFbxVector4 * points = mesh->GetControlPoints();
			int controlPointsCount = mesh->GetControlPointsCount();
			gen.Points.resize( controlPointsCount );
			for( int cp=0;cp<controlPointsCount;++cp)
				gen.Points[cp] = Transform( meshTransform , points[cp] );

			KFbxLayer * layer = mesh->GetLayer(0);

			//Get the normals from the mesh
			if( KFbxLayerElementNormal * normLayer = layer->GetNormals() )
			{
				ConvetToStl( gen.Normals , &normLayer->GetDirectArray() );
				//Remove translation from the matrix for the normals
				meshTransform.SetT( KFbxVector4(0,0,0,0) );
				for( int n=0;n<gen.Normals.size();++n)
				{
					gen.Normals[n] = Transform( meshTransform , gen.Normals[n] );
					gen.Normals[n].Normalize();
				}

				if( normLayer->GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT )
					ConvetToStl( gen.NIndices , &normLayer->GetIndexArray() );
				else
				{
					KFbxLayerElement::EMappingMode Mapping = normLayer->GetMappingMode();

					//Normals map directly to control points
					if( Mapping == KFbxLayerElement::eBY_CONTROL_POINT)
						gen.NIndices = gen.PositionIndices;

					if( Mapping == KFbxLayerElement::eBY_POLYGON_VERTEX)
						FillStl( gen.NIndices , polyVertexCount );

				}
			}

			//If there is UV process them
			if(  KFbxLayerElementUV * uvLayer = layer->GetUVs() )
			{
				ConvetToStl( gen.Uvs , &uvLayer->GetDirectArray() );
				KFbxLayerElement::EMappingMode Mapping = uvLayer->GetMappingMode();
				if( uvLayer->GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT )
					ConvetToStl( gen.UvIndices , &uvLayer->GetIndexArray() );
				else
					gen.UvIndices = gen.PositionIndices;
			}

			Logger::Log(MsgStd,"Generating Vertices for  Mesh '%s'\n" , nodeName.Buffer() );
			gen.GenerateVertices();

			Logger::Log( MsgResult , "Vertices Generated for Mesh '%s'\n" , nodeName.Buffer() );
			Logger::Log( MsgResult , "Triangles: %6d\n" , gen.ProcessedIndices.size() / 3  );
			Logger::Log( MsgResult , "Points:    %6d\n" , controlPointsCount );
			Logger::Log( MsgResult , "Indices:   %6d\n" , gen.ProcessedIndices.size() );
			Logger::Log( MsgResult , "Vertices:  %6d\n" , gen.ProcessedVertices.size() ) ;

		}
	}
}

//////////////////////Skin////////////////////

bool ExtractSkinWeights(KFbxPose* pPose,KFbxNode* pNode,SkinData& skinData,Skeleton& skel,Converter& converter)
{	
	KString nodeName = pNode->GetName();
	if( GetNodeAttributeType(pNode) == KFbxNodeAttribute::eMESH )
	{

		Logger::Log(MsgStd,"Extracting Skin Weights for '%s'\n" ,  nodeName.Buffer() );

		KFbxMesh * pMesh = pNode->GetMesh();
		int lVertexCount = pMesh->GetControlPointsCount();

		// All the links must have the same link mode.
		KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

		skinData.PointWeights.resize( lVertexCount );
		int lSkinCount=pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
		for(int i=0; i<lSkinCount; ++i)
		{
			KFbxSkin * pSkin = (KFbxSkin *)pMesh->GetDeformer(i, KFbxDeformer::eSKIN);
			int lClusterCount = pSkin->GetClusterCount();

			for (int j=0; j<lClusterCount; ++j)
			{
				KFbxCluster* pCluster = pSkin->GetCluster(j);
				if (!pCluster->GetLink())
					continue;

				KFbxNode * pLink = pCluster->GetLink();
				int boneIndex = GetBoneIndex(pLink,skel);

				//Bone does not have a bind pose so the Transform Link Matrix
				//as the bind pose
				int nodeIndex = pPose->Find( pLink );
				if( nodeIndex == -1 )
				{
					KFbxXMatrix linkBindMatrix;
					pCluster->GetTransformLinkMatrix( linkBindMatrix );	
					pPose->Add(pLink,KFbxMatrix(linkBindMatrix));
				}

				int vertexIndexCount = pCluster->GetControlPointIndicesCount();
				for (int k = 0; k < vertexIndexCount; ++k) 
				{            
					int lIndex = pCluster->GetControlPointIndices()[k];
					double lWeight = pCluster->GetControlPointWeights()[k];

					JointWeight j = {  lWeight , boneIndex };
					skinData.PointWeights[ lIndex ].push_back( j );
				}
			}
		}

		//Normalize the skin weights for 4 weights for vertex that sum to one
		for( uint i=0;i<skinData.PointWeights.size();++i)
		{
			//Make sure there is MaxWeights by inserting dummies
			while( skinData.PointWeights[i].size() < MaxWeights)
			{						
				JointWeight j = {  0 , 0 };
				skinData.PointWeights[i].push_back( j );
			}

			//Normalize the weights
			float sum = 0.0f;
			for(int w=0;w<MaxWeights;++w)
				sum += skinData.PointWeights[i][w].weight;
			for(int w=0;w<MaxWeights;++w)
				skinData.PointWeights[i][w].weight /= sum;
		}

		return true;

	}
	else
	{
		Logger::Log( MsgError , "Could not get bone weights for mesh '%s'" , pNode->GetName() );
		return false;
	}
}


void MeshData::CombineInto(MeshData& otherMesh)
{	
	//The base size of the original mesh
	int baseSize = Mesh.ProcessedVertices.size();
	int otherSize = otherMesh.Mesh.ProcessedVertices.size();

	//Append the other meshes vertices
	Mesh.ProcessedVertices.resize( baseSize + otherSize );
	for(uint i=0;i<otherSize;++i)
		Mesh.ProcessedVertices[ baseSize + i ] = otherMesh.Mesh.ProcessedVertices[i];

	//Copy over the indices increasing them by the size of the original vertex buffer
	int baseIndicesSize = Mesh.ProcessedIndices.size();
	int otherIndicesSize = otherMesh.Mesh.ProcessedIndices.size();
	Mesh.ProcessedIndices.resize( baseIndicesSize + otherIndicesSize );
	for(uint i=0;i<otherIndicesSize;++i)
		Mesh.ProcessedIndices[ baseIndicesSize + i ] = baseSize + otherMesh.Mesh.ProcessedIndices[i];

	//Append the other meshes skin weights
	int oPointSize = Skin.PointWeights.size();
	Skin.PointWeights.resize( Skin.PointWeights.size() + otherMesh.Skin.PointWeights.size() );
	for(uint i=0;i<otherMesh.Skin.PointWeights.size();++i)
		Skin.PointWeights[oPointSize+i] = otherMesh.Skin.PointWeights[i];

	//Copy over the indices incrementing the position index so the the weights
	//are still correct (Note the other values in th source buffer are not adjusted)
	Mesh.SourceIndices.resize( baseSize + otherSize );
	for(uint i=0;i<otherMesh.Mesh.SourceIndices.size();++i)
	{
		Mesh.SourceIndices[ baseSize + i ] = otherMesh.Mesh.SourceIndices[i];
		Mesh.SourceIndices[ baseSize + i ].posIndex+=oPointSize;
	}

}


///Write Out the Mesh
void WriteMesh(MeshData& meshData,ChunkFileWriter& writer)
{
	MeshVertices& mesh = meshData.Mesh;
	SkinData& CurSkin = meshData.Skin;

	//Start the a mesh chunk
	u32 meshStartPos = writer.StartChunk( MarkMesh );

	//Write the vertex type to the file
	writer.Write( u32(meshData.MeshVertexType) );

	//Write the Indices
	writer.Write( u32( mesh.ProcessedIndices.size() ) );
	for( uint i=0;i<mesh.ProcessedIndices.size();++i)
		writer.Write( u32(mesh.ProcessedIndices[i]) );

	//Write the Vertices
	writer.Write( u32( mesh.ProcessedVertices.size() ) );
	for( uint i=0;i<mesh.ProcessedVertices.size();++i)
	{
		Vertex& v = mesh.ProcessedVertices[i];

		writer.Write( f32( v.point[0] ) );
		writer.Write( f32( v.point[1] ) );
		writer.Write( f32( v.point[2] ) );

		writer.Write( f32( v.normal[0] ) );
		writer.Write( f32( v.normal[1] ) );
		writer.Write( f32( v.normal[2] ) );

		writer.Write( f32( v.texture[0] ) );
		writer.Write( f32( 1.0f - v.texture[1] ) );

		//Write the Skin Data for each vertex
		if( meshData.MeshVertexType ==  VertexTypeSkin)
		{
			//The originalPositionIndex is what control point was used to bind the skin
			//weights so we use this index to determine what skin weights to use
			int originalPositionIndex = mesh.SourceIndices[i].posIndex;
			WeightVector & weights = CurSkin.PointWeights[originalPositionIndex];
			writer.Write( CurSkin.PointWeights[originalPositionIndex][0].weight );
			writer.Write( CurSkin.PointWeights[originalPositionIndex][1].weight );
			writer.Write( CurSkin.PointWeights[originalPositionIndex][2].weight );
			writer.Write( CurSkin.PointWeights[originalPositionIndex][3].weight );
			writer.Write( byte( CurSkin.PointWeights[originalPositionIndex][0].index ) );
			writer.Write( byte( CurSkin.PointWeights[originalPositionIndex][1].index ) );
			writer.Write( byte( CurSkin.PointWeights[originalPositionIndex][2].index ) );
			writer.Write( byte( CurSkin.PointWeights[originalPositionIndex][3].index ) );
		}

	}

	//End the mesh chunk
	writer.EndChunk( meshStartPos );
}
