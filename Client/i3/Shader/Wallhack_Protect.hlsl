#if !defined( I3L_BINARY_SHADER)
#include "i3Common.hlsl"
#endif

//////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4  oPos            : POSITION;
};

//-----------------------------------------------------------------------------
// Vertex Shader: VertScene
// Desc: Process vertex for scene
//-----------------------------------------------------------------------------
#if !defined( I3L_VERTEX_BLEND)
	VS_OUTPUT VS_Def( in float4 iPos : POSITION)
	{
		VS_OUTPUT o;
				
		o.oPos = mul( iPos, g_mWVP );

		o.oPos /= o.oPos.w;
		o.oPos.z = 1.0f;
		
		return o;
	}
#else

	struct VS_INPUT2
	{
		float4  Pos             : POSITION;
		float4  BlendWeights    : BLENDWEIGHT;
		float4  BlendIndices    : BLENDINDICES;
	};
	
	#define WEIGHT(idx)				i.BlendWeights[(idx)]
	#define	INDEX(idx)				i.BlendIndices[(idx)]

	//-----------------------------------------------------------------------------
	// Vertex Shader: VertScene
	// Desc: Process vertex for scene
	//-----------------------------------------------------------------------------
	VS_OUTPUT VS_Def( VS_INPUT2 i)
	{
		VS_OUTPUT o;
		float       LastWeight = 0.0f;
		float3		Pos = 0.0f;
		    
		#if (NUM_BONES > 1)
			LastWeight = LastWeight + WEIGHT(0);			
			Pos					+= mul( i.Pos,		g_mBone[ INDEX(0)]) * WEIGHT(0);
		#endif
		
		#if (NUM_BONES > 2)
			LastWeight = LastWeight + WEIGHT(1);
			Pos					+= mul( i.Pos,		g_mBone[ INDEX(1)]) * WEIGHT(1);
		#endif
		
		#if (NUM_BONES > 3)
			LastWeight = LastWeight + WEIGHT(2);			
			Pos					+= mul( i.Pos,		g_mBone[ INDEX(2)]) * WEIGHT(2);
		#endif
		
		{
			LastWeight = 1.0f - LastWeight;
			
			Pos			+= mul( i.Pos,		g_mBone[ INDEX( NUM_BONES-1)]) * LastWeight;
		}
	    
		o.oPos = mul( float4( Pos, 1.0f), g_mProj);

		o.oPos /= o.oPos.w;
		o.oPos.z = 1.0f;
		
		return o;
	}
#endif

float4 PS_Def(void) : COLOR
{	
	return float4( 1, 0, 0, 1);
}

