#include "i3SceneDef.h"
#include "i3Sprite.h"

I3_CLASS_INSTANCE( i3Sprite);

i3Sprite::i3Sprite(void)
{
	m_pSprite = i3SpriteAttr::new_object();
	I3ASSERT( m_pSprite != nullptr);
	m_pSprite->Create( 1, true, true, true);
	m_pSprite->SetEnable( 0, true);
	m_pSprite->SetCenter( 0, 0.0f, 0.0f, 0.0f);
	m_pSprite->SetSize( 0, 1.0f, 1.0f);
	m_pSprite->SetColor( 0, 255, 255, 255, 255);
	m_pSprite->SetTextureCoord( 0, 0.0f, 0.0f, 1.0f, 1.0f);

	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	AddChild(m_pGeomery);
	m_pGeomery->AppendGeometryAttr(m_pSprite);

	m_pTexEnableAttr = i3TextureEnableAttr::new_object();
	I3ASSERT(m_pTexEnableAttr);
	m_pTexEnableAttr->Set(true);
	AddAttr(m_pTexEnableAttr);

	m_pTextureBindAttr = i3TextureBindAttr::new_object();
	I3ASSERT( m_pTextureBindAttr != nullptr);
	AddAttr( m_pTextureBindAttr);
	
	m_pBlendEnableAttr = i3BlendEnableAttr::new_object();
	m_pBlendEnableAttr->Set(true);
	AddAttr( m_pBlendEnableAttr);

	m_pBlendModeAttr = i3BlendModeAttr::new_object();
	I3ASSERT( m_pBlendModeAttr != nullptr);
	AddAttr( m_pBlendModeAttr);	
	m_pBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA);
	m_pBlendModeAttr->SetDestination(I3G_BLEND_INVSRCALPHA);

	m_pAlphaTestAttr = i3AlphaTestEnableAttr::new_object();
	m_pAlphaTestAttr->Set(true);
	AddAttr( m_pAlphaTestAttr);
}

i3Sprite::~i3Sprite(void)
{
	I3_SAFE_RELEASE( m_pSprite);
	I3_SAFE_RELEASE( m_pTexture);
	I3_SAFE_RELEASE( m_pGeomery);
	I3_SAFE_RELEASE( m_pTexEnableAttr);
	I3_SAFE_RELEASE( m_pTextureBindAttr);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pAlphaTestAttr);
}

void i3Sprite::SetSrcBlend( I3G_BLEND blend)
{
	m_SrcBlend = blend;

	m_pBlendModeAttr->SetSource( m_SrcBlend);
}

void i3Sprite::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	m_pBlendModeAttr->SetDestination( m_DestBlend);
}

void i3Sprite::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pTexture != nullptr)
	{
		m_pTexture->OnBuildObjectList( List);
	}

	i3AttrSet::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED SPRITE_INFO_1
	{
		UINT8				m_ID[4] = { 'B', 'B', 'D', '1' };
		UINT32				m_Count = 0;
		OBJREF				m_Texture = 0;
		UINT8				m_SrcBlend = 0;
		UINT8				m_DestBlend = 0;
		UINT8				m_Reserved[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Sprite::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SPRITE_INFO_1 info;

	Result = i3AttrSet::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	info.m_Count = m_Count;
	info.m_Texture = (OBJREF) pResFile->GetObjectPersistID( m_pTexture);
	info.m_SrcBlend = (UINT8) m_SrcBlend;
	info.m_DestBlend = (UINT8) m_DestBlend;

	Rc = pStream->Write( &info, sizeof( info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Sprite::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;


	return Result;
}

UINT32 i3Sprite::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Sprite::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SPRITE_INFO_1 info;

	Result = i3AttrSet::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &info, sizeof(info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Sprite::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;


	// Check Magin number
	// info.m_ID

	SetCount(info.m_Count);

	// Set Texture
	{
		i3Texture * pTex;
		if( info.m_Texture != 0)
		{
			pTex = (i3Texture *) pResFile->FindObjectByID( (UINT32) info.m_Texture);
			I3ASSERT( pTex != nullptr);

			SetTexture( pTex);
		}
	}

	SetSrcBlend( (I3G_BLEND)info.m_SrcBlend);
	SetDestBlend( (I3G_BLEND)info.m_DestBlend);

	return Result;
}

void i3Sprite::Create( UINT32 Count, bool bUV, bool bColor, bool bAlwaysUpdate)
{
	m_pGeomery->RemoveGeometryAttr(m_pSprite);
	I3_MUST_RELEASE( m_pSprite);

	m_Count = Count;
	m_pSprite = i3SpriteAttr::new_object();
	m_pSprite->Create( m_Count, bUV, bColor, bAlwaysUpdate);

	for(INT32 i=0;i<m_Count;i++)
	{
		m_pSprite->SetEnable( i, false);
		m_pSprite->SetCenter( i, 0.0f, 0.0f, 0.0f);
		m_pSprite->SetSize( i, 1.0f, 1.0f);
		m_pSprite->SetColor( i, 255,255,255,255);
		m_pSprite->SetTextureCoord( i, 0.0f, 0.0f, 1.0f, 1.0f);
	}

	m_pGeomery->AppendGeometryAttr(m_pSprite);
}

void i3Sprite::SetCount(INT32 count)
{
	m_pGeomery->RemoveGeometryAttr(m_pSprite);
    I3_MUST_RELEASE( m_pSprite);

	m_Count = count;
	m_pSprite = i3SpriteAttr::new_object();
	m_pSprite->Create( m_Count, true, true, false);

	for(INT32 i=0;i<m_Count;i++)
	{
		m_pSprite->SetEnable( i, false);
		m_pSprite->SetCenter( i, 0.0f, 0.0f, 0.0f);
		m_pSprite->SetSize( i, 1.0f, 1.0f);
		m_pSprite->SetColor( i, 255,255,255,255);
		m_pSprite->SetTextureCoord( i, 0.0f, 0.0f, 1.0f, 1.0f);
	}
	m_pGeomery->AppendGeometryAttr(m_pSprite);
}

void i3Sprite::SetRect( UINT32 Index, REAL32 x, REAL32 y, REAL32 width, REAL32 height)
{
	REAL32 CenterX, CenterY;

	#if defined( I3_FIXED_POINT)
		CenterX = x + (width >> 1);		// width  / 2
		CenterY = y + (height >> 1);		// height / 2
	#else
		CenterX = x + width*0.5f;		// width  / 2
		CenterY = y + height*0.5f;		// height / 2
	#endif

	SetCenter(Index, CenterX, CenterY, 0.0f);
	SetSize(Index, width, height);
}

void i3Sprite::SetTexture(i3Texture * pTex)
{
	I3_REF_CHANGE( m_pTexture, pTex);
	m_pTextureBindAttr->SetTexture( m_pTexture);
}

void i3Sprite::SetTexture(const char * TextureName)
{
	i3Texture * pTex;
	pTex = i3Texture::new_object();
	if( pTex->Create(TextureName) == false)
	{
		I3ASSERT_0;
	}
	I3_SAFE_RELEASE( m_pTexture);
	m_pTexture = pTex;
	m_pTextureBindAttr->SetTexture( m_pTexture);
}

void i3Sprite::SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
{
	m_pSprite->SetTextureCoord( Index, pMin, pMax);
}

void i3Sprite::SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
	m_pSprite->SetTextureCoord( Index, u1, v1, u2, v2);
}
