/*///====================================================================================
All content © 2004 DigiPen (USA) Corporation, all rights reserved.

File: sample.fx
Author: Max Wagner
Last Modified: May 26, 2004

Purpose:
Skin and bones vertex shader
/*///====================================================================================

#define VTX_POS				v0
#define VTX_NORM			v1
#define VTX_UV				v2
#define VTX_WEIGHT			v3
#define VTX_IDS				v4

#define VECTOR_012HALF		c0
#define MAT_VIEWPROJ		c1
#define LIGHT0_DIR			c5
#define LIGHT0_AMBIENT		c6
#define LIGHT0_DIFFUSE		c7

#define BASE_WORLDTM_REGISTER	17

#define ZERO				VECTOR_012HALF.x
#define ONE					VECTOR_012HALF.y
#define TWO					VECTOR_012HALF.z
#define HALF				VECTOR_012HALF.w
#define CONSTANTS_PER_TM	TWO
#define DYNAMIC_OFFSET		a0.x
#define DYNAMIC_XFORM		c[a0.x + BASE_WORLDTM_REGISTER]

#define FINAL_ROTATION		r10
#define FINAL_TRANS			r11

#define Lerp(dest, a, b, t)	\
	mul a, a, t				\
	add dest, ONE, -t		\
	mad dest, b, dest, a
	
#define Cross(dest, a, b)			\
	mul dest, a.yzx, b.zxy			\
	mad dest, -a.zxy, b.yzx, dest

#define Rot(dest, q, p, tmp0, tmp1, tmp2)			\
	mul tmp1.x, q.w, q.w							\
	dp3 tmp2.x, q.xyz, q.xyz						\
	add tmp1.x, tmp1.x, -tmp2.x						\
	mul tmp0.xyz, tmp1.x, p.xyz						\
	dp3 tmp2.x, p.xyz, q.xyz						\
	mul tmp1.xyz, tmp2.x, q.xyz						\
	Cross(tmp2.xyz, q, p)							\
	mul tmp2.xyz, q.w, tmp2.xyz						\
	add tmp1.xyz, tmp1.xyz, tmp2.xyz				\
	mad dest, TWO, tmp1.xyz, tmp0.xyz
	
#define QuatToMat(m0, m1, m2, q, tmp0, tmp1, tmp2, tmp3)					\
	add tmp0, q, q						/*tmp0: 2x, 2y, 2z, 2w*/			\
	mul tmp1, tmp0, q.xyz				/*tmp1: 2xx, 2yy, 2zz*/				\
	mul tmp2, tmp0.www, q.xyz			/*tmp2: 2wx, 2wy, 2wz*/				\
	add tmp3, tmp1.yxx, tmp1.zzy		/*tmp3: 2yy+2zz, 2xx+2zz, 2xx+2yy*/	\
	add m0.x, ONE, -tmp3.x				/*m00: 1 - (2yy+2zz)*/				\
	add m1.y, ONE, -tmp3.y				/*m11: 1 - (2xx+2zz)*/				\
	add m2.z, ONE, -tmp3.z				/*m22: 1 - (2xx+2yy)*/				\
	mad m0.y, tmp0.x, q.y, -tmp2.z		/*m01: 2xy - 2wz*/					\
	mad m0.z, tmp0.x, q.z,  tmp2.y		/*m02: 2xz + 2wy*/					\
	mad m1.x, tmp0.x, q.y,  tmp2.z		/*m10: 2xy + 2wz*/					\
	mad m1.z, tmp0.y, q.z, -tmp2.x		/*m12: 2yz - 2wx*/					\
	mad m2.x, tmp0.x, q.z, -tmp2.y		/*m20: 2xz - 2wy*/					\
	mad m2.y, tmp0.y, q.z,  tmp2.x		/*m21: 2yz + 2wx*/
	
	
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
	def VECTOR_012HALF,0,1,2,0.5		// ones vector

	/*///=======================================================================
	 Vertex blending
	/*///=======================================================================

	// get first rotation (quat[index 0] * weight)
	mul r0.x, VTX_IDS.x, CONSTANTS_PER_TM
	mov DYNAMIC_OFFSET, r0.x
	mul r0.xyzw, DYNAMIC_XFORM, VTX_WEIGHT.x

	// add in second rotation (quat[index 1] * (1-weight))
	mul r1.x, VTX_IDS.y, CONSTANTS_PER_TM
	mov DYNAMIC_OFFSET, r1.x
	add r1.x, ONE, -VTX_WEIGHT.x
	mad FINAL_ROTATION, DYNAMIC_XFORM, r1.x, r0
	
	// and renormalize
	dp4 r0.x, FINAL_ROTATION, FINAL_ROTATION
	rsq r0.x, r0.x
	mul FINAL_ROTATION, FINAL_ROTATION, r0.x

	// get first translation (trans[index 0] * weight)
	mad r0.x, VTX_IDS.x, CONSTANTS_PER_TM, ONE
	mov DYNAMIC_OFFSET, r0.x
	mul r0.xyz, DYNAMIC_XFORM, VTX_WEIGHT.x

	// add in second translation (trans[index 1] * (1-weight))
	mad r1.x, VTX_IDS.y, CONSTANTS_PER_TM, ONE
	mov DYNAMIC_OFFSET, r1.x
	add r1.x, ONE, -VTX_WEIGHT.x
	mad FINAL_TRANS.xyz, DYNAMIC_XFORM, r1.x, r0.xyz
	
	// convert quat to 3x3 rotation matrix
	QuatToMat(r0, r1, r2, FINAL_ROTATION, r3, r4, r5, r6)
	
	// rotate vtx
	m3x3 r3.xyz, VTX_POS, r0

	// translate vtx
	add r3.xyz, r3.xyz, FINAL_TRANS
	
	// ensure we have 1 in the w component of final position
	mov r3.w, ONE
	// Transform to projection space and output to Position register
	m4x4 oPos.xyzw, r3.xyzw, MAT_VIEWPROJ

	/*///=======================================================================
	 Lighting calculation
	/*///=======================================================================

	m3x3 r4.xyz, VTX_NORM, r0
	mov r4.w, ZERO

	// lighting calculation for diffuse and ambient term
	dp3 r1.x, r4, LIGHT0_DIR    		// r1 = normal dot light
	max r1.x, r1.x, ZERO				// if dot < 0 then dot = 0
	mul r0, r1.x, LIGHT0_DIFFUSE    	// Multiply with diffuse
	add r0, r0, LIGHT0_AMBIENT			// Add in ambient
	min oD0.xyz, r0, ONE				// clamp if > 1
	
	// just zero out specular register
	mov oD1.xyzw, ZERO

	/*///=======================================================================
	 Texture coordinates
	/*///=======================================================================

	// Just copy the texture coordinates
	mov oT0.xy, VTX_UV.xy

}; // end VertexShader asm body

#define VTX_DIFFUSE			v0
#define VTX_SPECULAR		v1

PixelShader SkinAndBonesPS = asm
{
	/*///=======================================================================
	 Shader Version
	/*///=======================================================================
	ps_1_1

	/*///=======================================================================
	 sample texture, combine with diffuse, and output
	/*///=======================================================================
	tex t0
	mul r0,t0,VTX_DIFFUSE
	
}; // end PixelShader asm body


technique SkinAndBonesTechnique
{
	pass P0
	{
		VertexShader = (SkinAndBonesVS);
		PixelShader = (SkinAndBonesPS);
	}
}



