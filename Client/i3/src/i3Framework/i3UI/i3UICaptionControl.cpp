#include "i3FrameworkPCH.h"
#include "i3UIManager.h"
#include "i3UITextManager.h"
#include "i3UIRenderer.h"
#include "i3UICaptionControl.h"

#include "i3UIListView_Box.h"
#include "i3UIListView_Item.h"
#include "i3UIListView_Cell.h"
#include "i3UIControl_Dummy.h"

#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/utf16_to_utf8.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/utf8_to_utf16.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#ifdef I3_DEBUG
#include "i3UIButtonComposed3.h"

#endif

I3_CLASS_INSTANCE( i3UICaptionControl);

// PropertyGrid 용 변수들입니다. 
static bool s_bUpdated = false;

static i3::vector<i3::rc_string> s_strAlignX;
static i3::vector<i3::rc_string> s_strAlignY;
static i3::vector<i3::rc_string> s_strShadowType;
static i3::vector<i3::rc_string> s_strFontQuality;


static void s_UpdateStaticVariables( void)
{
	if( s_bUpdated == false)
	{
		s_bUpdated = true;

		s_strAlignX.push_back("Left");
		s_strAlignX.push_back("Center");
		s_strAlignX.push_back("Right");
		
		s_strAlignY.push_back( "Top");
		s_strAlignY.push_back( "Center");
		s_strAlignY.push_back( "Bottom");

		s_strShadowType.push_back( "None");		
		s_strShadowType.push_back( "Thin");		
		s_strShadowType.push_back( "Thick");		
	
		s_strFontQuality.push_back( "Anti-Aliased");
		s_strFontQuality.push_back( "CleaerType");
		s_strFontQuality.push_back( "Proof");
	}
}

i3UICaptionControl::i3UICaptionControl(void)
{
	// m_lf
	m_lf.lfHeight = _PointToHeight( 10);
    m_lf.lfWidth = 0;
    m_lf.lfEscapement = 0;
    m_lf.lfOrientation = 0;
    m_lf.lfWeight = 400;	// bold => 700
    m_lf.lfItalic = FALSE;
    m_lf.lfUnderline = FALSE;
    m_lf.lfStrikeOut = FALSE;
    m_lf.lfCharSet = DEFAULT_CHARSET;
    m_lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
    m_lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
    m_lf.lfQuality = DEFAULT_QUALITY;
    m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    i3::string_ncopy_nullpad( m_lf.lfFaceName, "새굴림", sizeof(m_lf.lfFaceName));
		
	i3Color::ConvertRGBA32( 0xffffffff, &m_TextColor );
	i3Color::ConvertRGBA32( 0x000000ff, &m_TextShdowColor );
	
	s_UpdateStaticVariables();
}

i3UICaptionControl::~i3UICaptionControl(void)
{
	// 해제 순서의 다름으로 인해 ui renderer가 먼저 release되는 경우가 발생할 수 있으며,
	// m_pText의 완전한 해제가 필요하다.
	if( g_pUIRenderer == nullptr)
	{
		I3_SAFE_RELEASE( m_pText);
		return;
	}

	i3UITextManager * pTextSys = g_pUIRenderer->getTextManager();
	if((m_pText != nullptr) && (pTextSys != nullptr))
	{
		pTextSys->FreeText( m_pText);

		I3_SAFE_RELEASE( m_pText);
	}

}

void i3UICaptionControl::_UpdateShapeState(void)
{
	if( m_pText != nullptr && m_pText->getText() != nullptr &&
		i3::generic_string_size( m_pText->getText()) > 0)
	{
		bool bEnable = isEnable() && isReadyToShow() && isVisible();
		m_pText->RenderText();
		m_pText->setVisible( bEnable);

		if( bEnable && m_nShapeCount > 0 && m_hSprites != nullptr && m_bResetSprites == false)
		{
			m_pText->AdjustSpriteOrder( m_hSprites[m_nShapeCount-1]);
		}
	}
}

void i3UICaptionControl::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	if( m_pText != nullptr)
	{
		bEnable = bEnable && isReadyToShow();

		m_pText->setVisible( bEnable);

		if( bEnable && m_nShapeCount > 0 && m_bResetSprites == false)
		{
			m_pText->AdjustSpriteOrder( m_hSprites[m_nShapeCount-1]);
		}
	}
}

void i3UICaptionControl::OnCompletedLoading(void)
{
	i3UIControl::OnCompletedLoading();

	// Loading이 완료되어, 처음 화면에 뿌려지려 함.
	if( m_pText != nullptr && m_hSprites != nullptr)
	{
		bool bEnable = isEnable() && isReadyToShow();

		m_pText->setVisible( bEnable);

		if( bEnable && m_nShapeCount > 0 && m_bResetSprites == false)
		{
			m_pText->AdjustSpriteOrder( m_hSprites[m_nShapeCount-1]);
		}
	}
}

UINT32 i3UICaptionControl::_CalcTextWeight(void)
{
	REAL32 ry = 1.0f;
	UINT32 weight = 0;
	INT32 fontWeight = m_lf.lfWeight;

	if( i3UI::getViewer() != nullptr)
		ry = i3UI::getUIResolutionHeight(); // / I3UI_RESOLUTION_HEIGHT;

	switch( PRIMARYLANGID(GetSystemDefaultLangID()) )
	{
	case LANG_KOREAN:
		{
			weight = (UINT32)( fontWeight * ry);		//	fixed value. don't change - komet

			UINT32 val = weight % 100;

			if( val < 50) weight -= val;
			else weight += (100 - val);
		}
		break;

	case LANG_THAI:
		{
			weight = (UINT32)fontWeight;		//	fixed value. don't change - komet
		}
		break;
	default:
		{
			weight = (UINT32)( fontWeight * ry);		//	fixed value. don't change - komet

			UINT32 val = weight%100;

			if( val < 50) weight -= val;
			else weight += (100 - val);
		}
		break;
	}

	return weight;
}

