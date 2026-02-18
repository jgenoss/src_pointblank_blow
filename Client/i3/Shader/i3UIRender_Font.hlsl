#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

struct VS_INPUT_UI
{
	float3		Pos			: POSITION;
	float2		VtxTex		: TEXCOORD0;
	float2		TexRange	: TEXCOORD1;
	float4		Color		: COLOR;
};

//////////////////////////////////////////////////////
struct VS_OUTPUT_UI
{
    float4  oPos            : POSITION;
	float2	oVtxTex			: TEXCOORD0;
	float2	oTexRange		: TEXCOORD1;
	float4	oColor			: COLOR;
};

float4x4		g_mUIProj;

sampler2D		g_texFont0;

float4			g_vFontFactor;

//////////////////////////////////////////////////////////////////////
	
VS_OUTPUT_UI VS_Def( VS_INPUT_UI i)
{
	VS_OUTPUT_UI o;

	o.oPos		= mul( float4( i.Pos, 1), g_mUIProj);
	o.oVtxTex	= i.VtxTex;
	o.oTexRange	= i.TexRange;
	o.oColor	= i.Color;

	return o;
}


float4 PS_Def(in float2 VtxTex : TEXCOORD0,
			  in float2 TexRange : TEXCOORD1,
			  in float4	Color : COLOR) : COLOR0
{
	float2 uv;
	float4 o;

	uv = VtxTex;

	half3 c = tex2D( g_texFont0, uv);

	half fontA = c.r + TexRange.x;		// (TexRange.z는 Texture Mapping을 사용하지 않는 Caret의 경우를 위해 사용된다.)
	half fontC = 1;

	if( TexRange.y == 2)
	{
		// rounding border
		fontA = saturate( fontA + c.b);
		fontC = 1 - c.b;
	}
	else if( TexRange.y == 1)
	{
		// right-bottom shadow
		fontA = saturate( fontA + c.g);
		fontC = 1 - c.g;
	}

	half3 tempC = fontC * Color.rgb;

	o.rgb = lerp( 0.5, tempC, fontA);

	o.a = fontA * Color.a;

	return o;
}