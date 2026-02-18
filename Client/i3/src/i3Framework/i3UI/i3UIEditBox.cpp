#include "i3FrameworkPCH.h"
#include "i3UIEditBox.h"
#include "i3UIManager.h"
#include "i3UITemplate_EditBox.h"
#include "../i3Framework.h"
#include <algorithm>
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/generic_string_cat.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

// ============================================================================
//
// i3UIEditBox
//
// ============================================================================
I3_CLASS_INSTANCE( i3UIEditBox);

#if defined( I3_DEBUG)
static bool s_bUpdated = false;
static i3::vector<i3::rc_string> s_slMode;

static void s_UpdateStaticVariables( void)
{
	if( s_bUpdated == false)
	{
		s_bUpdated = true;

		s_slMode.push_back( "Normal");
		s_slMode.push_back( "Password");
		s_slMode.push_back( "Chat Input");
		s_slMode.push_back( "Chat View");
		s_slMode.push_back( "Scrolling Left");
		s_slMode.push_back( "Scrolling Right");
		s_slMode.push_back( "Scrolling Up");
		s_slMode.push_back( "Scrolling Down");
		s_slMode.push_back( "Numeric");
	}
}
#endif


i3UIEditBox::i3UIEditBox()
{
	SetReadOnly( false);
	
	setPasswordEnable(false);
	setChatEnable(false);
	setChatViewEnable(false);
	
#if defined( I3_DEBUG)
	s_UpdateStaticVariables();
#endif
	
	// revision 3518
	SetEllipsis(false);
}

i3UIEditBox::~i3UIEditBox()
{

	I3_SAFE_RELEASE( m_pUIScrollBar);
	if( m_pUIScrollBar != nullptr) {
		m_pUIScrollBar->EnableCtrl( false);
	}

	if( m_pIme != nullptr)
	{
		if( m_pIme->getOwner() == this)
		{
			m_pIme->setOwner( nullptr);
			m_pIme->SetUseCopyNPaste( true );
			m_pIme->KillFocus();
		}
	}
}

void i3UIEditBox::SetReadOnly( bool bFlag)
{
	if( bFlag)
	{
		m_EditBoxStyle |= I3UI_EDITBOX_STYLE_READONLY;
	}
	else
	{
		m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_READONLY;

		if( i3UI::getGlobalIme() == nullptr)
		{
			I3IME_CREATEINFO ime;

			I3ASSERT( i3Framework::getCurrentFramework() != nullptr);

			ime._hWnd = i3Framework::getCurrentFramework()->GetViewer()->GetWindowHandle();

			// GetModuleHandleEx는 Windows 2000에서 지원하지 않는 함수이므로, Windows 2000 Professional에서 지원하는 GetModuleHandle()함수로 대체합니다.
			//::GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, NULL, &ime._hInst);
			ime._hInst = ::GetModuleHandle(NULL);
			ime._nTextLimitCount = 4096;
			ime._StyleIME = 0;

			i3Ime * pIme = i3Ime::new_object_ref();
			pIme->Create( &ime);

			i3UI::setGlobalIme( pIme);
		}
	}
	
}

void i3UIEditBox::SetInputCharOnlyAlphaNumeric()
{
	SetInputCharOnlyByEditBoxStyle(I3UI_EDITBOX_STYLE_ALPHA_NUMERIC);
	ApplySetInputCharOnlyToIME();
}

void i3UIEditBox::SetInputCharOnlyNumeric()
{
	SetInputCharOnlyByEditBoxStyle(I3UI_EDITBOX_STYLE_NUMERIC);
	ApplySetInputCharOnlyToIME();
}

void i3UIEditBox::SetInputCharOnlyByEditBoxStyle(UINT32 editBoxStyle)
{
	const static UINT32 availStyle[] = {
		I3UI_EDITBOX_STYLE_NUMERIC,
		I3UI_EDITBOX_STYLE_REAL_NUMERIC,
		I3UI_EDITBOX_STYLE_ALPHA_NUMERIC,
		I3UI_EDITBOX_STYLE_ALPHA };

	const static INT32 num = sizeof(availStyle) / sizeof(availStyle[0]);

	for(INT32 i=0; i<num; i++)
	{
		UINT32 cur = availStyle[i];

		if(cur == editBoxStyle)
			addEditBoxStyle(cur);
		else
			removeEditBoxStyle(cur);
	}
}

void i3UIEditBox::SetInputAlphabetAutoCase(INT32 caseType)
{
	switch(caseType)
	{
	case 1: // 입력알파벳을 대문자로
		{
			removeEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_LOWERCASE);
			addEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_UPPERCASE);
		}
		break;

	case 2: // 입력알파벳을 소문자로
		{
			removeEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_UPPERCASE);
			addEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_LOWERCASE);			
		}
		break;

	default:
		{
			removeEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_LOWERCASE);
			removeEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_UPPERCASE);
		}
		break;
	}
}

void i3UIEditBox::SetLetterLimit( INT32 limit)
{
	m_nLimit = limit;
	if( m_pIme != nullptr)
		m_pIme->setLimit( m_nLimit);
}

void i3UIEditBox::setBorderVisible( bool bFlag)
{
	/*if( bFlag)
	{
		m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_NOBORDER;

		m_pSprite->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
	{
		m_EditBoxStyle |= I3UI_EDITBOX_STYLE_NOBORDER;

		m_pSprite->AddFlag( I3_NODEFLAG_DISABLE);
	}*/
}


void i3UIEditBox::_UpdateShapeState(void)
{
	INT32 i;

	for( i = 0; i < I3UI_EDITBOX_SHAPE_DISABLE_CLIENTAREA; ++i)
	{
		setShapeVisible( i, isValidShape(i));	
	}

	if( !isActivated())
	{
		setShapeVisible( I3UI_EDITBOX_SHAPE_DISABLE_CLIENTAREA, true);
		setShapeVisible( I3UI_EDITBOX_SHAPE_CLIENTAREA, false);
	}
	else
	{
		setShapeVisible( I3UI_EDITBOX_SHAPE_DISABLE_CLIENTAREA, false);
		setShapeVisible( I3UI_EDITBOX_SHAPE_CLIENTAREA, true);
	}

	this->_UpdateScrollBarValue();

	i3UICaptionControl::_UpdateShapeState(); 
}

void i3UIEditBox::_UpdateScrollBarRect()
{
	if( m_pUIScrollBar->isEnable())
	{
 		REAL32 x = getWidth() - m_fUIScrollBarWidth;
		REAL32 y = 0.0f;

		m_pUIScrollBar->SetSizeNoNotify( m_fUIScrollBarWidth, getHeight());
		m_pUIScrollBar->SetPosNoNotify( x, y);
	}
}

void i3UIEditBox::_UpdateScrollBarValue()
{
	if( m_pUIScrollBar == nullptr)
		return;
	if( m_pText == nullptr)
		return;
 
	INT32 nExtendedHeight = m_pText->getExtentHeight()+2;
	INT32 nBoxHeight = m_pText->getViewHeight();
	// Text 내용이 EditBox 높이보다 작으면 스크롤바 숨김(비활성화)
	// 또는 Use Scroll이 비활성화 상태면
	if( nExtendedHeight <= nBoxHeight || m_bUseScroll == false) {
		if(m_pUIScrollBar->isEnable()) {
			m_pUIScrollBar->EnableCtrl( false);
			//this->OnSetFocus();
		//	m_pUIScrollBar->SetEnable( false);
			m_pUIScrollBar->SetVisible(false);
		}

		m_pUIScrollBar->setScrollRange( 0, 0);
		m_pUIScrollBar->setPageSize( nBoxHeight);
		m_pUIScrollBar->setScrollPos( 0);

		
	}
	else {
		if( m_pUIScrollBar->isEnable() == false) {
			m_pUIScrollBar->EnableCtrl( true);
			m_pUIScrollBar->SetVisible(true);

			// 스크롤 바를 활성화 한 후, Text Rect Area 설정 및, 스크롤 마지막 pos를 재계산합니다. 2012-08-07 양승천
			i3::pack::RECT rt;
			if(GetTextRect(&rt) == true)
			{
				m_pText->SetScissorsRect(m_rtScissors);
				m_pText->SetViewRect((INT32)rt.left, (INT32)rt.top, (INT32)rt.right, (INT32)rt.bottom);
				_ScrollToEnd();
			}
		}

		m_pUIScrollBar->setScrollRange( 0, m_pText->getExtentHeight()+2);
		m_pUIScrollBar->setPageSize( nBoxHeight);
		m_pUIScrollBar->setScrollPos( m_pText->getShowY());	

		
	}

	_UpdateScrollBarRect();
}

