#include "i3GuiPCH.h"
#include "i3GuiEditBox.h"
#include "i3GuiPopupList.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/generic_string_cat.h"

// ============================================================================
//
// i3GuiEditBox
//
// ============================================================================
I3_CLASS_INSTANCE( i3GuiEditBox);

INT32 i3GuiEditBox::m_nDefaultFontSize = 8;		//	폰트 크기 설정이 -1일 경우 기본 폰트 설정 komet

i3GuiEditBox::i3GuiEditBox()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

i3GuiEditBox::~i3GuiEditBox()
{
	I3_SAFE_RELEASE( m_pCaretSprite);
	I3_SAFE_RELEASE( m_pText);

#if defined ( I3G_DX)
	I3_SAFE_RELEASE( m_pTextDX);
#endif
	
#if defined (I3_WINDOWS)
	I3_SAFE_RELEASE( m_pIME);
#endif
}

#if defined( I3_WINDOWS)
void i3GuiEditBox::CreateText( i3Font* pFont, INT32 TextCount, INT32 TextSize, bool bIMEEnable, CREATE_IME_ELEMENT* pStruct, UINT32 TextStyle, bool bShadowEnable, FONT_SHADOW_QUALITY ShadowQuality)
{
	m_FontSetInfo.SetFontName(i3Gui::GetDefaultFontName());
	m_nMaxTextCount = TextCount;

	if( TextSize == -1)
		m_nTextSize = m_nDefaultFontSize;
	else
		m_nTextSize = TextSize;

	m_TextStyle = TextStyle;
	m_bShadowEnable = bShadowEnable;
	m_ShadowQuality = ShadowQuality;

#if defined (I3_WINDOWS)
	// IME 생성
	if( bIMEEnable)
	{
		if( m_pIME) I3_SAFE_RELEASE(m_pIME);

		m_pIME = i3IME::new_object();
		
		m_pIME->CreateIME(pStruct);
		m_pIME->setOwner( this);

		{
			m_pIME->SetWindowPosition( (INT32)(getTotalPosX()), (INT32)getTotalPosY());
		}

		// Caret 생성
		CreateCaret();
	}
#endif

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{		
		REAL32 ry = 1.0f;
		
		if( i3GuiRoot::getViewer() != nullptr)
			ry = i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;

		if( !i3GuiRoot::isChangeResolution())
		{	
			ry = 1.0f;
		}

		INT32 fontSize = (INT32) (m_nTextSize * ry);
		UINT32 weight = _CalcTextWeight();

		m_pTextDX = i3TextNodeDX2::new_object();
	#if USE_VERTEX_BANK
		m_pTextDX->Create( &m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, true);
	#else
		m_pTextDX->Create( &m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, false);
	#endif
		m_pTextDX->setAutoWrapEnable(true);

		m_pTextDX->setPos((UINT32)((getTotalPosX()+m_fOffsetPosX)), (UINT32)((getTotalPosY()+m_fOffsetPosY)) );
		SetTextRectSize( m_nOriginalWidth, m_nOriginalHeight);
		m_nEditOriginalWidth = m_nOriginalWidth;
		m_nEditOriginalHeight = m_nOriginalHeight;

		GetNode()->AddChild( m_pTextDX);

		if( m_pIME != nullptr)
		{
			i3GfxFontDX2 * pFont = m_pTextDX->getFont();
			if( pFont != nullptr)
			{
				LOGFONTA * pLogFont = pFont->getLogFont();
				if( pLogFont != nullptr)
				{
					m_pIME->SetIMEFont( pFont->getHFont(), pLogFont);
				}
			}
		}
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		// Font 생성
		m_pFont = pFont;
		m_pText = i3TextNode::new_object();
		m_pText->Create(m_pFont, TextCount, true);
		m_pText->SetText(L"");
		m_pText->SetPos(0.0f, 0.0f);
		m_pText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pText->SetColor(255, 255, 255, 255);
		SetTextRectSize( m_nWidth, m_nHeight);
		GetNode()->AddChild(m_pText);
	}

	m_bUpdated = true;
}

void i3GuiEditBox::ReCreateText( i3Font* pFont, INT32 TextCount, INT32 TextSize, bool bIMEEnable, CREATE_IME_ELEMENT* pStruct, UINT32 TextStyle, bool bShadowEnable, FONT_SHADOW_QUALITY ShadowQuality)
{	
	m_FontSetInfo.SetFontName(i3Gui::GetDefaultFontName());
	m_nMaxTextCount = TextCount;
	
	if( TextSize == -1)
		m_nTextSize = m_nDefaultFontSize;
	else
		m_nTextSize = TextSize;

	m_TextStyle = TextStyle;
	m_bShadowEnable = bShadowEnable;
	m_ShadowQuality = ShadowQuality;

#if defined (I3_WINDOWS)
	// IME 생성
	if( bIMEEnable)
	{
		if( m_pIME) I3_SAFE_RELEASE(m_pIME);

		m_pIME = i3IME::new_object();

		m_pIME->CreateIME(pStruct);
		m_pIME->setOwner( this);

		{
			m_pIME->SetWindowPosition( (INT32)(getTotalPosX()), (INT32)getTotalPosY());
		}


		// Caret 생성
		CreateCaret();
	}
#endif

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{	
		REAL32 ry = 1.0f;

		if( i3GuiRoot::getViewer() != nullptr)
			ry = i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;

		if( !i3GuiRoot::isChangeResolution())
		{	
			ry = 1.0f;
		}

		INT32 fontSize = (INT32) (m_nTextSize * ry);
		UINT32 weight = _CalcTextWeight();

		if(m_pTextDX)
		{
			GetNode()->RemoveChild( m_pTextDX);
			I3_SAFE_RELEASE( m_pTextDX);
		}
		m_pTextDX = i3TextNodeDX2::new_object();
	#if USE_VERTEX_BANK
		m_pTextDX->Create(&m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, true);
	#else
		m_pTextDX->Create(&m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, false);
	#endif
		m_pTextDX->setAutoWrapEnable(true);

		m_pTextDX->setPos((UINT32)((getTotalPosX()+m_fOffsetPosX)), (UINT32)((getTotalPosY()+m_fOffsetPosY)) );
		SetTextRectSize( m_nOriginalWidth, m_nOriginalHeight);
		m_nEditOriginalWidth = m_nOriginalWidth;
		m_nEditOriginalHeight = m_nOriginalHeight;

		GetNode()->AddChild( m_pTextDX);

		if( m_pIME != nullptr)
		{
			i3GfxFontDX2 * pF = m_pTextDX->getFont();
			if( pF != nullptr)
			{
				LOGFONTA * pLogFont = pF->getLogFont();
				if( pLogFont != nullptr)
				{
					m_pIME->SetIMEFont( pF->getHFont(), pLogFont);
				}
			}
		}
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		// Font 생성
		if(m_pText)	
		{
			GetNode()->RemoveChild( m_pText);
			I3_SAFE_RELEASE( m_pText);
		}

		m_pFont = pFont;
		m_pText = i3TextNode::new_object();
		m_pText->Create(m_pFont, TextCount, true);
		m_pText->SetText(L"");	
		m_pText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);		
		m_pText->SetColor(255, 255, 255, 255);		
		SetTextRectSize( m_nWidth, m_nHeight);	
		m_pText->SetPos((getTotalPosX()+m_fOffsetPosX), (getTotalPosY()+m_fOffsetPosY));		
		GetNode()->AddChild(m_pText);
	}

	m_bUpdated = true;
}

