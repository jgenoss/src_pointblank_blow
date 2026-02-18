#if !defined( I3L_BINARY_SHADER)
#include "i3PostProcess_VS.hlsl"
#endif

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler2D	g_texInput;
float		g_thickness;
float		g_threshold;

float GetGreyScale(float2 texCoord)
{
	float3 cc = tex2D(g_texInput, texCoord);
	return GetLuminance(cc);
}

float4 GetEdge(float2 textureCoord)
{
	float3x3 result;

	float2 xOffset = float2(g_thickness / g_ScreenSize.x, 0.0f);
	float2 yOffset = float2(0.0f, g_thickness / g_ScreenSize.y);

	float2 uv = textureCoord.xy;

	float2 pp1 = uv - yOffset;
	float2 pp2 = uv + yOffset;

	float3x3 GreyScale = {
		GetGreyScale(pp1 - xOffset), GetGreyScale(pp1), GetGreyScale(pp1 + xOffset),
		GetGreyScale(uv - xOffset), GetGreyScale(uv), GetGreyScale(uv + xOffset),
		GetGreyScale(pp2 - xOffset), GetGreyScale(pp2), GetGreyScale(pp2 + xOffset),
	};


	float3x3 sobelY = {
		-1.0f, -2.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 2.0f, 1.0f,
	};

	float3x3 sobelX = transpose(sobelY);
	
	float Gx = (GreyScale * sobelX)._m00 + (GreyScale * sobelX)._m11 + (GreyScale * sobelX)._m22;
	float Gy = (GreyScale * sobelY)._m00 + (GreyScale * sobelY)._m11 + (GreyScale * sobelY)._m22;

	float norm = sqrt(Gx * Gx + Gy + Gy);

	if(norm > g_threshold)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

float4 PS_Def(	in float2 iTex0 : TEXCOORD0) : COLOR0 
{	
	float4 baseColor = tex2D(g_texInput, iTex0);
	baseColor = baseColor * GetEdge(iTex0);
	
	return baseColor;
}
