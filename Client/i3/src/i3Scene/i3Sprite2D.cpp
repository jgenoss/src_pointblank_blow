#include "i3SceneDef.h"
#include "i3Sprite2D.h"

I3_CLASS_INSTANCE( i3Sprite2D);

i3Sprite2D::i3Sprite2D(void)
{
	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	AddChild(m_pGeomery);

	m_pTextureBindAttr = i3TextureBindAttr::new_object();
	I3ASSERT( m_pTextureBindAttr != nullptr);
	AddAttr( m_pTextureBindAttr);

	m_pTexEnableAttr = i3TextureEnableAttr::new_object();
	I3ASSERT(m_pTexEnableAttr);
	m_pTexEnableAttr->Set(true);
	AddAttr(m_pTexEnableAttr);
	
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

i3Sprite2D::~i3Sprite2D(void)
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

void i3Sprite2D::SetSrcBlend( I3G_BLEND blend)
{
	m_SrcBlend = blend;

	m_pBlendModeAttr->SetSource( m_SrcBlend);
}

void i3Sprite2D::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	m_pBlendModeAttr->SetDestination( m_DestBlend);
}

void i3Sprite2D::SetTextureFilter( I3G_TEXTURE_FILTER minFilter, I3G_TEXTURE_FILTER magFilter)
{
	if( m_pTexFilterAttr == nullptr)
	{
		m_pTexFilterAttr = i3TextureFilterAttr::new_object_ref();

		AddAttr( m_pTexFilterAttr);
	}

	m_pTexFilterAttr->SetMinFilter( minFilter);
	m_pTexFilterAttr->SetMagFilter( magFilter);
}

void i3Sprite2D::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
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

UINT32 i3Sprite2D::OnSave( i3ResourceFile * pResFile)
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
		i3Log( "i3Sprite2D::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;


	return Result;
}

UINT32 i3Sprite2D::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Sprite2D::OnLoad( i3ResourceFile * pResFile)\n" );
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
		i3Log( "i3Sprite2D::OnLoad()", "Could not read data.");
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

void i3Sprite2D::OnInitAfterLoad()
{
	i3Node::OnInitAfterLoad();

	// load후 Texture가 없으면 필요없는 데이터 삭제..
	if( m_pTexture == nullptr)
	{
		I3_MUST_ADDREF(this);
		I3_MUST_RELEASE(this);
	}
}

void i3Sprite2D::Create( UINT32 Count, bool bUV, bool bColor, bool bUseVertexBank)
{
	if( m_pSprite != nullptr)
	{
		m_pGeomery->RemoveGeometryAttr(m_pSprite);
		I3_MUST_RELEASE( m_pSprite);
	}

	m_Count = Count;
	m_pSprite = i3Sprite2DAttr::new_object();
	m_pSprite->Create( m_Count, bUV, bColor, bUseVertexBank);

	m_pGeomery->AppendGeometryAttr(m_pSprite);

	// 텍스쳐가 없으면 비활성화
	m_pTexEnableAttr->Set( bUV );
}

void i3Sprite2D::SetCount(INT32 count)
{
	if( m_pSprite != nullptr)
	{
		m_pGeomery->RemoveGeometryAttr(m_pSprite);
		I3_MUST_RELEASE( m_pSprite);
	}

	m_Count = count;
	m_pSprite = i3Sprite2DAttr::new_object();
	m_pSprite->Create( m_Count, true, true);

	m_pGeomery->AppendGeometryAttr(m_pSprite);
}

void i3Sprite2D::SetRect( UINT32 Index, REAL32 x, REAL32 y, REAL32 width, REAL32 height)
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

void i3Sprite2D::SetTexture(i3Texture * pTex)
{
	I3_REF_CHANGE( m_pTexture, pTex);

	m_pTextureBindAttr->SetTexture( m_pTexture);
}

void i3Sprite2D::SetTexture(const char * TextureName)
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

void i3Sprite2D::SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
{
#if ! defined( I3G_PSP)
	// Texel 좌표계를 사용하며, PSP 이외에는 모두 실수형(0~1)을 사용하기 때문에...
	{
		I3ASSERT( m_pTexture != nullptr );
		I3ASSERT( m_pTexture->GetWidth() > 0 );
		I3ASSERT( m_pTexture->GetHeight() > 0 );

		VEC2D vMin, vMax;
		REAL32 px, py;

		px = 1.0f / m_pTexture->GetWidth();
		py = 1.0f / m_pTexture->GetHeight();

		i3Vector::SetU( &vMin,	i3Vector::GetU(pMin) * px + (px * 0.5f));
		i3Vector::SetV( &vMin,  i3Vector::GetV(pMin) * py + (py * 0.5f));

		i3Vector::SetU( &vMax,	i3Vector::GetU(pMax) * px - (px * 0.5f));
		i3Vector::SetV( &vMax,  i3Vector::GetV(pMax) * py - (py * 0.5f));

		m_pSprite->SetTextureCoord( Index, &vMin, &vMax);
	}
#else
	m_pSprite->SetTextureCoord( Index, pMin, pMax);
#endif
}

void i3Sprite2D::SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
#if ! defined( I3G_PSP)
	{
		REAL32 px, py;

		px = 1.0f / m_pTexture->GetWidth();
		py = 1.0f / m_pTexture->GetHeight();

		u1 = u1 * px + (px * 0.5f);
		v1 = v1 * py + (py * 0.5f);
		u2 = u2 * px - (px * 0.5f);
		v2 = v2 * py - (py * 0.5f);

		m_pSprite->SetTextureCoord( Index, u1, v1, u2, v2);
	}
#else
	m_pSprite->SetTextureCoord( Index, u1, v1, u2, v2);
#endif
}

void i3Sprite2D::SetTextureCoord2( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 width, REAL32 height)
{
#if ! defined( I3G_PSP)
	{
		REAL32 px, py;
		REAL32 u2, v2;

		u2 = u1 + width;
		v2 = v1 + height;

		px = 1.0f / m_pTexture->GetWidth();
		py = 1.0f / m_pTexture->GetHeight();

		u1 = u1 * px + (px * 0.5f);
		v1 = v1 * py + (py * 0.5f);
		u2 = u2 * px - (px * 0.5f);
		v2 = v2 * py - (py * 0.5f);

		m_pSprite->SetTextureCoord( Index, u1, v1, u2, v2);
	}
#else
	m_pSprite->SetTextureCoord( Index, u1, v1, u2, v2);
#endif
}

void i3Sprite2D::Set( INT32 idx, INT32 x, INT32 y, INT32 w, INT32 h, INT32 u, INT32 v)
{
	SetRect( (UINT32) idx, (REAL32) x, (REAL32) y, (REAL32)w, (REAL32) h);
	SetTextureCoord( (UINT32) idx, (REAL32) u, (REAL32) v, (REAL32)(u + w), (REAL32)(v + h));
}

