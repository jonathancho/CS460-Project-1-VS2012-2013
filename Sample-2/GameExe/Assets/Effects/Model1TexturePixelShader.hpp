
/*///=======================================================================
	Shader Version
/*///=======================================================================
ps_1_1

/*///=======================================================================
	sample texture, combine with diffuse, specular, and output
/*///=======================================================================
tex t0
mul r0, t0, VTX_DIFFUSE
add r0, r0, VTX_SPECULAR
