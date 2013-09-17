/*///====================================================================================
All content © 2004 DigiPen (USA) Corporation, all rights reserved.

File: sample.fx
Author: Max Wagner
Last Modified: May 26, 2004

Purpose:
Skin and bones vertex shader
/*///====================================================================================

#include "ModelEffectHeader.hpp"

VertexShader SkinAndBonesVS = asm
{
	/*///=======================================================================
	 Shader Version
	/*///=======================================================================
	vs_1_1

	/*///=======================================================================
	 Vertex Declaration
	/*///=======================================================================
	dcl_position VTX_POS
	dcl_normal VTX_NORM
	dcl_texcoord VTX_UV
	dcl_blendweight VTX_WEIGHT
	dcl_blendindices VTX_IDS

	/*///=======================================================================
	 Constants
	/*///=======================================================================
	def VECTOR_0123,0,1,2,3		// ones vector

	/*///=======================================================================
	 Vertex blending
	/*///=======================================================================

	// Transform position for world0 matrix...
	mul r0.x, VTX_IDS.x, CONSTANTS_PER_TM
	mov DYNAMIC_OFFSET, r0.x
	m4x3 r0.xyz, VTX_POS, GET_DYNAMIC_TM

	// Transform position for world1 matrix...
	mul r1.x, VTX_IDS.y, CONSTANTS_PER_TM
	mov DYNAMIC_OFFSET, r1.x
	m4x3 r1.xyz, VTX_POS, GET_DYNAMIC_TM

	// Lerp the two positions r0 and r1 into r2
	Lerp(FINAL_POS.xyz, r0.xyz, r1.xyz, VTX_WEIGHT.x)
	mov FINAL_POS.w, ONE

	// Transform to projection space
	// and output to Position register
	m4x4 oPos.xyzw, FINAL_POS.xyzw, MAT_VIEWPROJ

	/*///=======================================================================
	 Lighting calculation
	/*///=======================================================================

	// Transform normal for world0 matrix
	mul r0.x, VTX_IDS.x, CONSTANTS_PER_TM
	mov DYNAMIC_OFFSET, r0.x
	m3x3 r0.xyz, VTX_NORM, GET_DYNAMIC_TM

	// Transform normal for world1 matrix
	mul r1.x, VTX_IDS.y, CONSTANTS_PER_TM
	mov DYNAMIC_OFFSET, r1.x
	m3x3 r1.xyz, VTX_NORM, GET_DYNAMIC_TM

	// Lerp the two normals r0 and r1 into r2
	Lerp(FINAL_NORM.xyz, r0.xyz, r1.xyz, VTX_WEIGHT.x)
	mov FINAL_NORM.w, ZERO

	// now perform lighting calculation...
	#include "ModelEffectLighting.hpp"

	/*///=======================================================================
	 Texture coordinates
	/*///=======================================================================

	// Just copy the texture coordinates
	mov oT0.xy, VTX_UV.xy

}; // end VertexShader asm body

PixelShader SkinAndBonesPS = asm
{
	#include "Model1TexturePixelShader.hpp"
}; // end PixelShader asm body

technique SkinAndBonesTechnique
{
	pass P0
	{
		VertexShader = (SkinAndBonesVS);
		PixelShader = (SkinAndBonesPS);
	}
}



