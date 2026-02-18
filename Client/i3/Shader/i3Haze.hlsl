#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

sampler2D		g_texInputBase;
sampler2D		g_texInputBlend;
float2			g_UV;
float 			g_WaveVolume;

//////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4  oPos            : POSITION;
    float2  oTex0           : TEXCOORD0;
	float2  oTex1           : TEXCOORD1;
};

VS_OUTPUT VS_Def( in float4 iPos : POSITION,
							 in float2 iTex : TEXCOORD0)
{
	VS_OUTPUT o;
		
	o.oPos		= iPos;
	o.oTex0		= iTex;
	o.oTex1 	= iTex + g_UV;
	return o;
}

///////////////////////////////////////////////////////////
struct PS_INPUT
{
    float2  iTex0           : TEXCOORD0;
	float2  iTex1           : TEXCOORD1;
};

float4 PS_Def( PS_INPUT input ) : COLOR0
{	
	float4	BlendColor = tex2D(g_texInputBlend, input.iTex1);

	float	x = input.iTex0.x + (BlendColor.r-0.5) * g_WaveVolume;
	float	y = input.iTex0.y + (BlendColor.g-0.5) * g_WaveVolume;

	return tex2D(g_texInputBase, float2(x, y));
}
