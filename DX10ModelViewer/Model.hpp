///////////////////////////////////////////////////////////////////////////////
//
//	Model.hpp
//	Simple model class which supports static and animated models. 
//
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Animation.hpp"
#include "Mesh.hpp"

enum ModelType
{
	Static,
	Skinned
};

class Model
{
public:
	Model();
	~Model();
	AnimationController * Controller;
	Mesh * mesh;
	ModelType modelType;
	void Draw(ID3D11Device * device, ID3DX11EffectTechnique** technique, ID3D11DeviceContext* deviceContext, ID3DX11Effect* effect);
};

Model * LoadModel(std::string filename,ID3D11Device * device, ID3DX11EffectTechnique** technique, ID3D11InputLayout** inputLayout);

