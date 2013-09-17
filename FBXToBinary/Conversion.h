///////////////////////////////////////////////////////////////////////////////
//
//	Conversion.h
//	Converts homgeneous transform matrices between coordinate systems.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

class Converter
{
public:	
	Converter(KFbxScene* pScene);
	void ConvertMatrix( KFbxXMatrix& sourceMatX);
	void ConvertMeshMatrix( KFbxXMatrix& m);
	KFbxMatrix ConversionMatrix;
	bool NeedToChangedWinding;
};
