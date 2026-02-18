#include "pch.h"
#include "UIMessageBox.h"

#include "UIMainFrame.h"

#include "../WebControl.h"
#include "../GameGUI.h"
#include "../MessageBoxManager.h"

#include "UIPopupNickNameSetting.h"
#include "GMAuthorityContext.h"

#include "i3Base/RemoveColorString.h"


UIMessageBox * GetThis( LuaState * L)
{
	UIMessageBox * pMsgBox = g_pFramework->GetMessageBoxManager()->FindMsgBox( L);

	return pMsgBox;
}


extern "C" {
	int UIMessageBox_ClickUserButton( LuaState * L)
	{
		UIMessageBox * pMsgBox = GetThis( L);

		if( pMsgBox)
			pMsgBox->ClickUserButton();

		return 0;
	}
}

I3_CLASS_INSTANCE( UIMessageBox);//, UIPopupBase);

UIMessageBox::UIMessageBox()
{
	INT32 i;

	m_pManager = nullptr;
	m_MBStyle = UIMBS_START;
	m_bIdentified = false;

	i3mem::FillZero( &m_WebScreenSize, sizeof(m_WebScreenSize));

	m_bCreatedWebBilling = false;
	m_bShowWebBilling = false;
	m_hExplorerhWnd = nullptr;
	m_CheckCRC = 0;

	m_pCallBackFunc = nullptr;
	m_pThis = nullptr;
	m_nParam = 0;

	m_bReserved = false;

	m_nTextWidth = 0;
	m_nTextHeight = 0;

	m_nX = 0;
	m_nY = 0;

	m_pMsg = nullptr;

	i3mem::FillZero( m_pButton, sizeof( m_pButton));

	for( i = 0; i < UIBT_MAX; i++)
		m_ButtonStyle[i] = UIBTN_NONE;

	m_bScaleAnimation = false;
	m_bClosing = false;
	m_bInitilized = false;

	m_AlignX = 0;
	m_AlignY = 0;

	m_BTN_Style = UIBTN_NONE;
	m_pBGFrame = nullptr;
	cbExitFunc = nullptr;
}

UIMessageBox::~UIMessageBox()
{
}

/*virtual*/ void UIMessageBox::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pMsg = (i3UIStaticText*) pScene->FindChildByName( "msg");
	I3ASSERT( m_pMsg);

	m_pButton[ UIBT_OK] = (i3UIButtonComposed3*) pScene->FindChildByName( "btnOK");
	I3ASSERT( m_pButton[ UIBT_OK]);

	m_pButton[ UIBT_CANCEL] = (i3UIButtonComposed3*) pScene->FindChildByName( "btnCancel");
	I3ASSERT( m_pButton[ UIBT_CANCEL]);

	m_pButton[ UIBT_USER] = (i3UIButtonComposed3*) pScene->FindChildByName( "btnUser");
	I3ASSERT( m_pButton[ UIBT_USER]);

	// 인게임에서는 BG를 OFF
	if( g_pFramework->IsBattleStage() && ( m_pBGFrame != nullptr))
	{
		m_pBGFrame->EnableCtrl( false);
	}

	// 지난 팝업에 쌓여있던 포커스를 전부 죽여버립니다
	g_pFramework->ResetFocus();
}

void UIMessageBox::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	m_pMsg = nullptr;
	i3mem::FillZero( m_pButton, sizeof( m_pButton));
	m_pBGFrame = nullptr;
}

void UIMessageBox::_CreateWeb( const i3::rc_wstring& URL, SIZE32 Size)
{
	m_WebScreenSize = Size;

	INT32	Left	= static_cast<INT32>(g_pViewer->GetViewWidth() * 0.5f - Size.w * 0.5f);
	INT32	Top		= static_cast<INT32>(g_pViewer->GetViewHeight() * 0.5f - Size.h * 0.5f - 10.f);

	if( m_bCreatedWebBilling)
	{
		::ShowWindow( m_hExplorerhWnd, SW_SHOW);
		::MoveWindow( m_hExplorerhWnd, Left, Top,Size.w, Size.h, TRUE);
	}
	else
	{
		InitWebCtrl();

		m_hExplorerhWnd = ::CreateWebControl( URL.c_str(), WS_CLIPCHILDREN | WS_CHILD,
			Left, Top, Size.w, Size.h, g_pViewer->GetWindowHandle(), NULL);

		m_bCreatedWebBilling = true;

		::ShowWindow( m_hExplorerhWnd, SW_SHOW);
	}

	m_bShowWebBilling = true;
}