void i3UICaptionControl::_CreateText( void)
{
	if( m_MaxTextCount <= 0 || g_pUIRenderer == nullptr)
		return;

	i3UIText * pOldText = m_pText;

	i3UITextManager * pTextSys = g_pUIRenderer->getTextManager();
	
	m_pFont = _GetUIFont();
	
	m_pText = pTextSys->CreateText(0);

	I3ASSERT( m_pText != pOldText);
	
	I3_MUST_ADDREF( m_pText);

	m_pText->SetFont( m_pFont);
	
	m_pText->SetText( L"...");
	m_rEllipsisWidth = (REAL32) m_pText->getExtentWidth();
	m_nEllipsisSpace = m_pText->getSpaceofLettersX();
	m_pText->SetText( L"");

	bool bKeepFocus = false;

	if( pOldText != nullptr)
	{
		if( pOldText->isEditing())
			bKeepFocus = true;

		pOldText->CopyStyles( m_pText);
		pTextSys->FreeText( pOldText);
		I3_MUST_RELEASE( pOldText);
	}

	if( bKeepFocus)
		m_pText->BeginEdit();

	OnUpdateTextPos();


	{		
		m_pText->SetAlign( m_AlignX, m_AlignY );
		m_pText->SetDefColor( i3Color::ConvertRGBA32( &m_TextColor ) );
		m_pText->SetShadowColor( i3Color::ConvertRGBA32( &m_TextShdowColor ) );
		m_pText->SetShadowType( m_TextShadowType );
		m_pText->SetSpaceofLetters( m_SpaceofLettersX, m_SpaceofLettersY );
		m_pText->setLinePitch( m_nLinePitch ); 
		m_pText->setAutoLineFeed( m_bLineFeed ); 
	}
}

void i3UICaptionControl::_UpdateTextCaret( REAL32 rDeltaSeconds)
{
	I3ASSERT( m_pText != nullptr);

	if( m_pText->isEditing())
	{
		m_rCaretTime += rDeltaSeconds;

		if( m_rCaretTime > 1.f)
			m_rCaretTime -= 1.f;

		m_pText->SetCaretAlpha( i3Math::sin( m_rCaretTime * I3_PI));
	}
}

template<class StringType>
void i3UICaptionControl::_SetTextEllipsis( const StringType& wstrText)
{
	m_pText->SetText( wstrText);			// pText를 컨테이너로 바꿀수 있으면 더 좋을 것이다..
	
	if( IsEllipsisEnable() )
	{
		i3UICaptionControl::_TextEllipsisInternal();
	}
}

void	i3UICaptionControl::_SwapTextEllipsis( i3::wstring& strTmp)
{
	m_pText->SwapText( strTmp);		// 스웝되는 순간 strTmp는 쓰면 안된다..
	
	if (IsEllipsisEnable() )
	{
		i3UICaptionControl::_TextEllipsisInternal();
	}
}

void	i3UICaptionControl::_TextEllipsisInternal()
{
	INT32 w = (INT32) getWidth();

	// 글폭이 컨트롤폭을 넘어가면 "..."으로 끊는다
	if( m_pText->getExtentWidth() + m_SpaceX > w)
	{
		// Ellipsis가 켜졌는지 여부
		m_bVisibleEllipsis = true;

		INT32 ellipsisLength = 0;

		REAL32 margin = (REAL32)(w - m_rEllipsisWidth - m_SpaceX);
		margin -= (m_pText->getSpaceofLettersX() - m_nEllipsisSpace);
		margin -= (m_pText->getSpaceofLettersX() - m_nEllipsisSpace);

		// "..." 폭을 제외한 여유공간에 들어갈 글자 계산
		i3UILetter* pLetter = m_pText->getFirstLetter();

		while( margin > 0.0f)
		{
			I3ASSERT( pLetter != nullptr);

			margin -= pLetter->getWidth();
			margin -= m_pText->getSpaceofLettersX();

			if( margin > 0.0f)
				ellipsisLength++;

			pLetter = pLetter->getNext();
		}

		// 계산한 위치에 "..."을 붙인다
	//	WCHAR16 output[2048] = {0};
		i3::wstring wstr_output;
		
		if( ellipsisLength > 0)
		{
			if( m_pText->ContainsEscapeSequence())
			{
				INT32 convIdx = m_pText->ConvertIndexToRawString( ellipsisLength);

				i3::string_ncopy_nullpad( wstr_output, m_pText->getTextRange(), convIdx);
			}
			else
			{
				i3::string_ncopy_nullpad( wstr_output, m_pText->getTextRange(), ellipsisLength);
			}
		}

		i3::generic_string_cat( wstr_output, L"...");	
	//	output[i3::generic_string_size(wstr_output)] = 0;

		m_pText->SwapText( wstr_output);	// output이 스택메모리이기 때문에 bCopy = false는 에러구현이 된다..
	}
	else
	{
		m_bVisibleEllipsis = false;
	}
	
}



/*virtual*/ void i3UICaptionControl::OnUpdate( REAL32 rDeltaSeconds)
{
	i3UIControl::OnUpdate( rDeltaSeconds);

	if( m_pText != nullptr)
	{
		_UpdateTextCaret( rDeltaSeconds);
	}
}

void i3UICaptionControl::SetMaxTextCount( INT32 cnt, bool bRecreate)
{
	// Count는 항상 32의 배수로 설정한다.
	//m_nLimit = cnt;

	cnt = ((cnt + 127) / 128) * 128;		//buffer를 충분히 키우지 않으면 Line별로 Caching한 정보를 제대로 활용할 수 없다. : Recreate 때문

	if( m_MaxTextCount < cnt)
	{
		m_MaxTextCount = cnt;

		if( bRecreate)
			_CreateText();
	}
}

INT32 i3UICaptionControl::AdjustSpriteOrder( INT32 hSpriteEnd, bool bRec)
{
	/*
	INT32 idxEnd;
	
	idxEnd = i3UIControl::AdjustSpriteOrder( hSpriteEnd, bRec);

	if( m_pText != nullptr && idxEnd != -1)
	{
		idxEnd = m_pText->AdjustSpriteOrder( idxEnd);
	}

	return idxEnd;
	*/

	return 0;
}

i3UIFont* i3UICaptionControl::_GetUIFont( void)
{
	if( g_pUIRenderer == nullptr)
		return nullptr;

	//get i3UIFont* from m_lf
	i3UITextManager* pTextSys = g_pUIRenderer->getTextManager();
	I3ASSERT( pTextSys != nullptr);

	char szFont[LF_FACESIZE];
	i3::string_ncopy_nullpad( szFont, m_lf.lfFaceName, sizeof( szFont));

	INT32 nHeight = m_lf.lfHeight;
	
	bool bBold = false, bItalic = false, bUnderline = false, bStrikeout = false;
	if( m_lf.lfWeight > 400)	bBold = true;
	if( m_lf.lfItalic > 0)		bItalic = true;
	if( m_lf.lfUnderline > 0)	bUnderline = true;
	if( m_lf.lfStrikeOut > 0)	bStrikeout = true;
	
	i3UIFont* pFont = pTextSys->GetFont( szFont, nHeight, bBold, bItalic, bUnderline, bStrikeout, m_FontQuality);
	return pFont;
}

