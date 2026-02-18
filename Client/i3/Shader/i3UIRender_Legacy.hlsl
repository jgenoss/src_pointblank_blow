#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

#define		I3_VT_MAX		32768
#define		PIXEL			(1.0 / I3_VT_MAX)

struct VS_INPUT_UI
{
	float3		Pos			: POSITION;
	float2		VtxTex		: TEXCOORD0;
	float4		Color		: COLOR;
};

//////////////////////////////////////////////////////
struct VS_OUTPUT_UI
{
    float4  oPos            : POSITION;
	float2	oVtxTex			: TEXCOORD0;
	float4	oColor			: COLOR;
};

float4x4		g_mUIProj;

sampler2D		g_texCache;

//////////////////////////////////////////////////////////////////////
	
VS_OUTPUT_UI VS_Def( VS_INPUT_UI i)
{
	VS_OUTPUT_UI o;
	
	o.oPos		= mul( float4( i.Pos, 1), g_mUIProj);
	o.oVtxTex	= i.VtxTex;
	o.oColor	= i.Color;

	return o;
}

float4 PS_Def(in float2 VtxTex : TEXCOORD0,
			  in float4	Color : COLOR) : COLOR0
{
	float4 o;

	o = tex2D( g_texCache, VtxTex) * Color;

	return o;
}