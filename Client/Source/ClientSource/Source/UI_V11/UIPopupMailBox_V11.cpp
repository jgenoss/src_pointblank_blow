#include "pch.h"
#include "UI/UIMainFrame.h"
#include "UIPopupReceiveMail_V11.h"
#include "MailBoxContext.h"
#include "UIPopupMailBox_V11.h"
#include "UI/UIUtil.h"
#include "UITopMenu_V11.h"
#include  "../UserInfoContext.h"

#include "GameGUI.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
I3_CLASS_INSTANCE( UIPopupMailBox_V11);

bool UIPopupMailBox_V11::m_bWarningNoteCount = false;
static UIPopupMailBox_V11* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupMailBox_V11*) pFrame->GetPopup( UPW_MAILBOX);
}

extern "C" {
	int UIPopupMailBox_V11_ChangeTab( LuaState * L)
	{
		INT32	nIndex	= i3Lua::GetIntegerArg(L, 1, -1);
		GetThis()->ChangeTab( (MessageTab)nIndex );
		return 0;
	}

	int UIPopupMailBox_V11_SelectItem( LuaState * L)
	{
		GetThis()->OnSelItems(i3Lua::GetIntegerArg(L, 1, -1)); 
		return 0;
	}

	int UIPopupMailBox_V11_GetNote( LuaState * L)
	{
		memset(&GetThis()->m_szNote, 0x00, sizeof(GetThis()->m_szNote));
		i3Lua::GetStringArg(L, 1, GetThis()->m_szNote, MAX_STRING_COUNT);
		return 0;
	}

	int UIPopupMailBox_V11_MailDelete( LuaState * L)
	{
		INT32 row;
		INT32 column;
		row = i3Lua::GetIntegerArg(L, 1, -1);
		column = i3Lua::GetIntegerArg(L, 2);
		GetThis()->MailDelete(row, column);
		return 0;
	}

	int UIPopupMailBox_V11_OpenItem( LuaState * L)
	{
		GetThis()->OnOpenItem(  i3Lua::GetIntegerArg(L, 1, -1) );
		return 0;
	}
	int UIPopupMailBox_V11_SendMail(LuaState * L)
	{
		GetThis()->SendMail();
		return 0;
	}
	int UIPopupMailBox_V11_DeleteAll(LuaState * L)
	{
		GetThis()->DeleteAll();
		return 0;
	}

	int UIPopupMailBox_V11_TakeSelected(LuaState * L)
	{
		GetThis()->TakeSelectedGift();
		return 0;
	}

	int UIPopupMailBox_V11_OnScroll(LuaState * L)
	{
		GetThis()->OnScroll( i3Lua::GetIntegerArg(L, 1, 0) );
		return 0;
	}
	int UIPopupMailBox_V11_CheckBoxStateItem(LuaState* L)
	{
		GetThis()->GetCheckBoxState(i3Lua::GetIntegerArg(L, 1, -1));
		return 0;
	}
}

LuaFuncReg UIPopupMailBox_V11_Glue[] =
{
	{"ChangeTab",			UIPopupMailBox_V11_ChangeTab},
	{"SelectItem",			UIPopupMailBox_V11_SelectItem},
	{"GetNote",				UIPopupMailBox_V11_GetNote},
	{"MailDelete",			UIPopupMailBox_V11_MailDelete},
	{"OpenItem",			UIPopupMailBox_V11_OpenItem},
	{"SendMail",			UIPopupMailBox_V11_SendMail},
	{"DeleteAll",			UIPopupMailBox_V11_DeleteAll},
	{"TakeSelected",		UIPopupMailBox_V11_TakeSelected},
	{"MailList_OnScroll",	UIPopupMailBox_V11_OnScroll},
	{"CheckBoxStateItem",	UIPopupMailBox_V11_CheckBoxStateItem },
	{nullptr,					nullptr}
};

void	UIPopupMailBox_V11::OnOpenItem(INT32 sel_idx)
{
	if( sel_idx != -1)
	{
		INT32	nCount	= 0;

		switch( m_CurrentTab)
		{
		case	MessageTab_Receive :
			nCount	= MailBoxContext::i()->GetNoteReceiveCount();
			break;
		case	MessageTab_ReceivePresent :
			nCount	= MailBoxContext::i()->GetNoteGiftCount();
			break;
		default:
			break;
		}

		GetThis()->GetMainFrame()->OpenPopup( UPW_MAILREV, &m_CurrentTab, &sel_idx );
	}

}

