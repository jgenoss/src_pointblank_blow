#include "i3GfxType.h"
#include "i3Light.h"
#include "i3GfxUtil.h"

//I3_CLASS_INSTANCE( i3Light, i3GfxResource);
I3_CLASS_INSTANCE( i3Light);

//#define I3L_SAVSM
//#define I3L_VSM
//#define I3L_ESM

i3Light::~i3Light(void)
{
	I3_SAFE_RELEASE( m_pPatternMap);
	I3_SAFE_RELEASE( m_pShadowMap[0]);
	I3_SAFE_RELEASE( m_pShadowMap[1]);
	I3_SAFE_RELEASE( m_pDepthRT);
}

void i3Light::SetLightType( I3G_LIGHT_TYPE type)
{
	m_Type = type;
}

void i3Light::SetDiffuse( COLORREAL * pColor)
{
	i3Color::Set( &m_DiffuseColor, pColor);
}

void i3Light::SetSpecular( COLORREAL * pColor)
{
	i3Color::Set( &m_SpecularColor, pColor);
}

void i3Light::SetWorldSpaceTransform( MATRIX * pMatrix)
{
	i3Matrix::Copy( &m_mWorldTx, pMatrix);
}

bool i3Light::isAffect( VEC3D * pWSCenter, REAL32 r)
{
	VEC3D dir, sub;
	REAL32 len, dot;

	switch( GetLightType())
	{
		//case I3G_LIGHT_DIRECTIONAL :		return true;

		case I3G_LIGHT_POINT :
			i3Vector::Sub( &sub, GetWorldSpacePosition(), pWSCenter);

			len = i3Vector::Normalize( &dir, &sub);

			if( len > (getRange() + r))
				return false;

			this->setSqDistance( len * len);
			break;

		case I3G_LIGHT_SPOT :
			i3Vector::Sub( &sub, GetWorldSpacePosition(), pWSCenter);

			len = i3Vector::Normalize( &dir, &sub);

			if( len < I3_EPS)
			{
				setSqDistance( 0.0f);
				return true;
			}

			if( len > (getRange() + r))
				return false;

			dot = i3Vector::Dot( &dir, GetWorldSpaceDirection());

			REAL32 angle	= i3Math::acos( dot);		// Angular between (Center->Center) and LightDirection
			REAL32 margine	= asinf( r / len);

			if( getOuterAngle() < (margine + angle))
				return false;

			this->setSqDistance( len * len);
			break;
	}

	return true;
}

void i3Light::CreateShadowMap(void)
{
	I3ASSERT( m_ShadowMapSize > 0);
	I3G_IMAGE_FORMAT fmt, dfmt;
	INT32 mapCount = 1;

	#if defined( I3L_VSM)
		fmt = I3G_IMAGE_FORMAT_GR_32F;

		mapCount = 2;
	#elif defined( I3L_ESM)
		fmt = I3G_IMAGE_FORMAT_R_32F;

		mapCount = 2;
	#else
		fmt = I3G_IMAGE_FORMAT_R_32F;
	#endif
	
	dfmt = I3G_IMAGE_FORMAT_D24X8;

	I3_SAFE_RELEASE( m_pShadowMap[0]);
	I3_SAFE_RELEASE( m_pShadowMap[1]);
	I3_SAFE_RELEASE( m_pDepthRT);

	for( INT32 i = 0; i < mapCount; i++)
	{
		m_pShadowMap[i] = i3Texture::new_object();

		#if defined( I3_DEBUG)
			m_pShadowMap[i]->SetName( "ShadowMap");
		#endif

		m_pShadowMap[i]->Create( m_ShadowMapSize, m_ShadowMapSize, 1, fmt, I3G_USAGE_RENDERTARGET);
	}

	m_pDepthRT = i3Texture::new_object();
#if defined( I3_DEBUG)
	m_pDepthRT->SetName( "LightDepthRT");
#endif
	m_pDepthRT->Create( m_ShadowMapSize, m_ShadowMapSize, 1, dfmt, 0);
}

i3Texture * i3Light::getActualShadowMap(void)
{
#if defined( I3L_VSM) && !defined( I3L_SAVSM)
	return m_pShadowMap[1];		// Blurred Variance Shadow Map
#elif defined( I3L_ESM)
	return m_pShadowMap[1];		// Blurred Exponetial Shadow Map
#else
	return m_pShadowMap[0];
#endif
}

void i3Light::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pPatternMap != nullptr)
		m_pPatternMap->OnBuildObjectList( List);

	i3GfxResource::OnBuildObjectList( List);
}

