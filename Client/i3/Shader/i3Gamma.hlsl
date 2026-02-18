#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

//-----------------------------------------------------------------------------
// external values.
//-----------------------------------------------------------------------------
float3	g_fGammaRGB;

//-----------------------------------------------------------------------------
// Texture samplers.
//-----------------------------------------------------------------------------
sampler2D	g_texScene;

//-----------------------------------------------------------------------------
// vertex.
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4  Pos            : POSITION;
    float2  Tex0           : TEXCOORD0;
};

VS_OUTPUT VS_Def( in float4 iPos : POSITION,
							 in float2 iTex : TEXCOORD0)
{
	VS_OUTPUT vs_out;		

	vs_out.Pos = iPos;
	vs_out.Tex0 = iTex;

	return vs_out;
}

//-----------------------------------------------------------------------------
// pixel.
//-----------------------------------------------------------------------------
struct PS_INPUT
{
    float2  Tex0           : TEXCOORD0;
};

float4 PS_Def( PS_INPUT ps_in ) : COLOR0
{
	float3 color = tex2D(g_texScene, ps_in.Tex0).rgb;

	color = pow(color, 1.0 / g_fGammaRGB);

	return float4(color.rgb, 1.0);
}