void i3GuiEditBox::CreateTextEx(const i3TextFontSetInfo * pFontSet,INT32 TextCount, INT32 TextSize, bool bIMEEnable, CREATE_IME_ELEMENT* pStruct, UINT32 TextStyle, bool bShadowEnable, FONT_SHADOW_QUALITY ShadowQuality)
{
	I3ASSERT(pFontSet != nullptr);
	m_FontSetInfo = *pFontSet;
	m_nMaxTextCount = TextCount;
	
	if( TextSize == -1)
		m_nTextSize = m_nDefaultFontSize;
	else
		m_nTextSize = TextSize;

	m_TextStyle = TextStyle;
	m_bShadowEnable = bShadowEnable;
	m_ShadowQuality = ShadowQuality;

#if defined (I3_WINDOWS)
	// IME 생성
	if( bIMEEnable)
	{
		if( m_pIME) I3_SAFE_RELEASE(m_pIME);

		m_pIME = i3IME::new_object();
		
		m_pIME->CreateIME(pStruct);
		m_pIME->setOwner( this);

		{
			m_pIME->SetWindowPosition( (INT32)(getTotalPosX()), (INT32)getTotalPosY());
		}

		// Caret 생성
		CreateCaret();
	}
#endif

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{		
		REAL32 ry = 1.0f;
		
        if( i3GuiRoot::getViewer() != nullptr)
			ry = i3GuiRoot::getGuiResolutionHeight();

		if( !i3GuiRoot::isChangeResolution())
		{	
			ry = 1.0f;
		}

		INT32 fontSize = (INT32) (m_nTextSize * ry);
		UINT32 weight = _CalcTextWeight();
		
		m_pTextDX = i3TextNodeDX2::new_object();
	#if USE_VERTEX_BANK
		m_pTextDX->Create( &m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, true);
	#else
		m_pTextDX->Create( &m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, false);
	#endif	
		m_pTextDX->setAutoWrapEnable(true);

		m_pTextDX->setPos((UINT32)((getTotalPosX()+m_fOffsetPosX)), (UINT32)((getTotalPosY()+m_fOffsetPosY)) );		
		SetTextRectSize( m_nOriginalWidth, m_nOriginalHeight);
		m_nEditOriginalWidth = m_nOriginalWidth;
		m_nEditOriginalHeight = m_nOriginalHeight;

		GetNode()->AddChild( m_pTextDX);

		if( m_pIME != nullptr)
		{
			i3GfxFontDX2 * pFont = m_pTextDX->getFont();
			if( pFont != nullptr)
			{
				LOGFONTA * pLogFont = pFont->getLogFont();
				if( pLogFont != nullptr)
				{
					m_pIME->SetIMEFont( pFont->getHFont(), pLogFont);
				}
			}
		}
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		// Font 생성
		m_pFont = nullptr;
		m_pText = i3TextNode::new_object();
		m_pText->Create(m_pFont, TextCount, true);
		m_pText->SetText(L"");	
		m_pText->SetPos(0.0f, 0.0f);
		m_pText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);		
		m_pText->SetColor(255, 255, 255, 255);		
		SetTextRectSize( m_nWidth, m_nHeight);
		GetNode()->AddChild(m_pText);
	}

	m_bUpdated = true;
}

void i3GuiEditBox::CreateTextEx(const char* pszFont,INT32 TextCount, INT32 TextSize, bool bIMEEnable, CREATE_IME_ELEMENT* pStruct, UINT32 TextStyle, bool bShadowEnable, FONT_SHADOW_QUALITY ShadowQuality)
{
	i3TextFontSetInfo	fontSetInfo(pszFont);
	CreateTextEx(&fontSetInfo, TextCount, TextSize, bIMEEnable, pStruct, TextStyle, bShadowEnable, ShadowQuality);
}

