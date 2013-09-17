
texture texture0;
float4x4 worldViewProj : WorldViewProjection;
float4x4 world : World;
float3 lightDir = float3(0,0,-1);
static const int MaxMatrices = 80;
float4x3   WorldMatrixArray[MaxMatrices] : WORLDMATRIXARRAY;

sampler Sampler
{
    Texture   = texture0;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;            
};

struct VS_INPUT
{
	float3 position	: POSITION;
	float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;
};

struct VS_INPUT_SKIN
{
    float4 position	: POSITION;
    float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;
    float4 BlendWeights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
};


struct VS_OUTPUT
{
    float4 pos : POSITION;
	float2 tex0  : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

VS_OUTPUT SimpleVertex( VS_INPUT IN )
{
    VS_OUTPUT OUT;
	OUT.pos = mul( float4(IN.position, 1) , worldViewProj);
	OUT.normal = mul( float4(IN.normal, 0) , world );
	OUT.tex0 = IN.tex0;
	return OUT;
}

float4 SimplePixel( VS_OUTPUT IN ) : COLOR
{
	float3 normal = normalize(IN.normal);	
	float NL = saturate(dot( normal , lightDir.xyz));
	float4 texColor = tex2D( Sampler , IN.tex0 );
	return NL * texColor;
}

VS_OUTPUT SkinVertex( VS_INPUT_SKIN IN , uniform int NumBones )
{
    VS_OUTPUT OUT;
    
    float3 Pos = float3(0,0,0);
    float3 Normal = float3(0,0,0);	
   
    float BlendWeightsArray[4] = (float[4])IN.BlendWeights;
    int   IndexArray[4]        = (int[4])IN.BlendIndices;
	    
	for (int iBone = 0; iBone < NumBones ; iBone++)
    {       
        Pos += mul( IN.position , WorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(IN.normal , WorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
        
	Normal = normalize(Normal);
	OUT.pos = mul( float4( Pos , 1.0f) , worldViewProj);	
	OUT.normal = mul( float4(Normal, 0) , world );	
	OUT.tex0 = IN.tex0;
	return OUT;
}

technique Technique0
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 SimpleVertex();
        PixelShader  = compile ps_2_0 SimplePixel();
        ZEnable = true;
        AlphaBlendEnable = false;
    }
}

technique TechniqueSkin
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 SkinVertex( 4 );
        PixelShader  = compile ps_2_0 SimplePixel();
        ZEnable = true;
        AlphaBlendEnable = false;
    }
}