void i3UIEditBox::_DragStart( INT32 nX, INT32 nY )
{
	if( m_pIme == nullptr || m_pText == nullptr)
		return;

	INT32 nPosX = nX - (INT32)i3Vector::GetX(&m_AccPos);
	INT32 nPosY = nY - (INT32)i3Vector::GetY(&m_AccPos);

	INT32 height = 0;
	if( isTextMultiLine() == false)
		height = (INT32) getHeight();

	INT32 nIdx = m_pText->FindNearestLetterByPos( nPosX, nPosY, height );

	if(nIdx != -1) {
		SetTextCaretPos( nIdx);
		m_pIme->SetIsBlock( true);
		m_pIme->SetBlockIdx( nIdx, nIdx);
		SetBlock( nIdx, nIdx);
	}
}

void i3UIEditBox::_Dragging( INT32 nX, INT32 nY )
{
	if( m_pIme == nullptr || m_pText == nullptr)
		return;

	INT32 nPosX = nX - (INT32)i3Vector::GetX(&m_AccPos);
	INT32 nPosY = nY - (INT32)i3Vector::GetY(&m_AccPos);

	INT32 height = 0;
	if( isTextMultiLine() == false)
		height = (INT32) getHeight();

	INT32 nIdx = m_pText->FindNearestLetterByPos( nPosX, nPosY, height );

	if(nIdx != -1) {
		SetTextCaretPos( nIdx);

		INT32 nDragBlockStart = m_pIme->GetBlockStartIdx();
		m_pIme->SetBlockIdx( nDragBlockStart, nIdx);
		SetBlock( nDragBlockStart, nIdx);
	}
}

void i3UIEditBox::_DragEnd( INT32 nX, INT32 nY )
{
	if( m_pIme == nullptr || m_pText == nullptr)
		return;

	INT32 nPosX = nX - (INT32)i3Vector::GetX(&m_AccPos);
	INT32 nPosY = nY - (INT32)i3Vector::GetY(&m_AccPos);

	INT32 height = 0;
	if( isTextMultiLine() == false)
		height = (INT32) getHeight();

	INT32 nIdx = m_pText->FindNearestLetterByPos( nPosX, nPosY, height );

	if(nIdx != -1) {
		SetTextCaretPos( nIdx);

		INT32 nDragBlockStart = m_pIme->GetBlockStartIdx();

		if( nDragBlockStart == nIdx)
		{
			m_pIme->SetIsBlock( false);
			m_pIme->SetInsertPosition( nIdx);
		}

		m_pIme->SetBlockIdx( nDragBlockStart, nIdx);
		SetBlock( nDragBlockStart, nIdx);
	}
}

void i3UIEditBox::OnUpdate( REAL32 rDeltaSeconds)
{
	i3UICaptionControl::OnUpdate( rDeltaSeconds);

	_UpdateTextScroll( rDeltaSeconds);
}

void i3UIEditBox::OnUpdateTextPos( void)
{
	i3::pack::RECT rt;
	if( GetTextRect( &rt) == true)	//right, bottom => width, height
	{
		// Text가 그려지는 Rect의 최대 영역을 계산하여 
		// Rect의 left,top값만 바꿔주는 방식입니다. 
		// (실제 텍스트의 Align이 아님)
		if( m_pText!= nullptr)
		{
/*			이 코드는 돌지 않을것으로 판단되어 주석 겁니다... ( 2012.08.08.수빈)
			if( is_same( i3UIListView_Cell::static_meta()))
			{
				i3UIListView_Cell* pCell = (i3UIListView_Cell*)this;

				i3GameNode* pParent = getParent();	//Item
				i3UIListView_Box* pBox = (i3UIListView_Box*)pParent->getParent();	//Box

				i3Rect frt;
				pBox->getFullRect( pCell, &frt);

				rt.left = (REAL32)frt.left;
				rt.top = (REAL32)frt.top;
				rt.right = (REAL32)(frt.right - frt.left);
				rt.bottom = (REAL32)(frt.bottom - frt.top);
			}
*/
			REAL32 extHeight = (REAL32)(m_pText->getExtentHeight()+2);
			if( extHeight == 0.0f)
				extHeight = (REAL32)(m_pText->getDefFontHeight());

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
				if( i3::same_of<i3UIListView_Item*>(pParent))
					bCell = true;

				pParent = pParent->getParent();
			}

			if( pParent != nullptr && bCell && same_of( i3UIListView_Cell::static_meta()))
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
			
			if( m_BoxProperty == UIBOX_SCROLLING_LEFT ||
				m_BoxProperty == UIBOX_SCROLLING_RIGHT)
			{
				nShowX = m_TextStartPosX;
			}
			else if( m_BoxProperty == UIBOX_SCROLLING_UP ||
				m_BoxProperty == UIBOX_SCROLLING_DOWN)
			{
				nShowY = m_TextStartPosY;
			}
			
			m_pText->SetScissorsRect(m_rtScissors);
			m_pText->SetViewRect( rtIntersect.left, rtIntersect.top, rtIntersect.getWidth(), rtIntersect.getHeight());
			m_pText->SetShowPos( nShowX, nShowY);
			m_pText->SetCaretPos( m_pText->getCaretPos());
		}
	}
}

void i3UIEditBox::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);

	if( bEnable == false && m_pText != nullptr)
		m_pText->EndEdit();
	
	if(m_bUseScroll)
		if( m_pUIScrollBar != nullptr)	m_pUIScrollBar->EnableCtrl( bEnable);
}

void i3UIEditBox::ApplySetInputCharOnlyToIME()
{	
	if(!m_pIme)
		return;

	bool ignoreKeyInputIMEMode = false;

	// I3UI_EDITBOX_STYLE_NUMERIC
	if( isEditBoxStyle( I3UI_EDITBOX_STYLE_NUMERIC))
	{
		m_pIme->addStyle( IME_STYLE_NUMERIC_ONLY);

		m_pIme->removeStyle( IME_STYLE_NUMERIC_AND_NUMERICAL);
		m_pIme->removeStyle( IME_STYLE_ALPHA_NUMERIC_ONLY);
		m_pIme->removeStyle( IME_STYLE_ALPHA_ONLY);
	}
	else
	{
		m_pIme->removeStyle( IME_STYLE_NUMERIC_ONLY);
	}

	// I3UI_EDITBOX_STYLE_REAL_NUMERIC
	if( isEditBoxStyle( I3UI_EDITBOX_STYLE_REAL_NUMERIC))
	{
		m_pIme->addStyle( IME_STYLE_NUMERIC_AND_NUMERICAL);

		m_pIme->removeStyle( IME_STYLE_NUMERIC_ONLY);
		m_pIme->removeStyle( IME_STYLE_ALPHA_NUMERIC_ONLY);
		m_pIme->removeStyle( IME_STYLE_ALPHA_ONLY);		
	}
	else
	{
		m_pIme->removeStyle( IME_STYLE_NUMERIC_AND_NUMERICAL);
	}


	// I3UI_EDITBOX_STYLE_ALPHA_NUMERIC
	if( isEditBoxStyle(I3UI_EDITBOX_STYLE_ALPHA_NUMERIC))
	{
		m_pIme->addStyle(IME_STYLE_ALPHA_NUMERIC_ONLY);

		m_pIme->removeStyle(IME_STYLE_NUMERIC_ONLY);
		m_pIme->removeStyle(IME_STYLE_NUMERIC_AND_NUMERICAL);
		m_pIme->removeStyle(IME_STYLE_ALPHA_ONLY);		

		ignoreKeyInputIMEMode = true;
	}
	else
	{
		m_pIme->removeStyle(IME_STYLE_ALPHA_NUMERIC_ONLY);
	}


	// I3UI_EDITBOX_STYLE_ALPHA
	if( isEditBoxStyle(I3UI_EDITBOX_STYLE_ALPHA))
	{
		m_pIme->addStyle(IME_STYLE_ALPHA_ONLY);

		m_pIme->removeStyle(IME_STYLE_NUMERIC_ONLY);
		m_pIme->removeStyle(IME_STYLE_NUMERIC_AND_NUMERICAL);
		m_pIme->removeStyle(IME_STYLE_ALPHA_NUMERIC_ONLY);

		ignoreKeyInputIMEMode = true;
	}
	else
	{
		m_pIme->removeStyle(IME_STYLE_ALPHA_ONLY);
	}

	// 알파벳 입력시 자동 대소문자 자동변환처리
	m_pIme->removeStyle(IME_STYLE_AUTO_UPPERCASE);
	m_pIme->removeStyle(IME_STYLE_AUTO_LOWERCASE);

	if(isEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_UPPERCASE))
		m_pIme->addStyle(IME_STYLE_AUTO_UPPERCASE);
	else if(isEditBoxStyle(I3UI_EDITBOX_STYLE_AUTO_LOWERCASE))
		m_pIme->addStyle(IME_STYLE_AUTO_LOWERCASE);

	// 한영전환키 막는처리(숫자,알파벳입력전용모드시)
	if(ignoreKeyInputIMEMode)
		m_pIme->addStyle(IME_STYLE_IGNORE_KEYINPUT_IME_MODE); // 한영전환무시
	else
		m_pIme->removeStyle(IME_STYLE_IGNORE_KEYINPUT_IME_MODE);
}