void i3UICaptionControl::SetFont( LOGFONTA* plf)
{
	m_lf = *plf;

	SetFont( m_lf.lfFaceName, _HeightToPoint( m_lf.lfHeight), m_lf.lfWeight);
}

void i3UICaptionControl::SetFont( const char * pszFont, INT32 FontSize, UINT32 weight, bool bRecreate)
{
	i3::string_ncopy_nullpad( m_lf.lfFaceName, pszFont, sizeof( m_lf.lfFaceName));
	m_lf.lfHeight = _PointToHeight( FontSize);
	m_lf.lfWeight = weight;

	if( m_pText != nullptr)
	{
		m_pFont = _GetUIFont();

		m_pText->SetFont( m_pFont);
	}
}

void i3UICaptionControl::SetBlock( INT32 nStart, INT32 nEnd)
{
	if( m_pText != nullptr)
	{
		INT32 nMin = min( nStart, nEnd);
		INT32 nMax = max( nStart, nEnd);
		
		m_pText->SetBlock( nMin, nMax);

		addState( I3UI_STATE_NEED_UPDATE);
	}
}

void i3UICaptionControl::SetCaption( const WCHAR16* pszCaption)
{
	if( pszCaption != nullptr)
	{
		m_strCaption = pszCaption;

		SetText( m_strCaption );
	}
}

template<class StringType>
void	i3UICaptionControl::SetTextImpl(const StringType& str)
{

	// 할당된 텍스트 크기보다 큰 텍스트가 오면 재생성합니다.
	INT32 nCount = i3::generic_string_size( str);

	if( nCount > 0)
	{
		if((m_pText == nullptr) )//|| ( nCount > m_MaxTextCount) )
		{
			SetMaxTextCount( nCount);
		}
	}

	if( m_pText != nullptr)
	{
		//m_pText->SetDebugging( i3String::NCompare( GetName(), "i3UIStaticText16", MAX_PATH) == 0 && pText[0] != L'방');

		_SetTextEllipsis( str);
		
		//m_pText->SetText( (const STR16)pText, bCopy);
		
		bool bReady = isReadyToShow();
		bool bEnable = isEnable() && bReady;

		m_pText->setVisible( bEnable);

		if( bEnable && m_nShapeCount > 0)
		{
			if((isState( I3UI_STATE_INVALIDSPRITE) == false) && (m_hSprites != nullptr) && m_bResetSprites == false)
			{
				m_pText->AdjustSpriteOrder( m_hSprites[m_nShapeCount-1]);
			}
		}
	}

	SetTextShadowType( m_TextShadowType);

}

void	i3UICaptionControl::SetText( const i3::wliteral_range& wTextRng)
{
	i3UICaptionControl::SetTextImpl(wTextRng);
}

void	i3UICaptionControl::SetText( const i3::rc_wstring& wstrText)
{
	i3UICaptionControl::SetTextImpl(wstrText);
}

void	i3UICaptionControl::SetText( const i3::wstring& wstrText)
{
	i3UICaptionControl::SetTextImpl(i3::wliteral_range(wstrText) );
}


void	i3UICaptionControl::SetText( const wchar_t* wstrText)
{
	I3ASSERT( wstrText != nullptr);
	i3UICaptionControl::SetTextImpl(i3::wliteral_range(wstrText) );
}

void	i3UICaptionControl::SetText( const i3::stack_wstring& wstrText)
{
	i3UICaptionControl::SetTextImpl(i3::wliteral_range(wstrText) );
}


/*
void i3UICaptionControl::SetText( const WCHAR16 * pText)
{	
	I3ASSERT( pText != nullptr);

	// 할당된 텍스트 크기보다 큰 텍스트가 오면 재생성합니다.
	INT32 nCount = i3::generic_string_size( pText);

	if( nCount > 0)
	{
		if((m_pText == nullptr) )//|| ( nCount > m_MaxTextCount) )
		{
			SetMaxTextCount( nCount);
		}
	}

	if( m_pText != nullptr)
	{
		//m_pText->SetDebugging( i3String::NCompare( GetName(), "i3UIStaticText16", MAX_PATH) == 0 && pText[0] != L'방');

		_SetTextEllipsis( pText);
		
		//m_pText->SetText( (const STR16)pText, bCopy);
		
		bool bReady = isReadyToShow();
		bool bEnable = isEnable() && bReady;

		m_pText->setVisible( bEnable);

		if( bEnable && m_nShapeCount > 0)
		{
			if((isState( I3UI_STATE_INVALIDSPRITE) == false) && (m_hSprites != nullptr))
			{
				m_pText->AdjustSpriteOrder( m_hSprites[m_nShapeCount-1]);
			}
		}
	}
}
*/

void	i3UICaptionControl::_SwapText( i3::wstring& strTmp)
{
	
	// 할당된 텍스트 크기보다 큰 텍스트가 오면 재생성합니다.
	INT32 nCount = i3::generic_string_size( strTmp);

	if( nCount > 0)
	{
		if((m_pText == nullptr) )//|| ( nCount > m_MaxTextCount) )
		{
			SetMaxTextCount( nCount);
		}
	}

	if( m_pText != nullptr)
	{
		//m_pText->SetDebugging( i3String::NCompare( GetName(), "i3UIStaticText16", MAX_PATH) == 0 && pText[0] != L'방');

		_SwapTextEllipsis( strTmp);
		
		//m_pText->SetText( (const STR16)pText, bCopy);
		
		bool bReady = isReadyToShow();
		bool bEnable = isEnable() && bReady;

		m_pText->setVisible( bEnable);

		if( bEnable && m_nShapeCount > 0)
		{
			if((isState( I3UI_STATE_INVALIDSPRITE) == false) && (m_hSprites != nullptr) && m_bResetSprites == false)
			{
				m_pText->AdjustSpriteOrder( m_hSprites[m_nShapeCount-1]);
			}
		}
	}
}



void i3UICaptionControl::SetText( const char* pText)
{
	i3::wstring wstr;
	i3::utf8_to_utf16(pText, wstr);
	SetText( wstr);
}

