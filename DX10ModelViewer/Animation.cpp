///////////////////////////////////////////////////////////////////////////////////////
//
//	Animation.cpp
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "DXUT.h"
#include "Animation.hpp"
#include "ChunkReader.hpp"
#include "FileElements.hpp"

inline XMFLOAT4 Slerp(XMFLOAT4& q1,XMFLOAT4& q2, float param)
{
	//
	// Quaternion Interpolation With Extra Spins, pp. 96f, 461f
	// Jack Morrison, Graphics Gems III, AP Professional
	//

	XMFLOAT4 qt;

	float alpha, beta;
	float cosom = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w; 
	float slerp_epsilon = 0.00001f;

	bool flip;

	if (flip = (cosom < 0))
		cosom = -cosom;

	if ((1.0 - cosom) > slerp_epsilon)
	{
		float omega = acos (cosom);
		float sinom = sin (omega);
		alpha = (float)(sin((1.0 - param) * omega) / sinom);
		beta = (float)(sin(param * omega) / sinom);
	}
	else
	{
		alpha = (float)(1.0 - param);
		beta = (float)param;
	}

	if (flip) beta = -beta;

	qt.x = (float) (alpha*q1.x + beta*q2.x);
	qt.y = (float) (alpha*q1.y + beta*q2.y);
	qt.z = (float) (alpha*q1.z + beta*q2.z);
	qt.w = (float) (alpha*q1.w + beta*q2.w);

	return qt;
}



XMFLOAT4X4 AnimTransform::GetMatrix()
{
	return BuildTransform( T , R );
}

Animation::Animation()
{

}

Animation::~Animation()
{

}


void Animation::ReadFrom(ChunkReader& file)
{
	GChunk animation = file.ReadChunkHeader();

	UINT NumberOfTracks = 0;
	file.Read( Duration );
	file.Read( NumberOfTracks );

	Tracks.resize( NumberOfTracks );
	for( UINT t=0;t<NumberOfTracks;++t)
	{
		UINT NumberOfKeyframes = 0;
		file.Read( NumberOfKeyframes );
		Tracks[t].KeyFrames.resize( NumberOfKeyframes );
		file.ReadArray( &Tracks[t].KeyFrames[0] , NumberOfKeyframes );
	}
}

void Animation::CalculateTransform(float animTime,UINT trackIndex,AnimTransform& atx,TrackData& data)
{
	//Since keys are not spaced at regular intervals we need to search
	//for the keyframes that will be interpolated between.  The track data is
	//used to store what the last keyframe was to prevent searching the entire
	//track.

	int CurKey = data.LastKey;
	Track& CurPath = Tracks[trackIndex];

	//Search Forward in the keyframes for the interval
	while( CurKey!= CurPath.KeyFrames.size() - 1 && 
		CurPath.KeyFrames[ CurKey + 1 ].time < animTime  )
		++CurKey;

	//Search Backward in the keyframes for the interval
	while( CurKey!= 0 && CurPath.KeyFrames[ CurKey ].time > animTime  )
		--CurKey;

	if( CurKey == CurPath.KeyFrames.size()  - 1 )
	{
		//Past the last keyframe for this path so use the last frame and the transform data
		//so the animation is clamped to the last frame
		atx.R = CurPath.KeyFrames[CurKey].R;
		atx.T = CurPath.KeyFrames[CurKey].T;
	}
	else
	{
		//Generate transform data by interpolating between the two keyframes
		KeyFrame& KeyOne = CurPath.KeyFrames[ CurKey ];
		KeyFrame& KeyTwo = CurPath.KeyFrames[ CurKey + 1 ];

		float t1 = KeyOne.time;
		float t2 = KeyTwo.time;

		//Normalize the distance between the two keyframes
		float segLen = t2 - t1;
		float segStart = animTime - t1;
		float segNormalizedT = segStart/segLen;


		//Slerp Example Using D3DX
		//D3DXQuaternionSlerp( &atx.R , &KeyOne.R , &KeyTwo.R , segNormalizedT  );

		//Using provided slerp function as an example
		atx.R = Slerp( KeyOne.R , KeyTwo.R , segNormalizedT );

		//standard linear interpolation
		XMVECTOR one = XMLoadFloat3(&KeyOne.T);
		XMVECTOR two = XMLoadFloat3(&KeyTwo.T);
		XMVECTOR three = ( 1.0f - segNormalizedT ) * one + segNormalizedT * two;
		XMStoreFloat3(&atx.T, three);
		//atx.T = ( 1.0f - segNormalizedT ) * KeyOne.T  + segNormalizedT *  KeyTwo.T;
	}

	//Remember the last keyframe
	data.LastKey = CurKey;
}



AnimationController::AnimationController()
{
	animTime = 0.0f;
	animSpeed = 1.0f;
	skel = NULL;
	ActiveAnim = NULL;
}

AnimationController::~AnimationController()
{
	SafeRelease( skel );
	for( UINT i=0;i<Animations.size();++i)
		Animations[i]->Release();
}

void AnimationController::Update(float dt)
{
	animTime+=dt*animSpeed;
	//Just loop forever
	if( animTime > ActiveAnim->Duration )
	{
		animTime =  0.0f;
		ClearTrackData();
	}
}

