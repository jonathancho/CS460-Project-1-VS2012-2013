///////////////////////////////////////////////////////////////////////////////////////
//
//	Animation.hpp
//  Defines all classes need for skeletal animation and their support structures.
//	Animation - Keyframes and tracks for animation.
//	Skeleton - Bone Node Graph and bone space matrices.
//	AnimationController - Controls the animation for a animated model by tracking time and
//		using an animation and skeleton to generate a matrix buffer.
//	
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Asset.hpp"
#include "ChunkReader.hpp"

using namespace DirectX;

//A Keyframe is a snapshot of a node / bone
//at a point in time.
struct KeyFrame
{
	float time;
	XMFLOAT3 T;
	XMFLOAT4 R;
};

//A Track is a set of keyframes that are in temporal
//order from 0 to the animation duration
struct Track
{
	std::vector< KeyFrame > KeyFrames;
};

//Track data is used to help process the animation in this
//case it is the last keyframe the track was on
struct TrackData
{
	unsigned int LastKey;
};
typedef std::vector<TrackData> TrackBuffer;

//The matrix buffer is the result of processing an animation and
//what is need to render the animated mesh
typedef std::vector<XMFLOAT4X4> MatrixBuffer;


//A Bone has two primary functions. It defines the parent/child
//relationships between animation tracks and stores the model
//to bone space transform used to perform vertex skinning.
struct Bone
{
	std::string Name;
	int BoneIndex;
	int ParentBoneIndex;

	//Not all the following data is needed but is
	//helpful to debug and understand how skinning works

	//Translation of bone in bind position
	XMFLOAT3 BindT;
	//Rotation of bone in bind position
	XMFLOAT4 BindR;
	//Transform of the bone in bind position
	XMFLOAT4X4 BindTransform;

	//Transform from model space to bone space
	//this is the inverse of the BindTransform
	XMFLOAT4X4 ModelToBoneSpace;

	//Inverse Translation of bone in bind position
	XMFLOAT3 ModelToBoneSpaceT;
	//Inverse Rotation of bone in bind position
	XMFLOAT4 ModelToBoneSpaceR;

	std::vector<Bone*> Children;
};


//Result of interpolating an animation track. Stored in a struct
//so when new data is added to the animation all the function declarations
//do not need to be updated.
struct AnimTransform
{
	XMFLOAT3 T;
	XMFLOAT4 R;
	XMFLOAT4X4 GetMatrix();
};

//The animation stores the keyframes for each track and interpolate between them.
//It does not contain the graph data (parent/child) or match tracks to individual 
//transform nodes/bones (the skeleton contains this information). 
//This is because their can be many animations related to one skeleton.
class Animation : public Asset
{
public:
	Animation();
	~Animation();
	float Duration;
	std::vector< Track > Tracks;
	void CalculateTransform(float animTime,unsigned int trackIndex,AnimTransform& atx,TrackData& data);
	void ReadFrom(ChunkReader& file);
};

//The skeleton contains the bones which have the graph (parent/child) relationships and what animation tracks
//correspond to what bones.
class Skel : public Asset
{
public:
	void Initialize();
	void ReadFrom(ChunkReader& reader);
	void ProcessAnimationGraph(float time,MatrixBuffer& matrixBuffer,Animation& anim,TrackBuffer& trackData);
	void RecursiveProcess(float time,Bone& bone,Animation& anim,MatrixBuffer& matrixBuffer,TrackBuffer& trackData,XMFLOAT4X4& parentTransform);
	void ProcessBindPose(std::vector<XMFLOAT4X4>& buffer);
	XMFLOAT4X4& GetModelToBoneSpaceTransform(int boneIndex);
	std::vector<Bone> bones;
	std::vector<Bone*> RootBones;
};

//Controls the animation for a animated model by tracking time and
//using an animation and skeleton to generate a matrix buffer.
class AnimationController
{
public:
	AnimationController();
	~AnimationController();
	void Update(float dt);

	float animTime;
	float animSpeed;
	Skel * skel;
	Animation * ActiveAnim;
	TrackBuffer AnimTrackData;
	MatrixBuffer BoneMatrixBuffer;
	std::vector<Animation*> Animations;

	void ClearTrackData();
	void Process();
	void ProcessBindPose();
	void SetSkel(Skel * skel);
	void AddAnimation(Animation * anim);
};