void	i3UICaptionControl::SetTextMB( const char* pText)
{
	/*
	WCHAR16 wTemp[ 2048] = {0,};

	I3ASSERT( pText != nullptr);

	INT32 len = i3::generic_string_size( pText);

	//	유니코드 문자열의 길이를 받아온다.
	UINT32 codePage = i3Language::GetCurrentCodePage();
	INT32 length = MultiByteToWideChar( codePage, 0, pText, len+1, nullptr, 0);
	//	유니코드 버퍼로 문자열을 받아온다.
	MultiByteToWideChar( codePage, 0, pText, len+1, wTemp, length);
	*/
	i3::wstring wstr;
	i3::mb_to_utf16(pText, wstr);
	SetText( wstr);
}


void i3UICaptionControl::SetTextRectSize(UINT32 nWidth, UINT32 nHeight)
{
	if( m_pText != nullptr)
	{
		m_pText->SetScissorsRect(m_rtScissors);
		m_pText->SetViewRect(	m_pText->getViewRect()->left, 
								m_pText->getViewRect()->top, 
								(INT32)(nWidth * i3UI::getUIResolutionWidth()), 
								(INT32)(nHeight * i3UI::getUIResolutionHeight()));
	}
}

void i3UICaptionControl::SetTextSpace( INT32 nX, INT32 nY)
{
	m_SpaceX = nX;
	m_SpaceY = nY;

	addState( I3UI_STATE_NEED_UPDATE);

	//if( m_pText != nullptr)
	//{
	//	m_pText->SetShowPos((INT32)(-m_SpaceX * i3UI::getUIResolutionWidth()), (INT32)(-m_SpaceY * i3UI::getUIResolutionHeight()));
	//}

	/*if( m_pTextDX != nullptr)
		m_pTextDX->SetTextSpace( (INT32)(nX * i3UI::getUIResolutionWidth()), (INT32)(nY * i3UI::getUIResolutionHeight()));*/
}

const WCHAR16 * i3UICaptionControl::getText(void) const
{
	if( m_pText == nullptr)
		return nullptr;

	return m_pText->getText();
}

INT32				  i3UICaptionControl::getTextLength() const
{
	if (m_pText == nullptr) return 0;
	return INT32(m_pText->getTextLength());
}

namespace
{
	i3::const_wchar_range g_empty_literal = i3::as_literal(L"");
}

const i3::const_wchar_range& i3UICaptionControl::getTextRange() const
{
	if (m_pText == nullptr) return g_empty_literal;
	return m_pText->getTextRange();
}


void i3UICaptionControl::OnQueryStrings( i3::vector<i3::rc_string>& StrList)
{
	if( m_lf.lfFaceName[0] != 0)
	{
		// Font 이름만 등록한다. 실제 Text Data는 공통적인 경우가 거의 없다고 판단해 개별 저장한다.
		StrList.push_back( m_lf.lfFaceName);
	}

	i3UIControl::OnQueryStrings( StrList);
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 UICAPTIONCONTROL
	{
		UINT8			m_ID[4] = { 'U', 'C', 'C', '1' };
		UINT32			m_MaxTextCount = 0;
		UINT32			m_TextLength = 0;
		UINT32			m_TextStyle = 0;
		INT32			m_idxFontName = 0;
		UINT32			m_FontWeight = 0;
		INT32			m_FontSizePt = 0;
		UINT32			m_TextColor = 0;
		UINT32			m_TextShadowColor = 0;
		INT32			m_TextAlignX = 0;
		INT32			m_TextAlignY = 0;
		INT32			m_SpaceX = 0;
		INT32			m_SpaceY = 0;
		INT32			m_SpaceofLettersX = 0;
		INT32			m_SpaceofLettersY = 0;

		// 2011.10.22 추가
		UINT8			m_Italic = 0;
		UINT8			m_Underline = 0;
		UINT8			m_StrikeOut = 0;
		UINT8			m_FontQuality = 0;

		UINT8			m_ShadowType = 0;

		UINT8			pad2[3] = { 0 };
		UINT32			pad[28] = { 0 };
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3UICaptionControl::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	pack::UICAPTIONCONTROL data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);

	data.m_MaxTextCount		= m_MaxTextCount;

	if( m_pText != nullptr)
	{
		I3ASSERT( m_pText->getText() != nullptr);
		data.m_TextLength	= m_pText->getTextLength();
	}

	if( m_lf.lfFaceName[0] != 0)
	{
		INT32 idx = i3::vu::int_index_of(pResFile->GetStringTable(), m_lf.lfFaceName);
		data.m_idxFontName	= idx;
	}
	else
		data.m_idxFontName	= -1;

	data.m_FontWeight		= m_lf.lfWeight;
	data.m_FontSizePt		= _HeightToPoint( m_lf.lfHeight);

	data.m_TextStyle		= getTextStyle();
	data.m_TextColor		= getTextColor();
	data.m_TextShadowColor	= getTextShadowColor();
	data.m_TextAlignX		= getTextAlignX();
	data.m_TextAlignY		= getTextAlignY();

	data.m_Italic			= m_lf.lfItalic;
	data.m_Underline		= m_lf.lfUnderline;
	data.m_StrikeOut		= m_lf.lfStrikeOut;

	I3ASSERT( getFontQuality() < 255);
	data.m_FontQuality		= (UINT8) getFontQuality();

	I3ASSERT( getTextShadowType() < 255);
	data.m_ShadowType		= (UINT8) getTextShadowType();
	
	data.m_SpaceX			= m_SpaceX;
	data.m_SpaceY			= m_SpaceY;
	data.m_SpaceofLettersX	= getTextSpaceofLettersX();
	data.m_SpaceofLettersY	= getTextSpaceofLettersY();

	Rc = pResFile->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_TextLength > 0)
	{
		Rc = pResFile->Write( getText(), sizeof(WCHAR16) * data.m_TextLength);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3UICaptionControl::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	pack::UICAPTIONCONTROL data;
	const char * pszFontName;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_idxFontName != -1)
		pszFontName = pResFile->GetStringTable()[ data.m_idxFontName].c_str();
	else
		pszFontName = "굴림";

	i3::safe_string_copy( m_lf.lfFaceName, pszFontName, LF_FACESIZE);

	m_lf.lfHeight = _PointToHeight( data.m_FontSizePt);
	m_lf.lfWeight = data.m_FontWeight;
	m_lf.lfItalic = data.m_Italic;
	m_lf.lfUnderline = data.m_Underline;
	m_lf.lfStrikeOut = data.m_StrikeOut;

	if( data.m_TextColor == 0 )
		data.m_TextColor = 0xffffffff;

	SetTextAlign( (I3UI_TEXT_ALIGN_X)data.m_TextAlignX, (I3UI_TEXT_ALIGN_Y)data.m_TextAlignY);
	
	SetTextColor( data.m_TextColor);

	SetTextShadowType( (I3UI_TEXT_SHADOW) data.m_ShadowType);
	SetTextShadowColor( data.m_TextShadowColor);

	SetTextSpaceofLetters( data.m_SpaceofLettersX, data.m_SpaceofLettersY);
	

	SetFontQuality( (I3UI_FONT_QUALITY) data.m_FontQuality);

	SetTextSpace( data.m_SpaceX, data.m_SpaceY);
	
	if( data.m_MaxTextCount > 0)
	{
		SetMaxTextCount( data.m_MaxTextCount);
	}

	// 문자열을 읽어들인다.
	// m_szIMEBuf를 임시로 사용한다.
	if( data.m_TextLength > 0)
	{
		WCHAR16 szText[2048] = {0};	//뒤에 Null문자를 추가해주지 않으면 속도저하를 가져올 수 있다. 

		I3ASSERT( data.m_TextLength < 2048);

		Rc = pResFile->Read( szText, sizeof( WCHAR16) * data.m_TextLength);

		I3_CHKIO( Rc);
		Result += Rc;

		SetText( szText);
	}
	else
	{
		SetText( "");
	}

	setTextStyle( data.m_TextStyle);

	return Result;
}

