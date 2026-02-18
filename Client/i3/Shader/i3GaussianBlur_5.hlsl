#if !defined( I3L_BINARY_SHADER)
#include "i3PostProcess_VS.hlsl"
#endif

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler2D	g_texInput;

#define SAMPLE_MAX_COUNT 5

float3 g_OffsetWeight[SAMPLE_MAX_COUNT];	// ( OffX, OffY, Weight)

float4 PS_Def(	in float2 iTex0 : TEXCOORD0) : COLOR0 
{	
	float3 c = 0;
	float2 offset;
	float weight;

	// Combine a number of weighted image filter taps.
	for (int i = 0; i < SAMPLE_MAX_COUNT; i++)
	{
		offset = g_OffsetWeight[i].xy;
		weight = g_OffsetWeight[i].z;

		c += tex2D(g_texInput, iTex0 + offset).rgb * weight;
	}

	return float4( c, 1);
}