void i3GuiEditBox::ReCreateTextEx(const i3TextFontSetInfo * pFontSet, INT32 TextCount, INT32 TextSize, bool bIMEEnable, CREATE_IME_ELEMENT* pStruct, UINT32 TextStyle, bool bShadowEnable, FONT_SHADOW_QUALITY ShadowQuality)
{
	I3ASSERT(pFontSet != nullptr);
	m_FontSetInfo = *pFontSet;
	m_nMaxTextCount = TextCount;
	
	if( TextSize == -1)
		m_nTextSize = m_nDefaultFontSize;
	else
		m_nTextSize = TextSize;

	m_TextStyle = TextStyle;
	m_bShadowEnable = bShadowEnable;
	m_ShadowQuality = ShadowQuality;

#if defined (I3_WINDOWS)
	// IME 생성
	if( bIMEEnable)
	{
		if( m_pIME) I3_SAFE_RELEASE(m_pIME);

		m_pIME = i3IME::new_object();

		m_pIME->CreateIME(pStruct);
		m_pIME->setOwner( this);

		{
			m_pIME->SetWindowPosition( (INT32)(getTotalPosX()), (INT32)getTotalPosY());
		}

		// Caret 생성
		CreateCaret();
	}
#endif

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{	
		REAL32 ry = i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;

		if( !i3GuiRoot::isChangeResolution())
		{
			ry = 1.0f;
		}

		INT32 fontSize = (INT32) (m_nTextSize * ry);
		UINT32 weight = _CalcTextWeight();

		if(m_pTextDX)
		{
			GetNode()->RemoveChild( m_pTextDX);
			I3_SAFE_RELEASE( m_pTextDX);
		}

		m_pTextDX = i3TextNodeDX2::new_object();
	#if USE_VERTEX_BANK
		m_pTextDX->Create(&m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, true);
	#else
		m_pTextDX->Create(&m_FontSetInfo, TextCount, fontSize, weight, false, false, DEFAULT_QUALITY, bShadowEnable, ShadowQuality, 1.0f, false);
	#endif
		m_pTextDX->setAutoWrapEnable(true);

		m_pTextDX->setPos((UINT32)((getTotalPosX()+m_fOffsetPosX)), (UINT32)((getTotalPosY()+m_fOffsetPosY)) );		
		SetTextRectSize( m_nOriginalWidth, m_nOriginalHeight);
		m_nEditOriginalWidth = m_nOriginalWidth;
		m_nEditOriginalHeight = m_nOriginalHeight;

		GetNode()->AddChild( m_pTextDX);

		if( m_pIME != nullptr)
		{
			i3GfxFontDX2 * pFont = m_pTextDX->getFont();
			if( pFont != nullptr)
			{
				LOGFONTA * pLogFont = pFont->getLogFont();
				if( pLogFont != nullptr)
				{
					m_pIME->SetIMEFont( pFont->getHFont(), pLogFont);
				}
			}
		}
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		// Font 생성
		if(m_pText)
		{
			GetNode()->RemoveChild( m_pText);
			I3_SAFE_RELEASE( m_pText);
		}

		m_pFont = nullptr;
		m_pText = i3TextNode::new_object();
		m_pText->Create(m_pFont, TextCount, true);
		m_pText->SetText(L"");	
		m_pText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);		
		m_pText->SetColor(255, 255, 255, 255);		
		SetTextRectSize( m_nWidth, m_nHeight);	
		m_pText->SetPos((getTotalPosX()+m_fOffsetPosX), (getTotalPosY()+m_fOffsetPosY));		
		GetNode()->AddChild(m_pText);
	}

	m_bUpdated = true;
}

void i3GuiEditBox::ReCreateTextEx(const char* pszFont, INT32 TextCount, INT32 TextSize, bool bIMEEnable, CREATE_IME_ELEMENT* pStruct, UINT32 TextStyle, bool bShadowEnable, FONT_SHADOW_QUALITY ShadowQuality)
{
	i3TextFontSetInfo	fontSetInfo(pszFont);
	ReCreateTextEx(&fontSetInfo, TextCount, TextSize, bIMEEnable, pStruct, TextStyle, bShadowEnable, ShadowQuality);
}

#elif defined( I3_WINCE)
void i3GuiEditBox::CreateText( i3Font* pFont, INT32 TextCount)
{
	m_nMaxTextCount = TextCount;

	// Font 생성
	m_pFont = pFont;
	m_pText = i3TextNode::new_object();
	m_pText->Create(m_pFont, TextCount, true);
	m_pText->SetText("");	
	m_pText->SetPos(0.0f, 0.0f);
	m_pText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);		
	m_pText->SetColor(255, 255, 255, 255);		
	SetTextRectSize( m_nWidth, m_nHeight);
	GetNode()->AddChild(m_pText);

	m_bUpdated = true;
}

void i3GuiEditBox::ReCreateText( i3Font* pFont, INT32 TextCount)
{
	m_nMaxTextCount = TextCount;

	// Font 생성
	if(m_pText)	
	{
		GetNode()->RemoveChild( m_pText);
		I3_SAFE_RELEASE( m_pText);
	}

	m_pFont = pFont;
	m_pText = i3TextNode::new_object();
	m_pText->Create(m_pFont, TextCount, true);
	m_pText->SetText("");	
	m_pText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);		
	m_pText->SetColor(255, 255, 255, 255);		
	SetTextRectSize( m_nWidth, m_nHeight);	
	m_pText->SetPos((getTotalPosX()+m_fOffsetPosX), (getTotalPosY()+m_fOffsetPosY));		
	GetNode()->AddChild(m_pText);

	m_bUpdated = true;
}
#endif

void i3GuiEditBox::SetText( const i3::wliteral_range& wTextRng, bool bParseCmd)
{	

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		// 할당된 텍스트 크기보다 큰 텍스트가 오면 재생성합니다.
		{
			INT32 nCount = i3::generic_string_size( wTextRng);
			bool bReCreateText = false;
			while( m_nMaxTextCount <= nCount)
			{	
				m_nMaxTextCount *= 2;
				bReCreateText = true;
			}

			if( bReCreateText) 
			{
				// Text의 AutoWrap이 활성화 되어있는지 얻어옵니다.
				m_bTextAutoWrap = (bool)m_pTextDX->isState( I3_TEXT_STATE_AUTOWRAP);

				ReCreateTextEx(&m_FontSetInfo, m_nMaxTextCount, m_nTextSize);
				m_pTextDX->SetColor( i3Color::GetR(&m_TextColor), i3Color::GetG(&m_TextColor), i3Color::GetB(&m_TextColor), i3Color::GetA(&m_TextColor));
				m_pTextDX->SetAlign(m_TextAlignX, m_TextAlignY);
				m_pTextDX->SetOffsetPos( m_fOffsetPosX * i3GuiRoot::getGuiResolutionWidth(), m_fOffsetPosY * i3GuiRoot::getGuiResolutionHeight());
				m_pTextDX->setAutoWrapEnable( m_bTextAutoWrap);	
				m_pTextDX->SetTextSpace( (INT32)(m_nTextSpaceX * i3GuiRoot::getGuiResolutionWidth()), (INT32)(m_nTextSpaceY * i3GuiRoot::getGuiResolutionHeight()));
			}
		}

		m_pTextDX->SetText( wTextRng, bParseCmd);
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->SetText( wTextRng);
	}

	m_bUpdated = true;
}