bool i3UICaptionControl::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->addAttr( "MaxTextCount", m_MaxTextCount);		//m_nLimit

	if(	!m_strCaption.empty() )
	{
		i3::string utf8;

		i3::utf16_to_utf8( m_strCaption, utf8);

		pXML->addAttr( "Caption", utf8.c_str());
	}	
		
	pXML->addAttr( "FontName",		m_lf.lfFaceName);
	pXML->addAttr( "FontSize",		_HeightToPoint( m_lf.lfHeight));
	pXML->addAttr( "FontWeight",	m_lf.lfWeight);
	pXML->addAttr( "Italic",		m_lf.lfItalic);
	pXML->addAttr( "Underline",		m_lf.lfUnderline);
	pXML->addAttr( "StrikeOut",		m_lf.lfStrikeOut);
	pXML->addAttr( "FontQuality",	m_FontQuality);

	pXML->addAttr( "TextStyle",		getTextStyle());
	pXML->addAttr( "TextAlignX",	getTextAlignX());
	pXML->addAttr( "TextAlignY",	getTextAlignY());
	
	COLOR col;
	i3Color::ConvertRGBA32( getTextColor(), &col);
	i3Color::SetToXML( pXML, "TextColor", &col);

	i3Color::ConvertRGBA32( getTextShadowColor(), &col);
	i3Color::SetToXML( pXML, "TextShadowColor", &col);
	pXML->addAttr( "TextShadowType", getTextShadowType());
	

	pXML->addAttr( "TextSpaceX",	m_SpaceX);
	pXML->addAttr( "TextSpaceY",	m_SpaceY);

	pXML->addAttr( "TextSpaceofLettersX",	getTextSpaceofLettersX());
	pXML->addAttr( "TextSpaceofLettersY",	getTextSpaceofLettersY());

	bResult = true;

	return bResult;
}

bool i3UICaptionControl::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;
	const char * pszText;

	bResult = i3UIControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	UINT32 uVal;

	pXML->getAttr( "MaxTextCount", &uVal);
	if( uVal > 0)
	{
		SetMaxTextCount( uVal);		//m_nLimit
	}
	
	pXML->getAttr( "TextStyle", &uVal);
	setTextStyle( uVal);

	pszText = pXML->getAttr( "FontName");
	if( pszText != nullptr)
		strncpy( m_lf.lfFaceName, pszText, sizeof( m_lf.lfFaceName) - 1);
	
	INT32 nSizeInPoint = 0;
	pXML->getAttr( "FontSize",	&nSizeInPoint);
	m_lf.lfHeight = _PointToHeight( nSizeInPoint);

	INT32 val = 0;
	pXML->getAttr( "FontWeight", &val); 
	m_lf.lfWeight = (LONG) val;

	pXML->getAttr( "Italic",	&val); 
	m_lf.lfItalic = (BYTE)val;

	pXML->getAttr( "Underline", &val); 
	m_lf.lfUnderline = (BYTE)val;

	pXML->getAttr( "StrikeOut", &val);
	m_lf.lfStrikeOut = (BYTE)val;

	INT32 nVal1, nVal2;

	pXML->getAttr( "TextAlignX",	&nVal1);
	pXML->getAttr( "TextAlignY",	&nVal2);
	SetTextAlign( (I3UI_TEXT_ALIGN_X)nVal1, (I3UI_TEXT_ALIGN_Y)nVal2);

	COLOR col;
	i3Color::GetFromXML( pXML, "TextColor", &col);

	if( col.r == 0 &&
		col.g == 0 &&
		col.b == 0 &&
		col.a == 0 )
	{
		col.a = col.b = col.g = col.r = 1;
	}

	SetTextColor( &col);

	pXML->getAttr( "TextShadowType", &nVal1);
	SetTextShadowType( (I3UI_TEXT_SHADOW)nVal1);

	i3Color::GetFromXML( pXML, "TextShadowColor", &col);
	SetTextShadowColor( i3Color::ConvertRGBA32( &col));
	
	pXML->getAttr( "TextSpaceX",	&m_SpaceX);
	pXML->getAttr( "TextSpaceY",	&m_SpaceY);

	pXML->getAttr( "TextSpaceofLettersX",	&nVal1);
	pXML->getAttr( "TextSpaceofLettersY",	&nVal2);
	SetTextSpaceofLetters( nVal1, nVal2);
	
	pXML->getAttr( "FontQuality", (INT32*)&m_FontQuality);

	const char* buf = pXML->getAttr( "Caption");
	if( buf != nullptr)
	{
		i3::utf8_to_utf16( i3::string( buf), m_strCaption);
	}
	else
	{
		// "Caption" Attr가 없다는 것은, 빈 문자열이기 때문에 초기화. (i3UIFrameWnd Class의 경우, Constructor에서 "Frame Window"로 초기화 함.)
		m_strCaption = L"";
	}

	bResult = true;

	return bResult;
}

