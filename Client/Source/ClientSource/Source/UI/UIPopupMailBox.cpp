#include "pch.h"
#include "UIMainFrame.h"
#include "UIPopupMailBox.h"
#include "UITabMailBox_Note.h"
#include "UITabMailBox_Gift.h"

#include "UIUtil.h"
#include "UITopMenu_V15.h"

#include "MailBoxContext.h"
#include "UserInfoContext.h"

extern "C" 
{
	int UIPopupMailBox_ChangeTab( LuaState * L)
	{
		INT32 nIndex	= i3Lua::GetIntegerArg(L, 1, -1);
		UIPopupMailBox * pThis = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
		if( pThis != nullptr)
		{
			bool bRes		= pThis->ChangeTab( nIndex );
			i3Lua::PushBoolean( L, bRes);
			return 1;
		}

		return 0;
	}

	int UIPopupMailBox_SelectAll( LuaState * L)
	{
		bool bSelect = i3Lua::GetBooleanArg( L, 1);

		UIPopupMailBox * pThis = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
		if( pThis != nullptr)
			pThis->SetSelectAll(bSelect?true:false);

		return 0;
	}

	int UIPopupMailBox_DeleteMail( LuaState * L)
	{
		UIPopupMailBox * pThis = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
		if( pThis != nullptr)
			pThis->DeleteMail();
		return 0;
	}

	int UIPopupMailBox_SetReadedMail( LuaState * L)
	{
		UIPopupMailBox * pThis = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
		if( pThis != nullptr)
			pThis->SetReadedMail();
		return 0;
	}

	int UIPopupMailBox_SendMail( LuaState * L)
	{
		UIPopupMailBox * pThis = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
		if( pThis != nullptr)
			pThis->SendMail();
		return 0;
	}

	int UIPopupMailBox_ReceivaAll( LuaState * L)
	{
		UIPopupMailBox * pThis = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
		if( pThis != nullptr)
			pThis->ReceiveAllItem();
		return 0;
	}
}

LuaFuncReg UIPopupMailBox_Glue[] =
{
	{"ChangeTab",			UIPopupMailBox_ChangeTab},	
	{"SelectAll",			UIPopupMailBox_SelectAll},
	{"DeleteMail",			UIPopupMailBox_DeleteMail},	
	{"SetReadedMail",		UIPopupMailBox_SetReadedMail},	
	{"SendMail",			UIPopupMailBox_SendMail},	
	{"ReceiveAll",			UIPopupMailBox_ReceivaAll},
	{nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UIPopupMailBox);//, UIPopupBase );

UIPopupMailBox::UIPopupMailBox() : m_eCurrentTab( TAB_MESSAGE_LIST ), m_eNextTab(TAB_NONE), m_pMsgBoxDeleteCallback(nullptr)
{
	for( INT32 i = 0; i < MAX_MAILBOXTAB; ++i)
		m_pMailBoxTap[i] = nullptr;
}

UIPopupMailBox::~UIPopupMailBox()
{
	for( INT32 i = 0; i < MAX_MAILBOXTAB; ++i)
		if( m_pMailBoxTap[i]) m_pMailBoxTap[i]->Destroy();
}

void	UIPopupMailBox::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate( pParent );
	AddScene( "Scene/Popup/PBRe_PopUp_MessageBox.i3UIs", UIPopupMailBox_Glue );

	i3ClassMeta * pMeta = nullptr;

	for (INT32 i = 0 ; i < MAX_MAILBOXTAB ; ++i)
	{
		switch( i)
		{	
		case TAB_MESSAGE_LIST	: pMeta = UITabMailBox_Note::static_meta();	break;
		case TAB_GIFT_LIST		: pMeta = UITabMailBox_Gift::static_meta();	break;
			/*
		default :
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_FATAL,  "Not Setted MailBoxTab class...!!!");
			break;
			*/
		}

		if( pMeta != nullptr)
		{
			m_pMailBoxTap[ i] = (UITabMailBox *) pMeta->create_instance();
			I3ASSERT( m_pMailBoxTap[ i] != nullptr);
			I3ASSERT( m_pMailBoxTap[ i]->same_of( pMeta));

			m_pMailBoxTap[i]->OnCreate( this);
			I3_MUST_ADDREF(m_pMailBoxTap[i]);
		}
	}
}

bool	UIPopupMailBox::OnEntranceStart( void * pArg1, void * pArg2 )
{
	bool bRes = UIPopupBase::OnEntranceStart( pArg1, pArg2 );

	m_eCurrentTab = TAB_MESSAGE_LIST;
	m_eNextTab = TAB_NONE;
	m_pMsgBoxDeleteCallback = nullptr;

	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
		pMailBox->OnEntranceStart(pArg1, pArg2);
	return bRes;
}

void	UIPopupMailBox::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	_SetButtonState();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	if(!UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
		_SetEnableGiftTap(false);
}

bool	UIPopupMailBox::OnExitStart(void)
{
	if(m_pMsgBoxDeleteCallback != nullptr)
	{
		GameUI::CloseMsgBox(m_pMsgBoxDeleteCallback);
		m_pMsgBoxDeleteCallback = nullptr;
	}

	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
	{
		pMailBox->GetSlide()->SetTargetTime(false, 0.f);
		pMailBox->OnExitStart();
	}
	
	return UIPopupBase::OnExitStart();
}

