#if !defined( I3L_BINARY_SHADER)
#include "i3PostProcess_VS.hlsl"
#endif

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------

sampler2D	g_texInput;
float	g_threshold;

float4 PS_Def(	in float2 iTex0 : TEXCOORD0) : COLOR0 
{	
	// Look up the original image color.
    float4 c = tex2D(g_texInput, iTex0);

	float lum = max( c.r, max( c.g, c.b));

	c.rgb *= max((lum - g_threshold) / (1.0f - g_threshold), 0);

	return c;
}
