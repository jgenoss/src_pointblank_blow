#include "i3FrameworkPCH.h"
#include "i3UIDummyAttrSet.h"

#include "i3Framework.h"

I3_CLASS_INSTANCE( i3UIDummyAttrSet);

i3UIDummyAttrSet::i3UIDummyAttrSet()
{
	addStyle( I3UI_STYLE_NO_VT);
}

i3UIDummyAttrSet::~i3UIDummyAttrSet()
{
	I3_SAFE_NODE_RELEASE( m_pAttrSet);

	resetSprites( true);
}

/*virtual*/ void i3UIDummyAttrSet::prepareSprites()
{
	if( m_hSprites != nullptr)
	{
		if( m_bModifiedSetting)
		{
			resetSprites( false);
			m_bModifiedSetting = false;
		}
	}

	if( m_hSprites == nullptr)
		m_hSprites = (INT32*)i3MemAlloc( sizeof( INT32));

	*m_hSprites = g_pUIRenderer->AllocSprite( I3UI_SPRITE_STYLE_NODE, this);
	I3ASSERT( *m_hSprites >= 0);

	I3ASSERT( m_pAttrSet);
	g_pUIRenderer->getSprite( *m_hSprites)->setNode( m_pAttrSet);

	m_bResetSprites = false;
}

/*virtual*/ void i3UIDummyAttrSet::OnUpdate( REAL32 rDeltaSeconds)
{
	bool bEnable = isEnable() && isReadyToShow() && isVisible();

	if( m_pAttrSet != nullptr)
	{
		if( bEnable)
			m_pAttrSet->RemoveFlag( I3_NODEFLAG_DISABLE);
		else
			m_pAttrSet->AddFlag( I3_NODEFLAG_DISABLE);
	}

	i3GameNode::OnUpdate( rDeltaSeconds);
}

/*virtual*/ void i3UIDummyAttrSet::OnVisibleScene( void)
{
	i3UIControl::OnVisibleScene();

	if( m_pAttrSet != nullptr)
	{
		m_pAttrSet->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
}

/*virtual*/ void i3UIDummyAttrSet::OnInvisibleScene(void)
{
	i3UIControl::OnInvisibleScene();

	if( m_pAttrSet != nullptr)
	{
		m_pAttrSet->AddFlag( I3_NODEFLAG_DISABLE);
	}
}

void i3UIDummyAttrSet::SetDummy( i3Node * pParent, i3Node * pResource)
{
	I3_SAFE_NODE_RELEASE( m_pAttrSet);

	if( pResource == nullptr)
		return;

	m_pAttrSet = i3AttrSet::new_object();
	I3ASSERT( m_pAttrSet != nullptr);

	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pAttrSet->AddAttr( pAttr);
	}

	m_pAttrSet->AddChild( pResource);

	pParent->AddChild( m_pAttrSet);

	m_bModifiedSetting = true;

	prepareSprites();
}