void UIMessageBox::_SetButtonState( void)
{
	INT32 i32ButtonCnt = 0;
	bool	bUseTextBox = false;
	switch( m_MBStyle)
	{
	case UIMBS_OK :
		m_ButtonStyle[ UIBT_USER] = UIBTN_NONE;
		m_ButtonStyle[ UIBT_OK] = UIBTN_OK;
		m_ButtonStyle[ UIBT_CANCEL] = UIBTN_NONE;
		i32ButtonCnt = 1;
		break;
	case UIMBS_OKCANCEL :
		m_ButtonStyle[ UIBT_USER] = UIBTN_NONE;
		m_ButtonStyle[ UIBT_OK] = UIBTN_OK;
		m_ButtonStyle[ UIBT_CANCEL] = UIBTN_CANCEL;
		i32ButtonCnt = 2;
		break;
	case UIMBS_USER_OKCANCEL :
		m_ButtonStyle[ UIBT_USER] = UIBTN_NOTUSE;
		m_ButtonStyle[ UIBT_OK] = UIBTN_CANCEL;
		m_ButtonStyle[ UIBT_CANCEL] = UIBTN_ITEMUSE;
		i32ButtonCnt = 3;
		break;
	case UIMBS_WAIT :
		m_ButtonStyle[ UIBT_USER] = UIBTN_NONE;
		m_ButtonStyle[ UIBT_OK] = UIBTN_NONE;
		m_ButtonStyle[ UIBT_CANCEL] = UIBTN_NONE;
		i32ButtonCnt = 0;
		break;
	case UIMBS_WEB :
		m_ButtonStyle[ UIBT_USER] = UIBTN_NONE;
		m_ButtonStyle[ UIBT_OK] = UIBTN_OK;
		m_ButtonStyle[ UIBT_CANCEL] = UIBTN_NONE;
		i32ButtonCnt = 1;
		break;
	case UIMBS_BLOCK_NOTICE:
		m_ButtonStyle[UIBT_USER] = UIBTN_NONE;
		m_ButtonStyle[UIBT_OK] = UIBTN_OK;
		m_ButtonStyle[UIBT_CANCEL] = UIBTN_NONE;
		i32ButtonCnt = 1;
		bUseTextBox = true;
		break;
	default : I3PRINTLOG(I3LOG_FATAL, "cant support!!");	break;
	}

	// 스타일에 따른 버튼 설정
	LuaState * L = _CallLuaFunction( "SetButtonState");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_ButtonStyle[ UIBT_USER]);
		i3Lua::PushInteger( L, m_ButtonStyle[ UIBT_OK]);
		i3Lua::PushInteger( L, m_ButtonStyle[ UIBT_CANCEL]);
		i3Lua::PushInteger( L, i32ButtonCnt);
		i3Lua::PushBoolean( L, bUseTextBox);
		_EndLuaFunction( L, 5);
	}
}

