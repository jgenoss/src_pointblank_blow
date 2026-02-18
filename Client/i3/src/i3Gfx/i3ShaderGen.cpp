#include "i3GfxType.h"
#include "i3ShaderGen.h"
#include "i3ShaderContext.h"

//I3_CLASS_INSTANCE( i3ShaderGen, i3ElementBase);
I3_CLASS_INSTANCE( i3ShaderGen);

//#define SCREENSPACE_SHADOW

static char s_szLightModel[][32] =
{
	"I3L_LIGHTMODEL_NONE",
	"I3L_LIGHTMODEL_PHONG",
	"I3L_LIGHTMODEL_CUSTOM",
	"I3L_LIGHTMODEL_HSL",
	"I3L_LIGHTMODEL_GI1"
};

static char s_szLightType[][32] =
{
	"I3L_LIGHTTYPE_NONE",
	"I3L_LIGHTTYPE_DIRECTIONAL",
	"I3L_LIGHTTYPE_POINT",
	"I3L_LIGHTTYPE_SPOT"
};

static char s_szShadowType[][32] =
{
	"I3L_SHADOW_NONE",
	"I3L_SHADOW_SHADOWMAP"
};

static char s_szSemanticType[][32] =
{
	"float2",	// _SPT_SEMANTIC_TEX0,
	"float2",	// _SPT_SEMANTIC_TEX1,
	"float2",	// _SPT_SEMANTIC_TEX2,
	"float2",	// _SPT_SEMANTIC_TEX3,
	"float3",	// _SPT_SEMANTIC_NORMAL,
	"float3",	// _SPT_SEMANTIC_TANGENT,
	"float3",	// _SPT_SEMANTIC_BINORMAL,
	"float3",	// _SPT_SEMANTIC_HALF,
	"float4",	// _SPT_SEMANTIC_LS_POS,
	"float3",	// _SPT_SEMANTIC_CS_POS,
	"float3",	// _SPT_SEMANTIC_REFLECT,
	"float3",	// _SPT_SEMANTIC_SPECULAR,
	"float4",	//_SPT_SEMANTIC_VERTEX_COLOR,
	"float4",	// _SPT_SEMANTIC_SS_POS
};

