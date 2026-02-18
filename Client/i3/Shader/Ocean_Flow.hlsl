#define I3L_LIGHTMODEL			 I3L_LIGHTMODEL_PHONG
#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#include "i3DirectionalLight.hlsl"
#include "i3Phong_LightModel.hlsl"
#endif

#define		LOOP_DURATION			2.0
#define		HORZ_STEP				8.0
#define		VERT_STEP				8.0
#define		MAX_STEP				60

#define		PIXEL					(1.0 / 1024.0)

#define		STEP_CX					(1.0 / HORZ_STEP)
#define		STEP_CY					(1.0 / VERT_STEP)

//////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4  oPos            : POSITION;
    float2	oTex0			: TEXCOORD0;
	float3	oCSPos			: TEXCOORD1;
	float3	oNormal			: TEXCOORD2;
	float3	oTangent		: TEXCOORD3;
	float3	oBinormal		: TEXCOORD4;
	float2	oOffset			: TEXCOORD5;
};

VS_OUTPUT VS_Def( in float4 iPos : POSITION,
				 in float3 iNormal : NORMAL,
				 in float3 iTangent : TANGENT,
				 in float3 iBinormal : BINORMAL,
				 in float2 iTex : TEXCOORD0)
{
	VS_OUTPUT o;
	o.oPos		= float4( mul( iPos, g_mWorldView), 1.0f);
	o.oCSPos	= o.oPos.xyz;

	o.oPos		= mul(	o.oPos, g_mProj);
	o.oNormal	= normalize( mul( iNormal,		(float3x3) g_mWorldView));
	o.oTangent	= normalize( mul( iTangent,	(float3x3) g_mWorldView));
	o.oBinormal	= normalize( mul( iBinormal,	(float3x3) g_mWorldView));

	o.oTex0 = mul( float4( iTex, 1.0f, 1.0f), g_mTex).xy;

	float	dur = fmod( g_Time, LOOP_DURATION) / LOOP_DURATION;
	int		idx	=	(int) (dur * MAX_STEP);
	float2	offset;

	offset.x = (int) fmod( (float)idx, HORZ_STEP) * STEP_CX;
	offset.y = (int) (idx / HORZ_STEP) * STEP_CY;

	o.oOffset	 = offset;
    
	return o;
}


float4 PS_Def(	in float2 iTex0 : TEXCOORD0,
			  in float3	CSPos : TEXCOORD1,
			  in float3 iN : TEXCOORD2,
			  in float3	iT : TEXCOORD3,
			  in float3 iB : TEXCOORD4,
			  in float2 iOffset : TEXCOORD5) : COLOR
{	
	float4 o;
	float3 N;
	float4	spec;
	float2 seqUV;

	// Normal
	seqUV = frac( iTex0) * float2( STEP_CX, STEP_CY);
	seqUV = seqUV - (PIXEL * 0.5);
	seqUV = seqUV + iOffset;
	_getNormalMap( N, seqUV, iN, iT, iB);

	// Specular
	spec = tex2D( g_texSpecular, iTex0) * float4( 2, 2, 2, 255);
	//spec = g_vLightSpecular0;

	float litted = _Directional( N, g_vLightDir0);

	half3 sColor;
	
	sColor = _getSpecular_Dir( N, CSPos, spec.rgb, spec.a, g_vLightDir0);

	o.rgb = ((litted  * sColor)) * g_vLightDiffuse0.rgb;

	o.a = sColor.r;


	return o;
}

