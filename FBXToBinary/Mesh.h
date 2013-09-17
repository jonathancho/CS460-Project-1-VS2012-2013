///////////////////////////////////////////////////////////////////////////////
//
//	Mesh.h
//	Extract mesh data from fbx mesh node and converts them to a triangle buffer and
//	an index buffer. 
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ChunkFileWriter.h"
#include "Conversion.h"
#include "Animation.h"

enum VertexType
{
	VertexTypeDefault,
	VertexTypeSkin
};

struct Vertex
{
	KFbxVector4 point;
	KFbxVector4 normal;
	KFbxVector2 texture;
};

struct IndexedVertex
{
	int posIndex;
	int normalIndex;
	int uvIndex;
};

class MeshVertices
{	
public:
	//Input Data
	std::vector<int> PositionIndices;
	std::vector<int> UvIndices;
	std::vector<int> NIndices;
	std::vector<KFbxVector4> Points;
	std::vector<KFbxVector4> Normals;
	std::vector<KFbxVector2> Uvs;	
	std::vector<int> PolygonSizeArray;

	//Intermediate
	std::vector< std::vector<int> > ControlMap;
	std::vector< IndexedVertex > SourceIndices;

	//Resulting Data
	std::vector<Vertex> ProcessedVertices;
	std::vector<int> ProcessedIndices;

	void ConvertTriWinding();
	void Triangulate();
	int AddNewVertex(IndexedVertex v);
	bool IsMatchingVertex( Vertex& vertex , IndexedVertex& cur , IndexedVertex& v  );
	int FindMatchingVertex(IndexedVertex& v);
	void GenerateVertices();
	void Optimize();
};

void ExtractGeometry(KFbxNode* pNode,KFbxPose* pPose,MeshVertices& gen,Converter& converter);

//Skinning Data
struct JointWeight
{
	float weight;
	uint index;
};
typedef std::vector< JointWeight > WeightVector;

struct SkinData
{
	//Index in PointWeight vector correspond to mesh control points which
	//are mapped by the Position Indices
	std::vector< WeightVector > PointWeights;
};

struct Skeleton;
bool ExtractSkinWeights(KFbxPose* pPose,KFbxNode* pNode,SkinData& skinData,Skeleton& skel,Converter& converter);

class MeshData
{
public:
	MeshData(KFbxNode * meshNode) : MeshNode(meshNode) , MeshVertexType(VertexTypeDefault){};
	KFbxNode * MeshNode;
	VertexType MeshVertexType;
	MeshVertices Mesh;
	SkinData Skin;
	void CombineInto(MeshData& mesh);
};

void WriteMesh(MeshData& meshData,ChunkFileWriter& writer);
