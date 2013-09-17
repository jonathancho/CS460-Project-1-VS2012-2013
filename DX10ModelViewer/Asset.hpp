///////////////////////////////////////////////////////////////////////////////////////
//
//	Asset.hpp
//	Base asset class provides AddRef/Release semantics for all assets classes.	
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

//Base asset class provides AddRef/Release semantics for all assets.
class Asset
{
public:
	Asset() : ReferenceCount(1) {};
	virtual void AddRef();
	virtual int Release();
	virtual ~Asset(){};
private:
	int ReferenceCount;
};

inline void Asset::AddRef()
{
	++ReferenceCount;
}

inline int Asset::Release()
{
	--ReferenceCount;
	if( ReferenceCount == 0 )
	{
		delete this;
		return 0;
	}
	return ReferenceCount;
};

template< typename RefType >
void SafeRelease( RefType& interfacePtr )
{
	if( interfacePtr ) interfacePtr->Release();
	interfacePtr = NULL;
}

template< typename RefType >
void SafeDelete( RefType& objectPtr )
{
	if( objectPtr ) delete objectPtr;
	objectPtr = NULL;
}