#if defined (I3G_DX)
void i3GuiEditBox::SetTextEllipsis(const i3::wliteral_range& wTextRng)
{
	// 할당된 텍스트 크기보다 큰 텍스트가 오면 재생성합니다.
	{
		INT32 nCount = i3::generic_string_size( wTextRng);
		bool bReCreateText = false;
		while( m_nMaxTextCount <= nCount)
		{	
			m_nMaxTextCount *= 2;
			bReCreateText = true;
		}

		if( bReCreateText) 
		{
			// Text의 AutoWrap이 활성화 되어있는지 얻어옵니다.
			m_bTextAutoWrap = (bool)m_pTextDX->isState( I3_TEXT_STATE_AUTOWRAP);

			ReCreateTextEx(&m_FontSetInfo, m_nMaxTextCount, m_nTextSize);
			m_pTextDX->SetColor( i3Color::GetR(&m_TextColor), i3Color::GetG(&m_TextColor), i3Color::GetB(&m_TextColor), i3Color::GetA(&m_TextColor));
			m_pTextDX->SetAlign(m_TextAlignX, m_TextAlignY);
			m_pTextDX->SetOffsetPos( m_fOffsetPosX * i3GuiRoot::getGuiResolutionWidth(), m_fOffsetPosY * i3GuiRoot::getGuiResolutionHeight());
			m_pTextDX->setAutoWrapEnable( m_bTextAutoWrap);	
			m_pTextDX->SetTextSpace( (INT32)(m_nTextSpaceX * i3GuiRoot::getGuiResolutionWidth()), (INT32)(m_nTextSpaceY * i3GuiRoot::getGuiResolutionHeight()));
		}
	}

	m_pTextDX->SetTextEllipsis(wTextRng);
	m_bUpdated = true;
}
#endif
/*
void  i3GuiEditBox::SetText( const i3::wliteral_range& wTextRng)
{
#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
	
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->SetText( wTextRng);
	}

	m_bUpdated = true;
}
*/
void i3GuiEditBox::SetTextColor( COLOR * pCol)
{
	i3Color::Copy( &m_TextColor, pCol);

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->SetColor(pCol->r, pCol->g, pCol->b, pCol->a);
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->SetColor(pCol->r, pCol->g, pCol->b, pCol->a);	
	}
		
	m_bUpdated = true;
}

void i3GuiEditBox::SetTextAlign(INT32 AlignX, INT32 AlignY)
{
	m_TextAlignX = AlignX;
	m_TextAlignY = AlignY;

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->SetAlign(AlignX, AlignY);
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->SetAlign(AlignX, AlignY);
	}

	m_bUpdated = true;
}

void i3GuiEditBox::SetTextRectSize(UINT32 nWidth, UINT32 nHeight)
{
#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->setWidth((UINT32)(nWidth * i3GuiRoot::getGuiResolutionWidth()));
		m_pTextDX->setHeight((UINT32)(nHeight * i3GuiRoot::getGuiResolutionHeight()));

		//m_pTextDX->setWidth( (UINT32)(nWidth));
		//m_pTextDX->setHeight( (UINT32)(nHeight));
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->SetSize( (UINT32)(nWidth), (UINT32)(nHeight));
	}

	m_bUpdated = true;
}

void i3GuiEditBox::SetOnlyEditOffsetPos( REAL32 fX, REAL32 fY)	
{
	m_fOffsetPosX = fX;
	m_fOffsetPosY = fY;

	m_bUpdated = true;
}

void i3GuiEditBox::SetOnlyTextOffsetPos( REAL32 fX, REAL32 fY)
{
#if defined (I3_WINDOWS)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->SetOffsetPos( fX * i3GuiRoot::getGuiResolutionWidth(), fY * i3GuiRoot::getGuiResolutionHeight());
	}
#endif

	m_bUpdated = true;
}

void i3GuiEditBox::SetOffsetPos( REAL32 fX, REAL32 fY)	
{
	m_fOffsetPosX = fX;
	m_fOffsetPosY = fY;

#if defined (I3_WINDOWS)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->SetOffsetPos( fX * i3GuiRoot::getGuiResolutionWidth(), fY * i3GuiRoot::getGuiResolutionHeight());
	}
#endif
	
	m_bUpdated = true;
}

void i3GuiEditBox::SetTextAutoWrap(bool bEnable)
{
	m_bTextAutoWrap = bEnable;

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->setAutoWrapEnable( bEnable);		
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->SetAutoWrap( bEnable, m_nMaxTextCount);
	}
}

void i3GuiEditBox::SetWordLineFeed( bool bEnable)
{
	m_bTextWordLineFeed = bEnable;

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		m_pText->setWordLineFeed( bEnable);
	}
}

void i3GuiEditBox::SetTextSpace( INT32 nX, INT32 nY)
{
	m_nTextSpaceX = nX;
	m_nTextSpaceY = nY;

#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->SetTextSpace( (INT32)(nX * i3GuiRoot::getGuiResolutionWidth()), (INT32)(nY * i3GuiRoot::getGuiResolutionHeight()));
	}
#endif
}

void i3GuiEditBox::SetOutCharaProcess(bool bFlag)
{
	#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_bEnableOutCharacterProcess = bFlag;

		m_pTextDX->setOutCharacterProcess( bFlag);
	}
	#endif
}