void i3Light::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GfxResource::CopyTo( pDest, method);

	if( i3::kind_of<i3Light*>(pDest) == false )
		return;

	i3Light * pLight = (i3Light *) pDest;

	pLight->SetLightType( GetLightType());
	pLight->setGroup( getGroup());
	pLight->SetDiffuse( GetDiffuse());
	pLight->SetSpecular( GetSpecular());
	pLight->setRange( getRange());
	pLight->setInnerAngle( getInnerAngle());
	pLight->setOuterAngle( getOuterAngle());
	pLight->SetWorldSpaceTransform( GetWorldSpaceTransform());
	pLight->setPatternMap( getPatternMap());
	pLight->setShadowMapSize( getShadowMapSize());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED LIGHT_INFO
	{
		UINT16				m_Type = 0;
		i3::pack::COLORREAL	m_Temp;
		i3::pack::COLORREAL	m_Diffuse;
		i3::pack::COLORREAL	m_Specular;
		UINT32				pad[3] = { 0 };
		REAL32				m_InnerAngle = 0.0f;
		REAL32				m_OuterAngle = 0.0f;
		UINT32				m_ShadowMapSize = 0;
		REAL32				m_Range = 0.0f;
		OBJREF				m_pPatternMap = 0;
		UINT32				m_Group = 0;
		UINT32				m_Style = 0;
		UINT8				pad2[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Light::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::LIGHT_INFO Info;

	Info.m_Type = (UINT16)	i3Gfx::Convert( GetLightType());

	/*
	Info.m_Ambient.r = i3Color::GetR( &m_AmbientColor);
	Info.m_Ambient.g = i3Color::GetG( &m_AmbientColor);
	Info.m_Ambient.b = i3Color::GetB( &m_AmbientColor);
	Info.m_Ambient.a = i3Color::GetA( &m_AmbientColor);
	*/

	Info.m_Diffuse.r = i3Color::GetR( &m_DiffuseColor);
	Info.m_Diffuse.g = i3Color::GetG( &m_DiffuseColor);
	Info.m_Diffuse.b = i3Color::GetB( &m_DiffuseColor);
	Info.m_Diffuse.a = i3Color::GetA( &m_DiffuseColor);

	Info.m_Specular.r = i3Color::GetR( &m_SpecularColor);
	Info.m_Specular.g = i3Color::GetG( &m_SpecularColor);
	Info.m_Specular.b = i3Color::GetB( &m_SpecularColor);
	Info.m_Specular.a = i3Color::GetA( &m_SpecularColor);

	Info.m_InnerAngle = getInnerAngle();
	Info.m_OuterAngle = getOuterAngle();
	Info.m_Range	= getRange();
	Info.m_ShadowMapSize = getShadowMapSize();
	Info.m_Group		= getGroup();
	Info.m_Style		= m_Style;

	Info.m_pPatternMap = (OBJREF) pResFile->GetObjectPersistID( getPatternMap());

	Rc = pStream->Write( &Info, sizeof(pack::LIGHT_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Light::OnSave()", "Could not write light information.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3Light::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::LIGHT_INFO Info;

	Rc = pStream->Read( &Info, sizeof(pack::LIGHT_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Light::OnLoad()", "Could not read light information.");
		return STREAM_ERR;
	}

	SetLightType( i3Gfx::Convert( (I3G_PERSIST_LIGHT_TYPE) Info.m_Type));

	setStyle( Info.m_Style);
	setGroup( Info.m_Group);

	/*
	// Ambient color
	{
		COLORREAL col;

		i3Color::Set( &col, Info.m_Ambient.r, Info.m_Ambient.g, Info.m_Ambient.b, Info.m_Ambient.a);

		SetAmbient( &col);
	}
	*/

	// Diffuse color
	{
		COLORREAL col;

		i3Color::Set( &col, Info.m_Diffuse.r, Info.m_Diffuse.g, Info.m_Diffuse.b, Info.m_Diffuse.a);

		SetDiffuse( &col);
	}

	// Specular color
	{
		COLORREAL col;

		i3Color::Set( &col, Info.m_Specular.r, Info.m_Specular.g, Info.m_Specular.b, Info.m_Specular.a);

		SetSpecular( &col);
	}

	setInnerAngle( Info.m_InnerAngle);
	setOuterAngle( Info.m_OuterAngle);
	setRange( Info.m_Range);

	if( (GetLightType() == I3G_LIGHT_DIRECTIONAL) && (getRange() < 10.0f))
	{
		// 예전에 저장된 Light들 중에, Range가 올바르게 설정되어 있지 않은 경우에
		// 대한 예외 처리.
		setRange( 50.0f);
	}

	if( isStyle( I3G_LIGHT_STYLE_SHADOWMAP))
	{
		setShadowMapSize( Info.m_ShadowMapSize);
	}

	if( Info.m_pPatternMap != 0)
	{
		i3Texture * pTex = (i3Texture *) pResFile->FindObjectByID( Info.m_pPatternMap);
		I3ASSERT( pTex != nullptr);

		setPatternMap( pTex);
	}

	return Rc;
}

bool i3Light::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->SetAttribute( "Type", (INT32) i3Gfx::Convert( GetLightType()));

	i3Color::SetToXML( pXML, "Diffuse", &m_DiffuseColor);
	i3Color::SetToXML( pXML, "Specular", &m_SpecularColor);

	pXML->addAttr( "InnerAngle",	getInnerAngle());
	pXML->addAttr( "OuterAngle",	getOuterAngle());
	pXML->addAttr( "Range",		getRange());
	pXML->addAttr( "ShadowMapSize",	getShadowMapSize());
	pXML->addAttr( "Group",			getGroup());
	pXML->addAttr( "Style",			getStyle());

	pXML->addAttr( "PatternMap",		(INT32) pFile->GetObjectPersistID( getPatternMap()));

	return true;
}

bool i3Light::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 iVal;

	pXML->Attribute( "Type", &iVal);					SetLightType( i3Gfx::Convert( (I3G_PERSIST_LIGHT_TYPE) iVal));
	
	i3Color::GetFromXML( pXML, "Diffuse", &m_DiffuseColor);
	i3Color::GetFromXML( pXML, "Specular", &m_SpecularColor);

	pXML->getAttr( "InnerAngle", &m_InnerAngle);
	pXML->getAttr( "OuterAngle", &m_OuterAngle);
	pXML->getAttr( "Range",		&m_Range);
	pXML->getAttr( "ShadowMapSize",	&m_ShadowMapSize);
	pXML->getAttr( "Group",			&m_Group);
	pXML->getAttr( "Style",			&m_Style);

	pXML->getAttr( "PatternMap", &iVal);
	
	if( iVal > 0)
	{
		i3Texture * pTex = (i3Texture *) pFile->FindObjectByID( (OBJREF) iVal);
		I3ASSERT( pTex != nullptr);

		setPatternMap( pTex);
	}

	return true;
}