void i3UIEditBox::_loadEditInfo( void)
{
	if( m_pIme == nullptr)	return;

	m_pIme->setOwner( this);
	m_pIme->SetUseCopyNPaste( m_bUseCopyNPaste );
	m_pIme->SetImeWindowPosition( (INT32)m_AccPos.x, (INT32)m_AccPos.y);
	m_pIme->SetEnableMultiLine( isEditBoxStyle( I3UI_EDITBOX_STYLE_MULTILINE));

	if( m_bLimitByteSize )
		m_pIme->removeStyle( IME_STYLE_LIMIT_CHARACTER);
	else
		m_pIme->addStyle( IME_STYLE_LIMIT_CHARACTER);

	m_pIme->setLimit( m_nLimit);

	ApplySetInputCharOnlyToIME();

	m_pIme->setAlphaNumericOnly(IsInputCharOnlyAlphaNumeric());

	if( m_pText != nullptr)
	{
		if( m_pText->getText() != nullptr)
		{
			i3::wstring conv(m_pText->getText());
			RemoveColorKeyword( conv);

			SetText(conv);		// 이것 안해주면....텍스트가 남아버릴것 같다..(2013.06.05.수빈)
			m_pIme->SetInputText( (WCHAR*)conv.c_str(), i3::generic_string_size(conv.c_str()), (bool)(m_nLimit > 0));
		}
	}
	else
	{
		// edit box를 위한 caret은 m_pText가 nullptr인 상황에서 작동 하지 않는다.
		// 따라서 Init문자열을 넣어주어 m_pText를 생성 시킨후, 문자열을 초기화 한다.
		SetText(L"Init");
		SetText(L"");
	}

	I3ASSERT( m_pText != nullptr);

	i3UIFont * pFont = m_pText->getFont();

	m_pIme->setImeFont( pFont->getFontHandle(), &m_lf);
}

void i3UIEditBox::_UpdateTextScroll( REAL32 rDeltaSeconds)
{
	if( getUIText() != nullptr &&
		isScrolling() )
	{
		i3::pack::RECT rect;
		VEC3D vOff;

		switch( m_ScrollState)
		{
		case UBSS_INIT :
			{
				m_ScrollState = UBSS_FLOW;

				rect.left = 0.f;
				rect.top = 0.f;
				rect.right = getWidth();
				rect.bottom = getHeight();

				CalcAlignedPos( &rect, &vOff);
				
				// Scroll 기능
				// m_bContinueScrollPos == true : 문자가 박스 내 입력되있는 상태에서 스크롤 시작
				switch( m_BoxProperty)
				{
				case UIBOX_SCROLLING_LEFT :
					m_MoveOffset = (REAL32) - getUIText()->getExtentWidth();

					if(m_bContinueScrollPos == true)
						m_CurrentOffset = 0.0f;
					else
						m_CurrentOffset = getWidth();			
					
					m_TextStartPosX = (INT32) -m_CurrentOffset;
					I3ASSERT( m_MoveOffset < m_CurrentOffset);
					break;

				case UIBOX_SCROLLING_RIGHT :
					m_MoveOffset = getWidth();

					if(m_bContinueScrollPos == true)
						m_CurrentOffset = 0.0f;
					else
						m_CurrentOffset = (REAL32) -getUIText()->getExtentWidth();

					m_TextStartPosX = (INT32) -m_CurrentOffset;
					I3ASSERT( m_MoveOffset > m_CurrentOffset);
					break;

				case UIBOX_SCROLLING_UP :
					m_MoveOffset = (REAL32) -getUIText()->getExtentHeight()+2;
					
					if(m_bContinueScrollPos == true)
						m_CurrentOffset = 0.0f;
					else
						m_CurrentOffset = getHeight();
					
					m_TextStartPosY = (INT32) -m_CurrentOffset;
					I3ASSERT( m_MoveOffset < m_CurrentOffset);
					break;

				case UIBOX_SCROLLING_DOWN :
					m_MoveOffset = getHeight();

					if(m_bContinueScrollPos == true)
						m_CurrentOffset = 0.0f;
					else
						m_CurrentOffset = (REAL32) -getUIText()->getExtentHeight()+2;

					m_TextStartPosY = (INT32) -m_CurrentOffset;
					I3ASSERT( m_MoveOffset > m_CurrentOffset);
					break;
				}
			}
			break;
		case UBSS_FLOW :
			// Scroll 기능
			switch( m_BoxProperty)
			{
			case UIBOX_SCROLLING_LEFT :
				m_CurrentOffset -= (60.0f * m_MoveSpeed) * rDeltaSeconds;
				m_TextStartPosX = (INT32) -m_CurrentOffset;
				if( m_MoveOffset > m_CurrentOffset)
				{	// 다시 설정
					m_ScrollState = UBSS_END;
				}
				break;
			case UIBOX_SCROLLING_RIGHT :
				m_CurrentOffset += (60.0f * m_MoveSpeed) * rDeltaSeconds;
				m_TextStartPosX = (INT32) -m_CurrentOffset;
				if( m_MoveOffset < m_CurrentOffset)
				{	// 다시 설정
					m_ScrollState = UBSS_END;
				}
				break;
			case UIBOX_SCROLLING_UP :
				m_CurrentOffset -= (60.0f * m_MoveSpeed) * rDeltaSeconds;
				m_TextStartPosY = (INT32) -m_CurrentOffset;
				if( m_MoveOffset > m_CurrentOffset)
				{	// 다시 설정
					m_ScrollState = UBSS_END;
				}
				break;
			case UIBOX_SCROLLING_DOWN :
				m_CurrentOffset += (60.0f * m_MoveSpeed) * rDeltaSeconds;
				m_TextStartPosY = (INT32) -m_CurrentOffset;
				if( m_MoveOffset < m_CurrentOffset)
				{	// 다시 설정
					m_ScrollState = UBSS_END;
				}
				break;
			}

			addState( I3UI_STATE_NEED_UPDATE);
			break;

		case UBSS_END:
			// 일단 자동으로 하는 것은 막습니다.
			// 다시 Scroll하려면 State를 Init하십시오..
			break;

		case UBSS_STOP:
			break;
		}
	}
}


bool i3UIEditBox::OnSetFocus()
{
	if( isReadOnly() == false)
	{
		m_pIme = i3UI::getGlobalIme();

		I3ASSERT( m_pIme != nullptr);

		if ( !m_pIme->IsImeFocusLocked() )
		{
			m_pIme->SetCompositioning(false);
			_loadEditInfo();

			m_pIme->SetFocus(true);

			if( m_pText != nullptr)
			{
				// Caret이 EditBox 밖에있는지 확인
				if( isReadyToShow())
					m_pText->BeginEdit();
			}
		}
	}

	return i3UICaptionControl::OnSetFocus();
}

bool i3UIEditBox::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{
	if( isReadOnly() == false)
	{
		if( m_pIme != nullptr)
		{
			if ( !m_pIme->IsImeFocusLocked() )
			{
				if( m_pText != nullptr)
				{
					i3::wstring text;
					m_pIme->GetInput( &text);
					m_pIme->SetCompositioning(false);// 포커스 외부에서 게임으로 돌아올 시 강제로 세팅해준다.
					SetText(text.c_str());
					m_pIme->SetCompositioning(true);
					m_pText->EndEdit();
				}
			
				m_pIme->setOwner( nullptr);
				m_pIme->SetUseCopyNPaste( true );
				m_pIme->KillFocus();
				m_pIme = nullptr;
			}
		}
		else
		{
			if( m_pText != nullptr)
			{
				//EditBox가 focus를 잃는다면 어떠한 경우에라도 caret이 나타나서는 안된다.
				m_pText->EndEdit();
			}
		}
	}

	return i3UICaptionControl::OnKillFocus( pByCtrl);
}



bool i3UIEditBox::GetTextRect( i3::pack::RECT* pRect )
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

		if(m_pUIScrollBar != nullptr && m_pUIScrollBar->isEnable())
			pRect->right -= m_fUIScrollBarWidth;

		pRect->right = max( pRect->right, 0.0f);
		pRect->bottom = max( pRect->bottom, 0.0f);
	}

	return bResult;
}


