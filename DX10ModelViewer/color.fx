cbuffer PerObject
{
	float4x4 WorldViewProj;
	float4x4 World;
	float3 lightDir = float3(0,0,-1);
	static const int MaxMatrices = 80;
	float4x3 WorldMatrixArray[MaxMatrices];
};

Texture2D textureMap;

SamplerState sam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Norm  : NORMAL;
	float2 Tex   : TEXTURE;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 Tex   : TEX0;
	float3 Norm  : TEX1;
};

struct VertexInSkin
{
	float4 PosL         : POSITION;
	float3 Norm         : NORMAL;
	float2 Tex          : TEXTURE;
	float4 BlendWeights : BLENDWEIGHT;
	int4 BlendIndices   : BLENDINDICES;
};

VertexOut VS(VertexIn vertexIn)
{
	VertexOut vertexOut;
	
	// Transform to homogeneous clip space.
	vertexOut.PosH = mul(float4(vertexIn.PosL, 1.0f), WorldViewProj);
	
	vertexOut.Norm = mul(float4(vertexIn.Norm, 0.0f), World).xyz;
	
	vertexOut.Tex = vertexIn.Tex;
    
    return vertexOut;
}

float4 PS(VertexOut vertexOut) : SV_Target
{
	float3 normal = normalize(vertexOut.Norm);
	float NL = saturate(dot( normal , lightDir.xyz));
	float4 texColor = float4(1, 1, 1, 1);
	texColor = textureMap.Sample(sam, vertexOut.Tex);
	//return float4(1, 1, 1, 1);
	return NL * texColor;
}

VertexOut SkinVertex(VertexInSkin vis, uniform int NumBones)
{
	VertexOut vertexOut;

	float3 Pos = float3(0, 0, 0);
	float3 Normal = float3(0, 0, 0);

	float BlendWeightsArray[4] = (float[4])vis.BlendWeights;
	int IndexArray[4] = (int[4])vis.BlendIndices;

	for (int iBone = 0; iBone < NumBones; iBone++)
	{
		Pos += mul(vis.PosL, WorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
		Normal += mul(float4(vis.Norm, 0.0f), WorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
	}

	Normal = normalize(Normal);
	vertexOut.PosH = mul(float4(Pos, 1.0f), WorldViewProj);
	vertexOut.Norm = mul(float4(Normal, 0.0f), World).xyz;
	vertexOut.Tex = vis.Tex;
	return vertexOut;
}

technique10 ColorTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

technique10 SkinTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, SkinVertex(4)));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}