void AnimationController::Process()
{	
	skel->ProcessAnimationGraph( animTime , BoneMatrixBuffer , *ActiveAnim , AnimTrackData );
}

void AnimationController::ProcessBindPose()
{
	skel->ProcessBindPose( BoneMatrixBuffer );
}

void AnimationController::SetSkel(Skel * pSkel)
{
	pSkel->AddRef();
	skel = pSkel;
	BoneMatrixBuffer.resize( skel->bones.size() );
	AnimTrackData.resize( skel->bones.size() );
}

void AnimationController::AddAnimation(Animation * newAnim)
{
	Animations.push_back( newAnim );
	newAnim->AddRef();
	ActiveAnim = newAnim;
}

void AnimationController::ClearTrackData()
{
	//Reset all the keys back to zero
	for(UINT i=0;i<AnimTrackData.size();++i)
		AnimTrackData[i].LastKey = 0;

}

void Skel::ReadFrom( ChunkReader& file )
{
	GChunk skeletonChunk = file.ReadChunkHeader();

	UINT NumberOfBones = 0;
	file.Read( NumberOfBones );

	bones.resize( NumberOfBones );
	for(UINT i=0;i<NumberOfBones;++i)
	{
		file.Read( bones[i].Name );
		file.Read( bones[i].ParentBoneIndex );
		file.Read( bones[i].BindT );
		file.Read( bones[i].BindR );
		file.Read( bones[i].ModelToBoneSpaceT );
		file.Read( bones[i].ModelToBoneSpaceR );
	}
}

void Skel::Initialize()
{
	for(UINT i=0;i<bones.size();++i)
	{
		Bone& bone = bones[i];
		bone.BoneIndex = i;

		if( bone.ParentBoneIndex != -1 )
			bones[ bone.ParentBoneIndex ].Children.push_back( &bone );
		else
			RootBones.push_back( &bone );

		bone.ModelToBoneSpace = BuildTransform(bone.ModelToBoneSpaceT , bone.ModelToBoneSpaceR);
		bone.BindTransform = BuildTransform(bone.BindT , bone.BindR);

	}
}

XMFLOAT4X4& Skel::GetModelToBoneSpaceTransform(int boneIndex)
{
	return bones[boneIndex].ModelToBoneSpace;
}

void Skel::ProcessAnimationGraph(float time,MatrixBuffer& buffer,Animation& anim,TrackBuffer& trackData)
{

	const bool UseRecursive = false;
	//Why is this not #defined out? That way compile errors will not be missed.
	//Both methods are provided as examples.
	
	if( UseRecursive )
	{	
		//Recursive Form
		XMMATRIX transform = XMMatrixIdentity();
		//D3DXMatrixIdentity( &transform );
		for( UINT i=0;i<RootBones.size();++i)
		{
			XMFLOAT4X4 temp;
			XMStoreFloat4x4(&temp, transform);
			RecursiveProcess( time , *RootBones[i] , anim , buffer , trackData , temp );
		}
	}
	else
	{
		//Linear Form This only works because the bone parent is always guaranteed to be in front
		//of its children (breath first order)
		XMMATRIX identity = XMMatrixIdentity();
		//D3DXMatrixIdentity( &identity );
		for( UINT boneIndex=0;boneIndex<bones.size();++boneIndex)
		{
			Bone& bone = bones[boneIndex];
			AnimTransform atx;
			anim.CalculateTransform( time , boneIndex , atx , trackData[boneIndex] );
			XMMATRIX parentTransform;
			if (bone.ParentBoneIndex != -1)
			{
				parentTransform = XMLoadFloat4x4(&buffer[bone.ParentBoneIndex]);
			}
			else
			{
				parentTransform = identity;
			}
			//XMMATRIX parentTransform = bone.ParentBoneIndex != -1 ?  temp :  identity;
			XMMATRIX localTransform = XMLoadFloat4x4(&atx.GetMatrix());
			XMMATRIX modelTransform = localTransform  * parentTransform;	
			XMStoreFloat4x4(&buffer[boneIndex], modelTransform);
		}
	}
}

void Skel::ProcessBindPose(std::vector<XMFLOAT4X4>& buffer)
{
	for(UINT i=0;i<bones.size();++i)
	{
		buffer[i] = BuildTransform( bones[i].BindT , bones[i].BindR );
		//buffer[i] = BuildTransform( bones[i].BindT , bones[i].BindR );
	}
}

void Skel::RecursiveProcess(float time,Bone& bone,Animation& anim,MatrixBuffer& buffer,TrackBuffer& trackData,XMFLOAT4X4& parentTransform)
{
	AnimTransform atx;

	anim.CalculateTransform( time , bone.BoneIndex , atx , trackData[bone.BoneIndex] );	

	XMMATRIX localTransform = XMLoadFloat4x4(&atx.GetMatrix());
	XMMATRIX temp = XMLoadFloat4x4(&parentTransform);
	XMMATRIX modelTransform = localTransform  * temp;	

	XMStoreFloat4x4(&buffer[bone.BoneIndex], modelTransform);

	for(UINT i=0;i<bone.Children.size();++i)
	{
		XMFLOAT4X4 temp2; 
		XMStoreFloat4x4(&temp2, modelTransform);  
		RecursiveProcess( time , *bone.Children[i] , anim , buffer , trackData , temp2 );
	}
}