void i3UIEditBox::OnLButtonDown( UINT32 nFlag, POINT point )
{
	if( m_pIme == nullptr || m_pText == nullptr)
		return;

	INT32 nPosX = point.x - (INT32)i3Vector::GetX(&m_AccPos);
	INT32 nPosY = point.y - (INT32)i3Vector::GetY(&m_AccPos);

	INT32 newIdx = m_pText->FindNearestLetterByPos( nPosX, nPosY );

	if(newIdx != -1) {
		m_pIme->SetInsertPosition( newIdx);
		SetTextCaretPos( newIdx);
	}
	
	i3UICaptionControl::OnLButtonDown( nFlag, point);
}

void i3UIEditBox::OnMouseWheel( UINT32 nFlag, short zDelta, POINT point )
{
	if(m_pUIScrollBar != nullptr) {
		if(m_pUIScrollBar->isEnable() && m_bUseScroll) {
			INT32 nScroll = m_pUIScrollBar->getScrollPos();
			//INT32 nStep = m_nScrollStep * m_pText->getDefFontHeight();
			INT32 nStep = m_nScrollStep;//m_pUIScrollBar->getPageSize();

			nStep = zDelta < 0 ? nStep : -nStep;

			nScroll += nStep;

			if( nScroll >= 0  && nScroll < m_pUIScrollBar->getScrollLimit() )
				m_pUIScrollBar->setScrollPos( nScroll);
		}
	}

	i3UICaptionControl::OnMouseWheel( nFlag, zDelta, point);
}

void i3UIEditBox::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2 )
{
	// Scroll의 값이 변했을 때
	switch( event) 
	{
	case I3UI_EVT_SCROLL :
	{
		if( (m_pUIScrollBar != nullptr) && (m_pText != nullptr) && (m_pUIScrollBar->isEnable()) )
		{
			INT32 nPosX = m_pText->getShowX();
			INT32 nPosY = m_pUIScrollBar->getScrollPos();

			m_pText->SetShowPos(nPosX, nPosY);
			setModifyState();
		}
	}
	break;

	case I3UI_EVT_DRAGSTART :	_DragStart( param1, param2);	break;
	case I3UI_EVT_DRAGGING :	_Dragging( param1, param2);		break;
	case I3UI_EVT_DRAGEND :		_DragEnd( param1, param2);		break;
	case I3UI_EVT_MOUSEWHEEL :
		break;
	}
}


bool i3UIEditBox::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
			case EDIT_STATE_RETURN:
			case EDIT_STATE_CLEAR:
			case EDIT_STATE_CHANGE_COMPOSITION_CHAR:
			case EDIT_STATE_CHANGE_INPUT_CHAR:
			{
				SetText( m_pIme->GetText());
			}break;

			case EDIT_STATE_CHANGE_INSERT_POSITION:
				/*{
					INT32 pos = m_pIme->getInsertPos();
					SetCaretPos( pos);
				}*/break;
			case EDIT_STATE_COMPOSITION_START:
			{
				if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
					return true;

				if( i3UI::GetFocus() == this)
				{
					i3UI::setDisableUIInput( true);
				}
			}
			break;
			case EDIT_STATE_COMPOSITION_END:
			{
				if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
					return true;

				if( i3UI::GetFocus() == this)
				{
					i3UI::setDisableUIInput( false);

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

					if (m_pIme->getInputLanguage() == LANG_JAPANESE)
						SetText(m_pIme->GetText());
				}
			}
			break;
		default:
			break;
		}

		Notify( I3UI_EVT_IME, param2, 0);
	}

	switch( param2)
	{
	case EDIT_STATE_ONKEYDOWN_UP:				OnKeyDownUp();			break;
	case EDIT_STATE_ONKEYDOWN_DOWN:				OnKeyDownDown();		break;
	case EDIT_STATE_ONKEYDOWN_RIGHT:			OnKeyDownRight();		break;
	case EDIT_STATE_ONKEYDOWN_LEFT:				OnKeyDownLeft();		break;
	case EDIT_STATE_ONKEYDOWN_HOME:				OnKeyDownHome();		break;
	case EDIT_STATE_ONKEYDOWN_END:				OnKeyDownEnd();			break;	
	case EDIT_STATE_ONKEYDOWN_PAGEUP:			OnKeyDownPageup();		break;
	case EDIT_STATE_ONKEYDOWN_PAGEDOWN:			OnKeyDownPagedown();	break;
	case EDIT_STATE_RETURN:						OnKeyDownReturn();		break;
	case EDIT_STATE_ONKEYDOWN_ESCAPE:			OnKeyDownEscape();		break;
	case EDIT_STATE_CHANGE_COMPOSITION_CHAR:	OnChar( true);			break; 
	case EDIT_STATE_CHANGE_INPUT_CHAR:			OnChar( false , true);	break;
	default:	break;
	}

	if( getEventReceiver())
		getEventReceiver()->OnEvent( event, pObj, param2, code); 

	//SetBlock->SetText->SetCaretPos 순으로..
	if( m_pIme != nullptr)
	{
		SetBlock( m_pIme->GetBlockStartIdx(), m_pIme->GetBlockEndIdx());

		SetTextCaretPos( m_pIme->getInsertPos() + m_nCompPos);
	}

	return true;
}

// Up, Down, Pageup, Pagedown은 글자의 위치와 크기에 따라 다음 포지션이 결정되므로
// String Buffer만으로 판단할 수 없다. (Home, End도 AutoLineFeed 때문에 마찬가지)
// Right, Left는 String Buffer만으로도 판단할 수 있다. 
void i3UIEditBox::OnKeyDownUp()
{
	if( m_pText == nullptr)
		return;

	//m_pIme->getInsertPos();

	INT32 idx = m_pText->OnKeyDownUp();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}


void i3UIEditBox::OnKeyDownDown()
{
	if( m_pText == nullptr)
		return;

	//m_pIme->getInsertPos();

	INT32 idx = m_pText->OnKeyDownDown();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}


template<class StringType>
void		i3UIEditBox::SetTextImpl( const StringType& str)
{
	if( getPasswordEnable())
	{
		INT32 i = 0;
		INT32 nLength = i3::generic_string_size( str);

	//	WCHAR	szTemp[256] = {0,};

		i3::wstring wstrTemp;

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

//		bool bNeedCreate = false;
//		if( nLength > m_MaxTextCount) bNeedCreate = true;

		i3UICaptionControl::_SwapText( wstrTemp);			

//		size_t s = sizeof(wchar_t) * (m_MaxTextCount+1);
//		if( m_pPasswordBuffer == nullptr)
//		{
//			m_pPasswordBuffer = (wchar_t*)i3MemAlloc( s);
//			i3mem::FillZero( m_pPasswordBuffer, s );
//		}
//		else if( nLength > m_MaxTextCount)
//		{
//			m_pPasswordBuffer = (wchar_t*)i3MemReAlloc( m_pPasswordBuffer, s);
//		}

//		if( nLength > 0)
//		{
//			//PW 페스워드 저장하는코드입니다 엔터로 로그인시 예외처리로 합니다.
//			//if( pText[nLength-1] !='*' && pText[nLength-1] != '●')
//			if( i3::generic_string_size( m_pPasswordBuffer) < i3::generic_string_size( str))
//			{
//				m_pPasswordBuffer[nLength-1] = str[nLength-1];
//				m_pPasswordBuffer[nLength] = 0;
//			}
//			else if( i3::generic_string_size( m_pPasswordBuffer) > i3::generic_string_size( str))
//			{
//				m_pPasswordBuffer[i3::generic_string_size( m_pPasswordBuffer)-1] = 0;
//			}
//		}
//		else
//		{
//			i3mem::FillZero( m_pPasswordBuffer, s);
//		}
		const INT32 num_old_pw_size = (INT32)m_wstrPasswordBuffer.size();

		if ( nLength > num_old_pw_size )
		{
			std::size_t diff = nLength - num_old_pw_size;
			m_wstrPasswordBuffer.append( &str[num_old_pw_size], diff);
		}
		else
		{
			m_wstrPasswordBuffer.resize(nLength);
		}
		
		// revision 2944
		UpdateTextToIme();
	}
	else
	{
		//Comp 처리
		if( m_pIme != nullptr && m_pIme->isCompositioning())
		{
			i3::wstring szConv;	//조합 중인 문자열
			i3::wstring szTemp;	//조합이 끝난 문자열

			m_pIme->GetComp( &szConv);
			m_pIme->GetInput( &szTemp);
			
			szConv.insert( 0, L"{Col:255,100,100,255}", 21);
			szConv.insert( szConv.length(), L"{/Col}", 6);
			
			//조합 중인 문자열을 중간에 삽입
			INT32 nCaretPos = m_pIme->getInsertPos();
			szTemp.insert( nCaretPos, szConv.c_str(), szConv.length());

			i3UICaptionControl::_SwapText( szTemp);

			//Update
			m_nCompPos = m_pIme->getCompositionCursorPos();
		}
		else
		{
			m_nCompPos = 0;
			i3UICaptionControl::SetText(str);
			_UpdateShapeState();
		}

		if( getChatViewEnable()) 
		{
			_ScrollToEnd();
			// ChatView는 Focus가 없는 상태에서도 SetText 되기 때문에 수동으로 업데이트 (스크롤바 표시위해)
			_UpdateShapeState();
		}

		if ( isEditBoxStyle(I3UI_EDITBOX_STYLE_ANNOUNCE))
		{
			if ( m_pUIScrollBar && m_pText)
			{
				INT32 nExtendedHeight = m_pText->getExtentHeight()+2;
				INT32 nBoxHeight = m_pText->getViewHeight();
				if (nExtendedHeight <= nBoxHeight)
					m_pText->SetShowPos(m_TextStartPosX, m_TextStartPosY);
				else
					_ScrollToEnd();
			}

			_UpdateShapeState();
		}

		if( m_pText != nullptr)
		{
			if( isEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_LEFT) ||
				isEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_RIGHT) )
			{
				m_pText->setScrollingHorizon( true);
				m_ScrollState = i3UIEditBox::UBSS_INIT;
				_UpdateTextScroll(0.f);
				m_pText->SetShowPos(m_TextStartPosX, m_TextStartPosY);
			}
			OnChar(false);
		}
	}
		
}

