#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

#define		I3_VT_MAX		32768
#define		I3_VTTL_SZ		128
#define		PIXEL			(1.0 / I3_VT_MAX)

struct VS_INPUT_UI
{
	float3		Pos			: POSITION;
	float2		VtxTex		: TEXCOORD0;
	float3		LeftTop		: TEXCOORD1;
	float3		Size		: TEXCOORD2;
	float4		Tex0		: TEXCOORD3;
	float4		TexRange	: TEXCOORD4;
	float4		Color		: COLOR;
};

//////////////////////////////////////////////////////
struct VS_OUTPUT_UI
{
    float4  oPos            : POSITION;
	float2	oVtxTex			: TEXCOORD0;
	float4	oTex0			: TEXCOORD1;
	float4	oTexRange		: TEXCOORD2;
	float4	oColor			: COLOR;
};

float4x4		g_mUIProj;

sampler2D		g_texCache;
sampler2D		g_texRedirect;
sampler2D		g_texFont0;
sampler2D		g_texFont1;

float4			g_vFontFactor;
float4			g_vRedirFactor;			// Redirect texture		{ w, h, 1/w. 1/h }
float4			g_vCache;				// Cache Texture		{ w, h, 1/w, 1/h }
float4			g_vCacheTexel;			// Cache Texture		{ w, h, 1/w, 1/h } * Tile Size

//////////////////////////////////////////////////////////////////////
	
VS_OUTPUT_UI VS_Def( VS_INPUT_UI i)
{
	VS_OUTPUT_UI o;
	float4 pos;
		
	pos = float4( (i.Pos * i.Size) + i.LeftTop, 1);

	pos.xy -= 0.5f;

	o.oPos		= mul( pos, g_mUIProj);
	o.oVtxTex	= i.VtxTex;
	o.oTex0		= i.Tex0;
	o.oTexRange	= i.TexRange;
	o.oColor	= i.Color;

	return o;
}

float2	ResolveVTexCoord( in float2 Tex0)
{
	float2 txlCoord, txlFrac, txlInt;

	/* FP */	txlCoord	= Tex0 * g_vRedirFactor.xy;

	#if 0
		txlFrac		= frac( txlCoord) * g_vRedirFactor.zw;			// Cache Tile 내에서의 Offset 계산을 위해...
		txlInt		= Tex0 - txlFrac;								// 정수화한 좌표
	#else
		/* FP */	txlInt		= Tex0;
	#endif

	/* FP */	float2	texelOrigin = tex2D( g_texRedirect, txlInt).xy;

	float2	texelLocal = frac( txlCoord) * g_vCacheTexel.zw;				// Tile 내에서의 Offset 좌표

	// DirectX9의 Spec.에 따라 Shader 내에서의 Floating-Point 연산은 Half Precision으로 이루어진다.
	// 이 이유로 frac의 연산 결과 및 여러 multiply의 결과가 Tile의 Boundary를 벗어나는 경우가 있으며,
	// 실제 그 값이 벗어나지 않는 경우에도, 읽어 오는 Texel이 Tile 영역을 벗어나는 경우가 있다.
	// 이것을 회피하기 위해, 최종 계산된 'Tile내 에서의 Offset'인 texelLocal 좌표를
	// Tile 영역을 0.5 Pixel - deflate한 영역 안으로 clamping하는 과정을 거친다.
	float2 halfPixel = g_vCacheTexel.xy;
	texelLocal = clamp( texelLocal, halfPixel, g_vCacheTexel.zw - halfPixel);

	return texelOrigin + texelLocal;
}

float4 FetchVTex( in float2 VtxTex, in float4 TexScaleOffset, float2 offset)
{
	float2 uv, tex0;

	tex0 = (VtxTex * TexScaleOffset.xy) + TexScaleOffset.zw;
	uv = ResolveVTexCoord( tex0 + offset);

	return tex2D( g_texCache, uv);
}

float4 PS_Def(in float2 VtxTex : TEXCOORD0,
			  in float4	TexScaleOffset : TEXCOORD1,
			  in float4 TexRange : TEXCOORD2,
			  in float4	Color : COLOR) : COLOR0
{
	float2 uv, tex0;
	float4 o;

	if( TexRange.x < 0)
	{
		tex0 = (VtxTex * TexScaleOffset.xy) + TexScaleOffset.zw;
		uv = (tex0) * g_vFontFactor.zw;

		half4 c = tex2D( g_texFont0, uv);

		// Font를 좀 더 부각시키기 위한 조치.
		half fontA = c.r + TexRange.z;
		half fontC = 1;
		
		if( TexRange.w >= 2)
		{
			// rounding border
			fontA = saturate( fontA + c.b);
			fontC = 1 - c.b;
		}
		else if( TexRange.w >= 1)
		{
			// right-bottom shadow
			fontA = saturate( fontA + c.g);
			fontC = 1 - c.g;
		}

		// 항상 Font Image Pixel Color는 흰색으로 간주한다.
		half3 tempC = fontC * Color.rgb;

		o.rgb = lerp( 0.5, tempC, fontA);
		o.a = fontA * Color.a;
	}
	else
	{
		o = FetchVTex( VtxTex, TexScaleOffset, float2( 0, 0));

		o *= Color;
	}

	return o;
}