void UIPopupMailBox_V11::GetCheckBoxState(INT32 sel_idx)
{
	LuaState * L = _CallLuaFunction("getCheckBoxCtrl");
	if (L != nullptr)
	{
		INT32 nCount = -1;
		switch (m_CurrentTab)
		{
		case	MessageTab_Receive:
			nCount = MailBoxContext::i()->GetNoteReceiveCount();
			break;
		case	MessageTab_ReceivePresent:
			nCount = MailBoxContext::i()->GetNoteGiftCount();
			break;
		default:
			break;
		}

		i3Lua::PushInteger(L, sel_idx - m_scroll_idx);
		i3UICheckBox *pCtrl = reinterpret_cast<i3UICheckBox*>(_ReturnLuaPointer(L, 1));
		if(sel_idx >=0 && sel_idx < nCount)
			m_checkBoxState[sel_idx] = pCtrl->isChecked();
	}

	UpdateTakeButtonState();
}

void	UIPopupMailBox_V11::OnSelItems(INT32 sel_idx)
{
	m_SelectedListIdx = sel_idx;
}

UIPopupMailBox_V11::UIPopupMailBox_V11() : m_scroll_idx(0), m_view_item_count(0), m_pListBox(nullptr)
{
	m_CurrentTab = MessageTab_None;
	m_SelectedListIdx = -1;
	::memset(m_abLoadedItem, 0, sizeof(bool) * 256);
	::memset(m_checkBoxState, 0, sizeof(bool) * 256);
}

UIPopupMailBox_V11::~UIPopupMailBox_V11()
{

}

/*virtual*/ void UIPopupMailBox_V11::ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData)
{
	// 어느 위치에서든 Lobby Event를 받아야합니다.
	// 하위 Phase일 수도 있기에 Base에서 처리합니다. --swoongoo
	UIPopupBase::ProcessGameEvent( evt, arg, UserData);

	switch(evt)
	{
	case EVENT_DELETE_NOTE:
		ReceiveAllNote();
		break;
	case EVENT_AUTH_GIFT_GOODS:
		if( EV_SUCCESSED( arg ) )
		{
			// 선물이 인벤토리에 정상적으로 지급되었습니다.
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));

			INT32 nCount = MailBoxContext::i()->GetNoteGiftCount();

			for (int i = 0; i < nCount; i++)
			{
				if (m_checkBoxState[i])
				{
					NOTE_RECEIVE*	pNote = nullptr;
					pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote(i));

					if (pNote != nullptr && pNote->header._type == NT_GIFT)
					{
						pNote->header._state = NS_READED;		// 읽음상태로 변경합니다.

						INT8	ReadCount = 1;
						GameEventSender::i()->SetEvent(EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
						MailBoxContext::i()->SetNoteRefreshFlag();
					}
				}	
			}
			::memset(m_checkBoxState, 0, sizeof(bool) * 256);
			//AllNote_Update();
		}
		else
		{
			// 네트워크 문제로 인벤토리에 선물 지급이 실패했습니다.
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_FAILED_GIFT"));
		}
		break;
	default:			
		/* Do nothing */
		break;
	}
}

/*virtual*/ void UIPopupMailBox_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pListBox = (i3UIListView_Box*) pScene->FindChildByName( "i3UIListView_Box");
	I3ASSERT(m_pListBox != nullptr);

	i3UIListView_Item* vitem = m_pListBox->getBoundItem();
	i3UIListView_Cell* cell  = vitem->getCell(1);		// 타겟이 되는게 1번 열인듯 하다..

	i3UIFont*		font = cell->ObtainUIFont();

	m_subject_width = INT32(cell->getWidth() ) - cell->getTextSpaceX() * 2;	

	m_hdc = ::GetDC( g_hWnd );
	::SelectObject(m_hdc, font->getFontHandle() );
	SIZE size_ext;
	::GetTextExtentPoint32A(m_hdc, "....", 3, &size_ext);		// .하나를 더 추가해서 약간의 여유공백을 확보..
	m_ellipse_width = size_ext.cx;

	m_scroll_idx = m_pListBox->GetFirstVisibleIndex();

	m_view_item_count = m_pListBox->getItemCount();

	m_SelectedListIdx = -1;
}