void i3UICaptionControl::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of< i3UICaptionControl* >(pDest));

	i3UICaptionControl* pCaption = (i3UICaptionControl*)pDest;

	//Copy properties
	pCaption->m_strCaption = m_strCaption;

	pCaption->m_lf = m_lf;
	pCaption->m_FontQuality = m_FontQuality;

	pCaption->m_SpaceX			= m_SpaceX;
	pCaption->m_SpaceY			= m_SpaceY;
	pCaption->m_nLimit			= m_nLimit;
	pCaption->m_AlignX			= m_AlignX;
	pCaption->m_AlignY			= m_AlignY;
	pCaption->m_TextShadowType	= m_TextShadowType;
	pCaption->m_TextColor		= m_TextColor;
	pCaption->m_SpaceofLettersX	= m_SpaceofLettersX;
	pCaption->m_SpaceofLettersY	= m_SpaceofLettersY;
	pCaption->m_nLinePitch		= m_nLinePitch;
	pCaption->m_bLineFeed		= m_bLineFeed;
	pCaption->m_bLineFeedSplitWord	= m_bLineFeedSplitWord;
	
	if( m_strCaption.size() > 0)
		pCaption->SetText( m_strCaption);
	else if( getText() != nullptr)
		pCaption->SetText( getText());
	
	m_pText->CopyStyles( pCaption->getUIText());
}

void i3UICaptionControl::OnInitAfterLoad( void)
{
	i3UIControl::OnInitAfterLoad();

	//Font
	SetFont( m_lf.lfFaceName, _HeightToPoint( m_lf.lfHeight), m_lf.lfWeight);
	
	// 문자열이 없더라도 설정해 주어야 합니다. (이진균)
	//if( !m_strCaption.empty() ) 
		SetCaption( m_strCaption.c_str() );
		
	OnUpdateTextPos();
}

// 내부 TextNode를 위해서 오버라이딩합니다 - praptor
void i3UICaptionControl::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);

	//이걸 해주지 않으면 Control의 크기만 변하고 실제 UIText의 m_Box는 변하지 않는다. 
	if( m_pText != nullptr)
	{
		i3::pack::RECT rt;

		rt.left = 0;
		rt.top = 0;
		rt.right = cx;
		rt.bottom = cy;

		GetTextRect( &rt);

		m_pText->SetScissorsRect(m_rtScissors);
		m_pText->SetViewRect( (INT32)rt.left+1, (INT32)rt.top+1, (INT32)rt.right-1, (INT32)rt.bottom-1);
	}

	//SetTextRectSize( (UINT32) cx, (UINT32) cy);
}
//
//void i3UICaptionControl::OnMove( REAL32 x, REAL32 y)
//{
//	i3UIControl::OnMove( x, y);
//}

void i3UICaptionControl::OnUpdateTextPos(void)
{
	i3::pack::RECT rt;
	if( GetTextRect( &rt) == true)	//right, bottom => width, height
	{
		// Text가 그려지는 Rect의 최대 영역을 계산하여 
		// Rect의 left,top값만 바꿔주는 방식입니다. 
		// (실제 텍스트의 Align이 아님)
		if( m_pText!= nullptr)
		{
			bool bListView_Cell = false;
			i3UIListView_Cell* pCell = nullptr;
			i3UIListView_Box* pBox = nullptr;
			if( this->same_of( i3UIListView_Cell::static_meta()))
			{
				pCell = (i3UIListView_Cell*)this;

				i3GameNode* pParent = getParent();	//Item
				pBox = (i3UIListView_Box*)pParent->getParent();	//Box

				bListView_Cell = true;
			}
			else if( this->getParent() != nullptr && this->getParent()->same_of( i3UIControl_Dummy::static_meta()))
			{
				i3UIControl_Dummy* pDummy = (i3UIControl_Dummy*)this->getParent();
				pCell = (i3UIListView_Cell*)pDummy->getParent();

				i3GameNode* pItem = pCell->getParent();	//Item
				pBox = (i3UIListView_Box*)pItem->getParent();	//Box
				
				bListView_Cell = true;
				
			}

			if( bListView_Cell && pCell != nullptr && pBox != nullptr)
			{
				i3Rect frt;
				pBox->getFullRect( pCell, &frt);

				rt.left = (REAL32)frt.left;
				rt.top = (REAL32)frt.top;
				rt.right = (REAL32)(frt.right - frt.left);
				rt.bottom = (REAL32)(frt.bottom - frt.top);
			}

			REAL32 extHeight = (REAL32)(m_pText->getExtentHeight());
			if( extHeight == 0.0f)
				extHeight = (REAL32)(m_pText->getDefFontHeight());

			// Y Align
			//REAL32 offsetY = 0.0f;
			//if( getTextAlignY() == I3UI_TEXT_ALIGN_Y_MIDDLE)			//Center
			//{
			//	offsetY = (rt.bottom - extHeight) / 2;
			//}
			//else if( getTextAlignY() == I3UI_TEXT_ALIGN_Y_BOTTOM)		//Bottom
			//{
			//	offsetY = rt.bottom - extHeight;
			//}

			//rt.top = max( rt.top, rt.top + offsetY);

			i3Rect rtIntersect;
			rtIntersect.set( (INT32)rt.left, (INT32)rt.top, (INT32)(rt.left + rt.right), (INT32)(rt.top + rt.bottom));

			INT32 nShowX = m_pText->getShowX();
			INT32 nShowY = m_pText->getShowY();

			///////////////////////////////////////////////////////////
			// 여기서 부모 컨트롤과의 Intersection을 구해서 
			// 적절히 SetView, SetShowPos() 해주어야한다...
			// 일단 ListView..
			i3GameNode* pParent = getParent();
			bool bCell = false;

			while( pParent != nullptr && !i3::same_of<i3UIListView_Box*>(pParent))
			{
				if( i3::same_of< i3UIListView_Item* >(pParent))
					bCell = true;

				pParent = pParent->getParent();
			}

			bool bDummyControl = (getParent() != nullptr) && (getParent()->same_of( i3UIControl_Dummy::static_meta()));
			if( pParent != nullptr && bCell && (this->same_of( i3UIListView_Cell::static_meta()) || bDummyControl))
			{
				nShowX = 0;
				nShowY = 0;

				i3UIListView_Box* pLVBox = (i3UIListView_Box*)pParent;
				i3Rect rtCaption;
				i3Rect rtParent;

				pLVBox->getItemBox( &rtParent);

				//i3UIListView_Cell* pCell = (i3UIListView_Cell*)this;
				//pLVBox->getFullRect( pCell, &rtCaption);		// this == i3UIListView_Cell
				//rtCaption.top = max( rtCaption.top, rtCaption.top + offsetY);
				rtCaption.set( (INT32)rt.left, (INT32)rt.top, (INT32)(rt.left + rt.right), (INT32)(rt.top + rt.bottom));

				TOUCH_TYPE touch = rtParent.Intersect( &rtCaption, &rtIntersect);

				if( touch == TOUCH_PARTIAL)
				{
					if( rtIntersect.left != rtCaption.left)
						nShowX = rtIntersect.left - rtCaption.left;

					if( rtIntersect.top != rtCaption.top)
						nShowY = rtIntersect.top - rtCaption.top;

					nShowY -= (rtCaption.getHeight() - rtIntersect.getHeight()) / 2;
					
				}
				else if( touch == TOUCH_WHOLE)
				{
					nShowX = 0;
					nShowY = 0;
				}
				else if( touch == TOUCH_NONE)
				{
					rtIntersect.set( 0, 0, 0, 0);
				}
			}
			///////////////////////////////////////////////////////////
			

			m_pText->SetScissorsRect(m_rtScissors);
			m_pText->SetViewRect( rtIntersect.left+1, rtIntersect.top+1, rtIntersect.getWidth()-1, rtIntersect.getHeight()-1);
			m_pText->SetShowPos( nShowX, nShowY);
			//m_pText->SetView( (INT32)rt.left, (INT32)rt.top, (INT32)rt.right, (INT32)rt.bottom);
			m_pText->SetCaretPos( m_pText->getCaretPos());
			//I3TRACE("i3UICaptionControl::OnUpdateTextPos   SetView: %f, %f, %f, %f, %s\n", rt.left, rt.top, rt.right, rt.bottom, m_pText->getText());

			
		}
	}
}

