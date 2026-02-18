

struct VS_INPUT
{
	float3		Pos			: POSITION;
};

//////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4  oPos            : POSITION;
};

float4x4		g_mWorld;
float4x4		g_mViewProj;
float4			g_Color;
//////////////////////////////////////////////////////////////////////
	
VS_OUTPUT VS_Def( VS_INPUT i)
{
	VS_OUTPUT o;
	o.oPos = mul( float4( i.Pos, 1), g_mWorld);
	o.oPos = mul(o.oPos, g_mViewProj);
	return o;
}

float4 PS_Def(in float4 Color : COLOR) : COLOR0
{
	return g_Color;
}