const wchar_t* i3GuiEditBox::GetText(void) const
{
#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		return m_pTextDX->getText();
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		// 아직 미구현
		I3ASSERT_0;
	}

	return nullptr;
}

// 갱신
void i3GuiEditBox::OnUpdate( REAL32 rDeltaSeconds )
{
	//if(m_bUpdated) // 컨트롤의 위치 변경시 함께 변경되야합니다
	{
#if defined ( I3G_DX)
		if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
		{
			// 툴등에선 생성되지 않는다.
			if (m_pTextDX)
			{
				m_pTextDX->setPos((UINT32)(getTotalPosX()+m_fOffsetPosX), (UINT32)(getTotalPosY()+m_fOffsetPosY) );
			}
		}
		else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
		{
			// 툴등에선 생성되지 않는다.
			if (m_pText)
			{
				m_pText->SetPos((getTotalPosX()+m_fOffsetPosX), (getTotalPosY()+m_fOffsetPosY));		
			}
		}

		m_bUpdated = false;
	}

	UpdateRenderObjects();

	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED ||
		getControlState() == I3GUI_CONTROL_STATE_RESERVED2)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}
	
	if( i3GuiRoot::getFocusControl())
	{
		if( i3GuiRoot::getFocusControl() == this)
		{
			#if defined ( I3_WINDOWS)
 				if( m_pIME) 
				{
					m_pIME->SetWindowPosition( (INT32)(getTotalPosX()), (INT32)getTotalPosY());
					if( m_pIME->SetFocus(true))
					{
						return;
					}
				}
			#endif
		}

#if defined ( I3_WINDOWS)
		// IME관련 업데이트 
		if( m_pIME && i3GuiRoot::getFocusControl() == this)	UpdateIMEInput( rDeltaSeconds);
		else if( m_pCaretSprite) m_pCaretSprite->SetEnable(	0, false);
#endif
	}
	else
	{
		// 입력도중 포커스를 잃어버릴때, 남아있는 캐럿을 숨긴다.
		if (m_pCaretSprite) m_pCaretSprite->SetEnable(0, false);
	}

	i3GuiControl::OnUpdate( rDeltaSeconds ); 
}

void i3GuiEditBox::OnKillFocus(void)
{	
	if( m_pIME != nullptr)
		m_pIME->KillFocus();

	i3GuiControl::OnKillFocus();
}

void i3GuiEditBox::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	if (IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_LBUTTON_DCLICK))
	{
		if (this == m_pClickedControl)
		{
			setControlState(I3GUI_CONTROL_STATE_RESERVED2);
			PushControlNotify(I3GUI_EDIT_NOTIFY_DOUBLECLICK, (INT32)fX, (INT32)fY);

			if (m_pObserverControl) m_pObserverControl->OnEvent(0, this, I3GUI_CONTROL_STATE_RESERVED2, I3_EVT_CODE_ACTIVATE);
		}
		else
		{
			setControlState(I3GUI_CONTROL_STATE_CLICKED);
			PushControlNotify(I3GUI_EDIT_NOTIFY_CLICKED, (INT32)fX, (INT32)fY);
			m_pClickedControl = this;

			if (m_pObserverControl) m_pObserverControl->OnEvent(0, this, I3GUI_CONTROL_STATE_CLICKED, I3_EVT_CODE_ACTIVATE);
		}
	}
	else if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_LBUTTON_DOWN))
	{
		setControlState(I3GUI_CONTROL_STATE_CLICKED);
	
		PushControlNotify(I3GUI_EDIT_NOTIFY_CLICKED, (INT32)fX, (INT32)fY);
		m_pClickedControl = this;

		if (m_pObserverControl) m_pObserverControl->OnEvent(0, this, I3GUI_CONTROL_STATE_CLICKED, I3_EVT_CODE_ACTIVATE);
	}
	else if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_RBUTTON_DOWN))
	{
		setControlState(I3GUI_CONTROL_STATE_R_CLICKED);
		PushControlNotify(I3GUI_EDIT_NOTIFY_RCLICK, 0, 0);

		if (m_pObserverControl) m_pObserverControl->OnEvent(0, this, I3GUI_CONTROL_STATE_R_CLICKED, I3_EVT_CODE_ACTIVATE);
	}
	else
	{
		setControlState(I3GUI_CONTROL_STATE_ONMOUSE);		
		PushControlNotify(I3GUI_EDIT_NOTIFY_ONMOUSE,0,0);

		if (m_pObserverControl) m_pObserverControl->OnEvent(0, this, I3GUI_CONTROL_STATE_ONMOUSE, I3_EVT_CODE_ACTIVATE);
	}
}

