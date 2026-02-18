#include "pch.h"
#include "UIPhaseSelectServer.h"

#include "UIMainFrame.h"
#include "UIPopupQueryPassword.h"
#include "UIUtil.h"

#include "UIFloatMiniProfile.h"
#include "UIFloatChannelList.h"

#include "UI_V10/UITopMenu_V10.h"

#include "../GameGUI.h"
#include "../CommunityContext.h"
#include "../StageBattle/NetworkContext.h"

#include <tlhelp32.h>
#include <Psapi.h>
#include "../QuickJoinContext.h"
#include "../LobbyContext.h"
#include "UserInfoContext.h"
#include "i3Base/string/ext/itoa.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPhaseSelectServer_RequestChannelList( LuaState * L)
	{
		INT32 serverIdx = i3Lua::GetIntegerArg( L, 1);

		I3TRACE( "EnterServer %d\n", serverIdx);

		UIPhaseSelectServer * pThis = static_cast<UIPhaseSelectServer*>(GameUI::GetPhase(UIPHASE_SERVER_CHANNEL_LIST));

		if( pThis != nullptr)
			pThis->RequestChannelList(serverIdx);

		return 0;
	}

	int UIPhaseSelectServer_ToggleServerTextColor( LuaState * L)
	{
		INT32 serverIdx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseSelectServer * pThis = static_cast<UIPhaseSelectServer*>(GameUI::GetPhase(UIPHASE_SERVER_CHANNEL_LIST));
		if( pThis != nullptr)
			pThis->ToggleTextColor(serverIdx);
		return 0;
	}

	int UIPhaseSelectServer_QuickJoinClicked( LuaState * L)
	{
		UIMainFrame* f = g_pFramework->GetUIMainframe();
		I3ASSERT(f);
		if(LocaleValue::Enable("EnableQuickJoin") == true)
			f->OpenPopup(UPW_QUICK_JOIN);
		return 0;
	}

	int UIPhaseSelectServer_ServerGroupItemCliecked( LuaState * L)
	{
		INT32 serverIdx = i3Lua::GetIntegerArg( L, 1);

		UIPhaseSelectServer * pThis = static_cast<UIPhaseSelectServer*>(GameUI::GetPhase(UIPHASE_SERVER_CHANNEL_LIST));

		if( pThis != nullptr)
			pThis->SelectGroupListItem(serverIdx);

		return 0;
	}

}	// extern "C"


LuaFuncReg PhaseSelectServer_Glue[] = 
{
	{"RequestChannelList",				UIPhaseSelectServer_RequestChannelList},	// 채널리스트 요청
	{"ToggleServerTextColor",			UIPhaseSelectServer_ToggleServerTextColor},
	{"QuickJoinClicked",				UIPhaseSelectServer_QuickJoinClicked},
	{"SetGroupItem",					UIPhaseSelectServer_ServerGroupItemCliecked},	// Tab 이동
	{nullptr, nullptr}
};

//----------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPhaseSelectServer);//, UIPhaseBase);

UIPhaseSelectServer::UIPhaseSelectServer() : m_SelectedServerIndex(INVALID_SERVER_INDEX), m_LastRefreshTime(0.f)
	, m_CurrentTab( S_CHANNEL_TYPE_UNDEFINED), m_pInfoWaitIcon(nullptr), m_ConnectFailedCount(0), m_ConnectFailedTotalCount(0)
	, m_PrevServerIndex( INVALID_SERVER_INDEX), m_PrevServerTab( S_CHANNEL_TYPE_UNDEFINED), m_TargetServerIndex( INVALID_SERVER_INDEX)
	, m_TargetServerTab( S_CHANNEL_TYPE_UNDEFINED), m_bMemoryMessage(false), m_bOnceMemoryCheck(false), m_bChinaMessage(false)
	, m_bOnceChina(false), m_bViewChannelList(false)
{
	RegisterGameEventFunc( EVENT_CONNECT_SERVER_GAME,			&UIPhaseSelectServer::ReceiveGameEvent_Connect_Server_Game);
	RegisterGameEventFunc( EVENT_CONNECT_SERVER_GATEWAY,		&UIPhaseSelectServer::ReceiveGameEvent_Connect_Server_Gateway);
	RegisterGameEventFunc( EVENT_GET_USER_INFO,					&UIPhaseSelectServer::ReceiveGameEvent_GetInfo);
	RegisterGameEventFunc( EVENT_GET_INVEN_INFO,				&UIPhaseSelectServer::ReceiveGameEvent_GetInfo);
	RegisterGameEventFunc( EVENT_GET_SYSTEM_INFO,				&UIPhaseSelectServer::ReceiveGameEvent_GetInfo);
	RegisterGameEventFunc( EVENT_USER_ENTER,					&UIPhaseSelectServer::ReceiveGameEvent_User_Enter);
	RegisterGameEventFunc( EVENT_USER_LEAVE,					&UIPhaseSelectServer::ReceiveGameEvent_User_Leave);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,			&UIPhaseSelectServer::ReceiveGameEvent_Destroyed_Network);
	RegisterGameEventFunc( EVENT_ENTER_PW,						&UIPhaseSelectServer::ReceiveGameEvent_Enter_Password);
	RegisterGameEventFunc( EVENT_GET_CHANNEL_LIST,				&UIPhaseSelectServer::ReceiveGameEvent_Get_ChannelList);
	RegisterGameEventFunc( EVENT_JOIN_ROOM,						&UIPhaseSelectServer::ReceiveGameEvent_Join_Room);
	RegisterGameEventFunc( EVENT_SET_SERVERTAB_SELECT,			&UIPhaseSelectServer::ReceiveGameEvent_SetTab_AutoSelectServer);
}