void UIMessageBox::_SetText( const i3::rc_wstring& wstrMessage, const i3::rc_wstring& wstrCaption, 
							INT32 nAlignX, INT32 nAlignY)
{
	// revision 64134 메세지 크래쉬 수정
	if( m_pMsg == 0 ) return; //m_pMsg 생성전에 호출되는 경우가 있음.

	//	동일 메시지 체크용 CRC
	INT32 Length = i3::generic_string_size( wstrMessage);
	m_CheckCRC = CRC32(0xFFFFFFFF, (UINT8*) &wstrMessage[0], Length * sizeof(wchar_t) );

	I3ASSERT( m_pMsg != nullptr);
	I3ASSERT( m_pMsg->getUIText());
	
	m_nTextWidth = m_pMsg->getUIText()->getExtentWidth();
	m_nTextHeight = m_pMsg->getUIText()->getExtentHeight();

	const wchar_t * wstrLineCheck = wstrMessage.c_str();

	i3::wstring wstrColorRemovedMessage;

	if ( Length > 0 )
	{
		::RemoveColorStringIfNeeded(wstrMessage, wstrColorRemovedMessage);
		
		if (!wstrColorRemovedMessage.empty())
			wstrLineCheck = wstrColorRemovedMessage.c_str();
	}

	//	멀티라인일 경우 처리
	{
		static wchar_t temp[4096] = L"";
		static wchar_t szMost[2048] = L"";

		INT32 line = 1, idx = 0;
		INT32 most = 0;

		const wchar_t * pszChar = wstrLineCheck;
		i3mem::FillZero( temp, sizeof( temp));
	
		while(*pszChar)
		{
			if( idx < 4096)
			{
				//	캐리지 리턴일 경우 한 문장으로 인식한다.
				if( *pszChar == L'\n')
				{
					temp[idx] = 0;

					//	가장 긴 문장을 찾는다.
					if( most < idx)
					{
						most = idx;
					
						i3::string_ncopy_nullpad( szMost, temp, sizeof(szMost)/sizeof(wchar_t));
					}

					idx = 0;
					line++;
				}
				else
				{
					temp[idx] = *pszChar;

					idx++;
				}				
			}

			pszChar++;
		}

		//	가장 긴 문장을 찾는다.
		if( most < idx)
		{
			most = idx;

			i3::string_ncopy_nullpad( szMost, temp, sizeof(szMost)/sizeof(wchar_t));
		}

		if(line > 1)
		{
			//	가장 긴 문자을 기준으로 넓이를 계산한다.
			m_pMsg->SetText( szMost);
			m_nTextWidth = m_pMsg->getUIText()->getExtentWidth();

			//	멀티라인
			m_nTextHeight *= line;
		}
	}

	LuaState * L = _CallLuaFunction( "SetString");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstrMessage.c_str(), wstrMessage.size());
		i3Lua::PushInteger( L, nAlignX);
		i3Lua::PushInteger( L, nAlignY);
		i3Lua::PushStringUTF16To8( L, wstrCaption.c_str(), wstrCaption.size());
		
		_EndLuaFunction( L, 4);
	}

	// I3MEM_SAFE_FREE(strColorRemovedMessage);
}

void UIMessageBox::_SetBlockText( const i3::rc_wstring& wstrMessage, const i3::rc_wstring& wstrCaption)
{
	i3::rc_wstring wstrBlocKReason;
	wstrBlocKReason = GMAuthorityContext::i()->GetBlockComment();

	if (wstrBlocKReason.size() <= 0)
	{
		INT32 _PenaltyAmount = GMAuthorityContext::i()->GetPenaltyAmount();

		if (_PenaltyAmount == PERMANENT_BLOCK)
			wstrBlocKReason = GAME_RCSTRING("STR_GM_POPUP_BANMESSAGE");
		else if(_PenaltyAmount <= 0)
			wstrBlocKReason = L"";
		else
			i3::sprintf(wstrBlocKReason, GAME_RCSTRING("STR_GM_POPUP_BANDAY"), _PenaltyAmount);
	}

	LuaState * L = _CallLuaFunction("SetBlockString");
	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrCaption);
		i3Lua::PushStringUTF16To8(L, wstrMessage);
		i3Lua::PushStringUTF16To8(L, wstrBlocKReason);

		_EndLuaFunction(L, 3);
	}
}

void UIMessageBox::_RecalcShape( void)
{
	// 알림 팝업의 재계산이 필요 없을 것으로 판단되어 테스트 기간동안 주석처리함 - sanghun.han (2014. 12. 17)
	//if( m_pPopupFrame == nullptr)
	//	return;

	//i3UIManager * pUIMng = i3UI::getManager();
	//if( pUIMng == nullptr)
	//	return;

	//REAL32 centerX = (REAL32) pUIMng->getWidth() * 0.5f;
	//REAL32 centerY = (REAL32) pUIMng->getHeight() * 0.5f;

	//REAL32 posX = (REAL32) m_nX + centerX - (getX( &m_vOriginalSize) * 0.5f);
	//REAL32 posY = (REAL32) m_nY + centerY - (getY( &m_vOriginalSize) * 0.5f);

	//m_pPopupFrame->setPos( posX, posY);
	//m_slide.SetOriginalPos( posX, posY);
}

