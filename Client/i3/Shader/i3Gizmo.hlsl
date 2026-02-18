#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

//////////////////////////////////////////////////////
struct VS_OUTPUT_GIZMO
{
    float4  oPos            : POSITION;
    float3  oNormal         : TEXCOORD0;
	float3	oCSPos			: TEXCOORD1;
};

//-----------------------------------------------------------------------------
// Vertex Shader: VertScene
// Desc: Process vertex for scene
//-----------------------------------------------------------------------------
VS_OUTPUT_GIZMO VS_Def( in float4 iPos : POSITION,
				 in float3 iNormal : NORMAL)
{
	VS_OUTPUT_GIZMO o;

	o.oNormal = normalize( mul( iNormal.xyz, (float3x3) g_mWorldView));
	o.oCSPos = mul( iPos, g_mWorldView );
	o.oPos = mul( float4( o.oCSPos, 1), g_mProj );
    
	return o;
}

float4 PS_Def(	in float3 iNormal : TEXCOORD0,
				in float3 CSPos : TEXCOORD1) : COLOR
{	
	float4 o;
	float lit = 0;
	float3 norm = normalize( iNormal);

	float3 litDir = normalize( float3( -0.3, 0.5, 1));

	lit = saturate( dot( litDir, norm));
	
	lit = (lit * 0.5) + 0.7;
	lit = lit * lit;

	float3 spec = _getSpecular_Dir( norm, CSPos, float3( 1, 1, 1), 4, litDir);

	o = g_vColor * float4( lit, lit, lit, 1);
	o.rgb += spec;

	return o;
}