void	 UIPopupMailBox_V11::_ClearAtUnload( i3UIScene * pScene)
{
	::ReleaseDC(g_hWnd, m_hdc);

	// 쪽지함이 열린 상태로 게임에 입장할 경우, 
	// MainFrame의 Destructor 호출시 TopMenu가 먼저 Release되어 nullptr이 될 수 있습니다.
	UITopMenu_V11 * pTop = GetMainFrame()->GetTopMenu_V11();

	if(pTop != nullptr)
		pTop->_CheckUnReadMail();

	UIPopupBase::_ClearAtUnload(pScene);
}

void UIPopupMailBox_V11::_InitTab( void)
{
	m_CurrentTab	= MessageTab_Receive;

	LuaState * L = _CallLuaFunction( "InitTab");
	i3Lua::PushBoolean(L, UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT));
	_EndLuaFunction( L, 1);
	
	L = _CallLuaFunction( "ChangeTabState");
	i3Lua::PushInteger( L, m_CurrentTab);
	_EndLuaFunction(L, 1);

	ChangeTab( m_CurrentTab);
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPopupMailBox_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_MessageBox.i3UIs", UIPopupMailBox_V11_Glue);

	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_SECESSION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_MASTER");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_STAFF");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_REGULAR");
}

/*virtual*/ bool UIPopupMailBox_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	::memset(m_abLoadedItem, 0, sizeof(bool) *256);

	return true;
}

void UIPopupMailBox_V11::ChangeTab( MessageTab tab)
{
	m_CurrentTab = tab;
	m_SelectedListIdx = -1;
	ReceiveAllNote();
}

void UIPopupMailBox_V11::MailDelete(INT32 row, INT32 col)
{
	// Delete Btn Cell에서만 동작
	if(col == 5 && m_CurrentTab == MessageTab_Receive)
	{
		m_nDelMessage = row ;
		/*선택한 쪽지를 삭제 하시겠습니까?*/
		GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_DELETE_NOTE"), nullptr, MAKE_CALLBACK(CbDeleteNote), this);	
	}
	else if (col == 5 && m_CurrentTab == MessageTab_ReceivePresent)
	{
		const INT32 nCount = MailBoxContext::i()->GetNoteGiftCount();

		NOTE_RECEIVE* pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote(nCount - row - 1));
		if (pNote != nullptr)
		{
			GIFT_BUFFER* pGift = (GIFT_BUFFER*)pNote->body._user;
			if (pNote != nullptr)
			{
				m_nDelMessage = row + m_scroll_idx;

				UINT8 ui8Type = TYPE_SHOP_GIFT_AUTH;
				UINT8 ui8Count = 1;

				GameEventSender::i()->SetEvent(EVENT_AUTH_GIFT_GOODS, &ui8Type, &ui8Count, &pGift->_i32GiftWareDBIdx);
			}
		}
	}
}

void UIPopupMailBox_V11::SendEventTakeGift(UINT8 giftCount, T_GiftDBIdx* giftlist)
{
	UINT8 ui8Type = TYPE_SHOP_GIFT_AUTH;
	UINT8 ui8Count = giftCount;

	if (GameEventSender::i()->SetEvent(EVENT_AUTH_GIFT_GOODS, &ui8Type, &ui8Count, giftlist) == true)
	{
		MailBoxContext::i()->BuildGiftReqList(ui8Type, giftlist, ui8Count);
	}

	for (int j = 0; j < 8; j++)
		giftlist[j] = 0;
}