void UIMessageBox::_ConfirmParam( UIBTN_STYLE state)
{
	m_BTN_Style = state;
	switch( state)
	{
	case UIBTN_OK :			m_nParam = MBR_OK;		break;
	case UIBTN_CANCEL :		m_nParam = MBR_CANCEL;	break;
	case UIBTN_ITEMUSE :	m_nParam = MBR_USER;	break;
	default :				m_nParam = MBR_OK;		break;
	}
}

/*virtual*/ void UIMessageBox::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PointBlankRe_PopUp_Default.i3UIs");
}

bool UIMessageBox::Create( i3GameNode * pParent, CMessageBoxManager* pManager, const MsgBoxOpenParam & Param)
{
	OnCreate( pParent);

	UINT32 style = Param.Style;

	m_pCallBackFunc = Param.CB;
	m_pThis = Param.pThis;
	m_wstrMsg = Param.Msg;
	m_wstrCaption = Param.Caption;
	m_AlignX = Param.AlignX;
	m_AlignY = Param.AlignY;

	m_pManager = pManager;

	if (TESTBIT(style, MBF_WAIT))
	{
		m_MBStyle = UIMBS_WAIT;
	}
	else if(TESTBIT(style, MBF_OKCANCEL))
	{
		m_MBStyle = UIMBS_OKCANCEL;
	}
	else if(TESTBIT(style, MBF_USER_OKCANCEL))
	{
		m_MBStyle = UIMBS_USER_OKCANCEL;
	}
	else if(style == MBF_WEB)
	{
		m_MBStyle = UIMBS_WEB;

		_CreateWeb( Param.WebURL.c_str(), Param.WebScreenSize);
	}
	else if (style == MBF_SYSTEM_BLOCKNOTICE)
	{
		m_MBStyle = UIMBS_BLOCK_NOTICE;
	}
	else
	{
		m_MBStyle = UIMBS_OK;
	}

	if( TESTBIT( style, MBF_ERROR) )
	{
		PlaySound( UISND_ERROR);
	}

	m_CheckCRC = 0;

	return true;
}

void UIMessageBox::Hide( bool bForced)
{
	if( (m_bClosing == false && m_bReserved == false) || bForced)
	{
		m_bReserved = true;
		OnExitStart();
	}
}

void UIMessageBox::Close(void)
{
	m_bReserved = false;

	m_bClosing = true;

	OnExitStart();
}

bool	UIMessageBox::OnKey_Enter()
{
	if ( m_MBStyle == UIMBS_WAIT )
		return true;

	_ConfirmParam( m_ButtonStyle[ UIBT_OK]);

	UIMessageBox::Close();

	return true;
}

bool	UIMessageBox::OnKey_Escape()
{
	if ( m_MBStyle == UIMBS_WAIT )
		return true;

	_ConfirmParam( m_ButtonStyle[ UIBT_CANCEL]);
	UIMessageBox::Close();
	return true;
}

/*virtual*/ bool UIMessageBox::OnPlayClickSound(i3UIControl * pCtrl)
{
	if(pCtrl == m_pButton[UIBT_OK] )
	{
		// 확인 버튼
		return PlaySound(UISND_ITEM_CLICK_OK);
	}
	else if(pCtrl == m_pButton[UIBT_CANCEL])
	{
		// 취소 버튼
		return PlaySound(UISND_ITEM_CLICK_CANCEL);
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}

void	UIMessageBox::CloseDirect(void)
{
	m_bReserved = false;
	m_bClosing = true;
	m_bIdentified = true;

	// Notify없이 그냥 닫는다... (cancel Notify가 필요할수도 있었음...현재는 Notify를 두면 버그가 생기는 구현들이 있습니다...)
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame != nullptr)
	{
		pFrame->FocusPopup(this, false, __FILE__, __LINE__);
	}

	UIMessageBox::SetEnable(false);	
}