UIPhaseSelectServer::~UIPhaseSelectServer()
{
}

void UIPhaseSelectServer::__SetServerInfo( INT32 slot, const i3::wliteral_range& wNameRng, COLOR * pColor)
{
	LuaState* L = _CallLuaFunction( "SetServerInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);
		i3Lua::PushStringUTF16To8( L, wNameRng);
		i3Lua::PushColor(L, pColor);
		_EndLuaFunction( L, 6);
	}
}

i3::rc_wstring UIPhaseSelectServer::__GetServerTabTypeText( SChannelType TabType ) const
{
	switch(TabType)
	{
	case S_CHANNEL_TYPE_ALL			:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_ALL");
	case S_CHANNEL_TYPE_NORMAL		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_NORMAL");
	case S_CHANNEL_TYPE_NORMAL2		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_NORMAL2");
	case S_CHANNEL_TYPE_BEGIN		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_BEGIN");
	case S_CHANNEL_TYPE_CLAN		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_CLAN");
	case S_CHANNEL_TYPE_EXPERT		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_EXPERT");
	case S_CHANNEL_TYPE_CONBINATION	:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_COMBINATION");
	case S_CHANNEL_TYPE_CHAMPIONSHIP:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_TAB_CHAMPIONSHIP");
	case S_CHANNEL_TYPE_PCCAFE_SILVER:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_GWARNET_SILVER");
	case S_CHANNEL_TYPE_PCCAFE_GOLD:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_GWARNET_GOLD");
	case S_CHANNEL_TYPE_PCCAFE_CLAN:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_GWARNET_CLAN");
	}

	return INVALID_TEXTW;
}

i3::rc_wstring UIPhaseSelectServer::__GetServerTypeText( SChannelType ServerType ) const
{
	switch(ServerType)
	{
	case S_CHANNEL_TYPE_NORMAL		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_NORMAL");
	case S_CHANNEL_TYPE_NORMAL2		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_NORMAL");
	case S_CHANNEL_TYPE_BEGIN		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_BEGIN");
	case S_CHANNEL_TYPE_CLAN		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_CLAN");
	case S_CHANNEL_TYPE_EXPERT		:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_EXPERT");
	case S_CHANNEL_TYPE_CONBINATION	:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_COMBINATION");
	case S_CHANNEL_TYPE_CHAMPIONSHIP:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_CHAMPIONSHIP");
	case S_CHANNEL_TYPE_PCCAFE_SILVER:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_GWARNET_SILVER");
	case S_CHANNEL_TYPE_PCCAFE_GOLD:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_GWARNET_GOLD");
	case S_CHANNEL_TYPE_PCCAFE_CLAN:	return GAME_RCSTRING("STR_TBL_NETWORK_SERVER_GWARNET_CLAN");
	}
	return INVALID_TEXTW;
}

/*virtual*/ void UIPhaseSelectServer::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	I3ASSERT( m_pFrameWnd);
	m_pServerListBox = (i3UIListView_Box*) m_pFrameWnd->FindChildByName( "i3UIListView_Box_SL");
	I3ASSERT( m_pServerListBox);

#ifdef A_LOT_OF_SERVERS
	SChannelType	Type = S_CHANNEL_TYPE_NORMAL;
	SetCurrentTab(S_CHANNEL_TYPE_NORMAL);
	//UpdateServerTab();
	//OnChangeTab(S_CHANNEL_TYPE_NORMAL);
#else
	SChannelType	Type = S_CHANNEL_TYPE_ALL;
	SetCurrentTab(S_CHANNEL_TYPE_ALL);
	//UpdateServerTab();
	//OnChangeTab(S_CHANNEL_TYPE_ALL);
