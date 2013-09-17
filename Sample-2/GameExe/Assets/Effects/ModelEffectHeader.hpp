
// vertex components for Vertex Shader
#define VTX_POS					v0
#define VTX_NORM				v1
#define VTX_UV					v2
#define VTX_WEIGHT				v3
#define VTX_IDS					v4

// vertex components for Pixel Shader
#define VTX_DIFFUSE				v0
#define VTX_SPECULAR			v1

// constants supplied by the app thru 
// device->SetVertexShaderConstantF()
#define VECTOR_0123				c0
#define MAT_VIEWPROJ			c1
#define AMBIENT_COLOR			c5
#define LIGHT0_VECTOR			c6
#define LIGHT0_DIFFUSE			c7
#define LIGHT0_SPECULAR			c8
#define LIGHT1_VECTOR			c9
#define LIGHT1_DIFFUSE			c10
#define LIGHT1_SPECULAR			c11
#define EYE						c12

// useful constant defines
#define BASE_WORLDTM_REGISTER	17
#define ZERO					VECTOR_0123.x
#define ONE						VECTOR_0123.y
#define TWO						VECTOR_0123.z
#define THREE					VECTOR_0123.w
#define CONSTANTS_PER_TM		THREE
#define DYNAMIC_OFFSET			a0.x
#define GET_DYNAMIC_TM			c[DYNAMIC_OFFSET + BASE_WORLDTM_REGISTER]
#define FINAL_NORM				r10
#define FINAL_POS				r11

// utility macros/functions...

#define Normalize(vec, tmp)		\
	dp3 tmp.x, vec, vec			\
	rsq tmp.x, tmp.x			\
	mul vec, vec, tmp.x

#define Lerp(dest, a, b, t)	\
	mul a, a, t				\
	add dest, ONE, -t		\
	mad dest, b, dest, a	

#define Cross(dest, a, b)			\
	mul dest, a.yzx, b.zxy			\
	mad dest, -a.zxy, b.yzx, dest

#define CalcLight(outD, outS, pos, norm, lightVec, lightDif, lightSpec, view, dir, halfWay, tmp0)	\
	mad dir, -pos, lightVec.w, lightVec			/* L = Lpos - Vpos*Lpos.w */						\
	add halfWay, dir, view						/* halfWay = L + V */								\
	Normalize(dir, tmp0)						/* normalize direction vector */					\
	Normalize(halfWay, tmp0)					/* normalize halfWay vector */						\
	dp3 tmp0.x, norm, dir						/* N*L */											\
	dp3 tmp0.y, norm, halfWay					/* N*H */											\
	mov tmp0.w, lightSpec.w						/* Specular Exponent */								\
	lit tmp0, tmp0								/* clamp N*L, compute (N*H)^ns*/					\
	mad outD, tmp0.y, lightDif, outD			/* color = diffuse * N*L */							\
	mad outS, tmp0.z, lightSpec, outS			/* color += specular * (N*H)^ns */

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