/*virtual*/ bool UIMessageBox::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	
	m_bClosing = false;

	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame != nullptr)
	{
		pFrame->FocusPopup(this, true, __FILE__, __LINE__);	

		if(pFrame->IsCreatingNick() )
		{
			UIPopupNickNameSetting * pPopup = (UIPopupNickNameSetting*) pFrame->GetPopup(UPW_CREATENICKNAME);
			pPopup->SetEditFocus(false);
		}

		UIPopupBase * pPopup = pFrame->GetPopup(UPW_WEB_BOX);
		if(pPopup != nullptr && pPopup->isEnable() == true)
			pPopup->OnExitStart();
	}

	if (m_MBStyle == UIMBS_BLOCK_NOTICE)
		_SetBlockText( m_wstrMsg, m_wstrCaption);
	else
		_SetText( m_wstrMsg, m_wstrCaption, m_AlignX, m_AlignY);

	_SetButtonState();

	// Shape 크기 재계산
	if( m_bInitilized == false)
		_RecalcShape();

	i3UI::setDisableUIInput(false);

	m_bInitilized = true;

	return true;
}
/*virtual*/ void UIMessageBox::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	// Key : User Guide System
	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

/*virtual*/ bool UIMessageBox::OnExitStart( void)
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	if( m_bCreatedWebBilling && m_bShowWebBilling) 
	{
		// 캐쉬 정보 얻는 이벤트를 	
		GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);

		::ShowWindow( m_hExplorerhWnd,SW_HIDE);
		m_bShowWebBilling = false;
	}

	if(m_BTN_Style == UIBTN_CANCEL)
		OnExitComplete();

	return true;
}

/*virtual*/ void UIMessageBox::OnExitEnd( void)
{
	UIPopupBase::OnExitEnd();

//	Notify();

	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame != nullptr)
	{
		pFrame->FocusPopup(this, false, __FILE__, __LINE__);

		if(pFrame->IsCreatingNick() && m_bClosing)
		{
			UIPopupNickNameSetting * pPopup = (UIPopupNickNameSetting*) pFrame->GetPopup(UPW_CREATENICKNAME);
			pPopup->SetEditFocus(true);
		}
	}

	if( m_bReserved == false)
	{
		m_bIdentified = true;
		Notify();
	}

}

/*virtual*/ bool UIMessageBox::OnOKButton( void)
{
	m_bReserved = false;
	m_bClosing = true;

	if( UIPopupBase::OnOKButton() == false)
		return false;

	_ConfirmParam( m_ButtonStyle[ UIBT_OK]);

	return true;
}

/*virtual*/ bool UIMessageBox::OnCloseButton( void)
{
	m_bReserved = false;
	m_bClosing = true;

	if( UIPopupBase::OnCloseButton() == false)
		return false;

	_ConfirmParam( m_ButtonStyle[ UIBT_CANCEL]);

	return true;
}

void UIMessageBox::ClickUserButton( void)
{
	m_bReserved = false;
	m_bClosing = true;

	_ConfirmParam( m_ButtonStyle[ UIBT_CANCEL]);

	OnExitStart();
}

void UIMessageBox::Notify( void)
{
	if( m_pThis == nullptr)
		return;
	
	m_pCallBackFunc.OnCallback( m_pThis, m_nParam);
}

void UIMessageBox::SetPosition(INT32 x,INT32 y)
{
	// 현재 위치와 같다면 처리하지 않는다
	if (x == m_nX && y == m_nY)
	{
		return;
	}
	
	// 팝업 프레임이 없다면 처리하지 않음
	if( m_pPopupFrame == nullptr)
	{
		return;
	}

	// 값 설정
	m_nX = x;
	m_nY = y;

	_RecalcShape();
}

void UIMessageBox::ChangeButton( const char * pUser, const char * pOK, const char * pCancel)
{
	if( pUser)	m_pButton[ UIBT_USER]->SetText( pUser);
	if( pOK)	m_pButton[ UIBT_OK]->SetText( pOK);
	if( pCancel)m_pButton[ UIBT_CANCEL]->SetText( pCancel);
}

void UIMessageBox::ChangeMessage( const i3::rc_wstring& wstrMessage, const i3::rc_wstring* pwstrCaption)
{
	if (pwstrCaption == nullptr)
		_SetText( wstrMessage, i3::rc_wstring());
	else
		_SetText( wstrMessage, *pwstrCaption);
	// Shape 크기 재계산
	_RecalcShape();
}

void UIMessageBox::ChangeMessage()
{
	m_MBStyle = UIMBS_OKCANCEL;
	_SetText(m_wstrMsg, m_wstrCaption);
	_SetButtonState();
	_RecalcShape();
}