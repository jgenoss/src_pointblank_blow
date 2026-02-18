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

float4 FetchVTex( in float2 VtxTex, in float4 TexScaleOffset, in float4 TexRange, float2 offset)
{
	float2 uv, tex0;
	float2 rangeMax;

	tex0 = (VtxTex * TexScaleOffset.xy) + TexScaleOffset.zw + offset;

	rangeMax = (0.9999 * TexScaleOffset.xy) + TexScaleOffset.zw;

	tex0 = min( tex0, rangeMax); 
	
	uv = ResolveVTexCoord( tex0);

	return tex2D( g_texCache, uv);
}

#define		FCOLOR		0.1

float4 PS_Def(in float2 VtxTex : TEXCOORD0,
			  in float4	TexScaleOffset : TEXCOORD1,
			  in float4 TexRange : TEXCOORD2,
			  in float4	Color : COLOR) : COLOR0
{
	float2 uv, tex0;
	float4 o;
	half4 c00, c01, c10, c11;

	if( TexRange.x < 0)
	{
		tex0 = (VtxTex * TexScaleOffset.xy) + TexScaleOffset.zw;
		half2 uv = ((tex0) * g_vFontFactor.zw);

#if 0
		float2 lerps = frac( tex0);

		c00 = tex2D( g_texFont0, uv + half2( 0, 0));
		c01 = tex2D( g_texFont0, uv + half2( g_vFontFactor.z, 0));
		c10 = tex2D( g_texFont0, uv + half2( 0, g_vFontFactor.w));
		c11 = tex2D( g_texFont0, uv + g_vFontFactor.zw);

		half4 c = lerp( lerp( c00, c01, lerps.x ),
                        lerp( c10, c11, lerps.x ),
                                  lerps.y );
#else
		half4 c = tex2D( g_texFont0, uv);
#endif

		// Font를 좀 더 부각시키기 위한 조치.
		half fontA = c.r + TexRange.z;		// (TexRange.z는 Texture Mapping을 사용하지 않는 Caret의 경우를 위해 사용된다.)
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
		float2 texelpos = (VtxTex * TexScaleOffset.xy) + TexScaleOffset.zw;
		float2 lerps = frac( texelpos * I3_VT_MAX );

		c00 = FetchVTex( VtxTex, TexScaleOffset, TexRange, float2( 0, 0));
		c01 = FetchVTex( VtxTex, TexScaleOffset, TexRange, float2( PIXEL, 0));
		c10 = FetchVTex( VtxTex, TexScaleOffset, TexRange, float2( 0, PIXEL));
		c11 = FetchVTex( VtxTex, TexScaleOffset, TexRange, float2( PIXEL, PIXEL));

		o = lerp( lerp( c00, c01, lerps.x ),
                        lerp( c10, c11, lerps.x ),
                                  lerps.y );
		o *= Color;
	}

	return o;
}