void i3UIEditBox::ScrollToEnd()
{
	_ScrollToEnd();
	_UpdateShapeState();
}

void	i3UIEditBox::SetText( const wchar_t * pText)
{
	i3UIEditBox::SetTextImpl( i3::wliteral_range(pText) );
}

void	i3UIEditBox::SetText( const i3::wliteral_range& wTextRng)
{
	i3UIEditBox::SetTextImpl(wTextRng);
}

void	i3UIEditBox::SetText( const i3::rc_wstring& wstrText)
{
	i3UIEditBox::SetTextImpl(wstrText);
}
	
void	i3UIEditBox::SetText( const i3::wstring& wstrText)
{
	i3UIEditBox::SetTextImpl(i3::wliteral_range(wstrText) );
}

void	i3UIEditBox::SetText( const i3::stack_wstring& wstrText)
{
	i3UIEditBox::SetTextImpl(i3::wliteral_range(wstrText) );
}

/*
void i3UIEditBox::SetText( const WCHAR16 * pText)
{
	//I3TRACE(pText);

	if( getPasswordEnable())
	{
		INT32 i = 0;
		INT32 nLength = i3::generic_string_size( pText);

		WCHAR	szTemp[256] = {0,};
		for( i=0; i<nLength; i++)
		{
			switch( i3Language::GetCurrentCodePage())
			{
			case I3_LANG_CODE_KOREAN:		//	한국
				i3::generic_string_cat( szTemp, L"●");
				break;
			default:
				i3::generic_string_cat( szTemp, L"*");
				break;
			}
		}

		bool bNeedCreate = false;
		if( nLength > m_MaxTextCount) bNeedCreate = true;

		i3UICaptionControl::SetText( szTemp);

		size_t s = sizeof(wchar_t) * (m_MaxTextCount+1);

		if( m_pPasswordBuffer == nullptr)
		{
			m_pPasswordBuffer = (wchar_t*)i3MemAlloc( s);
			i3mem::FillZero( m_pPasswordBuffer, s );
		}
		else if( nLength > m_MaxTextCount)
		{
			m_pPasswordBuffer = (wchar_t*)i3MemReAlloc( m_pPasswordBuffer, s);
		}

		if( nLength > 0)
		{
			//PW 페스워드 저장하는코드입니다 엔터로 로그인시 예외처리로 합니다.
			//if( pText[nLength-1] !='*' && pText[nLength-1] != '●')
			if( i3::generic_string_size( m_pPasswordBuffer) < i3::generic_string_size( pText))
			{
				m_pPasswordBuffer[nLength-1] = pText[nLength-1];
				m_pPasswordBuffer[nLength] = 0;
			}
			else if( i3::generic_string_size( m_pPasswordBuffer) > i3::generic_string_size( pText))
			{
				m_pPasswordBuffer[i3::generic_string_size( m_pPasswordBuffer)-1] = 0;
			}
		}
		else
		{
			i3mem::FillZero( m_pPasswordBuffer, s);
		}
	}
	else
	{
		//Comp 처리
		if( m_pIme != nullptr && m_pIme->isCompositioning())
		{
			std::wstring szConv;	//조합 중인 문자열
			std::wstring szTemp;	//조합이 끝난 문자열

			m_pIme->GetComp( &szConv);
			m_pIme->GetInput( &szTemp);
			
			szConv.insert( 0, L"{Col:255,100,100,255}", 21);
			szConv.insert( szConv.length(), L"{/Col}", 6);
			
			//조합 중인 문자열을 중간에 삽입
			INT32 nCaretPos = m_pIme->getInsertPos();
			szTemp.insert( nCaretPos, szConv.c_str(), szConv.length());

			i3UICaptionControl::SetText( szTemp.c_str());

			//Update
			m_nCompPos = m_pIme->getCompositionCursorPos();
		}
		else
		{
			m_nCompPos = 0;
			i3UICaptionControl::SetText(pText);
		}

		if( getChatViewEnable()) 
		{
			_ScrollToEnd();
			// ChatView는 Focus가 없는 상태에서도 SetText 되기 때문에 수동으로 업데이트 (스크롤바 표시위해)
			_UpdateShapeState();
		}

		if( m_pText != nullptr)
		{
			if( isEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_LEFT) ||
				isEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_RIGHT) )
			{
				m_pText->setScrollingHorizon( true);
			}
		}
	}
}
*/


void i3UIEditBox::OnChar( bool bComp, bool keydown_event)
{
	//I3ASSERT( m_pIme != nullptr);

	if( m_pText != nullptr && m_pIme != nullptr)
	{
		if (bComp)
		{
			if (m_pIme->getInputLanguage() == LANG_CHINESE)
				m_pText->SetCaretCode(-1);
			else
			{
				if (m_pIme->getInputLanguage() == LANG_JAPANESE)
				{
					m_pText->CompBoxDisable();

					for (int i = 0; i < m_pIme->GetCompBoxCount(); i++)
					{
						if (m_pIme->getInsertPos() + m_nCompPos + i < MAXCOMPBOX)
							m_pText->SetCaretCode(m_pIme->getInsertPos() + m_nCompPos + i, i);
					}
				}
				else
				{
					m_pText->SetCaretCode(m_pIme->getInsertPos() + m_nCompPos);
				}
			}
		}
		else
		{
			m_pText->SetCaretCode( -1);
		}

		SetTextCaretPos( m_pIme->getInsertPos() + m_nCompPos);
		
		m_pText->ScrollToCaret();
		
		if( keydown_event && (m_BoxProperty == UIBOX_NORMAL) )
		{
			Notify(I3UI_EVT_KEYDOWN, 0x0D, 0);
		}
	}
}

void i3UIEditBox::OnKeyDownRight()
{
	if( m_pText == nullptr)
		return;

	INT32 idx = m_pText->OnKeyDownRight();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}

void i3UIEditBox::OnKeyDownLeft()
{
	if( m_pText == nullptr)
		return;

	INT32 idx = m_pText->OnKeyDownLeft();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}

void i3UIEditBox::OnKeyDownPageup()
{
	if( m_pText == nullptr)
		return;

	INT32 idx = m_pText->OnKeyDownPageUp();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}

void i3UIEditBox::OnKeyDownPagedown()
{
	if( m_pText == nullptr)
		return;

	INT32 idx = m_pText->OnKeyDownPageDown();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}

void i3UIEditBox::OnKeyDownHome()
{
	if( m_pText == nullptr)
		return;

	INT32 idx = m_pText->OnKeyDownHome();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}

void i3UIEditBox::OnKeyDownEnd()
{
	if( m_pText == nullptr)
		return;

	INT32 idx = m_pText->OnKeyDownEnd();
	
	m_pText->ScrollToCaret();

	m_pIme->SetInsertPosition( idx);
}