bool	i3GuiEditBox::OnEvent( UINT32 nEvent, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	//composition이 일어나는 language중 korean은 게임에서 ui를 사용하지 않으므로 inputdisable처리를 하지 않고
	// chinese와 japanese만 사용한다.
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
		return true;

	if( nEvent == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_COMPOSITION_START:
			{
				if( i3GuiRoot::getFocusControl() == this)
				{
					i3GuiRoot::setGuiDisableInput(true);
				}
			}
			break;
		case EDIT_STATE_COMPOSITION_END:
			{
				if( i3GuiRoot::getFocusControl() == this)
				{
					i3GuiRoot::setGuiDisableInput(false);

					i3Framework * pFramework  = i3Framework::getCurrentFramework();
					i3Viewer * pViewer = pFramework->GetViewer();
					if( pViewer != nullptr)
					{
						i3InputDeviceManager * pDevMng = pViewer->GetInputDeviceManager();
						if( pDevMng != nullptr)
						{
							pDevMng->Reset();
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return true;
}

#if defined (I3_WINDOWS)
void i3GuiEditBox::CreateCaret(void)
{
	if( m_pCaretSprite)
	{
		GetNode()->RemoveChild( m_pCaretSprite);
		I3_SAFE_RELEASE(m_pCaretSprite);
	}

	m_pCaretSprite = i3Sprite2D::new_object();
	m_pCaretSprite->Create( 1, false, true);

	m_CaretWidth = 1.0f;
	m_CaretHeight = (REAL32)m_nHeight;

	m_pCaretSprite->SetEnable(	0, false);
	m_pCaretSprite->SetCenter(	0, ((getTotalPosX()) + (m_CaretWidth * 0.5f) + m_CaretPosOffsetX + m_fOffsetPosX) ,
									((getTotalPosY()) + (m_CaretHeight * 0.5f) + m_CaretPosOffsetY + m_fOffsetPosY),
									0.0f);
	m_pCaretSprite->SetSize(	0, m_CaretWidth, m_CaretHeight);
	m_pCaretSprite->SetColor(	0, i3Color::GetR(&m_Col), i3Color::GetG(&m_Col), i3Color::GetB(&m_Col), i3Color::GetA(&m_Col));

	GetNode()->AddChild(m_pCaretSprite);
}

void i3GuiEditBox::SetIMEText( const i3::wliteral_range& wTextRng )
{
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		m_pTextDX->SetText( wTextRng, false);	
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
	{
		m_pText->SetText( wTextRng);
	}

	m_pIME->SetInput( wTextRng);

	m_bUpdated = true;
}

REAL32 i3GuiEditBox::GetCaretPosX()
{
	REAL32 fWidth = 0.0f;
	INT32 nLength = 0;

	if( m_pIME)
	{
		if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
		{
			nLength = 0;

			nLength += m_pIME->getIME()->GetCursorPosition();

			fWidth = (REAL32)m_pTextDX->getCaretPosX( nLength);
		}
		else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
		{
			INT32 i = 0;			
		//	WCHAR16	wszTemp[256] = {0,};
		//	char	szTemp[256] = {0,};
			i3::wstring  wstrTemp;
			const i3::wstring* pwstrText = &wstrTemp;

			// 입력된 모든 Text로부터 길이값을 가져와 최종 Caret의 위치를 정합니다.
			nLength = i3::generic_string_size( m_pIME->getText());		
			if( !m_bUsePassward)
			{
				pwstrText = &m_pIME->getText();
//				i3String::MultiByteToWideChar( CP_ACP, MB_COMPOSITE, m_pIME->getText(), nLength, wszTemp, 256);
			}
			else
			{			
				for( i=0; i<nLength; i++)
				{
					switch( i3Language::GetCurrentCodePage())
					{
					case I3_LANG_CODE_KOREAN:		//	한국
						i3::generic_string_cat( wstrTemp, L"●");
						break;
					default:
						i3::generic_string_cat( wstrTemp, L"*");
						break;
					}
				}
			//	i3String::MultiByteToWideChar( CP_ACP, MB_COMPOSITE, szTemp, nLength, wszTemp, 256);
			}
			
			if( i3::generic_string_size(m_pIME->getText()) > 0)
			{
				nLength = 0;
			
				if( m_pIME->getIME()->IsComp())
				{
					nLength++;
				}

				nLength += m_pIME->getIME()->GetCursorPosition();

				for( i=0; i < nLength; i++)
				{			
					I3_GLYPH_INFO info;
					m_pText->GetTextAttr()->getFont()->GetGlyphInfoByChar( &info, (*pwstrText)[i]);
					fWidth += info.nWidth;
				}
			}
		}
	}

	return fWidth;

}

REAL32 i3GuiEditBox::GetCharacterSizeX( const i3::wliteral_range& wTextRng) const
{
	INT32 i = 0;
	INT32 nLength = 0;

	REAL32 fWidth = 0.0f;

	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		fWidth = (REAL32)m_pTextDX->CalcTextWidth(wTextRng);
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
	{
		// 입력된 모든 Text로부터 길이값을 가져와 최종 Caret의 위치를 정합니다.
		nLength = i3::generic_string_size( wTextRng );		

		for( i=0; i < nLength; i++)
		{			
			I3_GLYPH_INFO info;
			m_pText->GetTextAttr()->getFont()->GetGlyphInfoByChar( &info, wTextRng[i]);
			fWidth += info.nWidth;
		}
	}

	return fWidth;
}

void i3GuiEditBox::UpdateIMEInput( REAL32 rDeltaSeconds)
{
	static REAL32 fOldCaretPosX = 0.0f;	

	// Passward사용
	if( m_bUsePassward)
	{
		INT32 i = 0;
		INT32 nLength = i3::generic_string_size( m_pIME->getText());
	//	char	szTemp[256] = {0,};
		i3::stack_wstring wstrTemp;

		for( i=0; i<nLength; i++)
		{
			switch( i3Language::GetCurrentCodePage())
			{
			case I3_LANG_CODE_KOREAN:		//	한국
				i3::generic_string_cat( wstrTemp, L"●");
				break;
			default:
				i3::generic_string_cat( wstrTemp, L"*");
				break;
			}
		}
		SetText( wstrTemp, false);
	}
	// 패스워드 미사용
	else 
	{
		if( m_pIME->getIME()->IsComp())
		{
			i3::wstring wstrTemp;
			i3::wstring wstrConv;
			i3::wstring wstrComp;
		
			INT32 nCaretPos = m_pIME->getIME()->GetInsertPositionW(); 
			INT32 ret = 0;
			

			ret = m_pIME->getIME()->GetInput( wstrTemp );
			ret += m_pIME->getIME()->GetComp( wstrConv);

			i3::sprintf(wstrComp, L"{Col:255,100,100,255}%s{/Col}", wstrConv );

			//	조합문자를 버퍼 중간에 삽입
			
			wstrTemp.insert(nCaretPos, wstrComp);

			SetText( wstrTemp, true);
		}
		else
		{
			SetText( m_pIME->getText(), false);
		}
	}

	// Caret연산
	m_CaretLocalTime += rDeltaSeconds;
	if( m_CaretLocalTime > m_CaretInvisibleTime)
	{
		m_pCaretSprite->SetEnable( 0, !(m_pCaretSprite->GetEnable(0)));
		m_CaretLocalTime = 0.0f;
	}

	REAL32 fCaretPosX = GetCaretPosX();
	//REAL32 fCaretPosX = 0.0f;

	INT32 nLength = 0;		
	if( m_pIME->getIME()->IsComp())	nLength+=1;
	nLength += m_pIME->getIME()->GetCursorPosition();
	REAL32 fCaretPosY  = (REAL32)m_pTextDX->GetCaretPosY( nLength);

	// Scroll 위치 조정
	{
		REAL32 x1, x2;

		x1 = 0.0f;
		x2 = (REAL32)m_pTextDX->getWidth();

		if( fCaretPosX < x1)
		{
			if( !m_pTextDX->isState( I3_TEXT_STATE_AUTOWRAP))
			{
				m_pTextDX->AdjustOffset( fCaretPosX - x1);		// minus로 들어가야 한다.
			}
		}
		else if( fCaretPosX >= x2)
		{
			if( !m_pTextDX->isState( I3_TEXT_STATE_AUTOWRAP))
			{
				m_pTextDX->AdjustOffset( fCaretPosX - x2);		// plus
			}			
		}
		else if( fCaretPosX == 0.0f)
		{
			if( !m_pTextDX->isState( I3_TEXT_STATE_AUTOWRAP))
			{
				m_pTextDX->AdjustOffset( -1.0f);		// minus로 들어가야 한다.
			}
		}

		fCaretPosX = GetCaretPosX();		
	}

	if( fOldCaretPosX != fCaretPosX)
	{
		fOldCaretPosX = fCaretPosX;

		m_pCaretSprite->SetEnable( 0, true);
		m_CaretLocalTime = 0.0f;
	}
	
	m_CaretHeight = (REAL32)m_pTextDX->getTextHeight()+2.0f;

	if( m_pIME->getIME()->IsComp())
	{
		i3::wstring wstrComp;

		if( m_pIME->getIME()->GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			m_pIME->m_pInputIME->GetComp( wstrComp);			
		}
		else
		{
			m_pIME->m_pInputIME->GetCompToIndex( wstrComp);
		}

		//if( m_pIME->m_pInputIME->isSelectingCandidate())
		//{
		//	m_pIME->m_pInputIME->GetCompToIndex( szTemp, 256);
		//}
		//else
		//{
		//	m_pIME->m_pInputIME->GetComp( szTemp, 256);			
		//}

		REAL32 fCharaWidth = (REAL32)m_pTextDX->getCharacterSizeX(wstrComp);
		REAL32 fTextWidth = (REAL32)m_pTextDX->CalcTextWidth(wstrComp);
		
		if( m_pIME->getIME()->GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			m_pCaretSprite->SetSize(	0, m_CaretWidth + fCharaWidth, m_CaretHeight);
			m_pCaretSprite->SetCenter(	0, (getTotalPosX()) + fCaretPosX + (m_CaretWidth * 0.5f) + m_CaretPosOffsetX + m_fOffsetPosX + fTextWidth - fCharaWidth * 0.5f ,
										(getTotalPosY()) + fCaretPosY + (m_CaretHeight * 0.5f) + m_CaretPosOffsetY - m_fOffsetPosY,
										0.0f);
		}
		else
		{
			m_pCaretSprite->SetSize(	0, m_CaretWidth, m_CaretHeight);
			m_pCaretSprite->SetCenter(	0, (getTotalPosX()) + fCaretPosX + (m_CaretWidth * 0.5f) + m_CaretPosOffsetX + m_fOffsetPosX + fTextWidth,
										(getTotalPosY()) + fCaretPosY + (m_CaretHeight * 0.5f) + m_CaretPosOffsetY - m_fOffsetPosY,
										0.0f);
		}
		
		m_pCaretSprite->SetColor(	0, i3Color::GetR(&m_Col), i3Color::GetG(&m_Col), i3Color::GetB(&m_Col), 100);

		{
			VEC3D caretStart;
			m_pCaretSprite->GetLeftTop(0, &caretStart);

			m_pIME->SetWindowPosition((INT32)(caretStart.x - fTextWidth), (INT32)caretStart.y);
		}
	}
	else
	{
		m_pCaretSprite->SetSize(	0, m_CaretWidth, m_CaretHeight);
		m_pCaretSprite->SetCenter(	0, (getTotalPosX()) + fCaretPosX + (m_CaretWidth * 0.5f) + m_CaretPosOffsetX + m_fOffsetPosX,
									(getTotalPosY()) + fCaretPosY + (m_CaretHeight * 0.5f) + m_CaretPosOffsetY - m_fOffsetPosY,
									0.0f);
		m_pCaretSprite->SetColor(	0, i3Color::GetR(&m_Col), i3Color::GetG(&m_Col), i3Color::GetB(&m_Col), i3Color::GetA(&m_Col));

		{
			VEC3D caretStart;
			m_pCaretSprite->GetLeftTop(0, &caretStart);

			m_pIME->SetWindowPosition((INT32)caretStart.x, (INT32)caretStart.y);
		}
	}

	// 캐럿이 Edit박스의 영역을 벗어날경우 화면에 표시하지 않습니다~!
	// 멀티라인에 대응하여 예외적인 처리도 합니다.
	if( fCaretPosX > m_nWidth || fCaretPosY >= m_nHeight || (m_pIME->isEnableMultiLine() && m_pTextDX->getTextAttrDX()->isOutChar()) )
	{
		m_pCaretSprite->SetEnable( 0, false);
		m_pIME->SetInput( m_wstrTextTemp);
	}
	else
	{
		if( !m_pTextDX->getTextAttrDX()->isSameText())
		{
			if( i3::generic_is_equal( m_wstrTextTemp, m_pIME->getText() ) == false) //!= 0)
			{
				m_wstrTextTemp = m_pIME->getText();
			}
		}
	}
}

IME_MODE i3GuiEditBox::GetIMEMode(void)
{
	return m_pIME->GetIMEMode();
}

void i3GuiEditBox::SetAlphaNumericOnly( bool bForce)
{
	m_pIME->setAlphaNumericOnly( bForce);
}

void i3GuiEditBox::SetIMEMode(IME_MODE mode) 
{
	//	m_bForcedIMEMode 와 m_forcedImeMode가 사라짐. komet
	//	엔진에 IME 생성시 style로 IME_STYLE_IGNORE_KEYINPUT_IME_MODE 주면 유저의 한/영 변환 버튼 입력을 무시하도록 되어 있음.
	
	m_pIME->SetIMEMode( mode);
}

EDIT_STATE i3GuiEditBox::getEditState(void)
{ 
	if( m_pIME) 
		return m_pIME->getIMEState(); 
	else
		return EDIT_STATE_NONE;
}	
#endif

void i3GuiEditBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiEditBox*>(pDest))
		return ;

	i3GuiEditBox * pEditBox = (i3GuiEditBox*)pDest;

	pEditBox->setControlState( m_ControlState );
}

void i3GuiEditBox::SetNotifyObserver(i3GuiControl* pControl)
{
	m_pObserverControl = pControl;
}

void i3GuiEditBox::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUIEDITBOX
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiEditBox::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIEDITBOX data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiEditBox::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiEditBox::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIEDITBOX data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiEditBox::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}

// 내부 TextNode를 위해서 오버라이딩합니다 - praptor
bool i3GuiEditBox::_ResizeControl(void)
{
	if (false == i3GuiControl::_ResizeControl())
	{
		return false;
	}
	
	ResizeText();	

	return true;
}

void i3GuiEditBox::ResizeText(void)
{
//	char szTemp[1024];
	i3::stack_wstring wstrTemp;
	REAL32 ry = 1.0f;

	if( m_nMaxTextCount <= 0) return;

	if( i3GuiRoot::getViewer() != nullptr)
	{
		ry = i3GuiRoot::getGuiResolutionHeight(); // / I3GUI_RESOLUTION_HEIGHT;
	}
	
	if( m_pTextDX)
	{	
		// 기존에 만들어져 있던것과 동일하므로 그냥 반환합니다.
		if( m_fCurGuiResolutionRateHeight == ry) return;

		if( (m_pTextDX->getState() & I3_TEXT_STATE_AUTOWRAP) != 0)	m_bTextAutoWrap = true;	
		else m_bTextAutoWrap = false;

		wstrTemp = m_pTextDX->getText();
		GetNode()->RemoveChild( m_pTextDX);
		I3_SAFE_RELEASE(m_pTextDX);
	}
	else
	{
		wstrTemp.clear();
	}

	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{			
		INT32 fontSize = (INT32) (m_nTextSize * ry);
		UINT32 weight = _CalcTextWeight();

		m_pTextDX = i3TextNodeDX2::new_object();
	#if USE_VERTEX_BANK
		m_pTextDX->Create( &m_FontSetInfo, m_nMaxTextCount, fontSize, weight, false, false, DEFAULT_QUALITY, m_bShadowEnable, m_ShadowQuality, 1.0f, true);
	#else
		m_pTextDX->Create( &m_FontSetInfo, m_nMaxTextCount, fontSize, weight, false, false, DEFAULT_QUALITY, m_bShadowEnable, m_ShadowQuality, 1.0f, false);
	#endif
		m_pTextDX->setAutoWrapEnable(true);

		m_pTextDX->setPos((UINT32)(getTotalPosX()+m_fOffsetPosX), (UINT32)(getTotalPosY()+m_fOffsetPosY) );
		SetTextRectSize( m_nOriginalWidth, m_nEditOriginalHeight);
		
		m_pTextDX->SetColor( i3Color::GetR(&m_TextColor), i3Color::GetG(&m_TextColor), i3Color::GetB(&m_TextColor), i3Color::GetA(&m_TextColor));
		m_pTextDX->SetAlign(m_TextAlignX, m_TextAlignY);
		m_pTextDX->SetOffsetPos( m_fOffsetPosX * i3GuiRoot::getGuiResolutionWidth(), m_fOffsetPosY * i3GuiRoot::getGuiResolutionHeight());
		m_pTextDX->setAutoWrapEnable( m_bTextAutoWrap);	
		m_pTextDX->SetTextSpace( (INT32)(m_nTextSpaceX * i3GuiRoot::getGuiResolutionWidth()), (INT32)(m_nTextSpaceY * i3GuiRoot::getGuiResolutionHeight()));
		m_pTextDX->setOutCharacterProcess(m_bEnableOutCharacterProcess);

		m_pTextDX->SetText( wstrTemp);
		GetNode()->AddChild( m_pTextDX);		
	}
}

UINT32 i3GuiEditBox::_CalcTextWeight(void)
{
	REAL32 ry = 1.0f;
	UINT32 weight = 0;

	if( i3GuiRoot::getViewer() != nullptr)
	{
		ry = i3GuiRoot::getGuiResolutionHeight(); // / I3GUI_RESOLUTION_HEIGHT;
	}

	switch( PRIMARYLANGID(GetSystemDefaultLangID()) )
	{
	case LANG_KOREAN:
		{
			weight = (UINT32)( m_TextStyle * ry);		//	fixed value. don't change - komet

			UINT32 val = weight%100;

			if( val < 50) weight -= val;
			else weight += (100 - val);
		}
		break;
	case LANG_THAI:
		{
			weight = (UINT32)m_TextStyle;		//	fixed value. don't change - komet
		}
		break;
	default:
		{
			weight = (UINT32)( m_TextStyle * ry);		//	fixed value. don't change - komet

			UINT32 val = weight%100;

			if( val < 50) weight -= val;
			else weight += (100 - val);
		}
		break;
	}

	return weight;
}

void i3GuiEditBox::SetDefaultFontSize(INT32 nSize)			
{ 
	m_nDefaultFontSize = nSize;				
}

INT32 i3GuiEditBox::GetDefaultFontSize(void)				
{ 
	return m_nDefaultFontSize;				
}