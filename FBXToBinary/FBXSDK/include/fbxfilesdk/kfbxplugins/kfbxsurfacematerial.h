/*!  \file kfbxsurfacematerial.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSURFACEMATERIAL_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSURFACEMATERIAL_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/

#include <fbxfilesdk/components/kbaselib/kaydaradef_h.h>
#include <fbxfilesdk/components/kbaselib/kaydara.h>

#include <fbxfilesdk/kfbxplugins/kfbxtakenodecontainer.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

// FBX includes
#include <fbxfilesdk/fbx3d/fbxshaders/kfbxshadingobject.h>

// FBX namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Material settings.
  * \nosubgrouping
  * A material is attached to an instance of class KFbxGeometry
  * by calling KFbxGeometry::AddMaterial(). Materials can be shared among 
  * many instances of class KFbxGeometry.
  */
class KFBX_DLL KFbxSurfaceMaterial : public KFbxShadingObject
{
	KFBXOBJECT_DECLARE(KFbxSurfaceMaterial, KFbxShadingObject);

public:
	/**
	  * \name Standard Material Property Names
	  */
	//@{	

	static char const* sShadingModel;
	static char const* sMultiLayer;
	
	static char const* sEmissive;
	static char const* sEmissiveFactor;
	
	static char const* sAmbient;
	static char const* sAmbientFactor;
	
	static char const* sDiffuse;
	static char const* sDiffuseFactor;
	
	static char const* sSpecular;
	static char const* sSpecularFactor;
	static char const* sShininess;
	
	static char const* sBump;
	static char const* sNormalMap;
    static char const* sBumpFactor;

	static char const* sTransparentColor;
	static char const* sTransparencyFactor;
	
	static char const* sReflection;
	static char const* sReflectionFactor;

    static char const* sDisplacementColor;
    static char const* sDisplacementFactor;

    
	//static char const* 
	//@}	

	/**
	  * \name Material Properties
	  */
	//@{	

	//! Reset the material to default values.
	void Reset();
	
	/** Get material shading model.
	  * \return The shading model type string.
	  */
	KFbxPropertyString GetShadingModel() const;

	/**	Get multilayer state.
	  * \return The state of the multi-layer flag.
	  */
	KFbxPropertyBool1 GetMultiLayer() const;
	
	//@}	

	//////////////////////////////////////////////////////////////////////////
	// Static values
	//////////////////////////////////////////////////////////////////////////

	// Default property values
	static fbxBool1		sMultiLayerDefault;
	static char const*	sShadingModelDefault;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

	// Clone
	virtual KFbxObject* Clone(KFbxObject* pContainer, KFbxObject::ECloneType pCloneType = eDEEP_CLONE) const;

protected:
	bool SetColorParameter(KFbxProperty pProperty, KFbxColor const& pColor);
	bool GetColorParameter(KFbxProperty pProperty, KFbxColor& pColor) const;
	bool SetDoubleParameter(KFbxProperty pProperty, double pDouble);
	bool GetDoubleParameter(KFbxProperty pProperty, double pDouble) const;
	
	KFbxSurfaceMaterial(KFbxSdkManager& pManager, char const* pName);

	virtual bool ConstructProperties(bool pForceSet);

	//! Assignment operator.
    KFbxSurfaceMaterial& operator=(KFbxSurfaceMaterial const& pMaterial);

public:
	// Comparison operator
	bool operator==(KFbxSurfaceMaterial const& pMaterial) const;

	// From KFbxObject
	virtual KString		GetTypeName() const;
	virtual KStringList	GetTypeFlags() const;

protected:
	KFbxPropertyString ShadingModel;
	KFbxPropertyBool1 MultiLayer;

	friend class KFbxLayerContainer;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

typedef KFbxSurfaceMaterial* HKFbxSurfaceMaterial;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSURFACEMATERIAL_H

