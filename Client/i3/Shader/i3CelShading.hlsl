#if !defined( I3L_BINARY_SHADER)
#include "i3PostProcess_VS.hlsl"
#endif

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler2D	g_texInput;

float4 PS_Def(	in float2 iTex0 : TEXCOORD0) : COLOR0 
{	
	float4 color = tex2D(g_texInput, iTex0);
	float lum = GetLuminance_Linear(color.rgb);

	float ratio = 0.0f;
	float4 thres = { 1.0f, 0.9f, 0.8f, 0.6f};

	if (lum > 0.8f)
        ratio = thres.x;
    else if (lum > 0.3f)
        ratio = thres.y;
    else if (lum > 0.05f)
        ratio = thres.z;
    else
        ratio = thres.w;

	color = ratio * color;

	return color;
}