void UIPopupMailBox_V11::TakeSelectedGift()
{
	LuaState* L;
	const INT32 nCount = MailBoxContext::i()->GetNoteGiftCount();
	const INT32 first_idx = m_scroll_idx;
	const INT32 end_idx = (nCount > first_idx + m_view_item_count) ? first_idx + m_view_item_count : nCount;
	bool isTakeAllGift = true;
	T_GiftDBIdx GiftIdx[MAX_SHOP_GOODS_BUY_COUNT] = { 0, };
	UINT8 GiftSelectedCount = 0;
	
	//선택받기
	for (INT32 i = first_idx; i < end_idx; ++i)
	{
		L = _CallLuaFunction("getCheckBoxCtrl");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, i - m_scroll_idx);
			i3UICheckBox *pCtrl = reinterpret_cast<i3UICheckBox*>(_ReturnLuaPointer(L, 1));

			if (pCtrl != nullptr && pCtrl->isChecked())
			{
				isTakeAllGift = false;

				NOTE_RECEIVE* pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote(nCount - i - 1));
				if (pNote != nullptr)
				{
					GIFT_BUFFER* pGift = (GIFT_BUFFER*)pNote->body._user;

					if (pGift->_eDeliver == TYPE_GIFT_DELIVER_RECEIVED)
						continue;

					GiftIdx[GiftSelectedCount++] = pGift->_i32GiftWareDBIdx;

					if (GiftSelectedCount >= (UINT8)MAX_SHOP_GOODS_BUY_COUNT)
					{
						SendEventTakeGift(GiftSelectedCount, GiftIdx);
						GiftSelectedCount = 0;
					}
				}
			}
		}
	}

	//전체받기
	if (isTakeAllGift)
	{
		for (INT32 i = 0; i < nCount; ++i)
		{
			//MailDelete(i, 5);
			NOTE_RECEIVE* pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote(i));
			if (pNote != nullptr)
			{
				GIFT_BUFFER* pGift = (GIFT_BUFFER*)pNote->body._user;

				if (pGift->_eDeliver == TYPE_GIFT_DELIVER_RECEIVED)
					continue;

				GiftIdx[GiftSelectedCount++] = pGift->_i32GiftWareDBIdx;

				if (GiftSelectedCount >= (UINT8)MAX_SHOP_GOODS_BUY_COUNT)
				{
					SendEventTakeGift(GiftSelectedCount, GiftIdx);
					GiftSelectedCount = 0;
				}
			}
		}
	}

	//마지막 리스트에 있는 여분의 선물들 인증요청
	SendEventTakeGift(GiftSelectedCount, GiftIdx);
}

void UIPopupMailBox_V11::DeleteAll()
{
	/*쪽지를 전부 삭제 하시겠습니까?*/
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_DELETE_NOTE_ALL"), nullptr, MAKE_CALLBACK(CbDeleteNoteAll), this);	
}

void UIPopupMailBox_V11::SendMail()
{
	// 선택한 쪽지가 없고, 선물 쪽지함일 경우
	if(m_CurrentTab == MessageTab_ReceivePresent)	 
	{
		TakeSelectedGift();
		return;
	}

	// 아이디 수동 입력 쪽지 보내기 팝업
	if (m_SelectedListIdx < 0)
	{
		GetMainFrame()->OpenPopup(UPW_SENDMAIL);
		return;
	}

	// 아이디 자동 입력 쪽지 보내기 팝업
	INT32 count = MailBoxContext::i()->GetNoteReceiveCount();
	const NOTE_RECEIVE* pNote = MailBoxContext::i()->GetReceiveNote(count - m_SelectedListIdx - 1);

	// 일반 쪽지만 닉네임 자동 입력
	if(pNote->header._type == NT_NORMAL)
		GetMainFrame()->OpenPopup( UPW_SENDMAIL, const_cast<TTCHAR*>(pNote->body._sender));
	else	
		GetMainFrame()->OpenPopup( UPW_SENDMAIL);		
}

void UIPopupMailBox_V11::_UpdateAlarmNote(REAL32 rDeltaSeconds)
{
	if( m_bAlarmCommunity)
	{
		m_rAlarmTerm += rDeltaSeconds;

		if (0.25f < m_rAlarmTerm)
		{
			m_rAlarmTerm = 0.0f;
			m_bAlarmFlag = !m_bAlarmFlag;
		}
	}
}

void UIPopupMailBox_V11::StartAlarmNote( void)
{
	m_bAlarmCommunity = true;

	INT32 note_count = MailBoxContext::i()->GetNoteReceiveCount();

	if( note_count >= 100)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));//쪽지함의 용량이 부족하여 더 이상 쪽지를 받으실 수 없습니다.\n불필요한 쪽지를 삭제해 주십시오.
	}
	else if( note_count >= 80)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));//쪽지함의 용량이 얼마 남지 않았습니다.\n쪽지함이 꽉 차게 되면 쪽지를 받으실 수 없으니\n불필요한 쪽지는 삭제해 주십시오.
	}

}