void i3UICaptionControl::UpdateRenderObjects()
{
	/*
	if( m_pText != nullptr)
	{
		char conv[512];

		WideCharToMultiByte( CP_ACP, 0, m_pText->getText(), -1, conv, sizeof(conv), NULL, NULL);

		I3TRACE( "TXT : %s\n", conv);
	}
	*/
	OnUpdateTextPos();

	i3UIControl::UpdateRenderObjects();
}

bool i3UICaptionControl::GetTextRect( i3::pack::RECT* pRect)
{
	bool bResult = false;

	if( m_pText != nullptr && m_pTemplate != nullptr)
	{
		bResult = m_pTemplate->GetTextArea( m_pRects, pRect);	//right, bottom => width, height
		
		pRect->left += getAccumulatedPos()->x;
		pRect->top	+= getAccumulatedPos()->y;

		pRect->left += m_SpaceX;
		pRect->top += m_SpaceY;
		pRect->right -= 2*m_SpaceX;
		pRect->bottom -= 2*m_SpaceY;

		pRect->right = max( pRect->right, 0);
		pRect->bottom = max( pRect->bottom, 0);
	}

	return bResult;
}

//Text Rect를 pRect의 크기로 설정하려면
//실제 컨트롤의 크기는 그것보다 더 커진다.
//pRect->right => width, pRect->bottom => height
void i3UICaptionControl::SetTextRect( i3::pack::RECT* pRect)
{
	if( m_pTemplate != nullptr)
	{
		i3::pack::RECT rtText;
		m_pTemplate->GetTextArea( m_pRects, &rtText);

		REAL32 ratioX = m_Size.x / rtText.right;
		REAL32 ratioY = m_Size.y / rtText.bottom;
		
		SetSizeNoNotify( pRect->right * ratioX, pRect->bottom * ratioY);
	}
}

void i3UICaptionControl::OnDraw( i3UIRenderer * pRenderer)
{
	m_bCalcDoneAccumPos = false;	// 초기화

	if( this->isEnable() == false)
		return;

	DrawShapes( pRenderer);

	{	// Draw Caption
		bool bTopDraw = false;

		if( m_pText != nullptr && isEnable() )
		{
			if ( i3::same_of<i3UIFrameWnd*>(getParent()) )
			{
				if ( static_cast<i3UIControl *> (getParent())->isStyle(I3UI_STYLE_TOP_DRAW))
				{
					bTopDraw = true;
					pRenderer->Draw_SetTopMost( true );
				}
			}

			m_pText->OnDraw( pRenderer);

			if ( bTopDraw )
			{
				pRenderer->Draw_SetTopMost( false );
			}
		}
	}

	{	// Recursive call
		i3UIControl * pChild = (i3UIControl *) getFirstChild();

		bool bTopDraw = false;

		while( pChild != nullptr)
		{
			bTopDraw = false;

			if ( pChild->isStyle(I3UI_STYLE_TOP_DRAW))
			{
				bTopDraw = true;
				pRenderer->Draw_SetTopMost( true);
			}

			pChild->OnDraw( pRenderer);

			pChild = (i3UIControl *) pChild->getNext();

			if ( bTopDraw )
			{
				pRenderer->Draw_SetTopMost( false);
			}
		}
	}
}

bool i3UICaptionControl::OnRevive( i3RenderContext * pCtx)
{
	if( m_pText != nullptr)
	{
		m_pText->CreateText();
	}

	return i3UIControl::OnRevive( pCtx);
}

bool i3UICaptionControl::OnLostDevice( bool bLostDevice)
{
	if( i3UIControl::OnLostDevice( bLostDevice) == false)
		return false;

	if( m_pText != nullptr)
	{
		m_pText->_FreeSprite();
		m_pText->_ClearLineInfoList();
	}

	return true;
}


void	i3UICaptionControl::resetSprites( bool bRelease)
{
	if (m_pText)
		m_pText->_FreeSprite();					// 텍스트로부터도 스프라잇을 빼봄..
	i3UIControl::resetSprites(bRelease);
}

void	i3UICaptionControl::EnableCtrl( bool bTrue, bool bRecursive )
{
	i3UIControl::EnableCtrl(bTrue, bRecursive);
	if (m_pText)
		m_pText->SetEnable(bTrue);
}

