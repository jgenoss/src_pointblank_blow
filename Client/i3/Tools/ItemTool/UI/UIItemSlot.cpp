#include "StdAfx.h"
#include "UIItemSlot.h"
#include "i3Base/string/ext/mb_to_utf16.h"


cUIItemSlot::cUIItemSlot(void)
:	m_pSpriteImage(NULL)
,	m_pTextName(NULL)
,	m_pBgTextName(NULL)
,	m_pSlotNode(NULL)
{
}

cUIItemSlot::~cUIItemSlot(void)
{
	Destroy();
}


bool cUIItemSlot::Create(i3Node* pSceneNode)
{
	if( pSceneNode == NULL )
		return false;

	m_pSlotNode = i3Node::new_object();
	pSceneNode->AddChild(m_pSlotNode);

	m_pBgSprite	= i3Sprite2D::new_object();
	m_pBgSprite->Create(1);
	m_pBgSprite->SetEnable(FALSE);
	m_pSlotNode->AddChild(m_pBgSprite);

	i3Color::Set(&m_BgColor,0.0f,0.3f,0.3f,1.0f);
	m_pBgSprite->SetColor(0,&m_BgColor);

 	m_pSpriteImage	= i3Sprite2D::new_object();
 	m_pSpriteImage->Create(1);
 	m_pSpriteImage->SetColor(0, 255, 255, 255, 255);
 	m_pSpriteImage->SetEnable(FALSE);
 	m_pSlotNode->AddChild(m_pSpriteImage);
	
	m_pBgTextName	= i3TextNodeDX2::new_object();
	m_pBgTextName->Create("¸¼Àº°íµñ", 256, 30, FW_MEDIUM, FALSE, FALSE, ANTIALIASED_QUALITY, FALSE);
	m_pBgTextName->SetText(L"");
	m_pBgTextName->SetColor(64, 64, 64, 255);
	m_pBgTextName->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	//m_pBgTextName->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pBgTextName->setAutoWrapEnable(TRUE);
	m_pBgTextName->SetEnable(TRUE);

	m_pSlotNode->AddChild(m_pBgTextName);

	m_pTextName	= i3TextNodeDX2::new_object();
	m_pTextName->Create("¸¼Àº°íµñ", 256, 30, FW_MEDIUM, FALSE, FALSE, ANTIALIASED_QUALITY, FALSE);
	m_pTextName->SetText(L"");
	m_pTextName->SetColor(255, 255, 255, 255);
	m_pTextName->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	//m_pTextName->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pTextName->setAutoWrapEnable(TRUE);
	m_pTextName->SetEnable(TRUE);

	m_pSlotNode->AddChild(m_pTextName);

	return true;
}

void cUIItemSlot::RecreateFont(INT32 nSize)
{
	//char* szText = m_pBgTextName->getText();

	m_pSlotNode->RemoveChild(m_pBgTextName);
	I3_SAFE_NODE_RELEASE(m_pBgTextName);

	m_pBgTextName	= i3TextNodeDX2::new_object();

	m_pBgTextName->Create("¸¼Àº°íµñ", 256, nSize, FW_BOLD, FALSE, FALSE, ANTIALIASED_QUALITY, FALSE);
	m_pBgTextName->SetText(L"");
	m_pBgTextName->SetColor(0, 0, 0, 255);
	m_pBgTextName->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	//m_pBgTextName->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pBgTextName->setAutoWrapEnable(TRUE);
	m_pBgTextName->SetEnable(TRUE);

	m_pSlotNode->AddChild(m_pBgTextName);

	//szText = m_pTextName->getText();

	m_pSlotNode->RemoveChild(m_pTextName);
	I3_SAFE_NODE_RELEASE(m_pTextName);

	m_pTextName	= i3TextNodeDX2::new_object();

	m_pTextName->Create("¸¼Àº°íµñ", 256, nSize, FW_BOLD, FALSE, FALSE, ANTIALIASED_QUALITY, FALSE);
	m_pTextName->SetText(L"");
	m_pTextName->SetColor(255, 255, 255, 255);
	m_pTextName->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	//m_pTextName->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pTextName->setAutoWrapEnable(TRUE);
	m_pTextName->SetEnable(TRUE);

	m_pSlotNode->AddChild(m_pTextName);
}
void cUIItemSlot::SetVisible(BOOL bVisible)
{
	m_pBgSprite->SetEnable(bVisible);
	m_pSpriteImage->SetEnable(bVisible);
	m_pTextName->SetEnable(bVisible);
	m_pBgTextName->SetEnable(bVisible);
}
void cUIItemSlot::Destroy(void)
{
	I3_SAFE_NODE_RELEASE( m_pSpriteImage );
	I3_SAFE_NODE_RELEASE( m_pTextName );
	I3_SAFE_NODE_RELEASE( m_pBgSprite );
	I3_SAFE_NODE_RELEASE(m_pSlotNode);
	I3_SAFE_NODE_RELEASE(m_pBgTextName);
}


