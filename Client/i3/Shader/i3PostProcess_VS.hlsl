#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

//////////////////////////////////////////////////////
struct VS_OUTPUT_POSTPROCESS
{
    float4  oPos            : POSITION;
    float2  oTex0           : TEXCOORD0;
};

VS_OUTPUT_POSTPROCESS VS_Def( in float4 iPos : POSITION,
							   in float2 iTex : TEXCOORD0)
{
	VS_OUTPUT_POSTPROCESS o;
		
	o.oPos = iPos;
	//o.oPos.xy -= g_ScreenSize.zw * float2( 0.5, -0.5);
	o.oTex0	 = iTex + (g_ScreenSize.zw * 0.5);
	
	return o;
}