void i3UIEditBox::OnKeyDownReturn()
{
	if( !isEditBoxStyle( I3UI_EDITBOX_STYLE_MULTILINE))
	{
		// 채팅 모드 EditBox인 경우
		if( m_EditBoxStyle & I3UI_EDITBOX_STYLE_CHAT) {
			// Return Keydown Notify
			Notify(I3UI_EVT_KEYDOWN, 10, 0);

			// OnSetFocus를 해주지 않으면 Text가 누적된다
			OnSetFocus();
		}
		else 
		{	
			OnSetFocus();

			//OnKillFocus();
			//i3UI::SetFocusControl(nullptr);
			
			////Enter 키가 눌렸을 때 Focus를 날려주고, 더이상 OnKeyEvent(이런 함수) 실행을 막는다.
			//i3InputDeviceManager* pManager = 
			//	i3Framework::getCurrentFramework()->GetViewer()->GetInputDeviceManager();
			//pManager->GetKeyboard()->Reset();

			Notify(I3UI_EVT_KEYDOWN, 0x0D, 0);			// 에딧박스 킬포커싱 뒤에 이런것이 필요할수 있음..(11.07.24.수빈)
		}
	}
}

void i3UIEditBox::OnKeyDownEscape()
{
	if( !isEditBoxStyle( I3UI_EDITBOX_STYLE_MULTILINE))
	{
		OnSetFocus();

		//i3UI::SetFocusControl(nullptr);
		//
		//if( !(m_EditBoxStyle & I3UI_EDITBOX_STYLE_CHAT) )
		//{
		//	//Escape 키가 눌렸을 때 Focus를 날려주고, 더이상 OnKeyEvent(이런 함수) 실행을 막는다.
		//	i3InputDeviceManager* pManager = 
		//		i3Framework::getCurrentFramework()->GetViewer()->GetInputDeviceManager();
		//	pManager->GetKeyboard()->Reset();
		//}
	}

	Notify( I3UI_EVT_KEYDOWN, 0x1B, 0);

	
	//KillFocus전에 Text에 변화를 주고 싶다면 
	//Ime에도 Text를 복사해주어야한다. 
	//See: OnKillFocus, OnSetFocus
	/*SetText( L"");
	UpdateTextToIme();
	OnKillFocus();*/
}

void i3UIEditBox::UpdateTextToIme()
{
	if( m_pText != nullptr && m_pIme != nullptr)
	{
		if( m_pText->getText() != nullptr)
		{
			m_pIme->SetInputText( m_pText->getText(), i3::generic_string_size(m_pText->getText()), (bool)(m_MaxTextCount > 0));
		}
	}
}

/*
void i3UIEditBox::RemoveColorKeyword(i3::wstring & str)
{
	enum
	{
		KEYWORD_BEGIN_SIZE = 5,	// "{col:"
		KEYWORD_END_SIZE = 5,	// "{/col"
	};

	i3::wstring	test_str = str;
	std::transform(test_str.begin(), test_str.end(), test_str.begin(), tolower);

	while (true)
	{
		// 시작 키워드 삭제 (대소문자 구분함)
		// reivision 3175
		// : (콜론) 은 컬러 스크립트 적용시 토큰에 해당하므로 시작 키워드 검사에서 제외해야함
		size_t sIdx = test_str.find(L"{col");
		if ( sIdx == i3::string::npos)
			break;

		size_t erase_count = test_str.find('}', sIdx);
		if ( erase_count != i3::string::npos)
			erase_count = erase_count-sIdx+1;
		else
			erase_count = KEYWORD_BEGIN_SIZE;

		test_str.erase(sIdx, erase_count);
		str.erase(sIdx, erase_count);
	}

	while (true)
	{
		// 종료 키워드 삭제 (대소문자 구분함)
		size_t sIdx = test_str.find(L"{/col");
		if ( sIdx == i3::string::npos)
			break;

		size_t erase_count = test_str.find('}', sIdx);
		if ( erase_count != i3::string::npos)
			erase_count = erase_count-sIdx+1;
		else
			erase_count = KEYWORD_END_SIZE;

		test_str.erase(sIdx, erase_count);
		str.erase(sIdx, erase_count);
	}
}
*/

// i3UIEditBox::RemoveColorKeyword의 함수 리팩토링 (2014.07.01.수빈)
// 일반화 요소가 충분하다면 다시 모듈 수준으로 변경해도 좋지만 일단 여기서 멈춘다.
// 
#include "i3Base/string/algorithm/find.h"
#include "i3Base/itl/range/algorithm/find.h"

void i3UIEditBox::RemoveColorKeyword(i3::wstring & str)
{
	i3::const_wchar_range test_rng(str);		// 아래로는 마지막 스웝이전에는 str을 당분간 쓰지 않는다..
	
	i3::const_wchar_range start_col_rng = i3::ifind_first(test_rng, L"{col");
	
	i3::wstring dest;

	if ( !start_col_rng.empty() )
	{
		dest.assign(test_rng.begin(), start_col_rng.begin());	// 앞 부분 복사..	

		test_rng = i3::const_wchar_range( start_col_rng.end(), test_rng.end() );
		
		i3::const_wchar_range::iterator it = i3::range::find( test_rng, L'}');
		
		if ( it != test_rng.end() )
		{
			test_rng = i3::const_wchar_range( ++it, test_rng.end() );
		}
	}

	i3::const_wchar_range end_col_rng = i3::ifind_first(test_rng, L"{/col");

	if ( !end_col_rng.empty() )
	{
		dest.append(test_rng.begin(), end_col_rng.begin());		// 중간 부분 복사..
		
		test_rng = i3::const_wchar_range( end_col_rng.end(), test_rng.end());

		i3::const_wchar_range::iterator it = i3::range::find(test_rng, L'}');

		if ( it != test_rng.end())
		{
			test_rng = i3::const_wchar_range(++it, test_rng.end());
		}
	}

	if ( test_rng.begin() == str.begin() )		// 바뀐게 전혀 없다는 의미이다..
		return;
	
	// 바뀐게 있다면 마지막 부분 마무리 복사를 하고, 스웝하고 종료시킨다...
	dest.append( test_rng.begin(), test_rng.end() );		// 마지막 부분 복사..
	i3::swap( dest, str );

}



void i3UIEditBox::OnSize( REAL32 cx, REAL32 cy)
{
	/*if( getResizeStyle() == I3UI_CTRL_RESIZE_FIXED_PIXEL)
	{
		ResizeShapeForFixedPixel( cx, cy,
					I3UI_EDITBOX_SHAPE_TOPLEFT, I3UI_EDITBOX_SHAPE_TOP, I3UI_EDITBOX_SHAPE_TOPRIGHT,
					I3UI_EDITBOX_SHAPE_LEFT, I3UI_EDITBOX_SHAPE_CLIENTAREA, I3UI_EDITBOX_SHAPE_RIGHT,
					I3UI_EDITBOX_SHAPE_BOTTOMLEFT, I3UI_EDITBOX_SHAPE_BOTTOM, I3UI_EDITBOX_SHAPE_BOTTOMRIGHT,
					I3UI_EDITBOX_SHAPE_DISABLE_CLIENTAREA);
	}*/

	i3UICaptionControl::OnSize( cx, cy);
}

i3UIControl* i3UIEditBox::OnSelect( void)
{
	if( (getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) && (getStyle() & I3UI_STYLE_SLAVE))
	{
		i3GameNode* pParent = getParent();
		if( pParent != nullptr)
		{
			if( i3::same_of<i3UIComboBox*>(pParent) ) /* add Here */ 
			{
				return (i3UIControl*)pParent;
			}
		}
	}

	return this;
}

void i3UIEditBox::SetIMEText( const WCHAR16 * pText)
{
	//m_pText->SetText( pText, true);
	SetText( pText);
	
	if( m_pIme != nullptr)
	{
		m_pIme->SetInputText( pText, i3::generic_string_size( pText));
	}
}

IME_MODE i3UIEditBox::GetIMEMode(void)
{
	return m_Imemode;
}

void i3UIEditBox::SetIMEMode(IME_MODE mode) 
{
	//	m_bForcedIMEMode 와 m_forcedImeMode가 사라짐. komet
	//	엔진에 IME 생성시 style로 IME_STYLE_IGNORE_KEYINPUT_IME_MODE 주면 유저의 한/영 변환 버튼 입력을 무시하도록 되어 있음.
	m_Imemode = mode;
}

EDIT_STATE i3UIEditBox::getEditState(void)
{ 
	if( m_pIme != nullptr) 
		return m_pIme->getEditState();
	else
		return EDIT_STATE_NONE;
}

