

// lighting calculation...

// calc view
add r2, c12, -FINAL_POS		

// start by adding in ambient for diffuse output register
mov r0.xyzw, AMBIENT_COLOR	
// zero out specular register
mov r1.xyzw, ZERO			

// compute and add in diffuse and specular components for each light source
CalcLight(r0, r1, FINAL_POS, FINAL_NORM, LIGHT0_VECTOR, LIGHT0_DIFFUSE, LIGHT0_SPECULAR, r2, r3, r4, r5)	
CalcLight(r0, r1, FINAL_POS, FINAL_NORM, LIGHT1_VECTOR, LIGHT1_DIFFUSE, LIGHT1_SPECULAR, r2, r3, r4, r5)	

// clamp outputs
min oD0, r0, ONE			
min oD1, r1, ONE
