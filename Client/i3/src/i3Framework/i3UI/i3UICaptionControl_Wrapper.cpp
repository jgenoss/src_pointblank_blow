#include "i3FrameworkPCH.h"
#include "i3UICaptionControl.h"

//Set을 하는 경우에는 m_pText==nullptr인 경우 새롭게 생성합니다.
//그렇지 않으면 설정하려는 정보가 날아가게 됩니다. 

//Align
I3UI_TEXT_ALIGN_X i3UICaptionControl::getTextAlignX(void)
{
	if( m_pText != nullptr)		return m_pText->getAlignX();
	return m_AlignX;
}

I3UI_TEXT_ALIGN_Y i3UICaptionControl::getTextAlignY(void)
{
	if( m_pText != nullptr)	return m_pText->getAlignY();
	return m_AlignY;
}

void i3UICaptionControl::SetTextAlign( I3UI_TEXT_ALIGN_X AlignX, I3UI_TEXT_ALIGN_Y AlignY)
{
	// Invalid(-1) 값을 가지고 저장된 파일이 있다.
	// 예외 코드이지만, 계속 있어도 문제가 될 코드는 아님. 어차피 INVALID에 해당하는 코드는 없으니...
	if( AlignX == I3UI_TEXT_ALIGN_X_INVALID)
		AlignX = I3UI_TEXT_ALIGN_X_LEFT;

	if( AlignY == I3UI_TEXT_ALIGN_Y_INVALID)
		AlignY = I3UI_TEXT_ALIGN_Y_TOP;

	m_AlignX = AlignX;
	m_AlignY = AlignY;

	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->SetAlign( AlignX, AlignY);
}

void i3UICaptionControl::SetTextMargin( INT32 iMarginX, INT32 iMarginY)
{
	m_SpaceX = iMarginX;
	m_SpaceY = iMarginY;
	OnUpdateTextPos();
}
//Space of letters
void i3UICaptionControl::SetTextSpaceofLetters( INT32 nX, INT32 nY)
{
	m_SpaceofLettersX = nX;
	m_SpaceofLettersY = nY;

	if( m_pText == nullptr)
		_CreateText();

	if( m_pText != nullptr)
		m_pText->SetSpaceofLetters( nX, nY);
}

INT32 i3UICaptionControl::getTextSpaceofLettersX(void)
{
	if( m_pText != nullptr)	return m_pText->getSpaceofLettersX();
	return m_SpaceofLettersX;
}

INT32 i3UICaptionControl::getTextSpaceofLettersY(void)
{
	if( m_pText != nullptr)	return m_pText->getSpaceofLettersY();
	return m_SpaceofLettersY;
}

//Color
UINT32 i3UICaptionControl::getTextColor(void)
{
	if( m_pText != nullptr)	return m_pText->getDefColor();

	return i3Color::ConvertRGBA32( &m_TextColor );
}

COLOR* i3UICaptionControl::getTextColorPtr(void)
{
	return &m_TextColor;
}

void i3UICaptionControl::SetTextColor( UINT32 uColor)
{
	i3Color::ConvertRGBA32( uColor, &m_TextColor );

	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->SetDefColor( uColor);
}

void i3UICaptionControl::SetTextColor( COLOR* pCol)
{
	if( pCol != nullptr )
		m_TextColor = *pCol;

	if( m_pText == nullptr)		
		_CreateText();

	if( (m_pText != nullptr) && (pCol != nullptr) )
		m_pText->SetDefColor( i3Color::ConvertRGBA32( pCol ) );
}

//Shadow Color
UINT32 i3UICaptionControl::getTextShadowColor(void)
{
	if( m_pText != nullptr)	return m_pText->getShadowColor();
	return i3Color::ConvertRGBA32( &m_TextShdowColor );
}

void i3UICaptionControl::SetTextShadowColor( UINT32 uColor)
{
	i3Color::ConvertRGBA32( uColor, &m_TextShdowColor );

	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->SetShadowColor( uColor);
}

//Shadow Type
I3UI_TEXT_SHADOW i3UICaptionControl::getTextShadowType(void)
{
	if( m_pText != nullptr)	return m_pText->getShadowType();
	return m_TextShadowType;
}

void i3UICaptionControl::SetTextShadowType( I3UI_TEXT_SHADOW shadow)
{
	m_TextShadowType = shadow;

	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->SetShadowType( shadow);
}

//Style
UINT32 i3UICaptionControl::getTextStyle(void)
{
	if( m_pText != nullptr)	return m_pText->getTextStyle();
	return 0;
}

void i3UICaptionControl::setTextStyle( UINT32 style)
{
	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->setTextStyle( style);
}

void i3UICaptionControl::addTextStyle( UINT32 style)
{
	if( m_pText == nullptr)		
		_CreateText();
	
	if( m_pText != nullptr)
		m_pText->addTextStyle( style);
}

void i3UICaptionControl::removeTextStyle( UINT32 style)
{
	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->removeTextStyle( style);
}

bool i3UICaptionControl::isTextStyle( UINT32 style)
{
	if( m_pText != nullptr)	return m_pText->isTextStyle( style);
	return false;
}

bool i3UICaptionControl::isTextMultiLine(void)
{
	if( m_pText != nullptr)	return m_pText->isMultiLine();
	return false;
}

void i3UICaptionControl::setTextMultiLine( bool bFlag)
{
	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->setMultiLine( bFlag);
}

bool i3UICaptionControl::isTextReadOnly(void)
{
	if( m_pText != nullptr)	return m_pText->isReadOnly();
	return false;
}

void i3UICaptionControl::setTextReadOnly( bool bFlag)
{
	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->setReadOnly( bFlag);
}

bool i3UICaptionControl::isTextAutoLineFeed(void)
{
	if( m_pText != nullptr)	return m_pText->isAutoLineFeed();
	return m_bLineFeed;
}

INT32 i3UICaptionControl::getLinePitch(void)
{
	if( m_pText != nullptr)	return m_pText->getLinePitch();
	return m_nLinePitch;
}

bool i3UICaptionControl::isTextLineFeedSplitWord( void)
{
	if( m_pText != nullptr)
		return m_pText->isTextStyle( I3UI_TEXT_STYLE_LINFEEDWITHWORD);
	return m_bLineFeed;
}

void i3UICaptionControl::setTextLineFeedSplitWord( bool bFlag)
{
	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
	{
		bFlag ? m_pText->addTextStyle( I3UI_TEXT_STYLE_LINFEEDWITHWORD) : m_pText->removeTextStyle( I3UI_TEXT_STYLE_LINFEEDWITHWORD);
	}
}

void i3UICaptionControl::setTextAutoLineFeed( bool bFlag)
{
	m_bLineFeed = bFlag;

	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->setAutoLineFeed( bFlag);
}

void i3UICaptionControl::setLinePitch( INT32 pitch)
{
	m_nLinePitch = pitch;

	if( m_pText == nullptr)		
		_CreateText();

	if( m_pText != nullptr)
		m_pText->setLinePitch( pitch);
}

void i3UICaptionControl::SetTextCaretPos( INT32 idx)
{
	if( m_pText != nullptr)
	{
		m_pText->SetCaretPos( idx);
	}
}