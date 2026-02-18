#if !defined( I3L_BINARY_SHADER)
#include "i3PostProcess_VS.hlsl"
#endif

//////////////////////////////////////////////////////
struct VS_OUTPUT_BIT
{
    float4  oPos            : POSITION;
    float2  oTex0           : TEXCOORD0;
};

sampler2D		g_texInputBase;
sampler2D		g_texInputBlend;
float4			g_blend;

half4 PS_Def(	in float2 iTex0 : TEXCOORD0) : COLOR0
{	
	float4 o1, o2;
	float a;

	o1 = tex2D( g_texInputBase, iTex0);
	o2 = tex2D( g_texInputBlend, iTex0);

	o2.rgb *= g_blend.rgb;
	a = g_blend.a;

	float3 blend = (o1.rgb * (1 - a)) + (o2.rgb * a);

	return float4( blend, 1.0f);
}
