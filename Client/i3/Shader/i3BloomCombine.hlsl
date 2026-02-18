#if !defined( I3L_BINARY_SHADER)
#include "i3PostProcess_VS.hlsl"
#endif

float2 g_Intensity;			// ( BaseIntensity, BloomIntensity )

//const float	g_BaseIntensity = g_Intensity.x;
//const float g_BloomIntensity = g_Intensity.y;

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------

sampler2D	g_texOriginal;
sampler2D	g_texBloom;

float4 PS_Def(	in float2 iTex0 : TEXCOORD0) : COLOR0 
{	
	float4 bloomColor = tex2D(g_texBloom, iTex0);
	float4 baseColor = tex2D(g_texOriginal, iTex0);

	// Adjust color saturation and intensity.
	bloomColor.rgb = bloomColor.rgb * g_Intensity.y;
	baseColor.rgb = baseColor.rgb * g_Intensity.x;

	baseColor.rgb += bloomColor.rgb;

	// Combine the two images.
	return baseColor;
}