void i3UIEditBox::SetEllipsis(bool bFlag)
{
	m_bIsEllipsis = bFlag;

	if(bFlag)
	{
		m_EditBoxStyle |= I3UI_EDITBOX_STYLE_ENABLE_ELLIPSIS;	
	}
	else
	{
		m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_ENABLE_ELLIPSIS;
	}
}


namespace
{
	UINT32& FileIDToUINT(UINT8 (&strID)[4] ) {  return reinterpret_cast<UINT32&>(strID); }
	const UINT32& FileIDToUINT(const char (&strID)[5] ) {  return reinterpret_cast<const UINT32&>(strID); }

	const UINT32& UEB1 = FileIDToUINT("UEB1");
	const UINT32& UEB2 = FileIDToUINT("UEB2");
	const UINT32& UEB3 = FileIDToUINT("UEB3");
}



#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 UIEDITBOX
	{
		UINT8			m_ID[4] = { 'U', 'E', 'B', '2' };
		UINT32			m_EditBoxStyle = 0;
		INT32			m_nBoxProperty = 0;
		BOOL			m_bUseScroll = FALSE;
		INT32			m_nScrollStep = 0;
		INT32			m_nMaxLetter = 0;
		BOOL			m_bEnableEllipsis = FALSE;
		REAL32			m_ScrollWidth = 0.0f;
		BOOL			m_bDisableScrollButton = FALSE;

		UINT32			pad[28] = { 0 };
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3UIEditBox::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	pack::UIEDITBOX data;

	Result = i3UICaptionControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.m_EditBoxStyle			= m_EditBoxStyle;
	data.m_nBoxProperty			= m_BoxProperty;
	data.m_bUseScroll			= m_bUseScroll ? 1 : 0;
	data.m_nScrollStep			= m_nScrollStep;
	data.m_nMaxLetter			= m_nLimit;
	data.m_bEnableEllipsis		= m_bIsEllipsis ? 1 : 0;
	data.m_ScrollWidth			= m_fUIScrollBarWidth;
	data.m_bDisableScrollButton	= m_bDisableScrollButton ? 1 : 0;

	Rc = pResFile->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIEditBox::OnLoad( i3ResourceFile * pResFile )
{
i3_prof_func();

	UINT32	Rc, Result = 0;
	pack::UIEDITBOX data;

	Result = i3UICaptionControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_EditBoxStyle			= data.m_EditBoxStyle;
	m_BoxProperty			= (UI_BOX_PROPERTY)data.m_nBoxProperty;
	m_bUseScroll			= (data.m_bUseScroll)? true : false;
	m_nScrollStep			= data.m_nScrollStep;
	m_nLimit				= data.m_nMaxLetter;
	m_bDisableScrollButton	= (data.m_bDisableScrollButton)? true : false;

	SetReadOnly( isReadOnly());
	setBorderVisible( isBorderVisible());
	m_bLimitByteSize	= isLimitByteSize();
	SetEllipsis(IsEllipsisEnable());

	switch( m_BoxProperty)
	{
	case UIBOX_SCROLLING_LEFT :		addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_LEFT);	break;
	case UIBOX_SCROLLING_RIGHT :	addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_RIGHT);	break;
	case UIBOX_SCROLLING_UP :		addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_UP);		break;
	case UIBOX_SCROLLING_DOWN :		addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_DOWN);	break;
	case UIBOX_NUMERIC :			addEditBoxStyle( I3UI_EDITBOX_STYLE_NUMERIC);		break;
	}

	if( isEditBoxStyle( I3UI_EDITBOX_STYLE_CHATVIEW))
	{
		m_bLineFeed = true;
	}
	
	UINT32 myID = FileIDToUINT(data.m_ID);

	if (myID == UEB2)
	{
		m_fUIScrollBarWidth = data.m_ScrollWidth;
	}

	return Result;
}

bool i3UIEditBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->addAttr( "EditBoxStyle",	m_EditBoxStyle);
	pXML->addAttr( "MaxLetter", m_nLimit);
	pXML->addAttr( "LimitByteSize", m_bLimitByteSize);
	pXML->addAttr( "BoxProperty", (INT32)m_BoxProperty);
	pXML->addAttr( "UseScroll", m_bUseScroll);
	pXML->addAttr( "ScrollStep", m_nScrollStep);
	pXML->addAttr( "EnableEllipsis", m_bIsEllipsis);
	pXML->addAttr( "ScrollWidth", m_fUIScrollBarWidth);
	pXML->addAttr( "ScrollPrevNextButton", m_bDisableScrollButton);
	bResult = true;

	return bResult;
}

bool i3UIEditBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;
	
	pXML->getAttr( "EditBoxStyle", &m_EditBoxStyle);
	pXML->getAttr( "MaxLetter", &m_nLimit);

	// 어이 없는 버그들 잡겠음...(2015.04.07.수빈) ...  옛날것부터 최근것까지 멤버변수 3개의
	// bool값 사용 잘못한 문제...커버리티는 이런것을 못잡아준다..

	INT32 boolRes = 0;

	pXML->getAttr( "LimitByteSize", &boolRes);

	m_bLimitByteSize = (boolRes != 0);
	pXML->getAttr( "BoxProperty", (INT32*) &m_BoxProperty);

	pXML->getAttr( "UseScroll", &boolRes);
	m_bUseScroll = (boolRes != 0);

	pXML->getAttr( "ScrollStep", &m_nScrollStep);


	pXML->getAttr( "EnableEllipsis",  &boolRes);
	
	m_bIsEllipsis = (boolRes != 0);

	pXML->getAttr( "ScrollPrevNextButton", &boolRes);
		
	m_bDisableScrollButton = (boolRes != 0);


	SetReadOnly( isReadOnly());
	setBorderVisible( isBorderVisible());

#if defined( I3_DEBUG)
	m_bMultiLine = isEditBoxStyle( I3UI_EDITBOX_STYLE_MULTILINE);
	SetEllipsis(IsEllipsisEnable());
#endif

	REAL32 initScrollWidth = m_fUIScrollBarWidth;
	if (pXML->getAttr("ScrollWidth", &m_fUIScrollBarWidth) == nullptr)
	{
		m_fUIScrollBarWidth = initScrollWidth;
	}

	bResult = true;

	return bResult;
}

void i3UIEditBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	if( !i3::same_of< i3UIEditBox* >(pDest))
		return ;

	i3UIEditBox * pEditBox		= (i3UIEditBox*)pDest;	

	pEditBox->m_EditBoxStyle	= m_EditBoxStyle;
	pEditBox->m_nLimit			= m_nLimit;
	pEditBox->m_bLimitByteSize	= m_bLimitByteSize;

#if defined( I3_DEBUG)
	pEditBox->m_bMultiLine = m_bMultiLine;
	pEditBox->m_bReadOnly = m_bReadOnly;
#endif
	pEditBox->m_bIsEllipsis = m_bIsEllipsis;
	pEditBox->m_fUIScrollBarWidth = m_fUIScrollBarWidth;
	pEditBox->m_bDisableScrollButton = m_bDisableScrollButton;
}

void i3UIEditBox::_ScrollToEnd( void )
{
	if(m_pText == nullptr)
		return;

	INT32 nShowY = m_pText->getExtentHeight() - m_pText->getViewHeight()+2;
	m_pText->SetShowPos(0, nShowY);
}

void i3UIEditBox::OnBindTemplate( void )
{
	i3UICaptionControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	if( m_pUIScrollBar == nullptr)
	{
		m_pUIScrollBar = i3UIScrollBar::new_object();
		AddControl( m_pUIScrollBar);
		m_pUIScrollBar->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pUIScrollBar->addStyle( I3UI_STYLE_SLAVE);
		m_pUIScrollBar->SetVisible(false);
		m_pUIScrollBar->setDisablePrevNextButton(m_bDisableScrollButton);
	}

	m_pUIScrollBar->SetTemplate( m_pTemplate->getSubTemplate( I3UI_EDITBOX_SUBTEMPLATE_VSCROLL));
}

#if defined( I3_DEBUG)
void i3UIEditBox::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIEditBox");
	AddMFCProperty_Bool( List, &m_bMultiLine, "MultiLine");
	
	m_bReadOnly = (m_EditBoxStyle & I3UI_EDITBOX_STYLE_READONLY) ? true : false;
	AddMFCProperty_Bool( List, &m_bReadOnly, "Read Only");

	AddMFCProperty_Int32( List, &m_nLimit, "Max Letters");
	AddMFCProperty_Bool( List, &m_bLimitByteSize, "Limit ByteSize");

	AddMFCProperty_Combo( List, s_slMode, (INT32*)&m_BoxProperty, "Box Style");

	AddMFCProperty_Bool( List, &m_bUseScroll, "Use Scroll");
	AddMFCProperty_Int32( List, &m_nScrollStep, "Scroll Step");
	AddMFCProperty_Real32(List, &m_fUIScrollBarWidth, "Scroll Width");
	AddMFCProperty_Bool(List, &m_bIsEllipsis, "Enable Ellipsis");
	AddMFCProperty_Bool(List, &m_bDisableScrollButton, "Scroll Prev_Next Button");

	AddMFCProperty_EndGroup( List);
}