#endif

	// StageReady가 생성될때마다 호출되기 때문에 매번 SetCurGameServerType을 호출하면 안된다.
	// 탕비이 지정안되어 있을 때만 설정하도록 해야한다.
	if ( g_pServerInfo->GetCurGameServerType() == S_CHANNEL_TYPE_UNDEFINED )
	{
		g_pServerInfo->SetCurGameServerType(Type);
	}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseSelectServer::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Main/PointBlankRe_Server_List.i3UIs", PhaseSelectServer_Glue);
}

/*virtual*/ void UIPhaseSelectServer::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	UIPhaseBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}

	UpdateServerSlotList();
}

bool UIPhaseSelectServer::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	switch (event)
	{
	case I3_EVT_INPUT:
		{
			i3InputDeviceManager * pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard * pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState((I3I_KEY_F5)) && LocaleValue::Enable("EnableQuickJoin") == true)
			{
				if (m_bViewChannelList)
				{
					UIMainFrame* f = g_pFramework->GetUIMainframe();
					I3ASSERT(f);
					f->OpenPopup(UPW_QUICK_JOIN);
				}
			}
			else if (pKeyboard->GetStrokeState(I3I_KEY_ESC) )
			{
				OnEscapeKey();
			}
		}
		break;

	default: break;
	}

	return true;
}

/*virtual*/ bool UIPhaseSelectServer::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// 내 정보는 단 한번만 받아야한다.
	m_bChinaMessage = false;
	m_bMemoryMessage = false;
	m_bOnceChina = false;
	m_bOnceMemoryCheck = false;
	m_bViewChannelList = false;
	
	if( UserInfoContext::i()->GetReceivedMyInfo() == false)
	{
		m_bOnceChina = true;
		m_bOnceMemoryCheck = true;

		// 로그인 후 단 한번만 받아야한다.
		I3ASSERT( m_pInfoWaitIcon == nullptr);
		i3UIControl * pCtrl = (i3UIControl*) GetScene(0)->FindChildByName( "FrameWnd_LoadingIcon");
		m_pInfoWaitIcon = GameUI::EnableLoadingIcon( this, pCtrl);

		DEV_LOG("CPhaseServerList::OnEntranceStart - EVENT_GET_MYINFO");

		GameEventSender::i()->SetEvent(EVENT_GET_SYSTEM_INFO);
		GameEventSender::i()->SetEvent(EVENT_GET_MYOPTION_N);
		// 서버에서 끊어서 일단 주석... swoongoo.jung 2013.10.31
		//GameEventSender::i()->SetEvent(EVENT_GET_RANKUP_ITEMLIST);
	}

	I3ASSERT( m_pFrameWnd );
	m_pFrameWnd->EnableCtrl(true);

	// 선택된 서버 인덱스를 대입하는 이유는 처음 서버리스트페이지를 보는 경우 말고
	// 게임 후 서버 변경이나 기타 등의 이유로 다시 서버리스트를 보게될경우
	// 내가 이전에 있었던 서버가 어떤것인지 알 수 있게 하기 위해서 입니다.
	// 처음에는 -1이 대입되겠지만 문제되지 않습니다.
	m_SelectedServerIndex	= g_pServerInfo->GetCurGameServerIndexByType();
	SetCurrentTab(g_pServerInfo->GetCurGameServerType());
	OnChangeTab(g_pServerInfo->GetCurGameServerType());

	m_PrevServerIndex	= g_pServerInfo->GetCurGameServerIndexByType();
	m_PrevServerTab		= g_pServerInfo->GetCurGameServerType();

	UpdateServerTab();
	UpdateServerSlotList();

	// Button의 상태를 설정
	SetButtonState();

	DEV_LOG("CPhaseServerList::OnEntranceStart");

	return true;
}

/*virtual*/ void UIPhaseSelectServer::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();

	m_bMemoryMessage = true;

	// 찾아가기 
	OnInviteMove();

	UITopMenu_V10 * pTop = GetMainFrame()->GetTopMenu_V10();
	if( pTop && i3::same_of<UITopMenu_V10*>(pTop) )
	{
		pTop->InitMyInfo();
	}

	//UITopMenu_V15 * pTop = GetMainFrame()->GetTopMenu_V15();
	//if( pTop != nullptr)
	//	pTop->cb_tmenu_gauge(false);
}

/*virtual*/ bool UIPhaseSelectServer::OnExitStart( void)
{
	OnSlideOut();

	m_SelectedServerIndex = -1;

	return true;
}

/*virtual*/ void UIPhaseSelectServer::OnExitEnd(void)
{
	UIPhaseBase::OnExitEnd();

	//I3ASSERT( m_pMBInfoWait == nullptr );

	//DEV_LOG("CPhaseServerList::OnExitEnd");
}