void	UIPopupMailBox_V11::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	_InitTab();

	INT32 note_count = MailBoxContext::i()->GetNoteReceiveCount();

	if( note_count >= MAX_NORMAL_NOTE_COUNT)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));//쪽지함의 용량이 부족하여 더 이상 쪽지를 받으실 수 없습니다.\n불필요한 쪽지를 삭제해 주십시오.
	}
	else if( note_count >= ( MAX_NORMAL_NOTE_COUNT - 20 ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));//쪽지함의 용량이 얼마 남지 않았습니다.\n쪽지함이 꽉 차게 되면 쪽지를 받으실 수 없으니\n불필요한 쪽지는 삭제해 주십시오.
	}
}

void UIPopupMailBox_V11::EndAlarmNote(void)
{
	m_bAlarmCommunity = false;
}

void UIPopupMailBox_V11::ReceiveAllNote_Size( const INT32 nCount )
{
	LuaState * L = _CallLuaFunction( "ClearList");

	if( L != nullptr )
	{
		i3Lua::PushInteger( L, nCount);
		_EndLuaFunction( L, 1);
	}


	L = _CallLuaFunction("getListItemCount");

	if( L != nullptr )
	{
		m_view_item_count = UILua::ReturnLuaInteger(L, 0);
	}

	::memset(m_abLoadedItem, 0, sizeof(bool) * 256);
	::memset(m_checkBoxState, 0, sizeof(bool) * 256);
}

void UIPopupMailBox_V11::ReceiveAllNote_Update( const INT32 nCount)
{
	LuaState* L;
	const INT32 first_idx = m_scroll_idx;
	const INT32 end_idx  = ( nCount > first_idx + m_view_item_count ) ? first_idx + m_view_item_count : nCount ;

	for(INT32 i= first_idx; i < end_idx ; ++i )
	{
		//if ( m_abLoadedItem[i] == true )
//			continue;

		m_abLoadedItem[i] = true;
		
		i3::rc_wstring wstrSubject;
		i3::rc_wstring wstrTemp; 

		NOTE_RECEIVE * pNote = nullptr;

		if(m_CurrentTab == MessageTab_Receive) // 메세지 탭
		{
			pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetReceiveNote(nCount-i-1));
		}
		else if (m_CurrentTab == MessageTab_ReceivePresent) // 선물 탭
		{
			pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote(nCount-i-1));
		}

		if(pNote == nullptr) continue;
		if( 0 <  pNote->body._messageSize )
		{
			i3::sprintf(wstrSubject, L"  %s", pNote->body._message);
		}

		i3::sprintf( wstrTemp, GAME_RCSTRING("STR_POPUP_WHAT_DAY"), pNote->header._remainDay);/*%d일*/

		INT32 deletable = -1;	// 삭제 여부
		INT32 nMailState = 0;	// 쪽지 상태

		// Template와 Mail State 값이 다르기때문에 강제로 변환시켜준다.
		// 선물 쪽지는 클라이언트에서 삭제가 불가능하다.
		// 읽은 쪽지만 삭제버튼이 활성화된다.
		switch(pNote->header._state)
		{
		case NS_READED	: 
			nMailState = 1; 
			if( pNote->header._type != NT_GIFT)
				deletable = 0;
			break;
		case NS_UNREAD	: 	nMailState = 0;		break;
		default			: 	nMailState = 2;		break;

		}

		if (pNote->header._type == NT_GIFT)
		{
			GIFT_BUFFER* pBuf = (GIFT_BUFFER*)pNote->body._user;
			if (pBuf)
			{
				if (pBuf->_eDeliver == TYPE_GIFT_DELIVER_NORMAL)
					deletable = 0;
				else if (pBuf->_eDeliver == TYPE_GIFT_DELIVER_READ)
					deletable = 1;
				else if (pBuf->_eDeliver == TYPE_GIFT_DELIVER_RECEIVED)
					deletable = 2;
				else
					deletable = 0;
			}
			else //@eunil. 선물함이 꺼져 있을 경우 타입은 NT_GIFT로 오지만 내용이 비어 있을 수 있다
				deletable = 0;

		}

		L = _CallLuaFunction("getLIstBoxCtrl");
		if(L != nullptr)
		{
			i3Lua::PushInteger(L, i - m_scroll_idx);
			i3UIControl *pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L, 1));
			if(pCtrl != nullptr)
			{
				if(m_CurrentTab != 0)
				{
					pCtrl->SetTemplateByName("Ico_Message_GiftBox"); 
					i3UIButton* btnIcon = dynamic_cast<i3UIButton*>(pCtrl);

					if (btnIcon && deletable == 2)
					{
						//btnIcon->setInputDisable(true);
						btnIcon->disable_btn();
						//btnIcon->UnSelectedNormalState();
					}
					else
					{
						if(btnIcon) btnIcon->enable_btn();
					}
					/*if (deletable == 2)
					{
						pCtrl->setInputDisable(true);
						pCtrl->SetPushFlag(true);
					}
					else
					{
						pCtrl->setInputDisable(false);
						pCtrl->removeState(I3UI_STATE_PUSHED);
					}*/
				}
				else
				{
					pCtrl->SetTemplateByName("Button_delete"); 
					pCtrl->setInputDisable(false);
					pCtrl->removeState(I3UI_STATE_PUSHED | I3UI_STATE_SELECTED | I3UI_STATE_ONMOUSE | I3UI_STATE_LBTN_DRAG);
					//pCtrl->addState(I3UI_STATE_ONMOUSE | I3UI_STATE_NEED_UPDATE);
				}
			}
		}

		L = _CallLuaFunction( "SetData");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, i);                                          // Index
			i3Lua::PushStringUTF16To8( L, pNote->body._sender);                   // 보낸이
			i3Lua::PushStringUTF16To8( L, wstrSubject);                               // 제목
			i3Lua::PushStringUTF16To8( L, wstrTemp);                                // 날짜
			i3Lua::PushInteger( L, nMailState);								    // 상태
			i3Lua::PushInteger( L, deletable);                                  // 삭제버튼 활성화 여부
			_EndLuaFunction( L, 6);
		}

		L = _CallLuaFunction("getCtrlInListBox");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, i - m_scroll_idx);
			i3Lua::PushInteger(L, 2);

			i3UIControl *pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L, 2));

			if (pCtrl != nullptr) {
				i3UIListView_Cell* pCapCtrl = reinterpret_cast<i3UIListView_Cell*>(pCtrl);
				pCapCtrl->SetTextSpace(20, 0);
			}
		}

		if (m_checkBoxState[i])
		{
			L = _CallLuaFunction("getCheckBoxCtrl");
			if (L != nullptr)
			{
				i3Lua::PushInteger(L, i - m_scroll_idx);
				i3UICheckBox *pCtrl = reinterpret_cast<i3UICheckBox*>(_ReturnLuaPointer(L, 1));
				if (pCtrl)
				{
					pCtrl->setCheck(true);
				}
			}
		}
	}

	UpdateTakeButtonState();
}