void i3UIEditBox::OnTDKPropertyChanged( const char* szName)
{
	if( i3::generic_is_equal( szName, "MultiLine") )
	{
		//if( m_pIMEHandler != nullptr)
			//m_pIMEHandler->EnableMultiLine( m_bMultiLine);
		if( m_bMultiLine)
			m_EditBoxStyle |= I3UI_EDITBOX_STYLE_MULTILINE;
		else
			m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_MULTILINE;
	}
	else if( i3::generic_is_equal( szName, "Read Only"))
	{
		if( m_bReadOnly)
			m_EditBoxStyle |= I3UI_EDITBOX_STYLE_READONLY;
		else
			m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_READONLY;
	}
	else if( i3::generic_is_equal( szName, "Box Style") )
	{
		removeEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL);
		removeEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_LEFT);
		removeEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_RIGHT);
		removeEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_UP);
		removeEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_DOWN);

		removeEditBoxStyle( I3UI_EDITBOX_STYLE_NUMERIC);

		switch( m_BoxProperty)
		{
		case UIBOX_NORMAL :
			this->setPasswordEnable(false);
			this->setChatEnable(false);
			this->setChatViewEnable(false);
			break;

		case UIBOX_PASSWORD :
			this->setPasswordEnable(true);
			this->setChatEnable(false);
			this->setChatViewEnable(false);
			break;

		case UIBOX_CHATINPUT :
			this->setPasswordEnable(false);
			this->setChatEnable(true);
			this->setChatViewEnable(false);
			break;

		case UIBOX_CHATVIEW :
			this->setPasswordEnable(false);
			this->setChatEnable(false);
			this->setChatViewEnable(true);
			break;

		case UIBOX_SCROLLING_LEFT :		addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_LEFT);	break;
		case UIBOX_SCROLLING_RIGHT :	addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_RIGHT);	break;
		case UIBOX_SCROLLING_UP :		addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_UP);		break;
		case UIBOX_SCROLLING_DOWN :		addEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL_DOWN);	break;

		case UIBOX_NUMERIC :			addEditBoxStyle( I3UI_EDITBOX_STYLE_NUMERIC);		break;
		}

	}
	else if( i3::generic_is_equal( szName, "Use Scroll") ) {
		this->_UpdateShapeState();
	}
	else if( i3::generic_is_equal( szName, "Limit ByteSize") )
	{
		if( m_bLimitByteSize )
			addEditBoxStyle( I3UI_EDITBOX_STYLE_LIMIT_BYTESIZE);
		else
			removeEditBoxStyle( I3UI_EDITBOX_STYLE_LIMIT_BYTESIZE);
	}
	else if ( i3::generic_is_equal(szName, "Scroll Width"))
	{
		_UpdateScrollBarValue();
	}
	// revision 3518
	else if( i3::generic_is_equal( szName, "Enable Ellipsis"))
	{
		if(m_bIsEllipsis)
		{
			addEditBoxStyle(I3UI_EDITBOX_STYLE_ENABLE_ELLIPSIS);
		}
		else
		{
			removeEditBoxStyle(I3UI_EDITBOX_STYLE_ENABLE_ELLIPSIS);
		}
	}
	else if( i3::generic_is_equal( szName, "Scroll Prev_Next Button"))
	{
		if(m_bDisableScrollButton)
		{
			addEditBoxStyle(I3UI_EDITBOX_STYLE_SCROLL_BUTTON_DISABLE);
		}
		else
		{
			removeEditBoxStyle(I3UI_EDITBOX_STYLE_SCROLL_BUTTON_DISABLE);
		}
	}
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG

////////////////////////////////////////////////////////////////////////////////////

extern "C" int i3UIED_setReadOnly(lua_State *L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nEnable = i3Lua::GetIntegerArg( L, 2, 0);
		bool bReadOnly = ( nEnable > 0)? false : true;

		pCtrl->SetReadOnly( bReadOnly);
	}

	return 0;
}

extern "C" int i3UIED_setAlphaNumeric(lua_State *L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nEnable = i3Lua::GetIntegerArg( L, 2, 0);
		bool bAlphaNumeric = ( nEnable > 0)? true : false;

		pCtrl->setAlphaNumericOnly( bAlphaNumeric);
	}
	return 0;
}

extern "C" int i3UIED_setPasswordmode(lua_State *L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nEnable = i3Lua::GetIntegerArg( L, 2, 0);
		bool bPasswordEnable = ( nEnable > 0)? true : false;

		pCtrl->setPasswordEnable( bPasswordEnable);
	}

	return 0;
}


extern "C" int i3UIED_getPassword(lua_State * L)
{
	i3UIEditBox* pCtrl = (i3UIEditBox*)i3Lua::GetUserDataArg(L, 1);
	if(pCtrl != nullptr)
	{
		i3Lua::PushString(L, pCtrl->getPassword());

		return 1;
	}
	return 0;
}

extern "C" int i3UIED_setChat(lua_State * L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*)i3Lua::GetUserDataArg(L, 1);
	if(pCtrl != nullptr)
	{
		INT32 nEnable = i3Lua::GetIntegerArg(L, 2, 0);
		bool bChatEnable = ( nEnable > 0)? true : false;

		pCtrl->setChatEnable( bChatEnable);
	}

	return 0;
}

extern "C" int i3UIED_setChatView(lua_State * L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*)i3Lua::GetUserDataArg(L, 1);
	if(pCtrl != nullptr)
	{
		INT32 nEnable = i3Lua::GetIntegerArg(L, 2, 0);
		bool bChatViewEnable = ( nEnable > 0)? true : false;

		pCtrl->setChatViewEnable( bChatViewEnable);
	}

	return 0;
}

extern "C" int i3UIED_SetIMEText( LuaState * L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*) i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		char szStr[2048] = {0,};
	
		if( i3Lua::GetStringArg( L, 2, szStr, sizeof( szStr)) != nullptr)
		{
			INT32 len = i3::generic_string_size( szStr);

			if( len > 0)
			{
				WCHAR16 wStr[2048] = {0,};

				//	유니코드 문자열의 길이를 받아온다.
				INT32 length = i3String::MultiByteToWideChar( CP_UTF8, 0, szStr, len, nullptr, 0);

				//	유니코드 버퍼로 문자열을 받아온다.
				i3String::MultiByteToWideChar( CP_UTF8, 0, szStr, len, wStr, length);

				pCtrl->SetIMEText( wStr);
			}
			else
				pCtrl->SetIMEText( L"");
		}
		else
			pCtrl->SetIMEText( L"");
	}

	return 0;
}

extern "C" int i3UIED_getIMETextLen( LuaState* L)
{
	i3UIEditBox * pCtrl = (i3UIEditBox*) i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Ime*	pIme	= pCtrl->GetIME();
		if( pIme != nullptr )
		{
			const WCHAR*	pstr = pIme->GetText();
			INT32			nLenW = i3::generic_string_size( pstr);

			if( pCtrl->getLimitByteSize() )
			{
				INT32		nLenM = i3String::WideCharToMultiByte( CP_ACP, 0, pstr, nLenW, 0, 0, 0, 0 );

				i3Lua::PushInteger( L, nLenM );
			}
			else
			{
				i3Lua::PushInteger( L, nLenW );
			}
		}
		else
		{
			i3Lua::PushInteger( L, 0 );
		}
	}

	return 1;
}


////////////////////////////////////////////////////////////////////////////////////
LuaFuncReg i3UIEditGlue[] = 
{
	{"uiEdit_setReadOnly",				i3UIED_setReadOnly},
	{"uiEdit_setAlphaNumericOnly",		i3UIED_setAlphaNumeric},
	{"uiEdit_setPasswordmodeEnable",	i3UIED_setPasswordmode},
	{"uiEdit_getPassword",				i3UIED_getPassword},
	{"uiEdit_setChatEnable",			i3UIED_setChat},
	{"uiEdit_setChatViewEnable",		i3UIED_setChatView},
	{"uiEdit_setImeText",				i3UIED_SetIMEText},

	{"uiEdit_getImeTextLen",			i3UIED_getIMETextLen},

	{nullptr, nullptr}
};

void i3UIEditBox::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIEditGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIEditGlue[i].name, i3UIEditGlue[i].func);
	}
}