/*virtual*/ void UIPhaseSelectServer::OnSlideIn( void)
{
	UIBase::OnSlideIn();
}

/*virtual*/ void UIPhaseSelectServer::OnGotoBackPhase( void)
{
	GameEventSender::i()->SetEventOnce( EVENT_USER_LEAVE);
	m_bMyPhase = false;
}


// 제거 예정
//void UIPhaseSelectServer::OnReceiveServerList( void)
//{	
//}


void UIPhaseSelectServer::ServerPasswordCallback(bool OK, const i3::string & Password, void * pUserContext)
{
	UIPhaseSelectServer * pPhaseServerList = (UIPhaseSelectServer *)pUserContext;

	if (OK)
	{
		pPhaseServerList->SetServerPassword(Password);
		//pPhaseServerList->ForceEnterSelectServer();
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();
		pPhaseServerList->SetInputDisable( false);
	}
}

void UIPhaseSelectServer::OnChangeTab(SChannelType TabIndex)
{
	m_CurrentTab = TabIndex;
	m_SelectedServerIndex	= INVALID_SERVER_INDEX;

	UpdateServerSlotList();
	__AutoServerSelect();
}

void UIPhaseSelectServer::SetButtonState( void)
{
	GetMainFrame()->DisableFloatWin(UFW_CHANNEL_LIST);

	LuaState * L = _CallLuaFunction( "SetButtonState");
	if( L != nullptr)
	{
		if( m_SelectedServerIndex != INVALID_SERVER_INDEX)
			i3Lua::PushInteger( L, 1);
		else
			i3Lua::PushInteger( L, 0);
		i3Lua::PushInteger( L, 1);
		_EndLuaFunction( L, 2);
	}
}

void UIPhaseSelectServer::UpdateServerTab()
{
	I3ASSERT( g_pConfigEx->GetGUI().pServerListTab != nullptr );
	const ConfigGUI::ServerTypeIndexContainer	& ServerTabList = (*g_pConfigEx->GetGUI().pServerListTab);

	i3::stack_wstring wstrServerTabText;

	// 현재 서버타입별 서버가 얼마 있는거 계산한다.
	// 해당 타입에 존재하는 서버 개수가 0이라면 탭을 제거한다.
	for ( size_t i = 0 ; i < ServerTabList.size() ; ++i )
	{
		const SChannelType	CheckType = ServerTabList[i].first;

		if ( g_pServerInfo->GetServerTypeCount(CheckType) <= 0 )
			continue;

#ifdef A_LOT_OF_SERVERS
		if(CheckType == S_CHANNEL_TYPE_ALL) {
			continue;
		}
#endif

		m_TabCheck.push_back(CheckType);

		wstrServerTabText += __GetServerTabTypeText(CheckType);
		wstrServerTabText += L";";
		//if ( ValidIndex < MAX_SERVER_TAB)
		//{
		//	m_TabCheck[ValidIndex] = CheckType;

		//	LuaState * L = _CallLuaFunction( "SetServerName");
		//	if( L != nullptr)
		//	{
		//		i3Lua::PushInteger( L, ValidIndex);
		//		i3Lua::PushStringUTF16To8( L, __GetServerTabTypeText(CheckType));
		//		_EndLuaFunction( L, 2);
		//	}

		//	ValidIndex++;
		//}
	}

	if(wstrServerTabText.empty() == false)
	{
		LuaState * L = _CallLuaFunction("SetServerGroupList");
		if( L != nullptr)
		{
			i3Lua::PushStringUTF16To8( L, wstrServerTabText);
			i3Lua::PushInteger( L, (INT32)m_CurrentTab);
			_EndLuaFunction( L, 2);
		}
	}

	// 현재 유저가 선택한 탭이 의미없는 탭이라면 ALL탭을 선택하도록 한다.
	if ( g_pServerInfo->GetServerTypeCount( m_CurrentTab) <= 0 )
	{
#ifdef A_LOT_OF_SERVERS
		OnChangeTab(S_CHANNEL_TYPE_NORMAL);
#else
		OnChangeTab(S_CHANNEL_TYPE_ALL);
#endif
	}
}

