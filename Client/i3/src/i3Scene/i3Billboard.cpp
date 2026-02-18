#include "i3SceneDef.h"
#include "i3Billboard.h"

I3_CLASS_INSTANCE( i3Billboard);

i3Billboard::i3Billboard(void)
{
	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	AddChild(m_pGeomery);

	m_pTexEnableAttr = i3TextureEnableAttr::new_object();
	m_pTexEnableAttr->Set(true);
	AddAttr( m_pTexEnableAttr);

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

i3Billboard::~i3Billboard(void)
{
	I3_SAFE_RELEASE( m_pBillboard);
	I3_SAFE_RELEASE( m_pTexture);
	I3_SAFE_RELEASE( m_pGeomery);
	I3_SAFE_RELEASE( m_pTexEnableAttr);
	I3_SAFE_RELEASE( m_pTextureBindAttr);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pAlphaTestAttr);
}

void i3Billboard::SetSrcBlend( I3G_BLEND blend)
{
	m_SrcBlend = blend;

	m_pBlendModeAttr->SetSource( m_SrcBlend);
}

void i3Billboard::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	m_pBlendModeAttr->SetDestination( m_DestBlend);
}

void i3Billboard::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
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
	struct PACKED BILLBOARD_INFO_1
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

UINT32 i3Billboard::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::BILLBOARD_INFO_1 info;

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
		i3Log( "i3Billboard::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3Billboard::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Billboard::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::BILLBOARD_INFO_1 info;

	Result = i3AttrSet::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &info, sizeof(info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Billboard::OnLoad()", "Could not read data.");
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

void i3Billboard::SetCount(INT32 count)
{
	if( m_pBillboard != nullptr)
	{
		m_pGeomery->RemoveGeometryAttr(m_pBillboard);
		I3_MUST_RELEASE( m_pBillboard);
	}

	m_Count = count;

	m_pBillboard = i3BillboardAttr::new_object();
	m_pBillboard->Create( m_Count, I3SG_BILLBOARD_SPACE_WORLD, true, true, false);
	m_pGeomery->AppendGeometryAttr(m_pBillboard);
}

void i3Billboard::SetTexture(i3Texture * pTex)
{
	I3_REF_CHANGE( m_pTexture, pTex);

	m_pTextureBindAttr->SetTexture( m_pTexture);
}

void i3Billboard::SetTexture(const char * TextureName)
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