void UIPopupMailBox_V11::AllNote_Update()
{
	switch( m_CurrentTab )
	{
	case	MessageTab_Receive :
		ReceiveAllNote_Update( MailBoxContext::i()->GetNoteReceiveCount());
		break;
	case	MessageTab_ReceivePresent :
		ReceiveAllNote_Update( MailBoxContext::i()->GetNoteGiftCount());
		break;
	default:
		break;
	}
}

void UIPopupMailBox_V11::ReceiveAllNote()
{
	INT32 nCount = 0;
	// 전체 삭제 버튼 (선물 탭 이외에는 활성화)

	bool bDeleteBtnEnable = true;	

	switch( m_CurrentTab )
	{
	case MessageTab_Receive			:	
		nCount = MailBoxContext::i()->GetNoteReceiveCount();	
		break;
	case MessageTab_ReceivePresent	:	
		nCount = MailBoxContext::i()->GetNoteGiftCount(); 
		bDeleteBtnEnable = false;	
		break;
	default:
		return;
	}

	LuaState* L = _CallLuaFunction("SetDeleteAllBtn");

	if( L != nullptr)
	{
		i3Lua::PushBoolean(L , bDeleteBtnEnable == true ? true : false);
		_EndLuaFunction(L , 1);
	}

	UIPopupMailBox_V11::ReceiveAllNote_Size( nCount );
	AllNote_Update();
}

void UIPopupMailBox_V11::CbDeleteNote( void * pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		UIPopupMailBox_V11* community = (UIPopupMailBox_V11*)pThis;
		community->OnDeleteNote();
	}
}