void UIPhaseSelectServer::UpdateServerSlotList( void)
{
	INT32		ServerCount		= g_pServerInfo->GetServerTypeCount(static_cast<SChannelType>(m_CurrentTab));
	const i3::vector<CServerInfo*>&	pServerInfoList	= g_pServerInfo->GetServerInfoList(static_cast<SChannelType>(m_CurrentTab));

	//Clear ServerList
	LuaState * L = _CallLuaFunction( "ClearServerList");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, ServerCount);
		_EndLuaFunction( L, 1);
	}

	//Add Servers
	for( INT32 i = 0; i < ServerCount; i++)
	{
		const CServerInfo *	pInfo = pServerInfoList[i];
		I3ASSERT( pInfo != nullptr );

		// Title
		i3::stack_wstring wstrText;
		i3::sprintf( wstrText, L"%s", __GetServerTypeText( pInfo->GetType()));
	
		COLOR col;

		if(m_SelectedServerIndex == i)
			i3Color::Set( &col, (UINT8)255, 255, 255, 255);
		else
			i3Color::Set( &col, (UINT8)146, 172, 194, 255);

		__SetServerInfo( i, wstrText, &col/*, -1, userGauge*/);

		/*if( userGauge == -1)
		{
			L = _CallLuaFunction( "SetActivateSlot");
			i3Lua::PushInteger( L, i);
			i3Lua::PushBoolean( L, false);
			_EndLuaFunction( L, 2);
		}
		else
		{
			L = _CallLuaFunction( "SetActivateSlot");
			i3Lua::PushInteger( L, i);
			i3Lua::PushBoolean( L, true);
			_EndLuaFunction( L, 2);
		}*/
	}
}

void UIPhaseSelectServer::CbOpenURL(void *pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		SIZE32 Size;
		Size.h = 800;		Size.w = 800;
		i3::rc_wstring wstrURL(L"http://fcm.sdo.com/PTInfo/FillAdultInfo.aspx");
		GameUI::WebBox( GAME_RCSTRING("STR_WEB_POPUP_CAPTION"), Size, wstrURL);
	}
}

namespace inner
{
	HHOOK hHook = nullptr;

	LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) 
	{
		if(nCode == HCBT_ACTIVATE) //윈도우 활성화
		{
			int x,y;
			RECT rectForm, rectMsg;

			::GetWindowRect( ::GetForegroundWindow(), &rectForm);
			::GetWindowRect((HWND)wParam, &rectMsg);
			x = (rectForm.left + rectForm.right - rectMsg.right + rectMsg.left) / 2;
			y = (rectForm.top + rectForm.bottom - rectMsg.bottom + rectMsg.top) / 2;

			::SetWindowPos ((HWND)wParam, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			::UnhookWindowsHookEx( hHook );
		}

		return ::CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	int center_msg(const i3::wliteral_range& wMsgRng, const wchar_t *title, UINT32 style)
	{
		inner::hHook = ::SetWindowsHookEx(WH_CBT, inner::CBTProc, 0, ::GetCurrentThreadId() );
		return ::MessageBoxW(NULL, wMsgRng.c_str(), title, style);
	}
}

/*virtual*/ void UIPhaseSelectServer::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate(rDeltaSeconds);

	if( m_pInfoWaitIcon == nullptr)
	{
		if(m_bChinaMessage && m_bOnceChina)
		{
			m_bChinaMessage = false;
			m_bOnceChina = false;
		}

		if(m_bMemoryMessage && m_bOnceMemoryCheck)
		{
			char memStatus[1024];
			memset(memStatus, 0, 1024);

			MEMORYSTATUSEX state;
			state.dwLength =  sizeof(MEMORYSTATUSEX);

			::GlobalMemoryStatusEx(&state);
			
			UINT64 MyMemory = state.ullAvailPhys/1024;
			UINT64 MyVirtualMemory = state.ullAvailVirtual/1024;
			UINT64 MyPageFile = state.ullAvailPageFile/1024;

			i3::stack_string strMyMemory;		i3::itoa(MyMemory, strMyMemory);
			i3::stack_string strMyVirtualMemory;	i3::itoa(MyVirtualMemory, strMyVirtualMemory);
			i3::stack_string strMyPageFile;			i3::itoa(MyPageFile, strMyPageFile);
			
			I3PRINTLOG(I3LOG_DEFALUT, "Avail Physical Memory : %s kb, Virtual Memory : %s kb, Page File : %s kb", 
				strMyMemory.c_str(), strMyVirtualMemory.c_str(), strMyPageFile.c_str());

// 싱말&필리핀은 퍼블리셔의 요청에 의해 메모리 부족 팝업을 띄우지 않습니다.
#if !defined(LOCALE_SINGMAL) && !defined(LOCALE_PHILIPPINES)
			if( MyMemory < (1024 * 1024) ) //1gb
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_USER_WARNING_MEMORY") );
			}
#endif // LOCALE_SINGMAL && LOCALE_PHILIPPINES

#ifndef I3_DEBUG
			if( MyVirtualMemory < (1024 * 400) ) //400mb
			{
				if( inner::center_msg( GAME_RCSTRING("STR2_USER_WARNING_VIRTUAL_MEMORY"), L"Waring", MB_OK) == IDOK )
				{
					g_pFramework->GotoNextStage("Exit");
				}
			}

			if( MyPageFile < (1024 * 400) ) //400mb
			{
				if( inner::center_msg( GAME_RCSTRING("STR2_USER_WARNING_VIRTUAL_MEMORY"), L"Waring", MB_OK) == IDOK )
				{
					g_pFramework->GotoNextStage("Exit");
				}
			}
#endif

			m_bMemoryMessage = false;
			m_bOnceMemoryCheck = false;
		}
	}
}

