#include "i3SceneDef.h"
#include "i3TextNode.h"

I3_CLASS_INSTANCE( i3TextNode);

i3TextNode::~i3TextNode(void)
{
	I3_SAFE_RELEASE( m_pTextAttr);
	I3_SAFE_RELEASE( m_pGeomery);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pAlphaTestAttr);
}


bool i3TextNode::Create(  i3Font *pFont, UINT32 nMaxCount, bool bAlwaysUpdate )
{
	I3ASSERT( m_pTextAttr == nullptr);
	m_pTextAttr = i3TextAttr::new_object();
	I3ASSERT( m_pTextAttr != nullptr);
	if( ! m_pTextAttr->Create( pFont, nMaxCount, bAlwaysUpdate ) )
	{
		I3TRACE( "%s (%d) : [i3TextNode::Create]\n", __FILE__, __LINE__ );
		I3ASSERT_0;
		return false;
	}

	I3ASSERT( m_pGeomery == nullptr);
	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	AddChild(m_pGeomery);
	m_pGeomery->AppendGeometryAttr(m_pTextAttr);

	m_pBlendEnableAttr = i3BlendEnableAttr::new_object();
	m_pBlendEnableAttr->Set( true );
	AddAttr( m_pBlendEnableAttr );

	m_pBlendModeAttr = i3BlendModeAttr::new_object();
	I3ASSERT( m_pBlendModeAttr != nullptr );
	AddAttr( m_pBlendModeAttr );	
	m_pBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA );
	m_pBlendModeAttr->SetDestination( I3G_BLEND_INVSRCALPHA );

	m_pAlphaTestAttr = i3AlphaTestEnableAttr::new_object();
	m_pAlphaTestAttr->Set( true );
	AddAttr( m_pAlphaTestAttr );

	return true;
}

void i3TextNode::SetBlendEnable( bool bTrue )
{
	m_pBlendEnableAttr->Set( bTrue );
}

void i3TextNode::SetSrcBlend( I3G_BLEND blend)
{
	m_SrcBlend = blend;

	m_pBlendModeAttr->SetSource( m_SrcBlend);
}

void i3TextNode::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	m_pBlendModeAttr->SetDestination( m_DestBlend);
}

void i3TextNode::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
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
		UINT8				m_SrcBlend = 0;
		UINT8				m_DestBlend = 0;
		UINT8				m_Reserved[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SPRITE_INFO_1 info;

	Result = i3AttrSet::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	info.m_SrcBlend = (UINT8) m_SrcBlend;
	info.m_DestBlend = (UINT8) m_DestBlend;

	Rc = pStream->Write( &info, sizeof( info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextNode::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;


	return Result;
}

UINT32 i3TextNode::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextNode::OnLoad( i3ResourceFile * pResFile)\n" );
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
		i3Log( "i3TextNode::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_pBlendModeAttr != nullptr)
	{
		SetSrcBlend( (I3G_BLEND)info.m_SrcBlend);
		SetDestBlend( (I3G_BLEND)info.m_DestBlend);
	}

	return Result;
}