void UIPopupMailBox_V11::CbDeleteNoteAll( void * pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		UIPopupMailBox_V11* community = (UIPopupMailBox_V11*)pThis;
		community->OnDeleteNoteAll();
	}
}

void UIPopupMailBox_V11::OnDeleteNoteAll(void)
{
	UINT8 DeleteCount = 0;
	INT32 DeleteArray[MAX_NOTE_COUNT];

	bool bGiftTabFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT);

	for(INT32 i = MailBoxContext::i()->GetNoteReceiveCount() - 1; i>=0; i--)
	{
		const NOTE_RECEIVE* pNote = MailBoxContext::i()->GetReceiveNote( i);
		//	선물쪽지는 따로 관리되므로 여기서 삭제되면 안됩니다. komet
		//  @eunil. 선물함 머지 이후 선물함이 꺼져 있으면 선물 쪽지도 삭제합니다.
		// PBBUG-16612 읽은 쪽지만 삭제합니다.
		if (pNote && pNote->header._state == NS_READED)
		{
			if (!bGiftTabFlag)
				DeleteArray[DeleteCount++] = pNote->header._id;

			else if (pNote->header._type != NT_GIFT)
				DeleteArray[DeleteCount++] = pNote->header._id;
		}
	}

	if(DeleteCount == 0)	return;

	GameEventSender::i()->SetEvent( EVENT_DELETE_NOTE, &DeleteCount, DeleteArray);
	m_nDelMessage = -1;
	m_SelectedListIdx = -1;
}

void UIPopupMailBox_V11::OnDeleteNote(void)
{
	UINT8 DeleteCount = 0;
	INT32 DeleteArray[MAX_NOTE_COUNT];

	NOTE_RECEIVE* pNote = nullptr;

	// 선택된 삭제 Row가 -1 일경우 이벤트를 실행하지 않음
	if(m_nDelMessage == -1)
		return;

	bool bGiftTabFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT);
	INT32 nListIdx = MailBoxContext::i()->GetNoteReceiveCount() - m_nDelMessage - 1;

	pNote = (NOTE_RECEIVE*)MailBoxContext::i()->GetReceiveNote( nListIdx);
	//	선물쪽지는 따로 관리되므로 여기서 삭제되면 안됩니다. komet
	//  @eunil. 선물함 머지 이후 선물함이 꺼져 있으면 선물쪽지도 삭제합니다.
	if(bGiftTabFlag == false || pNote->header._type != NT_GIFT)
	{
		DeleteArray[DeleteCount] = pNote->header._id;
		DeleteCount++;
	}

	GameEventSender::i()->SetEvent( EVENT_DELETE_NOTE, &DeleteCount, DeleteArray);
	m_nDelMessage = -1;
	m_SelectedListIdx = -1;
}

/*virtual*/ void UIPopupMailBox_V11::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	if( isSliding() == false ) _UpdateNote();
}

void UIPopupMailBox_V11::_UpdateNote(void)
{
	if( MailBoxContext::i()->IsNoteNew())
	{
		// 쪽지 리스트 인덱스 초기화 안함
		ReceiveAllNote();
		MailBoxContext::i()->ResetNoteNewFlag();
	}

	if( MailBoxContext::i()->IsNoteRefresh() )
	{
		ReceiveAllNote();
		MailBoxContext::i()->ResetNoteRefreshFlag();
	}

	//메일 안 읽은거 있는지 체크해보자.
	//GetMainFrame()->GetTopMenu()->CheckUnReadMail();
}

void	UIPopupMailBox_V11::OnScroll(INT32 first_idx)
{
	m_scroll_idx = first_idx;
	AllNote_Update(); 
}

void	UIPopupMailBox_V11::UpdateTakeButtonState()
{
	LuaState* L;
	int nCount = MailBoxContext::i()->GetNoteGiftCount();
	bool isSelectedAnyGiftCheck = false;

	for (int i = 0; i < nCount; i++)
	{
		if (m_checkBoxState[i])
		{
			isSelectedAnyGiftCheck = true;
		}
	}

	L = _CallLuaFunction("ChangeReciveBtnText");
	if (L != nullptr) {
		i3Lua::PushBoolean(L, isSelectedAnyGiftCheck);
		_EndLuaFunction(L, 1);
	}
}