/*virtual*/ bool UIPhaseSelectServer::OnInviteMove(void)
{
	if (CCommunityContext::i()->IsMovingToUser())
	{
		INT32 MoveServerIndex = static_cast<INT32>(CCommunityContext::i()->GetMoveTargetServer());
#ifdef A_LOT_OF_SERVERS
		UINT32 uiIndex = 0;
		SChannelType eType = S_CHANNEL_TYPE_UNDEFINED;

		g_pServerInfo->GetIndexByTypeAndType(MoveServerIndex, uiIndex, eType);

		if(eType == S_CHANNEL_TYPE_UNDEFINED) {
			I3ASSERT_0;
			return;
		}
		RequestChannelList(uiIndex);
#else
		RequestChannelList(MoveServerIndex);
#endif // A_LOT_OF_SERVERS

		return true;
	}

	return false;
}

void UIPhaseSelectServer::RequestChannelList(INT32 ServerIndex)
{

	// 이미 정보를 받는중이면 그냥 리턴한다
	//if(m_pMBInfoWait != nullptr)
		//return;

	if( m_pInfoWaitIcon != nullptr)
		return;

	ToggleTextColor(ServerIndex);

	if (m_SelectedServerIndex != ServerIndex)
	{
		// 로비 -> 서버리스트로 이동하는 케이스를 위해 SelectedServerIndex로 서버 인덱스를 비교,
		// 하지만 실제로 서버에 관련된 정보는 m_TargetServerIndex를 사용한다.
		m_SelectedServerIndex	= ServerIndex;
		m_TargetServerIndex		= ServerIndex;
		m_TargetServerTab		= GetCurrentTab();

		GetMainFrame()->DisableFloatWin( UFW_CHANNEL_LIST);

		m_bMyPhase = true;

		// 서버 정보의 첫번째 배열이 트랜스서버라 클라이언트에서는 카운팅을 -1 해주기 때문에
		// 서버 인덱스는 클라이언트가 1이 작으므로 동기화를 위해 서버인덱스를 + 1 해주고 요청한다.
		INT32 i32ServerIdx = ServerIndex + 1;
		I3TRACE("RequestChannelList() - EVENT_GET_CHANNEL_LIST\n");
		GameEventSender::i()->SetEvent(EVENT_GET_CHANNEL_LIST, &i32ServerIdx);

		SetInputDisable( true);
		m_bMyPhase = false;

		QuickJoinContext::i()->setServerIndex( ServerIndex);
	}
}

void UIPhaseSelectServer::WaitForCloseChannelList(bool bWait)
{
	if( m_pInfoWaitIcon)
	{
		GameUI::DisableLoadingIcon( m_pInfoWaitIcon);
		m_pInfoWaitIcon = nullptr;
	}

	if(m_bViewChannelList == true)
	{
		GetMainFrame()->EnableFloatWin(UFW_CHANNEL_LIST);
		m_bViewChannelList = false;
	}
}

void UIPhaseSelectServer::SendEventForChannel(INT32 event, const void * pArg1/* = nullptr*/, const void * pArg2/* = nullptr*/)
{
	m_bMyPhase = true;
	GameEventSender::i()->SetEventOnce(event, pArg1, pArg2);
	m_bMyPhase = false;

}

bool UIPhaseSelectServer::TryEnterServerChannel(INT32 nChannelList)
{
	bool oldPhase = m_bMyPhase;

	m_bMyPhase = true;
	UINT32 tabidx = __GetCurrnetTabIndex(m_TargetServerIndex);

	if (g_pServerInfo->GetGameServerIndex(tabidx, m_TargetServerTab) != g_pServerInfo->GetCurGameServerIndex())
	{
		g_pServerInfo->SetGameServer(tabidx, m_TargetServerTab);
		GameEventSender::i()->SetEvent(EVENT_CONNECT_SERVER_GAME);
//		SetInputDisable( true);
	}
	else
	{
		GameEventSender::i()->SetEvent(EVENT_SELECT_CHANNEL, &nChannelList);
		// 이사님 요청으로 실행인자 확인합니다.
		// 반드시 여기서 호출할 필요가 없기 때문에
		// 실행방법을 변경한다면 호출 위치를 변경하여도 무방합니다.
		GameEventSender::i()->SetEvent(EVENT_RUN_PARAM_CHECK);
	}

	m_bMyPhase = oldPhase;

	return true;
}

