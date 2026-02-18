#include "i3GfxType.h"
#include "i3ShaderCode.h"
#include "i3ShaderContext.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/generic_string_size.h"

void i3ShaderCode::SetName( const char * pszName)
{
	char szName[ MAX_PATH];

	i3::safe_string_copy( szName, pszName, MAX_PATH);
	i3::make_unix_path( szName);
	i3::to_upper( szName);

	UINT32 crc = CRC32( 0xFFFFFFFF, (UINT8 *) szName, i3::generic_string_size( szName));

	m_Code = (m_Code & 0x00000000FFFFFFFFL) | ((UINT64) crc << 32);
}

void i3ShaderCode::GenDefName( char * pszName, UINT16 nBufSize)
{
	static char s_szLightModelName[][32] =
	{
		"Unlit_",
		"Phong_",
		"Custom_",
		"HSL_",
		"GI1_",
	};

	static char s_szLightType[][32] =
	{
		"None",
		"Dir",
		"Point",
		"Spot"
	};

	pszName[0] = 0;

	strncat( pszName, s_szLightModelName[ getLightModel()], nBufSize);

	if( isPPLShader() == false)
		strncat( pszName, "PVL_", nBufSize);

	if( getXrayColorEnable())
		strcat( pszName, "XR_");

	if( getVertexColorEnable())
		strncat( pszName, "C", nBufSize);

	if( getDiffuseMapEnable())
		strncat( pszName, "D", nBufSize);

	if( getSpecularMapEnable())
		strncat( pszName, "S", nBufSize);

	if( getNormalMapEnable())
		strncat( pszName, "N", nBufSize);

	if( getEmissiveMapEnable())
		strncat( pszName, "E", nBufSize);

	if( getReflectMapEnable())
		strncat( pszName, "R", nBufSize);

	if( getReflectMaskMapEnable())
		strncat( pszName, "M", nBufSize);

	if( getLuxMapEnable())
		strncat( pszName, "LX", nBufSize);

	if( getFresnelEnable())
		strncat( pszName, "F", nBufSize);

	if( isTexCoordTransform())
		strncat( pszName, "TTX", nBufSize);

	if( isInstancingEnable())
		strncat( pszName, "I", nBufSize);

	if( pszName[0] == 0)
	{
		strncpy( pszName, "NoMat_", nBufSize);
	}
	else
	{
		strncat( pszName, "_", nBufSize);
	}

	// Light 0
	if( getLightType(0) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
	{
		// Light Type
		strncat( pszName, s_szLightType[ getLightType(0)], nBufSize);

		if( getTexturedLight(0))
			strncat( pszName, "Tex", nBufSize);

		if( getShadowType( 0) == I3G_SHADER_FLAG_SHADOW_SHADOWMAP)
			strncat( pszName, "SHM", nBufSize);

		strncat( pszName, "_", nBufSize);

		// Light 1
		if( getLightType(1) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
		{
			strncat( pszName, s_szLightType[ getLightType(1)], nBufSize);

			if( getTexturedLight(1))
				strncat( pszName, "Tex", nBufSize);

			if( getShadowType( 1) == I3G_SHADER_FLAG_SHADOW_SHADOWMAP)
				strncat( pszName, "SHM", nBufSize);
		}
	}
}

inline UINT32	_getLightCode( i3Light * pLight)
{
	UINT32 LT, TL, SH;

	switch( pLight->GetLightType())
	{
		case I3G_LIGHT_NONE :			LT = 0;	break;
		case I3G_LIGHT_DIRECTIONAL :	LT = I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL;	break;
		case I3G_LIGHT_POINT :			LT = I3G_SHADER_FLAG_LIGHT_TYPE_POINT;	break;
		default :						LT = I3G_SHADER_FLAG_LIGHT_TYPE_SPOT;	break;
	}

	if( pLight->getPatternMap() != nullptr)
		TL = 1;
	else
		TL = 0;

	if( (pLight->getStyle() & I3G_LIGHT_STYLE_SHADOWMAP) && (pLight->getShadowMap() != nullptr))
		SH = I3G_SHADER_FLAG_SHADOW_SHADOWMAP;
	else
		SH = 0;

	return	(LT << I3G_SHADER_SHIFT_LIGHT_TYPE) | 
			(TL << I3G_SHADER_SHIFT_TEXTURE_LIGHT) |
			(SH << I3G_SHADER_SHIFT_SHADOW);
}

void i3ShaderCode::setLights( i3Light * pLight1, i3Light * pLight2)
{
	m_Code &= ~((I3G_SHADER_MASK_LIGHT_ALL << I3G_SHADER_SHIFT_LIGHT_ALL) | I3G_SHADER_MASK_LIGHT_ALL);

	if( pLight1 != nullptr)
	{
		if( pLight2 == nullptr)
		{
			// Light가 1개뿐.
			m_Code |= _getLightCode( pLight1);
		}
		else
		{
			m_Code |= _getLightCode( pLight1) | (_getLightCode( pLight2) << I3G_SHADER_SHIFT_LIGHT_ALL);
		}
	}
}

void i3ShaderCode::SaveToXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "DiffuseMap", getDiffuseMapEnable());
	pXML->addAttr( "SpecularMap", getSpecularMapEnable());
	pXML->addAttr( "ReflectMap", getReflectMapEnable());
	pXML->addAttr( "ReflectMaskMap", this->getReflectMaskMapEnable());
	pXML->addAttr( "EmissiveMap", getEmissiveMapEnable());
	pXML->addAttr( "NormalMap", getNormalMapEnable());
	pXML->addAttr( "PPL", isPPLShader());
	pXML->addAttr( "VertexColor", getVertexColorEnable());
	pXML->addAttr( "LuxMap", getLuxMapEnable());
	pXML->addAttr( "Lighting", getLightingEnable());
	pXML->addAttr( "Fresnel", getFresnelEnable());
	pXML->addAttr( "TexTransformed", isTexCoordTransform());
	pXML->addAttr( "Instancing", isInstancingEnable());

	switch( getLightModel())
	{
		case I3G_SHADER_FLAG_LIGHT_MODEL_NOLIGHT :	pXML->addAttr( "LightModel", "NoLight");	break;
		case I3G_SHADER_FLAG_LIGHT_MODEL_PHONG :	pXML->addAttr( "LightModel", "Phong");	break;
		case I3G_SHADER_FLAG_LIGHT_MODEL_CUSTOM :	pXML->addAttr( "LightModel", "Custom");	break;
		case I3G_SHADER_FLAG_LIGHT_MODEL_HSL :		pXML->addAttr( "LightModel", "HSL");	break;
		case I3G_SHADER_FLAG_LIGHT_MODEL_GI1 :		pXML->addAttr( "LightModel", "GI1");	break;
	}

	// Light1
	{
		i3XMLElement * pLightXML = new i3XMLElement( "Light0");

		pLightXML = (i3XMLElement*) pXML->LinkEndChild( pLightXML);
		I3ASSERT( pLightXML != nullptr);
		switch( getLightType( 0))
		{
			case I3G_SHADER_FLAG_LIGHT_TYPE_NONE		:	pLightXML->addAttr( "Type", "None");	break;
			case I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL :	pLightXML->addAttr( "Type", "Directional");	break;
			case I3G_SHADER_FLAG_LIGHT_TYPE_POINT		:	pLightXML->addAttr( "Type", "Point");	break;
			case I3G_SHADER_FLAG_LIGHT_TYPE_SPOT		:	pLightXML->addAttr( "Type", "Spot");	break;
		}

		if( getLightType( 0) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
		{
			pLightXML->addAttr( "Textured", getTexturedLight( 0));

			switch( getShadowType( 0))
			{
				case I3G_SHADER_FLAG_SHADOW_NONE		:	pLightXML->addAttr( "Shadow", "None");	break;
				case I3G_SHADER_FLAG_SHADOW_SHADOWMAP	:	pLightXML->addAttr( "Shadow", "ShadowMap");	break;
			}
		}
	}
	
	// Light1
	{
		i3XMLElement * pLightXML = new i3XMLElement( "Light1");

		pLightXML = (i3XMLElement*) pXML->LinkEndChild( pLightXML);
		I3ASSERT( pLightXML != nullptr);

		switch( getLightType( 1))
		{
			case I3G_SHADER_FLAG_LIGHT_TYPE_NONE		:	pLightXML->addAttr( "Type", "None");	break;
			case I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL :	pLightXML->addAttr( "Type", "Directional");	break;
			case I3G_SHADER_FLAG_LIGHT_TYPE_POINT		:	pLightXML->addAttr( "Type", "Point");	break;
			case I3G_SHADER_FLAG_LIGHT_TYPE_SPOT		:	pLightXML->addAttr( "Type", "Spot");	break;
		}

		if( getLightType( 1) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
		{
			pLightXML->addAttr( "Textured", getTexturedLight( 1));

			switch( getShadowType( 1))
			{
				case I3G_SHADER_FLAG_SHADOW_NONE		:	pLightXML->addAttr( "Shadow", "None");	break;
				case I3G_SHADER_FLAG_SHADOW_SHADOWMAP	:	pLightXML->addAttr( "Shadow", "ShadowMap");	break;
			}
		}
	}
}

void i3ShaderCode::LoadFromXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 flag;
	const char * pszStr;
	i3XMLElement * pLightXML;

	m_Code = 0;

	if( pXML->getAttr( "DiffuseMap", &flag) != nullptr)
		setDiffuseMapEnable( flag != 0);

	if( pXML->getAttr( "SpecularMap", &flag) != nullptr)
		setSpecularMapEnable( flag != 0);

	if( pXML->getAttr( "ReflectMap", &flag) != nullptr)
		setReflectMapEnable( flag != 0);

	if( pXML->getAttr( "ReflectMaskMap", &flag) != nullptr)
		setReflectMaskMapEnable( flag != 0);

	if( pXML->getAttr( "EmissiveMap", &flag) != nullptr)
		setEmissiveMapEnable( flag != 0);

	if( pXML->getAttr( "NormalMap", &flag) != nullptr)
		setNormalMapEnable( flag != 0);

	if( pXML->getAttr( "PPL", &flag) != nullptr)
		setPPLShader( flag != 0);

	if( pXML->getAttr( "VertexColor", &flag) != nullptr)
		setVertexColorEnable( flag != 0);

	if( pXML->getAttr( "LuxMap", &flag) != nullptr)
		setLuxMapEnable( flag != 0);

	if( pXML->getAttr( "Lighting", &flag) != nullptr)
		setLightingEnable( flag != 0);

	if( pXML->getAttr( "Fresnel", &flag) != nullptr)
		setFresnelEnable( flag != 0);

	if( pXML->getAttr( "TexTransformed", &flag) != nullptr)
		setTexCoordTransform( flag != 0);

	if( pXML->getAttr( "Instancing", &flag) != nullptr)
		setInstancingEnable( flag != 0);

	// Light Model
	{
		pszStr = pXML->getAttr( "LightModel");
		if( pszStr != nullptr)
		{
			if( i3::generic_is_iequal( pszStr, "Phong") )
				setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_PHONG);
			else if( i3::generic_is_iequal( pszStr, "Custom") )
				setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_CUSTOM);
			else if( i3::generic_is_iequal( pszStr, "HSL") )
				setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_HSL);
			else if( i3::generic_is_iequal( pszStr, "GI1") )
				setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_GI1);
			else
				setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_NOLIGHT);
		}
	}

	// Light1
	{
		setLightType( 0, I3G_SHADER_FLAG_LIGHT_TYPE_NONE);

		pLightXML = (i3XMLElement *) pXML->FirstChild( "Light0");

		if( pLightXML != nullptr)
		{
			pszStr = pLightXML->getAttr( "Type");

			if( pszStr != nullptr)
			{
				if( i3::generic_is_iequal( pszStr, "Directional") )
					setLightType( 0, I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL);
				else if( i3::generic_is_iequal( pszStr, "Point") )
					setLightType( 0, I3G_SHADER_FLAG_LIGHT_TYPE_POINT);
				else if( i3::generic_is_iequal( pszStr, "Spot") )
					setLightType( 0, I3G_SHADER_FLAG_LIGHT_TYPE_SPOT);
			}

			if( pLightXML->getAttr( "Textured", &flag) != nullptr)
				setTexturedLight( 0, flag != 0);

			pszStr = pLightXML->getAttr( "Shadow");
			if( pszStr != nullptr)
			{
				if( i3::generic_is_iequal( pszStr, "ShadowMap") )
					setShadowType( 0, I3G_SHADER_FLAG_SHADOW_SHADOWMAP);
				else
					setShadowType( 0, I3G_SHADER_FLAG_SHADOW_NONE);
			}
		}
	}
	
	// Light1
	{
		setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_NONE);

		pLightXML = (i3XMLElement *) pXML->FirstChild( "Light1");

		if( pLightXML != nullptr)
		{
			pszStr = pLightXML->getAttr( "Type");

			if( pszStr != nullptr)
			{
				if( i3::generic_is_iequal( pszStr, "Directional") )
					setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL);
				else if( i3::generic_is_iequal( pszStr, "Point") )
					setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_POINT);
				else if( i3::generic_is_iequal( pszStr, "Spot") )
					setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_SPOT);
			}

			if( pLightXML->getAttr( "Textured", &flag) != nullptr)
				setTexturedLight( 1, flag  != 0);

			pszStr = pLightXML->getAttr( "Shadow");
			if( pszStr != nullptr)
			{
				if( i3::generic_is_iequal( pszStr, "ShadowMap") )
					setShadowType( 1, I3G_SHADER_FLAG_SHADOW_SHADOWMAP);
				else
					setShadowType( 1, I3G_SHADER_FLAG_SHADOW_NONE);
			}
		}
	}

	// 사후 조정
	if( getLightingEnable())
	{
		// Lighting이 일단 enable 상태라면 Light Model 설정은 반드시 필요하다.
		setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_PHONG);

		// Lighting이 On이라면 Vertex Color가 사용되는 경우가 없으므로...
		setVertexColorEnable( false);
	}

	if( isPPLShader() == false)
	{
		setSpecularMapEnable( false);
		setFresnelEnable( false);
		setNormalMapEnable( false);
//		setEmissiveMapEnable( false);
	}
}