bool i3ShaderGen::Generate( i3Shader * pShader, i3Stream * pStream)
{
	i3FileStream strm;
	char szPath[MAX_PATH] = { 0, };
	char conv[4096] = { 0, };
	INT32 i = 0;
	i3ShaderCode * pCode = pShader->getShaderCode();

	if( pStream == nullptr)
	{
		pStream = &strm;

		////////////////////////////
		// i3Decl.hlsl
		sprintf( szPath, "%s/i3Decl.hlsl", m_pszWorkDir);
		if( strm.Create( szPath, STREAM_WRITE) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not create %s file.", szPath);
			return false;
		}
	}

	if( pCode->getDiffuseMapEnable() || 
		pCode->getNormalMapEnable() || 
		pCode->getEmissiveMapEnable() || 
		pCode->getSpecularMapEnable() || 
		(pCode->getReflectMapEnable() && pCode->getReflectMaskMapEnable()))
	{
		_addParam( "Tex0", _SPT_SEMANTIC_TEX0);
	}

	if( pCode->getLuxMapEnable())
	{
		_addParam( "Tex1", _SPT_SEMANTIC_TEX1);
	}

	if( pCode->getLightingEnable() || pCode->getReflectMapEnable())
	{
		// Lighting을 한다.
		if( pCode->isPPLShader())
		{
			_addParam( "Normal", _SPT_SEMANTIC_NORMAL);

			if( pCode->getNormalMapEnable())
			{
				_addParam( "Tangent", _SPT_SEMANTIC_TANGENT);
				_addParam( "Binormal", _SPT_SEMANTIC_BINORMAL);
			}

			if( (pCode->getLightType(0) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE) || (pCode->getLightType(1) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE) ||
				(pCode->getReflectMapEnable()))
				_addParam( "CSPos", _SPT_SEMANTIC_CS_POS);
		}
		else
		{
			_addParam( "Color",	_SPT_SEMANTIC_VERTEX_COLOR, _SPT_VIA_COLOR);
			_addParam( "Specular",	_SPT_SEMANTIC_SPECULAR);

			if( pCode->getReflectMapEnable())
				_addParam( "Reflect", _SPT_SEMANTIC_REFLECT);
		}

		if( pCode->getShadowType( 0) != 0)
		{
			_addParam( "LSPos0", _SPT_SEMANTIC_LS_POS);

			#if defined( SCREENSPACE_SHADOW)
			_addParam( "SSPos", _SPT_SEMANTIC_SS_POS);
			#endif
		}

		if( pCode->getShadowType( 1) != 0)
		{
			_addParam( "LSPos1", _SPT_SEMANTIC_LS_POS);
		}
	}
	else
	{
		if( pCode->isPPLShader())
		{
			if( pCode->getVertexColorEnable())
			{
				_addParam( "Color", _SPT_SEMANTIC_VERTEX_COLOR, _SPT_VIA_COLOR);
			}
		}
		else
		{
			_addParam( "Color", _SPT_SEMANTIC_VERTEX_COLOR, _SPT_VIA_COLOR);
		}
	}

	// Code generation
	if( pCode->isTexCoordTransform())
		pStream->Printf( conv, "#define I3L_TEXCOORD_TRANSFORM\n");

	if( pCode->isPPLShader())
		pStream->Printf( conv, "#define I3L_PPL\n");

	if( pCode->getDiffuseMapEnable())
		pStream->Printf( conv, "#define I3L_DIFFUSE_MAP\n");

	if( pCode->getSpecularMapEnable())
		pStream->Printf( conv, "#define I3L_SPECULAR_MAP\n");

	if( pCode->getReflectMapEnable())
	{
		pStream->Printf( conv, "#define I3L_REFLECT_MAP\n");

		if( pCode->getReflectMaskMapEnable())
			pStream->Printf( conv, "#define I3L_REFLECT_MASK_MAP\n");
	}

	if( pCode->getEmissiveMapEnable())
		pStream->Printf( conv, "#define I3L_EMISSIVE_MAP\n");

	if( pCode->getNormalMapEnable())
		pStream->Printf( conv, "#define I3L_NORMAL_MAP\n");

	if( pCode->getXrayColorEnable() )
		pStream->Printf( conv, "#define I3L_XRAYCOLOR\n");

	if( pCode->getLightingEnable() || (pCode->getReflectMapEnable()))
	{
	}
	else
	{
		if( pCode->getVertexColorEnable())
			pStream->Printf( conv, "#define I3L_VERTEX_COLOR\n");
	}

	if( pCode->getLuxMapEnable())
		pStream->Printf( conv, "#define I3L_LUX_MAP\n");

	if( pCode->getFresnelEnable())
		pStream->Printf( conv, "#define I3L_FRESNEL\n");

	if( pCode->isInstancingEnable())
		pStream->Printf( conv, "#define I3L_INSTANCING\n");

	pStream->Printf( conv, "\n\n/////////////////////////////////////////////\n// Lighting\n");

	if( pCode->getLightingEnable())
	{
		// Light가 있음.
		pStream->Printf( conv, "#define I3L_LIGHTING\n");

		pStream->Printf( conv, "#define I3L_LIGHTMODEL		%s\n", s_szLightModel[ pCode->getLightModel()]);

		// 1st Light
		{
			// Light Type
			pStream->Printf( conv, "#define I3L_LIGHTTYPE_0			%s\n", s_szLightType[ pCode->getLightType(0)]);

			if( pCode->getLightType( 0) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
			{
				// Textured Light
				if( pCode->getTexturedLight(0))
					pStream->Printf( conv, "#define I3L_TEXTURE_LIGHT_0\n");

				// Shadow
				pStream->Printf( conv, "#define I3L_SHADOW_0		%s\n", s_szShadowType[ pCode->getShadowType(0)]);
			}
		}

		// 2nd Light
		{			
			// Light Type
			pStream->Printf( conv, "#define I3L_LIGHTTYPE_1			%s\n", s_szLightType[ pCode->getLightType(1)]);

			if( pCode->getLightType( 1) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
			{
				// Textured Light
				if( pCode->getTexturedLight(1))
					pStream->Printf( conv, "#define I3L_TEXTURE_LIGHT_1\n");

				// Shadow
				pStream->Printf( conv, "#define I3L_SHADOW_1		%s\n", s_szShadowType[ pCode->getShadowType(1)]);
			}
		}
	}
	else
	{
		// Light가 없음.
		pStream->Printf( conv, "#define I3L_LIGHTMODEL			I3L_LIGHTMODEL_NONE\n");
	}

	pStream->Printf( conv, "\n\n");

	// VS_OUTPUT
	pStream->Printf( conv, "struct VS_OUTPUT\n");
	pStream->Printf( conv, "{\n");
	pStream->Printf( conv, "	float4	Pos			: POSITION;\n");

	for( i = 0; i < m_SPCount; i++)
	{
		switch( m_SPT[i]._via)
		{
			case _SPT_VIA_COLOR :		
				pStream->Printf( conv, "	float4	%s		: COLOR;\n", m_SPT[i]._name);
				break;

			case _SPT_VIA_VPOS :
				//pStream->Printf( conv, "	float4	%s		: VPOS;\n", m_SPT[i]._name);
				break;

			default :
				pStream->Printf( conv, "	%s	%s		: TEXCOORD%d;\n", s_szSemanticType[ m_SPT[i]._semantic], m_SPT[ i]._name, m_SPT[i]._via);
				break;
		}
	}

	pStream->Printf( conv, "};\n\n");

	// PS_INPUT
	pStream->Printf( conv, "struct PS_INPUT\n");
	pStream->Printf( conv, "{\n");

	for( i = 0; i < m_SPCount; i++)
	{
		switch( m_SPT[i]._via)
		{
			case _SPT_VIA_COLOR :		
				pStream->Printf( conv, "	float4	%s		: COLOR;\n", m_SPT[i]._name);
				break;

			case _SPT_VIA_VPOS :
				pStream->Printf( conv, "	float4	%s		: vPos;\n", m_SPT[i]._name);
				break;

			default :
				pStream->Printf( conv, "	%s	%s		: TEXCOORD%d;\n", s_szSemanticType[ m_SPT[i]._semantic], m_SPT[ i]._name, m_SPT[i]._via);
				break;
		}
	}

	pStream->Printf( conv, "};\n\n");

	if( pStream == &strm)
	{
		pStream->Close();
	}

	// Compile
	return true;
}