INT32 UIPhaseSelectServer::__GetCurrnetTabIndex( UINT32 targetIdx )
{
	INT32	ServerCount	= g_pServerInfo->GetServerTypeCount(static_cast<SChannelType>(m_CurrentTab));
	const i3::vector<CServerInfo*>&	pServerInfoList	= g_pServerInfo->GetServerInfoList(static_cast<SChannelType>(m_CurrentTab));

	INT32 i = 0;
	for(i = 0; i < ServerCount; i++)
	{
		if( targetIdx == pServerInfoList[i]->GetNumber())
			return i;
	}

	return -1;
}

void UIPhaseSelectServer::ToggleTextColor(INT32 nServerIdx)
{
	// 서버 인덱스가 서버 카운트보다 크면, 셀렉션된 슬롯의 Color를 초기화 후 리턴
	// 그 외에는 슬롯값에 따라 TextColor를 변경한다.
	INT32		ServerCount		= g_pServerInfo->GetServerTypeCount(static_cast<SChannelType>(m_CurrentTab));
	COLOR col;
	if(nServerIdx >= ServerCount)
	{
		i3Color::Set( &col, (UINT8)193, 168, 134, 255);
		SetCellTextColor(m_SelectedServerIndex, &col);
	}
	else
	{
		i3Color::Set( &col, (UINT8)193, 168, 134, 255);
		SetCellTextColor(m_SelectedServerIndex, &col);

		i3Color::Set( &col, (UINT8)255, 255, 255, 255);
		SetCellTextColor(nServerIdx, &col);
	}
}

void UIPhaseSelectServer::SetCellTextColor(INT32 nSlot, COLOR * pColor)
{
	if(nSlot == -1)
		return;

	LuaState * L = _CallLuaFunction("SetCellTextColor");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L , nSlot);
		i3Lua::PushColor(L , pColor);
		_EndLuaFunction(L , 5);		
	}
}

void UIPhaseSelectServer::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseSelectServer::ReceiveGameEvent_Connect_Server_Game( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		INT32 targetIdx = __GetCurrnetTabIndex(m_TargetServerIndex);
		const CServerInfo * pInfo = g_pServerInfo->GetServerInfo(targetIdx, m_TargetServerTab);
		
		if( pInfo == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN,  "Server Info nullptr!!");
			return;
		}

		if ( pInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			// nullptr문자 크기도 함께 보낸다.
			UINT8 Size = static_cast<UINT8>(m_ServerPassword.size()+1);
			GameEventSender::i()->SetEvent(EVENT_ENTER_PW, &Size, m_ServerPassword.c_str());
		}
		else
		{
			m_bMyPhase = true;
			GameEventSender::i()->SetEventOnce(EVENT_USER_LEAVE);
			DEV_LOG("Phase Server List - EVENT_USER_LEAVE Req [%d][%d] // EVENT_CONNECT_SERVER_GAME", targetIdx, m_CurrentTab);
		}

	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();

		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
		
		SetInputDisable( false);
	}
}

void UIPhaseSelectServer::ReceiveGameEvent_Connect_Server_Gateway( INT32 arg, const i3::user_data&)
{
}

void UIPhaseSelectServer::ReceiveGameEvent_GetInfo( INT32 arg, const i3::user_data&)
{
	if ( EV_FAILED(arg) )
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_GET_MYINFO"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}

	if( m_pInfoWaitIcon != nullptr)
	{
		GameUI::DisableLoadingIcon( m_pInfoWaitIcon);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_REMAIN_MONEY);
		m_pInfoWaitIcon = nullptr;
	}
}

void UIPhaseSelectServer::ReceiveGameEvent_User_Enter( INT32 arg, const i3::user_data&)
{
}

void UIPhaseSelectServer::ReceiveGameEvent_User_Leave( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		NetworkContext::i()->ChangeSocket();
		m_bMyPhase = true;
		GameEventSender::i()->SetEventOnce(EVENT_USER_ENTER);
		m_bMyPhase = false;
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_MOVE"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}
}

