/*///====================================================================================
All content © 2004 DigiPen (USA) Corporation, all rights reserved.

File: StaticMesh.fx
Author: Max Wagner
Last Modified: May 26, 2004

Purpose:
Skin and bones vertex shader
/*///====================================================================================

#include "ModelEffectHeader.hpp"

VertexShader StaticMeshVS = asm
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

	/*///=======================================================================
	 Constants
	/*///=======================================================================
	def VECTOR_0123,0,1,2,3		// ones vector

	/*///=======================================================================
	 Vertex blending
	/*///=======================================================================

	// Transform position for world0 matrix...

	// Transform position to world
	m4x3 FINAL_POS.xyz, VTX_POS, c[BASE_WORLDTM_REGISTER]
	mov FINAL_POS.w, ONE

	// Transform to projection space
	// and output to Position register
	m4x4 oPos.xyzw, FINAL_POS.xyzw, MAT_VIEWPROJ

	/*///=======================================================================
	 Lighting calculation
	/*///=======================================================================

	// Transform normal vector
	m3x3 FINAL_NORM.xyz, VTX_NORM, c[BASE_WORLDTM_REGISTER]
	mov FINAL_NORM.w, ZERO

	// now perform lighting calculation...
	#include "ModelEffectLighting.hpp"

	/*///=======================================================================
	 Texture coordinates
	/*///=======================================================================

	// Just copy the texture coordinates
	mov oT0.xy, VTX_UV.xy

}; // end VertexShader asm body

PixelShader StaticMeshPS = asm
{
	#include "Model1TexturePixelShader.hpp"
}; // end PixelShader asm body


technique StaticMeshTechnique
{
	pass P0
	{
		VertexShader = (StaticMeshVS);
		PixelShader = (StaticMeshPS);
	}
}