i3UIControl * i3UICaptionControl::TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl)
{
	if( isActivated()  && isVisible())
	{
		if (m_pText != nullptr)
		{
			const wchar_t * pszCtrl = m_pText->getText();

			if (bMustBeSame)
			{
				if (_wcsicmp(pszCtrl, pszText) == 0)
					return this;
			}
			else
			{
				if (wcsstr(pszCtrl, pszText) != nullptr)
					return this;
			}
		}
	}

	return i3UIControl::TA_ScanText(pszText, bMustBeSame, bSubControl);
}


#if defined( I3_DEBUG)
void i3UICaptionControl::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	// 먼저 Base Class의 GetTDKGridProperties를 호출
	i3UIControl::GetTDKGridProperties( List);	

	// BeginGroup은 EndGroup과 짝을 맞추어야 합니다.
	AddMFCProperty_BeginGroup( List, "i3UICaptionControl");

	
	AddMFCProperty_RcStringW( List, m_strCaption, "Caption");

	m_AlignX = getTextAlignX();
	AddMFCProperty_Combo( List, s_strAlignX, (INT32*)&m_AlignX, "Text Align X");

	m_AlignY = getTextAlignY();
	AddMFCProperty_Combo( List, s_strAlignY, (INT32*)&m_AlignY, "Text Align Y");

	i3Color::ConvertRGBA32( getTextColor(), &m_TextColor);
	AddMFCProperty_Font( List,	&m_lf, &m_TextColor, "Font");

	// 버튼의 색상은 캡션컨트롤에서 편집하지 못하도록 최대한 막아본다..
	if (i3::same_of<i3UIButton*, i3UIButtonComposed3*>(this) == false )
		AddMFCProperty_Color( List, &m_TextColor, "Font Color");
	
	m_TextShadowType = getTextShadowType();
	AddMFCProperty_Combo( List, s_strShadowType, (INT32*)&m_TextShadowType, "Shadow Type");

	/*i3Color::ConvertRGBA32( getTextShadowColor(), &s_TextShadowColor);
	AddMFCProperty_Color( pList, &s_TextShadowColor, "Shadow Color");*/
	
	AddMFCProperty_Combo( List, s_strFontQuality, (INT32*)&m_FontQuality, "Font Quality");
		
	AddMFCProperty_Int32( List, &m_SpaceX, "Left Margin");
	AddMFCProperty_Int32( List, &m_SpaceY, "Top Margin");

	m_SpaceofLettersX = getTextSpaceofLettersX();
	m_SpaceofLettersY = getTextSpaceofLettersY();
	AddMFCProperty_Int32( List, &m_SpaceofLettersX, "Space of Letters X");
	AddMFCProperty_Int32( List, &m_SpaceofLettersY, "Space of Letters Y");

	m_bLineFeed = isTextAutoLineFeed();
	AddMFCProperty_Bool( List, &m_bLineFeed, "Line Feed");

	m_nLinePitch = getLinePitch();
	AddMFCProperty_Int32( List, &m_nLinePitch, "Line Pitch");

	m_bLineFeedSplitWord = isTextLineFeedSplitWord();
	AddMFCProperty_Bool( List, &m_bLineFeedSplitWord, "Line Feed With Word");

	AddMFCProperty_EndGroup( List);
}

void i3UICaptionControl::OnTDKPropertyChanged(const char *szName)
{
	if( i3::generic_is_iequal( szName, "Caption") )
	{
		SetCaption( m_strCaption.c_str());
	}
	else if( i3::generic_is_iequal( szName, "Font")  ||
			i3::generic_is_iequal( szName, "Font Quality")  ||
			i3::generic_is_iequal( szName, "Font Color")  )
	{
		SetFont( m_lf.lfFaceName, _HeightToPoint(m_lf.lfHeight), m_lf.lfWeight);
		SetTextColor( &m_TextColor);

		//m_TextColor.a = 0xff;		
		addState( I3UI_STATE_NEED_UPDATE);
	}
	else if( i3::generic_is_iequal( szName, "Left Margin")  || 
			i3::generic_is_iequal( szName, "Top Margin") )
	{
		SetTextMargin(m_SpaceX, m_SpaceY);
	}
	else if( i3::generic_is_iequal( szName, "Text Align X")  ||
			i3::generic_is_iequal( szName, "Text Align Y") )
	{
		SetTextAlign( (I3UI_TEXT_ALIGN_X) m_AlignX, (I3UI_TEXT_ALIGN_Y) m_AlignY);
		OnUpdateTextPos();
	}
	else if( i3::generic_is_iequal( szName, "Space of Letters X")  ||
			i3::generic_is_iequal( szName, "Space of Letters Y") )
	{
		SetTextSpaceofLetters( m_SpaceofLettersX, m_SpaceofLettersY);
	}
	else if( i3::generic_is_iequal( szName, "Line Feed") )
	{
		setTextAutoLineFeed( m_bLineFeed);

		OnUpdateTextPos();
	}
	else if( i3::generic_is_iequal( szName, "Line Pitch") )
	{
		setLinePitch( m_nLinePitch);
		SetCaption( m_strCaption.c_str());
		OnUpdateTextPos();
	}
	else if( i3::generic_is_iequal( szName, "Line Feed With Word") )
	{
		setTextLineFeedSplitWord( m_bLineFeedSplitWord);

		OnUpdateTextPos();
	}
	else if(i3::generic_is_iequal( szName, "Shadow Type") )
	{
		SetTextShadowType( m_TextShadowType);
	}
	else
	{
		i3UIControl::OnTDKPropertyChanged( szName);
	}
}

void i3UICaptionControl::GetMessagesForPropertyGrid( i3::vector<PROPERTYMSG*>& List)
{
	i3UIControl::GetMessagesForPropertyGrid( List);

	PROPERTYMSG* pMsg = (PROPERTYMSG*) i3MemAlloc( sizeof( PROPERTYMSG));
	
	i3::string_ncopy_nullpad( pMsg->propertyName, "Line Feed", sizeof( pMsg->propertyName));
	
	//if( getTextAlignX() != I3UI_TEXT_ALIGN_X_LEFT)
	//	pMsg->msg = EI3TDK_PROPERTY_MSG_DISABLE;
	//else
		pMsg->msg = EI3TDK_PROPERTY_MSG_ENABLE;

	List.push_back( pMsg);
}
#endif


