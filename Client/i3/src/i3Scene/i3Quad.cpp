#include "i3SceneDef.h"
#include "i3Quad.h"

I3_CLASS_INSTANCE( i3Quad);

i3Quad::i3Quad(void)
{
	m_pQuad = i3QuadAttr::new_object();
	I3ASSERT( m_pQuad != nullptr);

	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	AddChild(m_pGeomery);
	m_pGeomery->AppendGeometryAttr(m_pQuad);


	m_pTexEnableAttr = i3TextureEnableAttr::new_object();
	m_pTexEnableAttr->Set(false);
	AddAttr( m_pTexEnableAttr);

	m_pTextureBindAttr = i3TextureBindAttr::new_object();
	I3ASSERT( m_pTextureBindAttr != nullptr);
	AddAttr( m_pTextureBindAttr);
	
	{
		m_pBlendEnableAttr = i3BlendEnableAttr::new_object();
		m_pBlendEnableAttr->Set(true);

		AddAttr( m_pBlendEnableAttr);
	}

	m_pBlendModeAttr = i3BlendModeAttr::new_object();
	I3ASSERT( m_pBlendModeAttr != nullptr);
	AddAttr( m_pBlendModeAttr);

	m_pBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA);
	m_pBlendModeAttr->SetDestination(I3G_BLEND_INVSRCALPHA);

	m_pAlphaTestAttr = i3AlphaTestEnableAttr::new_object();
	m_pAlphaTestAttr->Set(true);
	AddAttr( m_pAlphaTestAttr);
}

i3Quad::~i3Quad(void)
{
	I3_SAFE_RELEASE( m_pQuad);
	I3_SAFE_RELEASE( m_pTexture);
	I3_SAFE_RELEASE( m_pGeomery);
	I3_SAFE_RELEASE( m_pTexEnableAttr);
	I3_SAFE_RELEASE( m_pTextureBindAttr);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pAlphaTestAttr);
}

void i3Quad::SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
{
#ifdef I3G_DX
	if(m_Space == I3SG_QUAD_SPACE_SCREEN)
	{
		I3ASSERT( m_pTexture != nullptr );
		I3ASSERT( m_pTexture->GetWidth() > 0 );
		I3ASSERT( m_pTexture->GetHeight() > 0 );

		VEC2D vMin, vMax;
		REAL32 rcx, rcy;

		#if defined( I3_FIXED_POINT)
			rcx = m_pTexture->GetWidth();		// 정도를 유지하기 위한 조치
			rcy = m_pTexture->GetHeight();

			i3Vector::SetU( &vMin, i3Vector::GetU(pMin) / rcx);
			i3Vector::SetV( &vMin, i3Vector::GetV(pMin) / rcy);

			i3Vector::SetU( &vMax, i3Vector::GetU(pMax) / rcx);
			i3Vector::SetV( &vMax, i3Vector::GetV(pMax) / rcy);
		#else
			rcx = i3Math::div( 1.0f, (REAL32)m_pTexture->GetWidth());
			rcy = i3Math::div( 1.0f, (REAL32)m_pTexture->GetHeight());

			i3Vector::SetU( &vMin, i3Math::Mul( i3Vector::GetU(pMin), rcx));
			i3Vector::SetV( &vMin, i3Math::Mul( i3Vector::GetV(pMin), rcy));

			i3Vector::SetU( &vMax, i3Math::Mul( i3Vector::GetU(pMax), rcx));
			i3Vector::SetV( &vMax, i3Math::Mul( i3Vector::GetV(pMax), rcy));
		#endif

		m_pQuad->SetTextureCoord( Index, &vMin, &vMax);
	}
	else
#endif
	{
		m_pQuad->SetTextureCoord( Index, pMin, pMax);
	}
}

void i3Quad::SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
#ifdef I3G_DX
	if(m_Space == I3SG_QUAD_SPACE_SCREEN)
	{
		REAL32 rcx, rcy;

		#if defined( I3_FIXED_POINT)
			// 정도를 유지하기 위한 조치
			rcx = m_pTexture->GetWidth();
			rcy = m_pTexture->GetHeight();

			u1 = u1 / rcx;
			v1 = v1 / rcy;
			u2 = u2 / rcx;
			v2 = v2 / rcy;
		#else
			rcx = i3Math::div( 1.0f, (REAL32)m_pTexture->GetWidth());
			rcy = i3Math::div( 1.0f, (REAL32)m_pTexture->GetHeight());

			u1 = i3Math::Mul( u1, rcx);
			v1 = i3Math::Mul( v1, rcy);
			u2 = i3Math::Mul( u2, rcx);
			v2 = i3Math::Mul( v2, rcy);
		#endif
		m_pQuad->SetTextureCoord( Index, u1, v1, u2, v2);
	}
	else