bool	UIPopupMailBox::ChangeTab( int nTab )
{
	if( nTab < 0 || MAX_MAILBOXTAB <= nTab )
	{
		I3PRINTLOG(I3LOG_NOTICE, "invalid tab arg... tab = %d", nTab );
		return false;
	}

	if( m_eCurrentTab != nTab )
	{
		m_eNextTab = static_cast<MAILBOXTAB>(nTab);

		UITabMailBox * pMailBox = GetCurrentTab();
		if( pMailBox != nullptr)
		{
			pMailBox->GetSlide()->SetTargetTime(false, 0.f);
			pMailBox->OnExitStart();	
		}
	}

	_SetButtonState();

	return true;
}

void UIPopupMailBox::ProcessNextTab( void)
{
	if(m_eNextTab != TAB_NONE)
	{
		m_eCurrentTab = m_eNextTab;
		m_eNextTab = TAB_NONE;

		UITabMailBox * pMailBox = GetCurrentTab();
		if( pMailBox != nullptr )
		{
			pMailBox->GetSlide()->SetTargetTime( true, 0.f );				
			pMailBox->OnEntranceStart();
		}
	}
}

void	UIPopupMailBox::SetNoticeMessage(const i3::wliteral_range& wNoticeRng)
{
	LuaState * L = _CallLuaFunction("SetNoticeMessage");
	if( nullptr != L)
	{
		i3Lua::PushStringUTF16To8(L, wNoticeRng);
		_EndLuaFunction(L , 1);
	}
}

void	UIPopupMailBox::_ClearAtUnload( i3UIScene* pScene )
{
	// 쪽지함이 열린 상태로 게임에 입장할 경우, 
	// MainFrame의 Destructor 호출시 TopMenu가 먼저 Release되어 nullptr이 될 수 있습니다.
	UITopMenu_V15 * pTop = GetMainFrame()->GetTopMenu_V15();

	if(pTop != nullptr)
		pTop->_CheckUnReadMail();

	UIPopupBase::_ClearAtUnload( pScene );
}

void	UIPopupMailBox::SetSelectAll( bool bSelect )
{
	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
		pMailBox->SetSelectAll(bSelect);
}

void	UIPopupMailBox::SetReadedMail()
{
	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
		pMailBox->SetReadedMail();
}

void	UIPopupMailBox::DeleteMail()
{
	switch(m_eCurrentTab)
	{
	case TAB_MESSAGE_LIST	: GetCurrentTab()->DeleteMail(); break;
	case TAB_GIFT_LIST		: 
		{
			// 체크된 쪽지가 있는 경우에만 팝업 출력.
			if( GetCurrentTab()->IsCheckedNote() )
			{
				/*'선물이 남아 있을 수 있습니다.정말 삭제 하시겠습니까?'*/
				m_pMsgBoxDeleteCallback = GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_GIFTDELETE"), nullptr, MAKE_CALLBACK(cbDeleteMail), this);
			}
		}
		break;
	}		
}

void	UIPopupMailBox::SendMail(void)
{
	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
		pMailBox->SendMail();
}

void	UIPopupMailBox::ReceiveAllItem(void)
{
	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
		pMailBox->ReceiveItem();
}

void	UIPopupMailBox::ProcessGameEvent( GAME_EVENT event, INT32 Arg , const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData );

	UITabMailBox * pMailBox = GetCurrentTab();
	if( pMailBox != nullptr)
		pMailBox->ProcessGameEvent(event, Arg, UserData);
}

/*vurtial*/ bool UIPopupMailBox::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName() , "btn_Exit")  )
	{
		return PlaySound(UISND_ITEM_CLICK_CANCEL);
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}

void	UIPopupMailBox::StartAlarmNote(void)
{
	int nNoteCnt = MailBoxContext::i()->GetNoteReceiveCount();

	if( nNoteCnt >= 100)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));//쪽지함의 용량이 부족하여 더 이상 쪽지를 받으실 수 없습니다.\n불필요한 쪽지를 삭제해 주십시오.
	}
	else if( nNoteCnt >= 80)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));//쪽지함의 용량이 얼마 남지 않았습니다.\n쪽지함이 꽉 차게 되면 쪽지를 받으실 수 없으니\n불필요한 쪽지는 삭제해 주십시오.
	}
}

void UIPopupMailBox::_SetButtonState(void)
{
	LuaState * L = _CallLuaFunction("SetButtonState");

	if(nullptr != L)
	{
		i3Lua::PushInteger(L, m_eCurrentTab);
		_EndLuaFunction(L , 1);
	}
}

void UIPopupMailBox::_SetEnableGiftTap(bool bEnable)
{
	LuaState * L = _CallLuaFunction("SetEnableGiftTap");

	if(nullptr != L)
	{
		i3Lua::PushBoolean(L, bEnable);
		_EndLuaFunction(L , 1);
	}
}

void UIPopupMailBox::cbDeleteMail(void* pThis,INT32 nParam)
{
	UIPopupMailBox * pPopup = (UIPopupMailBox*)pThis;

	if (MBR_OK == nParam)
	{
		UITabMailBox * pMailBox = pPopup->GetCurrentTab();
		if( pMailBox != nullptr)
			pMailBox->DeleteMail();
	}

	pPopup->Reset_DeleteCallback_MsgBox();
}