void cUIItemSlot::SetRect(REAL32 x, REAL32 y, REAL32 width, REAL32 height, REAL32 ImageHeight, 
						 REAL32 SpriteWidth, REAL32 SpriteHeight)
{
	if (m_pBgSprite)
	{
		m_pBgSprite->SetRect( 0, x, y, width, height );
	}

	if( m_pTextName )
	{
		UINT32		nTextPosY		= static_cast<UINT32>(y + ImageHeight);
		UINT32		nTextHeight		= static_cast<UINT32>(height - ImageHeight);

		//ÃÖ¼ÒÅ©±â 18
		RecreateFont(18);
		m_pTextName->setPos( static_cast<UINT32>(x+5), nTextPosY);
		m_pTextName->setSize( static_cast<UINT32>(width), nTextHeight);

		m_pBgTextName->setPos( static_cast<UINT32>(x+8), nTextPosY+3);
		m_pBgTextName->setSize( static_cast<UINT32>(width), nTextHeight);
		
		
	}

	SetRectImage( x, y, width, ImageHeight, SpriteWidth, SpriteHeight );
}

void cUIItemSlot::SetRectImage(REAL32 x, REAL32 y, REAL32 width, REAL32 height, 
							   REAL32 SpriteWidth, REAL32 SpriteHeight)
{
	if( m_pSpriteImage )
	{
		REAL32		PosX			= x + (width - SpriteWidth) / 2.0f;
		REAL32		PosY			= y + (height - SpriteHeight) / 2.0f;

		m_pSpriteImage->SetRect(0, PosX, PosY, SpriteWidth, SpriteHeight);
	}
}


void cUIItemSlot::SetTexture(i3Texture* pTexture, CRect& rect)
{
	if( pTexture && m_pSpriteImage )
	{
  		m_pSpriteImage->SetTexture(pTexture);
  		m_pSpriteImage->SetTextureCoord( 0, 
  			static_cast<REAL32>(rect.left),
  			static_cast<REAL32>(rect.top),
  			static_cast<REAL32>(rect.right),
  			static_cast<REAL32>(rect.bottom) );

		m_pSpriteImage->SetEnable(0, TRUE);
	}
}
void cUIItemSlot::SetText(const char* pszName)
{
	if(m_pTextName)
	{
		if( pszName )
		{
			i3::wstring str;
			i3::mb_to_utf16( pszName, str);
			m_pTextName->SetText( str);
			m_pTextName->SetEnable(TRUE);

			m_pBgTextName->SetText( str);
			m_pBgTextName->SetEnable(TRUE);
		}
		else
		{
			m_pTextName->SetEnable(FALSE);
			m_pBgTextName->SetEnable(FALSE);
		}
	}
}

void cUIItemSlot::SetBgTexture(i3Texture* pTexture, CRect& rect)
{
	if( pTexture && m_pBgSprite )
	{
		m_pBgSprite->SetTexture(pTexture);
		m_pBgSprite->SetTextureCoord( 0, 
			static_cast<REAL32>(rect.left),
			static_cast<REAL32>(rect.top),
			static_cast<REAL32>(rect.right),
			static_cast<REAL32>(rect.bottom) );

		m_pBgSprite->SetEnable(0, TRUE);
	}
}

void cUIItemSlot::SetBgColor(bool bSelected)
{
	if( m_pBgSprite )
	{
		if( bSelected )
			i3Color::Set(&m_BgColor,0.7f,0.1f,0.1f,1.0f);
		else
			i3Color::Set(&m_BgColor,0.0f,0.3f,0.3f,1.0f);
		m_pBgSprite->SetColor(0,&m_BgColor);
	}
}