void UIPhaseSelectServer::ReceiveGameEvent_Destroyed_Network( INT32 arg, const i3::user_data&)
{
	DEV_LOG("Phase Server List - DESTROYED NETWORK");
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseSelectServer::ReceiveGameEvent_Enter_Password( INT32 arg, const i3::user_data&)
{
	if ( EV_SUCCESSED(arg))
	{
		GameEventSender::i()->SetEventOnce(EVENT_USER_LEAVE);
		m_bMyPhase = false;
		DEV_LOG("Phase Server List - EVENT_USER_LEAVE Req [%d][%d] // EVENT_ENTER_PW", m_TargetServerIndex, g_pServerInfo->GetCurGameServerType());
	}
	else
	{
		// 실패하면 붙어 있던 서버로 설정한다.
		g_pServerInfo->SetGameServer(m_PrevServerIndex, m_PrevServerTab);

		QPParam Param;

		Param.wstrMsg		= GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_PW_Q");
		Param.LimitCount	= NET_SERVER_PW-1;
		Param.OnlyNumber	= false;
		Param.ShowPassword	= false;
		Param.Callback		= ServerPasswordCallback;
		Param.pUserContext	= this;

		GetMainFrame()->TogglePopup( UPW_QUERY_PASSWORD, &Param);

		SetInputDisable( true);
	}
}

void UIPhaseSelectServer::ReceiveGameEvent_Get_ChannelList( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin(UFW_CHANNEL_LIST);
	SetInputDisable(false);
	m_bViewChannelList = true;
	m_bMyPhase = true;

	// 빠른입장 버튼 활성화
	if( LocaleValue::Enable("EnableQuickJoin") == true )
	{
		LuaState* L = _CallLuaFunction("ActivateQuickJoin");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, UserInfoContext::i()->HaveNickName());
			_EndLuaFunction(L, 1);
		}
	}
}

void UIPhaseSelectServer::ReceiveGameEvent_Join_Room( INT32 arg, const i3::user_data&)
{
	// 채널/서버 리스트에서 바로 레디룸으로 입장(로비 거치지 않음)
	g_pFramework->GetUIMainframe()->SetLockInput(false);
	i3UI::setDisableUIInput(false);

	I3TRACE( "-------------------------------- setDisableInput -> false\n\n");
	CCommunityContext::i()->CompleteMovingToUser();

	if( EV_SUCCESSED(arg))
	{
		I3ASSERT(QuickJoinContext::i()->getServerIndex() != -1);

		g_pServerInfo->SetCurGameServerType(static_cast<SChannelType>(QuickJoinContext::i()->getChannelType()));
		g_pServerInfo->SetCurChannelNumber(QuickJoinContext::i()->getChannelIndex());
		g_pServerInfo->SetGameServer(QuickJoinContext::i()->getServerIndex(), m_TargetServerTab);

		m_SelectedServerIndex = m_TargetServerIndex = QuickJoinContext::i()->getServerIndex();

		I3TRACE("[QUICKJOIN] : server(%d), channel(%d) \n", 
			QuickJoinContext::i()->getServerIndex(), QuickJoinContext::i()->getChannelIndex());

		LobbyContext::i()->InitLobbyData();

		UIMainFrame * pFrame = GetMainFrame();

		if (pFrame->IsOpenedPopup(UPW_QUICK_JOIN_SECOND_BEST))
			pFrame->ClosePopup(UPW_QUICK_JOIN_SECOND_BEST);

		if (pFrame->IsOpenedPopup(UPW_QUICK_JOIN))
			pFrame->ClosePopup(UPW_QUICK_JOIN);

		pFrame->MovePhase( UIPHASE_READY_ROOM);
	}
	else
	{
		i3UI::setDisableUIInput(false);
		m_bMyPhase = true;
	}
}

void UIPhaseSelectServer::__AutoServerSelect()
{
	INT32		ServerCount		= g_pServerInfo->GetServerTypeCount(static_cast<SChannelType>(m_CurrentTab));

	// server count 가 없을 경우 그냥 넘어갑니다.
	if( ServerCount < 1) return;
	
	const i3::vector<CServerInfo*>&	pServerInfoList	= g_pServerInfo->GetServerInfoList(static_cast<SChannelType>(m_CurrentTab));

	// 서버 맨 위딴부터 Active되어 있으면 클릭합니다.
	for(INT32 i = 0; i < ServerCount; i++)
	{
		CServerInfo * pServerInfo = pServerInfoList[i];
		if(pServerInfo == nullptr) return;

		if(pServerInfo->GetActive() == true)
		{
			RequestChannelList(pServerInfo->GetNumber());
			return;
		}
	}
}

void UIPhaseSelectServer::SelectGroupListItem( INT32 idx )
{
	INT32 tabSize = m_TabCheck.size();

	// 방어 코드
	if(idx > tabSize && idx < 0) return;

	OnChangeTab(m_TabCheck[idx]);
}

void UIPhaseSelectServer::ReceiveGameEvent_SetTab_AutoSelectServer( INT32 arg, const i3::user_data& )
{
	// Auto Server Select
	__AutoServerSelect();

	// Update Server Tab
	UpdateServerTab();
	OnChangeTab(m_CurrentTab);
}

void UIPhaseSelectServer::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}