#endif
	{
		m_pQuad->SetTextureCoord( Index, u1, v1, u2, v2);
	}
}

void i3Quad::SetSrcBlend( I3G_BLEND blend)
{
	m_SrcBlend = blend;

	m_pBlendModeAttr->SetSource( m_SrcBlend);
}

void i3Quad::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	m_pBlendModeAttr->SetDestination( m_DestBlend);
}

void i3Quad::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
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
	struct PACKED QUAD_INFO_1
	{
		UINT8				m_ID[4] = { 'B', 'B', 'D', '2' };
		UINT32				m_Count = 0;
		OBJREF				m_Texture = 0;
		UINT8				m_SrcBlend = 0;
		UINT8				m_DestBlend = 0;
		UINT8				m_Space = 0;
		UINT8				m_bUV = 0;
		UINT8				m_bColor = 0;
		UINT8				m_bAlwaysUpdate = 0;
		UINT8				m_Reserved[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Quad::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::QUAD_INFO_1 info;

	Result = i3AttrSet::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	info.m_Count = m_Count;
	info.m_Texture = (OBJREF) pResFile->GetObjectPersistID( m_pTexture);
	info.m_SrcBlend = (UINT8) m_SrcBlend;
	info.m_DestBlend = (UINT8) m_DestBlend;
	info.m_Space = (UINT8) m_Space;
	
	if( m_pQuad->GetVertexArray()->GetFormat()->GetTextureCoordSetCount() > 0) info.m_bUV = 1;
	else info.m_bUV = 0;

	if( m_pQuad->GetVertexArray()->GetFormat()->GetHasColor() ) info.m_bColor = 1;
	else info.m_bColor = 0;
	
	info.m_bAlwaysUpdate = (UINT8) m_bAlwaysUpdate;
	info.m_bColor = (UINT8) m_DestBlend;
	info.m_bAlwaysUpdate = (UINT8) m_DestBlend;


	Rc = pStream->Write( &info, sizeof( info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Quad::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3Quad::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Quad::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::QUAD_INFO_1 info;

	Result = i3AttrSet::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &info, sizeof(info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Quad::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;


	// Check Magin number
	// info.m_ID

	if( info.m_ID[3] < '2')
	{
		Create(info.m_Count, I3SG_QUAD_SPACE_WORLD, true, true, true);
	}
	else
	{
		Create( info.m_Count, (I3SG_QUAD_SPACE) info.m_Space,  info.m_bUV != 0, info.m_bColor != 0, info.m_bAlwaysUpdate != 0 );
	}

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

void i3Quad::Create( UINT32 Count, I3SG_QUAD_SPACE Space, bool bUV, bool bColor, bool bAlwaysUpdate)
{
	m_Count = Count;
	m_Space = Space;
	m_bAlwaysUpdate = bAlwaysUpdate;
	m_pQuad->Create( m_Count, m_Space, bUV, bColor, m_bAlwaysUpdate);
	 
	for(INT32 i=0;i<m_Count;i++)
		m_pQuad->SetEnable( i, false);

	m_pGeomery->AppendGeometryAttr(m_pQuad);
}

void i3Quad::SetRect( UINT32 Index, REAL32 x, REAL32 y, REAL32 width, REAL32 height)
{
	REAL32 CenterX, CenterY;

	CenterX = x + i3Math::Mul( width, 0.5f);		// width  / 2
	CenterY = y + i3Math::Mul( height, 0.5f);		// height / 2

	SetCenter(Index, CenterX, CenterY, 0.0f);
	SetSize(Index, width, height);
}

void i3Quad::GetRect( UINT32 Index, VEC4D * pRect )
{
	VEC3D * pCenter;
	VEC2D Size;
	
	pCenter = GetCenter( Index );
	GetSize( Index, &Size);

	i3Vector::SetX( pRect, i3Vector::GetX(pCenter) - i3Math::Mul( i3Vector::GetX(&Size), 0.5f));
	i3Vector::SetY( pRect, i3Vector::GetY(pCenter) - i3Math::Mul( i3Vector::GetY(&Size), 0.5f));
	i3Vector::SetZ( pRect, i3Vector::GetX(&Size) );
	i3Vector::SetW( pRect, i3Vector::GetY(&Size) );
}


void i3Quad::SetTexture(i3Texture * pTex)
{
	I3_REF_CHANGE( m_pTexture, pTex);
	
	m_pTexEnableAttr->Set( ( m_pTexture != nullptr) );
	m_pTextureBindAttr->SetTexture( m_pTexture);
}

void i3Quad::SetTexture(const char * TextureName)
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
	m_pTexEnableAttr->Set( ( m_pTexture != nullptr) );
}

