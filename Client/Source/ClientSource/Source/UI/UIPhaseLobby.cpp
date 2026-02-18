#include "pch.h"
#include "UIPhaseLobby.h"

#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UIPopupNickNameSetting.h"
#include "UIPopupQueryPassword.h"
#include "UIFloatChat.h"
#include "UIFloatMiniProfile.h"
#include "UIFloatUserList.h"
#include "UIFloatPlayTimeAlarm.h"
#include "UINewAndHotBanner.h"
#include "UIToolTipLobbyMapInfo.h"
#include "UIPopupAttendanceCheck.h"
#include "UICharaView.h"
#include "UIUtil.h"
#include "../CommunityContext.h"
#include "../GameGUI.h"
#include "../TextSet.h"
#include "IGShopContext.h"
#include "UIPhaseSubBase.h"
#include "MedalContext.h"
#include "Useable_Item_Def.h"

#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/compare/generic_icompare.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "InviteContext.h"
#include "AttendanceContext.h"
#include "ClanGameContext.h"
#include "AbuseContext.h"
#include "TwitchTVContext.h"

#include "../StageBattle/AIModeContext.h"
#include "../StageBattle/UserContext.h"
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"

#if legacy_gui_function
	#include "../GuiNotifyReceiver.h"
#endif

#include "GameLoginContext.h"
#include "KickContext.h"
#include "ChannelContext.h"
#include "UserInfoContext.h"
#include "LobbyContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "TutorialContext.h"
#include "../QuickJoinContext.h"

#include "ClientStageInfoUtil.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPhaseLobby_ClickButton( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->ClickButton( btn);
		return 0;
	}

	int UIPhaseLobby_JoinRoom( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));

		if( pThis != nullptr)
		{
			// 스크롤 좌표 보정
			idx += MAX(0, pThis->GetRoomListScrollPos() );
			pThis->EnterRoom( idx);
		}

		return 0;
	}

// TODO : 쓰이지 않는듯 하여 주석 처리 합니다...  나중에 유용성이 없다면 그때 삭제 (2014.09.19.수빈)
/*
	int UIPhaseLobby_CreateNickname( LuaState * L)
	{
		char strBuf[MAX_STRING_COUNT];

		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));

		if( pThis != nullptr)
		{
			if( i3Lua::GetStringArg( L, 0, strBuf, MAX_STRING_COUNT) )
			{	
				i3::rc_wstring	wstrNick;
				i3::mb_to_utf16(strBuf, wstrNick);		// MB인지 UTF8인지 확인해봐야 한다. (2014.07.18.수빈)

				// NickName
				if (false == IsValidNicknameLetterSize(wstrNick))
				{
					// ! praptor - 최소 글자수 포함되도록 내용 수정해야합니다.
					GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_LENGTH"), nullptr, MAKE_CALLBACK(UIPhaseLobby::CbSameNick), pThis);
					return 0;
				}

				if ( ! g_pFramework->CheckValidNickname(wstrNick) )
				{
					GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_NICK"), nullptr, MAKE_CALLBACK(UIPhaseLobby::CbSameNick), pThis);
					return 0;
				}

				GameLoginContext::i()->setUserNick(wstrNick);
				GameEventSender::i()->SetEvent(EVENT_CREATE_NICK_NAME);
			}
			else
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_EMPTY"), nullptr, MAKE_CALLBACK(UIPhaseLobby::CbSameNick), pThis);
			}
		}

		return 0;
	}
*/

	int UIPhaseLobby_SelectRoom( LuaState * L)
	{
		INT32 selectIdx = i3Lua::GetIntegerArg( L, 1);

		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
		{
			// 스크롤 좌표 보정
			selectIdx += MAX(0, pThis->GetRoomListScrollPos() );
			pThis->SelectRoom( selectIdx);
		}
		return 0;
	}

	int UIPhaseLobby_SortRoomList( LuaState * L)
	{
		INT32 nSortType = i3Lua::GetIntegerArg(L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->SortRoomList(nSortType);
		return 0;
	}

	int UIPhaseLobby_ViewAdmissionRoom( LuaState * L)
	{
		bool bViewAdmissionRoom = i3Lua::GetBooleanArg(L, 1) == true ? true : false;
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->SortAdmissionRoom(bViewAdmissionRoom);
		return 0;
	}
	int Clan_ClanLobby( LuaState* L)
	{
		g_pFramework->GetUIMainframe()->MovePhase( UIPHASE_CLANLOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(true);
		return 0;
	}
	int UIPhaseLobby_CouponInput(LuaState* L)
	{
		g_pFramework->GetUIMainframe()->TogglePopup(UPW_COUPON_INPUT);
		return 0;
	}

	int UIPhaseLobby_ScrollRoomList(LuaState * L)
	{
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->OnScrollRoomList();

		return 0;
	}

	int UIPhaseLobby_WheelRoomList(LuaState * L)
	{
		INT32 zDelta = i3Lua::GetIntegerArg( L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->OnWheelRoomList(zDelta);

		return 0;
	}

	int UIPhaseLobby_AttendanceCheck(LuaState * L)
	{
		INT32 EvtType = i3Lua::GetIntegerArg(L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->AttendanceOpenPopup(EvtType);

		return 0;
	}
}

LuaFuncReg	PhaseLobby_Glue[] =
{
	{"ClickButton",				UIPhaseLobby_ClickButton	},
	{"ClickJoinRoom",			UIPhaseLobby_JoinRoom		},
	{"SelectRoom",				UIPhaseLobby_SelectRoom		},
	{"SortRoomList",			UIPhaseLobby_SortRoomList	},
	{"ViewAdmissionRoom",		UIPhaseLobby_ViewAdmissionRoom},
	{"ClanLobby",				Clan_ClanLobby },
	{"CouponInput",				UIPhaseLobby_CouponInput	},
	{"ScrollRoomList",			UIPhaseLobby_ScrollRoomList },
	{"WheelRoomList",			UIPhaseLobby_WheelRoomList },
	{"AttendanceClick",			UIPhaseLobby_AttendanceCheck },
	{ nullptr,		nullptr}
};

//------------------------------------------------------------------------------//
//								Callback rutine									//
//------------------------------------------------------------------------------//
static void JoinRoomPasswordCallback( bool OK, const i3::string & Password, void * pUserContext)
{
	if ( OK )
	{
		I3ASSERT(Password.size() <= sizeof(char) * NET_ROOM_PW);

		if( Password.size() > 0)
		{
			std::size_t cnt = sizeof(char) * NET_ROOM_PW;
			if ( Password.size() < sizeof(char) * NET_ROOM_PW)
				cnt = Password.size();

			MyRoomInfoContext::i()->SetMyRoomPW( Password.c_str(), cnt );
		}
		else
		{
			MyRoomInfoContext::i()->ClearMyRoomPW();
		}

		UIPhaseLobby *	pLobby = (UIPhaseLobby *)pUserContext;

		// 성공시에만 Input을 막습니다.
		if( MyRoomInfoContext::i()->JoinRoom(&pLobby->GetJoinTargetRoom()) == true)
			i3UI::setDisableUIInput(true);
	}
}

//------------------------------------------------------------------------------//

bool UIPhaseLobby::sm_bAwayInput = false;
SORT_ORDER_INFO UIPhaseLobby::sm_SortOrder[MAX_SORT_TYPE] = 
{
	{ROOM_SORT_NUMBER,		false}, 
	{ROOM_SORT_ROOMNAME,	false}, 
	{ROOM_SORT_MODE,		false}, 
	{ROOM_SORT_MAP,			false}, 
	{ROOM_SORT_PERSON,		false},
	{ROOM_SORT_STATE,		false}
};
INT32 UIPhaseLobby::sm_nCurSortType = 0;

bool UIPhaseLobby::m_bPcCafeNotice = false;				// PcCafe 공지 창을 보여주는지에 대한 여부
bool UIPhaseLobby::m_bEventCardSetNotice = false;
bool UIPhaseLobby::m_bEvnetNotice = false;


I3_CLASS_INSTANCE( UIPhaseLobby);//, UIPhaseBase);

UIPhaseLobby::UIPhaseLobby() : m_bNickNameCreate(false), m_bAllowRefresh(false), m_bRefreshButtonActive(false)
	, m_rEnterLobbyWaitTime(0.f), m_pKickedMassageBox(nullptr), m_pWaitRecevingRoomList(nullptr)
	, m_bCheckInvite(false), m_bEnableNicknameMsgBox(false), m_bEnableCreateRoomMsgBox(false)
	, m_SelectedElement(0), m_fRefreshElapsedTime(0.f), m_bFirstLogin(false), m_bLogin(false), m_bRequestRoomInfoAdd(false)
	, m_LastRefreshTime(0.f), m_pAbusingBox(nullptr), m_bWaitingFromServer(false), m_bViewAdmissionRoom(false)
	, m_pTTMapInfo(nullptr), m_bTutorialEnable(false)
{
	
	ResetInviteInfo(m_InviteInfo);

	i3mem::FillZero( &m_JoinTargetRoom, sizeof( m_JoinTargetRoom));

	RegisterGameEventFunc( EVENT_READY_BATTLE,				&UIPhaseLobby::ReceiveGameEvent_Ready_Battle);
	RegisterGameEventFunc( EVENT_JOIN_QUICK,				&UIPhaseLobby::ReceiveGameEvent_Join_Quick);
	RegisterGameEventFunc( EVENT_CREATE_ROOM,				&UIPhaseLobby::ReceiveGameEvent_Create_Room);
	RegisterGameEventFunc( EVENT_JOIN_ROOM,					&UIPhaseLobby::ReceiveGameEvent_Join_Room);
	RegisterGameEventFunc( EVENT_LEAVE_LOBBY,				&UIPhaseLobby::ReceiveGameEvent_Leave_Lobby);
	RegisterGameEventFunc( EVENT_GET_ROOM_ADDINFO_N,		&UIPhaseLobby::ReceiveGameEvent_Get_RoomAddInfo);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,		&UIPhaseLobby::ReceiveGameEvent_DestroyedNetwork);
	RegisterGameEventFunc( EVENT_UPDATE_USER_LIST,			&UIPhaseLobby::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_READYBOX_MESSAGE,			&UIPhaseLobby::ReceiveGameEvent_Ready_Box_Message);
	RegisterGameEventFunc( EVENT_ATTENDANCE_CLEAR,			&UIPhaseLobby::ReceiveGameEvent_Attendance_Clear);
	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,			&UIPhaseLobby::ReceiveGameEvent_PlayTimeAlarm);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,		&UIPhaseLobby::ReceiveGameEvent_ShutDownTimeAlarm);

	m_bEnableUsingClanChannel = true;
}

UIPhaseLobby::~UIPhaseLobby()
{
	i3::cu::for_each_SafeFree(m_RoomList);

	m_pTTMapInfo->Destroy();
}

void UIPhaseLobby::__AutoRefreshRoomList( REAL32 rDeltaSeconds)
{
	// 현재 스텝이 STEP_nullptr이 아니면 방리스트를 보고 있는상태가 아니므로
	// 방리스트를 요청하지 않도록 한다.
	// 스텝, 페이지구분 등 명확히 안되어 있어서 이런식으로 코딩할 수 밖에 없다.
	if( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE)
		return;

	m_fRefreshElapsedTime -= rDeltaSeconds;

	if( m_fRefreshElapsedTime <= 0.0f)
	{		
#if defined(I3_DEBUG) && 0
		{
			static UINT32 a = 0;
			I3TRACE1("[%d]Refresh Room List....\n", a++);
		}
#endif // _DEBUG

		GameEventSender::i()->SetEvent(EVENT_LOBBY_REFRESH_N);
		m_fRefreshElapsedTime = g_pConfigEx->GetNetwork().RefreshInterval_RoomList;
		m_bRefreshButtonActive = false;
	}
}

void UIPhaseLobby::__ProcessAbusing( REAL32 rDeltaSeconds)
{
	if( m_pAbusingBox != nullptr)
	{
		m_rAbuseTime += rDeltaSeconds;

		if( (UINT32)m_rAbuseTime > AbuseContext::i()->GetAbuseTime())
		{
			GameUI::CloseMsgBox( m_pAbusingBox);
			m_pAbusingBox = nullptr;
		}
	}
}

void UIPhaseLobby::_SetButtonState( void)
{
	LuaState * L = _CallLuaFunction("SetButtonState");
	
	if( L != nullptr)
	{	
		static const bool bEnableTutorial = true;
		i3Lua::PushBoolean(L , bEnableTutorial);
		i3Lua::PushBoolean(L, m_bViewAdmissionRoom);
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L , 3);
	}
}

void UIPhaseLobby::_SetCurSelectItem( void)
{
	if(m_SelectedElement < 0)
		return;

	INT32 i32Selidx = m_SelectedElement - MAX(0, GetRoomListScrollPos() );

	LuaState * L = _CallLuaFunction("SetCurSelectItem");

	if( L != nullptr)
	{	
		i3Lua::PushInteger(L, i32Selidx);
		_EndLuaFunction(L, 1);		
	}
}

void UIPhaseLobby::_InitRoomList(void)
{
	LuaState * L = _CallLuaFunction("InitRoomList");
	if(L != nullptr)
		_EndLuaFunction(L , 0);

	// Init Room List Scroll
	{
		m_pScrollRoomList->setTickSize(1);
		m_pScrollRoomList->setPageSize(UI_LOBBY_ROOMLIST_VIEWCOUNT);
		m_pScrollRoomList->setScrollPos(0);
		m_pScrollRoomList->setScrollRange(0, m_RoomList.size());
	}
}

void UIPhaseLobby::_SetRoomList( void)
{
	// 출력되는 영역에 있다면, 비밀방, 핑 표시
	bool bEnterRoom;

	for( size_t idx = 0 ; idx < UI_LOBBY_ROOMLIST_VIEWCOUNT ; ++idx)
	{
		UINT32 u32FindIdx = idx + MAX(0, m_pScrollRoomList->getScrollPos());

		if( u32FindIdx >= m_RoomList.size() )
		{
			_SetEmptyRoomInfo( idx);
			continue;
		}

		LOBBY_ROOM_INFO * pInfo = m_RoomList[ u32FindIdx ];
		if (ROOM_STATE_EMPTY == pInfo->_RoomInfo._State)
		{
			_SetEmptyRoomInfo( idx);
			continue;
		}

		ROOM_INFO_BASIC* pRoomInfo = &pInfo->_RoomInfo;

		bEnterRoom = true;

		if( pRoomInfo->_PersonNow >= pRoomInfo->_PersonPlayerMax)			// 만땅
			bEnterRoom = false;
		else if( (GET_ROOM_INFO_INTERENTER( pRoomInfo->_InfoFlag) > 0) &&
			(pRoomInfo->_State != ROOM_STATE_READY))						//	난입 제한, 플레이중.
			bEnterRoom = false;

		_SetRoomInfo(idx, pRoomInfo->_Idx, pRoomInfo);

		//	입장 할 수 여부에 따라 방 리스트의 색상을 결정
		if( bEnterRoom)
			_RestoreRoomColor( idx);				//GCT_VALID_C
		else
			_SetRoomColor( idx, GameGUI::GetColor( GCT_INVALID_C));
	}

	m_pScrollRoomList->setScrollRange(0, m_RoomList.size());

	INT32 i32NewPos = GetRoomListScrollPos();
	i32NewPos = MINMAX( 0, i32NewPos, (INT32)m_RoomList.size() );
	m_pScrollRoomList->setScrollPos( i32NewPos );
}

void UIPhaseLobby::_BuildRoomListDummy(void)
{
#if defined (TEST_LOBBY_ROOM_LIST)
	INT32 i;

	static bool BuildedDummyRoomList = false;
	static INT32 g_nRoomInfoDummyCount = 100;

	if( BuildedDummyRoomList)		return;

	BuildedDummyRoomList = true;
	
	for( i=0; i<g_nRoomInfoDummyCount; i++)
	{
		LOBBY_ROOM_INFO * pDummy = (LOBBY_ROOM_INFO*) i3MemAlloc( sizeof( LOBBY_ROOM_INFO));

		i3mem::FillZero( pDummy, sizeof( ROOM_INFO_BASIC));

		pDummy->_RoomInfo._Idx = i;
		pDummy->_RoomInfo._State = (UINT8) (i3Math::Randf() * (REAL32)(ROOM_STATE_BATTLE_END + 1));

		// EXPORT_AVALIABLE_CODE_VALUE
		while(pDummy->_RoomInfo._StageID == 0)
		{
			STAGE_ORDINAL_ID ord = (STAGE_ORDINAL_ID)(i3Math::Rand() % (STAGE_ORD_MAX - 1));
			ord = (STAGE_ORDINAL_ID)(ord == 0 ? ord+1 : ord);

			pDummy->_RoomInfo._StageID = getStageId( ord);
		}


		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_RoomInfo._StageMask |= STAGE_MASK_TYPE_CLANMATCH;
		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_RoomInfo._StageMask |= STAGE_MASK_TYPE_CHALLENGE;
		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_RoomInfo._StageMask |= STAGE_MASK_TYPE_HEADHUNTER;
		
		pDummy->_RoomInfo._PersonPlayerMax = (UINT8) (i3Math::Randf() * 16.0f);
		pDummy->_RoomInfo._PersonPlayerMax = MINMAX( 2, pDummy->_RoomInfo._PersonPlayerMax, 16);
		pDummy->_RoomInfo._PersonNow = (UINT8) (i3Math::Randf() * (REAL32) pDummy->_RoomInfo._PersonPlayerMax);
		pDummy->_RoomInfo._PersonNow = MINMAX( 1, pDummy->_RoomInfo._PersonNow, pDummy->_RoomInfo._PersonPlayerMax);
		pDummy->_RoomInfo._Ping = (UINT8) (i3Math::Randf() * 5.f) + 1;
		pDummy->_RoomInfo._Ping = MINMAX( 1, pDummy->_RoomInfo._Ping, 5);
		pDummy->_RoomInfo._WeaponFlag = 
							((UINT8) (i3Math::Randf() * 2.0)) << 5 |
							((UINT8) (i3Math::Randf() * 2.0)) << 4 |
							((UINT8) (0x00000001)) << 3 |
							((UINT8) (0x00000001)) << 2 |
							((UINT8) (i3Math::Randf() * 2.0)) << 1 |
							((UINT8) (i3Math::Randf() * 2.0));

		i3::snprintf( pDummy->_RoomInfo._Title, NET_ROOM_NAME_SIZE, _TT("DUMMY_STAGE_%d"), i);
		m_RoomList.push_back(pDummy);
	}

#endif
}

void UIPhaseLobby::_UpdateRoomList(void)
{
	if( IsReadyToUpdate() == false)return;


#if defined( TEST_LOBBY_ROOM_LIST)
	_BuildRoomListDummy();
#else
	INT32 i;
	// Empty All
	_EmptyRoomList();

	// Find View
	INT32 nRoomCount = LobbyContext::i()->getLobbyRoomCount();

	for( i = 0; i < nRoomCount; i++)
	{
		const ROOM_INFO_BASIC * pRoomInfo = LobbyContext::i()->getRoomInfo(i);
		
		if ( ! pRoomInfo)	
			continue;

		// PersonNow가 0이면 깨진 방
		if (0 == pRoomInfo->_PersonNow)
			continue;

		LOBBY_ROOM_INFO* pRoom = nullptr;

		// 변경된 정보를 확인
		bool bModify = false;
		
		for(size_t j = 0; j < m_RoomList.size(); j++)
		{
			pRoom = m_RoomList[ j];
			
			if (pRoomInfo->_Idx == pRoom->_RoomInfo._Idx)
			{
				if (ROOM_STATE_EMPTY != pRoom->_RoomInfo._State)
				{
					pRoom->_bModify = false;
					bModify = true;
					break;
				}
			}
		}

		if( bModify)
		{	// 이미 있다면 변경된 것을 반영
			// Modify
			if( (pRoom->_RoomInfo._InfoFlag != pRoomInfo->_InfoFlag) ||
				(pRoom->_RoomInfo._PersonNow != pRoomInfo->_PersonNow) ||
				(pRoom->_RoomInfo._PersonPlayerMax != pRoomInfo->_PersonPlayerMax) ||
				(pRoom->_RoomInfo._Ping != pRoomInfo->_Ping) ||
				(pRoom->_RoomInfo._StageID != pRoomInfo->_StageID) ||
				(pRoom->_RoomInfo._State != pRoomInfo->_State) ||
				(i3::generic_is_iequal( pRoom->_RoomInfo._Title, pRoomInfo->_Title) == false) ||
				(pRoom->_RoomInfo._WeaponFlag != pRoomInfo->_WeaponFlag) )
			{
				pRoom->_bModify = true;

				i3mem::Copy( &pRoom->_RoomInfo, pRoomInfo, sizeof(ROOM_INFO_BASIC));
			}
		}
		else
		{
			// AddTail
			LOBBY_ROOM_INFO* pNewRoom = (LOBBY_ROOM_INFO*)i3MemAlloc(sizeof(LOBBY_ROOM_INFO));
			i3mem::Copy( &pNewRoom->_RoomInfo, pRoomInfo, sizeof(ROOM_INFO_BASIC));
			pNewRoom->_bModify = true;
			m_RoomList.push_back(pNewRoom);
		}
	}
#endif

	_SetRoomList();
}

void UIPhaseLobby::_ProcessFirstLogin(void)
{	
	if( CCharaInfoContext::i()->HasBasicChara() )
	{
		_FirstLoginPopup();
	}

#if defined(USE_LINK_SHOP)
	//상점바로가기 기능 관련 시간 저장
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	
	i3::sprintf(g_pEnvSet->m_strDay, L"%dMon%dDay",sysTime.wMonth,sysTime.wDay);

	//현재 화면 모드 정보를 설정한뒤 날짜를 저장합니다.
	g_pEnvSet->m_bFullScreen = g_pViewer->isFullScreen();
	g_pEnvSet->Save();
#endif
	
	GameLoginContext::i()->ResetFirstLogin();
}

void UIPhaseLobby::_ProcessCheckInvite(REAL32 rDeltaSeconds)
{
	
	UIPhaseSubBase*	_SubPhase = (UIPhaseSubBase*)GetMainFrame()->GetCurrentSubPhase();

	if( IsReadyToUpdate() == false)return;
	if( _SubPhase != nullptr )		return;

	// 초대 메시지 검사
	if (InviteContext::i()->PopInviteInfo(&m_InviteInfo))
	{	
		bool bPass = false;

		// 차단 유저이면 무시한다.

		for(INT32 i=0; i < CCommunityContext::i()->GetBlockUserCount(); i++)
		{
			const BLOCK_USER * t_User = CCommunityContext::i()->GetBlockUser(i);
			if(i3::generic_is_iequal(t_User->_wstrNick, m_InviteInfo._wstr_nick) )
				return;
		}

		// 옵션에 따라 초대하기 창을 활성/비활성 상태를 결정한다.
		switch( g_pEnvSet->m_nInviteType)
		{
		case COMMUNITY_PERMISSION_ALL		: bPass = true;			break;		// 전체 허용	
		case COMMUNITY_PERMISSION_NO		: bPass = false;		break;		// 전체 차단	
		case COMMUNITY_PERMISSION_FRIEND	: bPass = false;		break;		// 친구/클랜 허용	
		default:bPass = true;		break;			
		}

		// 친구/클랜 허용상태라면, 조건을 검색합니다.
		if( g_pEnvSet->m_nInviteType == COMMUNITY_PERMISSION_FRIEND)
		{
			INT32 i = 0;
			INT32 nFriendCount = CCommunityContext::i()->GetFriendCount();
			for( i=0; i<nFriendCount; i++)
			{
				const FRIEND_NODE* pNode = CCommunityContext::i()->GetFriend(i);
				if(pNode->_uid == m_InviteInfo._iUID)
				{
					bPass = true;
					break;
				}
			}

			if( !bPass)
			{
				INT32 nClanMemberCount = CCommunityContext::i()->GetClanMemberCount();
				for( i=0; i<nClanMemberCount; i++)
				{
					const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetClanMember(i);
					if(pNode->_uid == m_InviteInfo._iUID)
					{
						bPass = true;
						break;
					}
				}
			}
		}
		
		if( bPass)
		{
			// 한번에 하나씩 처리하기위한 메시지 검사 중지
			m_bCheckInvite = false;

			i3::rc_wstring wstrBuf;

			// 수락 여부 확인		
			i3::sprintf(wstrBuf, GAME_RCSTRING("STR_TBL_GUI_LOBBY_INVITE_MESSAGE1"), m_InviteInfo._idxRoom + 1, m_InviteInfo._wstr_nick);

		//	const char * str = GAME_RCSTRING("STR_TBL_GUI_LOBBY_INVITE_MESSAGE1");		/*%d번 방의 %s님으로 부터 초대 요청 메시지가 *//*도착하였습니다.\n수락 하시겠습니까?*/
		//	std::string fmt = BOOST_FORMAT2( str, m_InviteInfo._idxRoom + 1, m_InviteInfo._nick);
		//	i3::safe_string_copy( strBuf, fmt.c_str(), MAX_STRING_COUNT );
			
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrBuf, nullptr, MAKE_CALLBACK(CbBeInvited), this);
		}
	}
}

void UIPhaseLobby::_ProcessCommon( REAL32 rDeltaSeconds)
{
	
	// 초대 메시지 검사
	if( m_bCheckInvite)
	{
		_ProcessCheckInvite(rDeltaSeconds);
	}

	// 진급 포인트 시스템 - 하사이상 초기 보너스
	// 레벨업 체크
	if (UserInfoContext::i()->IsInitialRankUp())
	{
		i3::rc_wstring wstrMessage;
		i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_UPGRADE_CHARA"), g_InitialRankUpPoint);/*축하 드립니다!!\n \n진급 포인트 지급 이벤트로 {col:255,0,0,255}%d 포인트{/col}를\n진급 포인트로 지급합니다.\n \n(이후부터는 진급 시 포인트를 획득합니다.)*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
	}	

	if (UserInfoContext::i()->IsGeneralRankUp())
	{
		UINT32 point = UserInfoContext::i()->GetRankUpPoint();

		i3::rc_wstring wstrMessage;
		i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_GENERAL_RANK_UP"), point);/*진급을 축하 드립니다!!\n \n({col:255,0,0,255}%d 포인트{/col}를\n진급 포인트로 지급합니다.)*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
		
		UserInfoContext::i()->ResetRankUp();
		UserInfoContext::i()->SetGeneralRankUpFlag(false);
	}

	if (UserInfoContext::i()->isEventReawardFlage())
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));
		UserInfoContext::i()->SetEventReawardFlag(false);
	}
}

void UIPhaseLobby::_SetRoomInfo( INT32 idx, INT32 RoomIdx, ROOM_INFO_BASIC* pInfo)
{
	LuaState * L;

	// 방 번호
	i3::stack_wstring wstrNumber;
	i3::sprintf( wstrNumber, L"%03d", pInfo->_Idx + 1);

	// 게임모드
	UI_MODE_TYPE mode = GameUI::GetStageModeType( CStageInfoUtil::GetStageData(pInfo->_StageID), pInfo->_IsClanMatch );

	// 맵정보
	i3::stack_wstring wstr_mapNameText;
	INT32 domi_round_shape = -1;
	{
		{

			if (GET_ROOM_INFO_RANDOM_MODE( pInfo->_InfoFlag ))
			{
				wstr_mapNameText = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
			}
			else
			{
				if ( CStageInfoUtil::GetStageData(pInfo->_StageID)->m_ui8Mark == GOODS_MARK_NEW )
				{
					wstr_mapNameText = GAME_RCSTRING("STR_TBL_GUI_LOBBY_NEWMAP_PREFIX");
				}

				i3::wstring wstrStageName = CStageInfoUtil::GetMapData((UINT32)pInfo->_StageID)->getDisplayName();
				i3::generic_string_cat(wstr_mapNameText, wstrStageName);
			}
		}
	}

	// 상태
	UI_LOBBY_ROOM_STATE state = ULRS_ADMISSION;
	i3::rc_wstring wstrState;

	//제압 미션 난이도
	INT32 nDifficulty = -1;

	// 게임 상태에 따라 "게임중" or "대기중" 표시
	if (ROOM_STATE_READY != pInfo->_State)
	{
		// 게임중일때 난입제한 or 방 인원이 가득차면 입장 불가 처리
		if( GET_ROOM_INFO_INTERENTER(pInfo->_InfoFlag) > 0 ||
			pInfo->_PersonNow == pInfo->_PersonPlayerMax)
		{
			state = ULRS_NO_ADMISSION;
			wstrState = GAME_RCSTRING("STR_UI_CANT_ENTER");
		}
		else
		{
			state = ULRS_PLAY;
			wstrState = GAME_RCSTRING("STBL_IDX_ROOM_STATE_PLAY");
			
			//제압 미션 게임 중 일 경우에만 난이도 표시
			if( mode == UMT_DOMINATION)
			{
				nDifficulty = 0;
				wstrState.clear();
			}
		}
	}
	else
	{
		// 대기중일때 방 인원이 가득 차면 입장 불가 처리
		if( pInfo->_PersonNow == pInfo->_PersonPlayerMax)
		{
			state = ULRS_NO_ADMISSION;
			wstrState = GAME_RCSTRING("STR_UI_CANT_ENTER");
		}
		else
		{
			state = ULRS_ADMISSION;
			wstrState = GAME_RCSTRING("STBL_IDX_ROOM_STATE_WAIT");
		}
	}	

	// 사용자
	i3::stack_wstring wstrPerson;
	i3::sprintf( wstrPerson, L"%02d/%02d", pInfo->_PersonNow, pInfo->_PersonPlayerMax);

	// 비밀방
	INT32 secret = GET_ROOM_INFO_PRIVATE_MODE( pInfo->_InfoFlag )?0:-1;

	//	공수 교대 & 무기 제한 여부
	INT32 mode_option = GameUI::GetModeOptionShapeIndex(*pInfo);

	//	리얼 IP가 아니면 마크 표시
	bool bRealIP = false;

	//	리얼 IP면 느낌표 마크가 표시 안된다.
	bRealIP = pInfo->_InfoFlag ? true : false;

	L = _CallLuaFunction( "SetRoomInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		i3Lua::PushStringUTF16To8( L, wstrNumber);			// 방번호
		i3Lua::PushInteger( L, secret);						// 비밀방표시
		i3Lua::PushStringUTF16To8(L, pInfo->_Title);		// 방제목
		i3Lua::PushInteger( L, mode_option);				// 공수 교대 & 무기 제한 여부
		i3Lua::PushInteger( L, mode);						// 모드
		
		i3Lua::PushInteger( L, domi_round_shape);			// 제압 라운드 Icon
		i3Lua::PushStringUTF16To8( L, wstr_mapNameText);	// 맵 이름
		
		i3Lua::PushStringUTF16To8( L, wstrPerson);		// 사용자 수
		i3Lua::PushInteger( L, state);				// 상태(이미지)
		i3Lua::PushStringUTF16To8( L, wstrState);		// 상태

		i3Lua::PushInteger( L, nDifficulty);		// 제압 난이도
		_EndLuaFunction( L, 12);
	}
}

void UIPhaseLobby::_SetEmptyRoomInfo( INT32 idx)
{
	LuaState * L;

	const i3::wliteral_range wEmptyRng = L""; 

	L = _CallLuaFunction( "SetRoomInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// 방번호
		i3Lua::PushInteger( L, -1);				// 비밀방표시
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// 방제목
		i3Lua::PushInteger( L, -1);				// 제한 여부
		i3Lua::PushInteger( L, -1);				// 모드
		i3Lua::PushInteger( L, -1);				// 제압 라운드 Icon
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// 맵 이름
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// 사용자 수
		i3Lua::PushInteger( L, -1);				// 상태(이미지)
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// 상태
		
		i3Lua::PushInteger( L, -1);				// 제압 난이도
		_EndLuaFunction( L, 12);
	}
}

void UIPhaseLobby::_SetRoomColor( INT32 RoomIdx, I3COLOR * pColor)
{
	LuaState * L = _CallLuaFunction( "SetRoomColor");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, RoomIdx);
		i3Lua::PushInteger( L, i3Color::GetR( pColor));
		i3Lua::PushInteger( L, i3Color::GetG( pColor));
		i3Lua::PushInteger( L, i3Color::GetB( pColor));
		i3Lua::PushInteger( L, i3Color::GetA( pColor));
		_EndLuaFunction( L, 5);
	}
}

void	UIPhaseLobby::_RestoreRoomColor(INT32 RoomIdx)
{
	LuaState * L = _CallLuaFunction( "RestoreRoomColor");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, RoomIdx);
		_EndLuaFunction( L, 1);
	}	
}

void UIPhaseLobby::_FirstLoginPopup()
{
	if( g_pEnvSet->m_bWrongEnvSet)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_DUPLICATED_KEY"));/*중복된 키 설정이 있어 키설정을 초기화 하였습니다*/
	}

	// 팝업 체인: 쿠폰팝업 -> PC방 팝업
	const COUPON_INFO* info = LobbyContext::i()->getCouponInfo();

	if ( 0 < info->_count)
	{
		i3::rc_wstring wstr_couponMessage;

		/*%d개의 쿠폰이 사용되었습니다.\n \n지급 내역\n \n　　　　　　　　　　　　%d 포인트*/
		i3::sprintf(wstr_couponMessage, GAME_RCSTRING("STR_TBL_GUI_LOBBY_CUPON_MESSAGE"), info->_count,info->_point);
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_couponMessage, nullptr, MAKE_CALLBACK(CbSuppliedConfirm), this);
		LobbyContext::i()->clearCouponInfo();
		return;
	}

#if defined(USE_LINK_SHOP )
	/*//팝업 : 상점바로가기 : 로컬타임과 EnySet에 저장된 날짜를 비교합니다. 
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	char szTemp[128];
	if((i3String::Compare(g_pEnvSet->m_szDay,szTemp)!=0) && (InfoEquipWeapon._noprim == 0) )
	{
		if(g_pViewer->isFullScreen()&&i3String::Contain(g_pEnvSet->m_szDay,szTemp)!=-1 )
		{	//풀스크린모드시 m_szDay에 저장하는 값이 달라지므로 Contain으로 한번더 검사합니다.
		}
		else
		{
			//상점 바로가기 기능 
			GetMainFrame()->TogglePopup( UPW_LINK_SHOP, (void*) 1));
		}
		return;
	}*/
#endif
	if (!m_bPcCafeNotice)
		ShowPcCafePopup();

	// Key : 출석 체크 추가
	if( AttendanceContext::i()->IsAttendanceEvent() )
	{
		ShowAttendancePopup();
	}

	ShowNoticePopup();

	if(!TwitchTVContext::i()->isMsgBox())
	{
		if(!TwitchTVContext::i()->isFastStreamingStop())
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,GAME_RCSTRING("STR_POPUP_TWITCH_EXCEPTION_EXIT"),&GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"),MAKE_CALLBACK(TwitchTVContext::i()->CbFastStopStreaming),this);
	}

	// 만우절 위장 계급 팝업
	if (UserInfoContext::i()->IsAprilFoolsDay())
	{
		USER_INFO_INVITEM_DATA myUserInfoInvItemData;

		UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

		INT32 itemID = CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL_CLIENT;
		USE_ITEM_POPUP_TYPE pType = USE_ITEM_DISGUISE_RANK;

		if (myUserInfoInvItemData._wAprilFoolRank == RANK_NONE)
		{
			GetMainFrame()->OpenPopup(UPW_DISGUISE_RANK, &itemID, &pType);
		}
	}
}

void UIPhaseLobby::_CloseAllSubPhaseAndPopup( void)
{
	UIMainFrame * pFrame = GetMainFrame();
	
	// 메세지박스도 깨끗이 정리한다..(11.11.01.수빈)
	//GameUI::CloseAllMsgBox();
	pFrame->TogglePopup( UPW_NULL);
	
	if (USP_NONE != pFrame->GetCurrentSubPhaseType())
	{
		pFrame->ChangeSubPhase( pFrame->GetCurrentSubPhaseType());
	}
}

void UIPhaseLobby::_ClearRoomInfo( void)
{
	// Lobby Banner는 아직 미구현..
	//GetMainFrame()->EnableFloatWin( UFW_LOBBYBANNER);
	GetMainFrame()->EnableFloatWin( UFW_CHARACTER);
	
	DisableRoomTooltip();

	m_SelectedElement = -1;
}

void UIPhaseLobby::_AddMsgBoxToTopMenuScreen(void)
{
	if( m_pKickedMassageBox != nullptr)
	{
		// 최상위로 그리도록 한다.
		i3UIScene * pScene = m_pKickedMassageBox->GetScene(0);

		if( pScene != nullptr && pScene->getParent())
		{
			pScene->ChangeParent( pScene->getParent());
		}	
	}
}

bool UIPhaseLobby::CompareRoom( const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2)
{
	const ROOM_INFO_BASIC* pRoom[2];

	pRoom[0] = &pItem1->_RoomInfo;
	pRoom[1] = &pItem2->_RoomInfo;

	INT32 nResult = 0;
	
	switch( sm_SortOrder[sm_nCurSortType]._nType)
	{		
	case ROOM_SORT_NUMBER:
		{
			// 방 번호로 정렬
			nResult = SortRoom::SortbyNumber(pRoom[0], pRoom[1]);
		}												
		break;	
	case ROOM_SORT_ROOMNAME:
		{
			// 방제 순으로 정렬
			nResult = SortRoom::SortbyRoomName(pRoom[0], pRoom[1]);
		}												
		break;	
	case ROOM_SORT_MODE:
		{
			// 모드별 정렬
			nResult = SortRoom::SortbyMode(pRoom[0], pRoom[1]);
		}
		break;
	case ROOM_SORT_MAP:
		{
			// 스테이지 이름 순으로 정렬
			nResult = SortRoom::SortbyStageName(pRoom[0], pRoom[1]);
		}
		break;
	case ROOM_SORT_STATE:
		{
			// 게임 상태 순으로 정렬
			nResult = SortRoom::SortbyRoomState(pRoom[0], pRoom[1]);
		}

		break;
	case ROOM_SORT_PERSON:
		{
			// 인원 수 순으로 정렬
			nResult = SortRoom::SortbyPerson(pRoom[0], pRoom[1]);
		}
		break;
	default:
		nResult = 0;
		break;
	}

	if( sm_SortOrder[sm_nCurSortType]._bAscending == false )
	{
		nResult = -nResult;
	}

	return nResult < 0;
}


bool UIPhaseLobby::CompareAdmissionRoom( const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2)
{
	const ROOM_INFO_BASIC* pRoom[2];

	pRoom[0] = &pItem1->_RoomInfo;
	pRoom[1] = &pItem2->_RoomInfo;

	INT32 nResult = 0;

	INT32 nCompareValue[2] = {0};

	// 입장 가능/불가능을 체크,
	// 방에 빈 슬롯은 최대 15이기 때문에 16값으로 플래그를 대체하여 계산합니다.
	if((pRoom[0]->_State != ROOM_STATE_READY && GET_ROOM_INFO_INTERENTER(pRoom[0]->_InfoFlag) > 0) ||
		pRoom[0]->_PersonNow == pRoom[0]->_PersonPlayerMax)
	{
		nCompareValue[0] = 0x00000010;
	}

	if((pRoom[1]->_State != ROOM_STATE_READY && GET_ROOM_INFO_INTERENTER(pRoom[1]->_InfoFlag) > 0) ||
		pRoom[1]->_PersonNow == pRoom[1]->_PersonPlayerMax)
	{
		nCompareValue[1] = 0x00000010;
	}

	// 현재 빈 자리를 체크합니다 (0~15 slot)
	nCompareValue[0] += pRoom[0]->_PersonPlayerMax - pRoom[0]->_PersonNow;
	nCompareValue[1] += pRoom[1]->_PersonPlayerMax - pRoom[1]->_PersonNow;

	nResult = nCompareValue[0] - nCompareValue[1];

	return nResult < 0;
}

bool UIPhaseLobby::OnInviteMove( void)
{
	if( m_bWaitingFromServer)
		return false;

	m_bWaitingFromServer = true;
	m_bAllowRefresh = false;

	i3UI::setDisableUIInput(true);
	// 스탭, 팝업 닫는다.
	_CloseAllSubPhaseAndPopup();

	// 서버 / 채널이 다른지 확인
	bool bOtherServer = CCommunityContext::i()->GetMoveTargetServer() != (UINT32)g_pServerInfo->GetCurGameServerIndex();
	bool bOtherChannel = CCommunityContext::i()->GetMoveTargetChannel() != (UINT32)g_pServerInfo->GetCurChannelNumber();

	// 둘중 하나라도 다르다면, 서버/채널 페이지로 이동
	if( bOtherServer == true || bOtherChannel == true)
	{
		GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
	}
	else
	{
		// 방만들기 창 닫기
		OnCreateRoomCancel();
		
		UINT32 ui32TargetMatch = CCommunityContext::i()->GetMoveTargetMatch();

		if( MATCH_TEAM_INVALID_IDX != ui32TargetMatch)
		{
			GetMainFrame()->MovePhase(UIPHASE_CLANLOBBY);
		}
		else
		{
			m_SelectedElement = static_cast<INT32>(CCommunityContext::i()->GetMoveTargetRoom());
			MyRoomInfoContext::i()->setMyRoomIndex(m_SelectedElement);

			bool bInvite = true;

			GameEventSender::i()->SetEventOnce(EVENT_JOIN_ROOM, &bInvite);
		}
	}

	m_bMyPhase = false;

	return true;
}

void UIPhaseLobby::SelectRoom( INT32 idx)
{
	if( idx >= (INT32)m_RoomList.size() || idx < 0)
	{
		_ClearRoomInfo();
	}
	else
	{
		if( m_SelectedElement != idx)
		{
			m_bRequestRoomInfoAdd = true;
			m_SelectedElement	= idx;
		}
		else
		{
			// 툴팁을 다시 띄울 경우 빠른 처리를 위해 추가
			m_bQueRoomInfo = true;
		}
	}
}

void UIPhaseLobby::RefreshSelectedRoom( void)
{
	// 선택되어 있는 방의 정보를 갱신하는 이벤트입니다.
	// 룸리스트를 갱신하는 이벤트가 아닙니다...
	// 왜 이런지 모르겠습니다.
	if ( m_SelectedElement == -1 )
		return;

	// 반복해서 눌러도 2초간격으로 되게 합니다.
	if ( g_pFramework->GetAccTime() - m_LastRefreshTime <= g_pConfigEx->GetNetwork().RefreshInterval_RoomInfo)
		return;
	m_LastRefreshTime = g_pFramework->GetAccTime();

	GameEventSender::i()->SetEvent( EVENT_GET_ROOM_ADDINFO_N );
	m_bRequestRoomInfoAdd = true;

	I3TRACE0("EVENT_GET_ROOM_ADDINFO_N [CPhaseLobby::OnRefresh]\n");

	_SortRoomList();
	//m_SelectedElement = -1;
}

void	UIPhaseLobby::OnRefreshButton()
{
	// 서버로 자주 이벤트가 가지 않도록 살살 처리한다..
	
	if (m_bRefreshButtonActive )
		return;
	
	// 리프레시 갱신시간은 2초인데...이것보다는 자주 처리될수 있도록 해야한다..
	
	m_bRefreshButtonActive = true;

	GameEventSender::i()->SetEvent(EVENT_LOBBY_REFRESH_N);		// Send패킷이 잦으면 서버가 괴로울것이다.
	// 우선 해당 이벤트가 SEND되었는데, RECEIVE가 없다면 막는 처리를 추가한다..

	m_fRefreshElapsedTime = g_pConfigEx->GetNetwork().RefreshInterval_RoomList;

}

void  UIPhaseLobby::OnScrollRoomList(void)
{
	if(m_pScrollRoomList == nullptr)
		return;

	if( m_RoomList.size() <= UI_LOBBY_ROOMLIST_VIEWCOUNT )
		return;

	_SetRoomList();
	_SetCurSelectItem();
}

void  UIPhaseLobby::OnWheelRoomList(INT32 zDelta)
{
	if(m_pScrollRoomList == nullptr)
		return;

	if( m_RoomList.size() <= UI_LOBBY_ROOMLIST_VIEWCOUNT )
		return;

	INT32 i32NewPos = GetRoomListScrollPos() + (zDelta / -120);	
	INT32 i32Min, i32Max = 0;

	m_pScrollRoomList->getScrollRange(&i32Min, &i32Max);
	i32NewPos = MINMAX(i32Min, i32NewPos, i32Max);

	m_pScrollRoomList->setScrollPos( i32NewPos );

	_SetRoomList();
	_SetCurSelectItem();
}

void  UIPhaseLobby::SortRoomList(INT32 nType)
{
	sm_nCurSortType = nType;

	if(sm_SortOrder[nType]._bAscending == true)
		sm_SortOrder[nType]._bAscending = false;
	else
		sm_SortOrder[nType]._bAscending =  true;

	_ClearRoomInfo();

	_SortRoomList();

	// 바로 화면을 갱신
	_UpdateRoomList();
}

void	UIPhaseLobby::SortAdmissionRoom(bool bViewAdmissionRoom)
{
	m_bViewAdmissionRoom = bViewAdmissionRoom;

	_ClearRoomInfo();

	_SortRoomList();

	// 바로 화면을 갱신
	_UpdateRoomList();
}

void UIPhaseLobby::ConfirmAbuseOK( void)
{
	AbuseContext::i()->ResetAbuse();

	GetMainFrame()->SetEffectButtonBegin();

	//어뷰징 완료 To 서버
	GameEventSender::i()->SetEvent(EVENT_ABUSING_SET);

	SetInputDisable( false);
}

void UIPhaseLobby::DisableRoomTooltip(void)
{
	if(m_pTTMapInfo != nullptr)
	{
		m_pTTMapInfo->DisableToolTip();
		I3TRACE( "Disable RoomInfo\n");
	}
}


void UIPhaseLobby::_SortRoomList(void)
{
#if defined( TEST_LOBBY_ROOM_LIST)
	_BuildRoomListDummy();
#else
	// Clear Room List
	i3::cu::for_each_SafeFree_clear(m_RoomList);

    // Copy Room List
	INT32 nRoomCount = LobbyContext::i()->getLobbyRoomCount();
	for(INT32 i = 0; i < nRoomCount; i++)
	{
		const ROOM_INFO_BASIC * pRoomInfo = LobbyContext::i()->getRoomInfo(i);
		if ( ! pRoomInfo )
		{
			I3ASSERT(0 && "Invalid room index.");
			continue;
		}
		// PersonNow가 0이면 깨진 방
		if (0 == pRoomInfo->_PersonNow)
		{
			continue;
		}

		LOBBY_ROOM_INFO* pNewRoom = (LOBBY_ROOM_INFO*)i3MemAlloc(sizeof(LOBBY_ROOM_INFO));
		i3mem::Copy( &pNewRoom->_RoomInfo, pRoomInfo, sizeof(ROOM_INFO_BASIC));
		m_RoomList.push_back(pNewRoom);
		
	}
#endif

	// Sort Room List
	// 라디오버튼을 사용한 입장 가능/불가능 방 소팅을 할 경우에는
	// 무조건 CompareAdmissionRoom 을 사용합니다.
	if(m_bViewAdmissionRoom == true)
		i3::sort(m_RoomList.begin(), m_RoomList.end(), CompareAdmissionRoom);
	else
		i3::sort(m_RoomList.begin(), m_RoomList.end(), CompareRoom);

	m_is_sort_room_list = true;
}

void UIPhaseLobby::_EmptyRoomList(void)
{
	INT32 j;
	INT32 nPreRemoveRoomCount = 0;

	INT32 nRoomCount = LobbyContext::i()->getLobbyRoomCount();

	for(size_t i = 0; i < m_RoomList.size(); i++)
	{
		bool bBeRoom = false;
		LOBBY_ROOM_INFO * pInfo = m_RoomList[i];
		ROOM_INFO_BASIC* pRoom = &pInfo->_RoomInfo;

		if (ROOM_STATE_EMPTY != pRoom->_State)
		{
			for( j = 0; j < nRoomCount; j++)
			{
				const ROOM_INFO_BASIC* pRoomInfo = LobbyContext::i()->getRoomInfo(j);
				if ( ! pRoomInfo )
					continue;

				if (pRoom->_Idx == pRoomInfo->_Idx)
				{
					// PersonNow가 0이면 깨진 방
					if (0 != pRoomInfo->_PersonNow)
					{
						bBeRoom = true;
					}
					break;
				}
			}
		}
		
		if (false == bBeRoom)
		{
			// 화면에 보여지는 슬롯이하, 선택된 슬롯 이하는 없어져도 비우기만 한다. - 목록 땡겨지는 것 방지
			//if ((UINT32)i < m_pList->GetCurrentTop() + MAX_ROOM_PER_PAGE_EX || (-1 != m_SelectedElement && i < m_SelectedElement))

			// 보여지는 슬롯 이후에 존재하는 선택슬롯이 제거되면, 선택 슬롯 초기화 -> 뒷부분은 빈 방 제거되도록
			if ((INT32)i < m_SelectedElement)
			{
				nPreRemoveRoomCount++;				
			}
			else if ((INT32)i == m_SelectedElement)
			{
				_ClearRoomInfo();
				m_SelectedElement = -1;
			}

			i3MemFree(m_RoomList[i]);
			i3::vu::erase_index(m_RoomList, i);
			i--;
		}
	}

	// 선택 슬롯 이하부분 땡겨진 만큼 선택 슬롯 이동
	if (-1 != m_SelectedElement)
	{
		m_SelectedElement = MAX(-1, m_SelectedElement - nPreRemoveRoomCount);
	}
}


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

/*virtual*/ void UIPhaseLobby::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_Lobby_RoomList.i3UIs",  PhaseLobby_Glue);

	m_pTTMapInfo = UIToolTipLobbyMapInfo::new_object();
	m_pTTMapInfo->OnCreate(this);
}

void	UIPhaseLobby::OnUpdate_NoneRsc(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate_NoneRsc(rDeltaSeconds);
	
	if( !GameLoginContext::i()->IsFirstLogin())
	{
		_ProcessCommon( rDeltaSeconds);
	}
}


/*virtual*/ void UIPhaseLobby::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	if( ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN &&
		UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
	{
		if(UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
		{
			// 클랜매치ON - 용병에서 제거되면 로비에서 나가야 함 
			// UIFloatMercenaryLIst에서는 해줄 수 없기 때문에 여기서 처리
			if(GetMainFrame()->getDeprotationClanChannel() == false &&
				ClanGameContext::i()->getMyMercenaryState() == false &&
				m_PreClanMemberLevel == (CLAN_MEMBER_LEVEL_TYPE)UserInfoContext::i()->GetMyClanState() )
			{
				m_bAllowRefresh = false;
				m_bRefreshButtonActive = false;

				i3UI::setDisableUIInput( true);

				GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
				m_bMyPhase = false;
				return;
			}

			m_PreClanMemberLevel = (CLAN_MEMBER_LEVEL_TYPE)UserInfoContext::i()->GetMyClanState();
		}
	}



	if( isSliding())
		return;

	if( m_bAllowRefresh)
	{
		__AutoRefreshRoomList(rDeltaSeconds);
	}

	// 어뷰징 관련 시간 함수
	if( AbuseContext::i()->IsAbusing())
	{
		__ProcessAbusing(rDeltaSeconds);
	}

	// 아바타 로딩중이 아닌 경우
	if( !CharaViewMgr::i()->isLoadWaiting() && CharaViewMgr::i()->isLoadedChara() )
	{
		m_pBtnTutorial->removeState( I3UI_STATE_DEACTIVATED);
		m_pBtnTutorial->setModifyState();

		m_bTutorialEnable = true;
	}
	else
	{
		m_pBtnTutorial->addState( I3UI_STATE_DEACTIVATED);
		m_pBtnTutorial->setModifyState();
	}

	// EnterLobby 시 RoomList를 받아오기까지 대기 시간
	if( m_rEnterLobbyWaitTime < ENTER_LOBBY_WAIT_TIME )
	{		
		m_rEnterLobbyWaitTime += rDeltaSeconds;
		_AddMsgBoxToTopMenuScreen();
		return;
	}

	//방정보 받은 후 방 정렬되어야 다음 행동을 할 수 있다.
	//그 전까지는 입력이 막힌다.
	if( m_is_sort_room_list == false ) return; 

	if( m_pWaitRecevingRoomList != nullptr)
	{
		//GameUI::DisableLoadingIcon( m_pWaitRecevingRoomList);
		m_pWaitRecevingRoomList = nullptr;
		
		m_bCheckInvite = true;
	}
	else
	{
		if(ButtonTermTime > 1.5f && !AbuseContext::i()->IsAbusing())
		{
			((UIFramework*) m_pParent)->GetUIMainframe()->SetEffectBegin();
		}
		else
		{
			ButtonTermTime += rDeltaSeconds;
		}
	}

	if( GameLoginContext::i()->IsFirstLogin())
	{
		g_pFramework->GetMessageBoxManager()->m_is_open_message_box = false; //message box 활성화, 비활성화 유무를 알기 위한 변수

		_ProcessFirstLogin();

		if( GetMainFrame()->IsPopupFocused() == false && 
			g_pFramework->GetMessageBoxManager()->m_is_open_message_box == false )
		{
			g_pFramework->SetFocus(nullptr);
			GetMainFrame()->InputDisableMainUI(false);
		}

		/*GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);*/
	}

	if( m_SelectedElement > -1 && m_SelectedElement < (INT32)m_RoomList.size() )
	{
		LOBBY_ROOM_INFO * pInfo = m_RoomList[ m_SelectedElement];
		I3ASSERT( pInfo != nullptr);

		// 툴팁 메세지로 인해 사용합니다.
		if (m_bQueRoomInfo == true)
		{
			if(m_pTTMapInfo != nullptr)
			{
				if( m_pTTMapInfo->isEnable() == false )
					m_pTTMapInfo->EnableToolTip(&pInfo->_RoomInfo, LobbyContext::i()->getSelectedRoomInfoAdd());
				else
					m_pTTMapInfo->ChangeToolTip(&pInfo->_RoomInfo, LobbyContext::i()->getSelectedRoomInfoAdd());

				m_bQueRoomInfo = false;
			}
		}

		if( m_bRequestRoomInfoAdd)
		{
			// 같은 방의 슬롯을 반복해서 눌러도 2초간격으로 되게 합니다.
			if ( g_pFramework->GetAccTime() - m_LastRefreshTime > g_pConfigEx->GetNetwork().RefreshInterval_RoomInfo)
			{
				ROOM_INFO_BASIC * pRoomInfo = &pInfo->_RoomInfo;

				m_LastRefreshTime = g_pFramework->GetAccTime();

				LobbyContext::i()->setSelectedRoomIdx( pRoomInfo->_Idx);

				I3TRACE("EVENT_GET_ROOM_ADDINFO_N [UIPhaseLobby::SelectRoom]\n");
				GameEventSender::i()->SetEvent( EVENT_GET_ROOM_ADDINFO_N );

				m_bRequestRoomInfoAdd = false;
			}
		}
	}

	//new hot 상품 목록 만들기.
	if( LocaleValue::Enable("EnableLobbyNewHot") )
	{
		UINewAndHotBanner* pfnewAndHot = static_cast<UINewAndHotBanner *>(GameUI::GetFloating( UFW_NEWANDHOT));
		if( pfnewAndHot != nullptr)
		{
			if( pfnewAndHot->isEnable() == true && pfnewAndHot->GetMakeList() == false)
			{
				pfnewAndHot->MakeNewAndHotGoodsList();
				pfnewAndHot->SelectGoodsInList();
			}
		}
	}
}

/*virtual*/ void	UIPhaseLobby::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	// 어느 위치에서든 Lobby Event를 받아야합니다.
	// 하위 Phase일 수도 있기에 Base에서 처리합니다. --swoongoo
	UIPhaseBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

bool UIPhaseLobby::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_INPUT)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC) ||
				pKeyboard->GetStrokeState(I3I_KEY_ENTER) )
			{
				if( GetMainFrame()->IsOpenedPopup( UPW_PCCAFE))
					return GetMainFrame()->GetPopup( UPW_PCCAFE)->OnEvent( event, pObj, param2, code);

				//패스워드 팝업창에서 editbox에 포커스가 있는 상태에서 엔터키를 누르면 이 함수가 호출되기 때문에 
				//lobby 클래스에서 패스워드 팝업에 OnKey_Enter 를 인위적으로 호출해야 된다.
				//하지만, 패스워드 팝업창 자체에 포커스가 있는 경우엔 패스워드 팝업창 OnKey_Enter 함수가 호출된다.
				if( GetMainFrame()->IsOpenedPopup( UPW_QUERY_PASSWORD)) 
					return GetMainFrame()->GetPopup(UPW_QUERY_PASSWORD)->OnKey_Enter();
			}

			// 빠른 입장 오픈
			if( pKeyboard->GetStrokeState(I3I_KEY_F5))
			{
				// SubPhase == NONE , 열린 팝업이 없을때 동작
				if(GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE && 
					GetMainFrame()->IsPopupFocused() == false && LocaleValue::Enable("EnableQuickJoin") == true)
				{
					UIMainFrame* f = g_pFramework->GetUIMainframe();
					I3ASSERT(f);
					f->OpenPopup(UPW_QUICK_JOIN);
				}
			}
		}
	}

	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;

		i3UIButton * pButton = i3::same_cast<i3UIButton*>(pNotify->m_pCtrl);

		if( pButton != nullptr)
		{
			i3UIStaticText * pTextCtrl = i3::same_cast<i3UIStaticText*>(pButton->FindChildByType(i3UIStaticText::static_meta()));
			if( pTextCtrl != nullptr)
			{
				COLOR color;

				if( pButton->isSelected() )					i3Color::Set(&color, (UINT8)0, 51, 96, 255);
				if( pButton->isState( I3UI_STATE_ONMOUSE) )	i3Color::Set(&color, (UINT8)6, 82, 149, 255);
				else										i3Color::Set(&color, (UINT8)184, 216, 255, 255);

				pTextCtrl->SetTextColor(&color);
			}
		}
	}

	return UIPhaseBase::OnEvent(event, pObj, param2, code);
}

bool UIPhaseLobby::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	//BattleSlotContext::i()->setMySlotIdxEx( 0 );
	
	//	클랜전 예외처리
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
			return false;
	}
	
	_InitRoomList();

	// sort value init
	{
		m_bViewAdmissionRoom = false;

		for(INT32 idx = 0 ; idx < MAX_SORT_TYPE; ++idx)
			sm_SortOrder[idx]._bAscending = false;

		sm_nCurSortType = 0;
		sm_SortOrder[0]._bAscending = true;
	}

	g_pFramework->getChatBox()->setWhisperEnable( true);
	g_pFramework->getChatBox()->ResetHistoryBufferIndex();
	
	//	로비에 들어오면 제일 먼저 서버에 상품 리스트를 요청한다. 
	//	커뮤니티에서 선물 쪽지 확인하기 전에 서버로부터 선물 리스트를 정상적으로 모두 받아야 선물 쪽지가 정상 작동하게 됩니다.  komet
	//	(상품 리스트와 선물 대조를 위해)
	GameEventSender::i()->SetEvent( EVENT_REQUEST_GOODS_LIST);

	if ( LocaleValue::Enable("FieldShop"))
	{
		IGShopContext* igcont = g_pFramework->GetIGShopContext();
		igcont->SetEvent_GetInfo();
	}

	UITopMenu * pTop = GetMainFrame()->GetTopMenu();
	if( pTop != nullptr)
		pTop->SetMode(TOPMENU_MODE_READY);

	// Lobby Button을 설정합니다.
	m_SelectedElement = -1;
	_SetButtonState();
	
	UI_CHARA_INFO	* pNetCharaInfo = UserInfoContext::i()->SetMyUICharaInfo();
	CCharaInfoContext::i()->GetUICharaInfo( pNetCharaInfo);
	m_bWaitingFromServer = false;

	m_slide.ForceEnd();	//slide 행동을 강제한다.

	if( GameLoginContext::i()->IsFirstLogin() == true )
	{
		g_pFramework->SetFocus(nullptr);
		GetMainFrame()->InputDisableMainUI(true);

		// 선물함 리스트 요청. (쪽지함 알람 기능으로 인해 로비 첫 진입시 Recv)
		GameEventSender::i()->SetEvent(EVENT_GET_GIFT_LIST);
	}	

	if(MedalContext::i()->GetReceivedMyMedal() == false)
	{
		GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_NORMAL_INFO);
	}

	GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);

	return true;
}

// 입장중
/*virtual*/ bool UIPhaseLobby::OnEntrancing( REAL32 rDeltaSeconds)
{
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		return UIPhaseBase::OnEntrancing( rDeltaSeconds);
	}

	// 로비에서는 자신의 슬롯 번호를 0번으로 고정
	BattleSlotContext::i()->setMySlotIdxEx(0);

	return true;
}


// 입장 완료
/*virtual*/ void UIPhaseLobby::OnEntranceEnd( void)
{
	UIPhaseBase::OnEntranceEnd();

	SortRoom::InitPriority();

#ifndef NC_BUILD
	AutoCheckEquipments();
#endif

	m_rEnterLobbyWaitTime = 0.0f;
	m_bAllowRefresh = true;
	m_bRefreshButtonActive = false;

	// enable invite
	InviteContext::i()->ClearInviteInfo();
	m_bCheckInvite = true;

	LuaState * L = _CallLuaFunction("SetClanButton");
	if( L != nullptr)
	{
		bool bClanChannel = ChannelContext::i()->GetBattleChannel();
		bool bClanMatchEnable =	UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW); 

		i3Lua::PushBoolean(L , bClanChannel && bClanMatchEnable);
		_EndLuaFunction(L , 1);
	}

	if ((UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN))
	{ 
		m_pBtnTeamLIst->disable_btn();
	}
	else
		m_pBtnTeamLIst->enable_btn();


	//	클랜전 예외처리
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		//  어뷰징 관련 하동익 20110402
		if(AbuseContext::i()->IsAbusing())
		{
			//GetMainFrame()->TogglePopup( UPW_ABUSE);
			//서버에서 받은 어뷰징 레벨에 맞게 텍스트 설정 각 국가 번역 필요
			if( m_pAbusingBox == nullptr)
			{
				switch( AbuseContext::i()->GetAbuseLevel())
				{
				case ABUSING_LEVEL_NORMAL:
					m_pAbusingBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_ABUSING_LEVEL_NORMAL"));
					break;
				case ABUSING_LEVEL_STRONG:
					m_pAbusingBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_ABUSING_LEVEL_STRONG"));
					break;
				case ABUSING_LEVEL_STRONGEST:
					m_pAbusingBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_ABUSING_LEVEL_STRONGEST"));
					break;
				default :
					I3PRINTLOG(I3LOG_WARN,  "switch invalid call!!!");
					break;
				}
			}
			
			m_rAbuseTime = 0.f;
		}
		else
		{
			if ( KickContext::i()->IsKickedByVote() )
			{		// 강퇴자의 경우 대기메세지 대신 강퇴되었다고 알려준다..
				USER_INFO_BASIC basic;
				UserInfoContext::i()->GetMyUserInfoBasic(&basic);

				i3::rc_wstring wstrMsg;
				i3::sprintf(wstrMsg, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), basic.m_strNick);		// 자동형변환..? ( 2014.07.21.수빈)

				i3UI::getGlobalIme()->KillFocus();
				GetMainFrame()->EnableFloatWin( UFW_MINIPROFILE);
				m_pKickedMassageBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg, nullptr, MAKE_CALLBACK(CbKickedByVote), this); /*강퇴 알려줌*/

				KickContext::i()->ClearKickedByVote();
			}
		}
	}

	m_is_sort_room_list = false;
	m_bRequestRoomInfoAdd = false;
	m_SelectedElement = -1;

	// Battle에서 에러팝업 후 퇴장 시 처리
	if( g_pFramework->IsMessageBoxEnabled() )
	{
		i3UI::setDisableUIInput( true);
	}

	// 이하 블록내 순서 중요합니다. - praptor
	{
		if (CCommunityContext::i()->IsMovingToUser())
		{
			OnInviteMove();
			return;
		}

		INT32 Channel_Index = g_pServerInfo->GetCurChannelNumber();
		INT32 Server_Index = g_pServerInfo->GetCurGameServerIndex();
		ChannelType Channel_Type = ChannelContext::i()->GetChannelType(Server_Index, Channel_Index);

		if (ChannelContext::i()->IsMoveChannel()
			&& (Channel_Type != CHANNEL_TYPE_CLAN && Channel_Type != CHANNEL_TYPE_PCCAFE_CLAN) )
		{
			m_bAllowRefresh = false;
			ChannelContext::i()->ClearMoveChannel(); 

			i3UI::setDisableUIInput(true);
			GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
			m_bMyPhase = false;	// 자신의 Phase가 아니다.
			return;
		}
	}

	if( TutorialContext::i()->WasTutorial())
	{
		TutorialContext::i()->SetWasTutorial(false);
		GetMainFrame()->EnableFloatWin( UFW_MINIPROFILE);
		GetMainFrame()->UpdateLocation();

		if( TutorialContext::i()->IsFirstFinishTutorial() && TutorialContext::i()->WasTutorial())
		{
			//GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("튜토리얼 모드 완료를 축하드립니다. AI전을 하시겠습니까?"), nullptr, MAKE_CALLBACK(CbAIMode), this);
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_COMPLETE_TUTORIAL_TO_AI"), nullptr, MAKE_CALLBACK(CbAIMode), this);
		}
	}

	// 내구도 체크
	if( UserInfoContext::i()->IsNewRepair() )
		GameUI::CheckWeaponEndurance();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	// 자기자신이 클랜전 중 이탈한 용병(패널티 적용된 용병)이면 클랜채널 일반로비에서 강제 퇴장되어야 함

	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
		ChannelContext::i()->GetBattleChannel())
	{
		if (ClanGameContext::i()->getMyMercenaryState() == true &&
			ClanGameContext::i()->IsMyMercDepartured())
		{
			if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE"), nullptr, MAKE_CALLBACK(CbMercPenalty), this);
			}
			else
			{
				GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE"), nullptr, nullptr, this);
			}

		}
	}
}

/*virtual*/ bool UIPhaseLobby::OnExitStart( void)
{
	if( UIPhaseBase::OnExitStart() == false)
		return false;

	if( m_pKickedMassageBox)
	{
		GameUI::CloseMsgBox( m_pKickedMassageBox);
		m_pKickedMassageBox = nullptr;
	}

	if( m_pWaitRecevingRoomList != nullptr)
	{
		//GameUI::DisableLoadingIcon( m_pWaitRecevingRoomList);
		m_pWaitRecevingRoomList = nullptr;
	}

	m_bCheckInvite = false;

	DisableRoomTooltip();

	//slide 행동을 강제한다.
	// 주의 이 함수가 호출되면 강제로 메모리 해제가 된다. 마지막에 호출되야함!!!!
	m_slide.ForceEnd();

	return true;
}

/*virtual*/ void UIPhaseLobby::OnGotoBackPhase( void)
{
	m_bAllowRefresh = false;
	m_bRefreshButtonActive = false;

	i3UI::setDisableUIInput( true);
	
	GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
	m_bMyPhase = false;
}

/*virtual*/ bool UIPhaseLobby::OnDblClick( i3UIControl * pControl)
{
	if( UIPhaseBase::OnDblClick( pControl) == false)
		return false;

	if( i3::same_of<i3UIListView_Item*>(pControl))
		PlaySound( UISND_CLICK);

	return true;
}

void UIPhaseLobby::ClickButton( INT32 BtnIdx)
{
	if( BtnIdx > -1 && BtnIdx < UI_LOBBY_BUTTON_COUNT)
	{
		switch(BtnIdx)
		{
		case ULB_QUICKJOIN :	SendEventQuickJoin();	break;
		//case ULB_CREATEROOM :	GetMainFrame()->TogglePopup( UPW_CREATEROOM);	break;
		case ULB_CREATEROOM :
			{
				if(	
					ClanGameContext::i()->getMyMercenaryState() == true &&
					ChannelContext::i()->GetBattleChannel()
				  )
				{
					GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_MERC_JOIN_NORMAL_MATCH_ROOM"));
					break;
				}

				bool isLobby = true;
				GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, (void*)&isLobby );	
			}			
			break;
		case ULB_ENTERROOM :	EnterRoom( -1);			break;
		case ULB_TUTORIAL :		EnterTutorial();		break;
		}
	}
}

void UIPhaseLobby::EnterAIMode(void)
{
	// 방생성 이름이 기본 제공하는 제목이라면 필터링하지 않는다. 기본 방제에서 필터에 걸려 방 생성이 안되는 문제를 해결함. komet
	const i3::rc_wstring& wstrDefaultRoomName = GAME_RCSTRING("STBL_IDX_ROOMNAME_1");	// 1 ~ 4

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	MyRoomInfoContext::i()->getMyRoomInfo(&MyRoomInfo);
	MyRoomInfoContext::i()->getMyRoomInfoAdd(&MyRoomInfoAdd);
	
	SetInputDisable( false);

	// 방생성 정보를 셋합니다!
	
	// 1.방이름
	i3::safe_string_copy(MyRoomInfo._Title, wstrDefaultRoomName, NET_ROOM_NAME_SIZE);

	// 2.게임모드
	{	
		MyRoomInfo._StageID = CStageInfoUtil::GetRuleData((INT32)0)->MakeDefaultStageID();
	}

	// 3.비밀방 여부
	{
		MyRoomInfo._InfoFlag = 0;
	}

	// 인원수
	{
		MyRoomInfo._PersonPlayerMax = 16;
	}

	// 4. 게임 고급 설정 메뉴
	{			
		//// 무기 제한	
		m_LockWeapon = 0;

		UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
		UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
		UNSETBIT(m_LockWeapon, LOCK_RPG7);
		UNSETBIT(m_LockWeapon, LOCK_THROW);
		SETBIT( m_LockWeapon, LOCK_PRIMARY); 
		SETBIT( m_LockWeapon, LOCK_SECONDARY); 
		SETBIT( m_LockWeapon, LOCK_THROW);
		UNSETBIT(m_LockWeapon, LOCK_RPG7);
		SETBIT( m_LockWeapon, LOCK_MELEE);

		MyRoomInfo._WeaponFlag = m_LockWeapon;

		UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_1PVONLY);

		// 난입제한
		m_LockInterEnter = 0;
		m_LockTeamBalance = 1;
		m_SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;

		MyRoomInfoAdd._SubType = m_SubType;
		MyRoomInfoAdd._LockInterEnter = m_LockInterEnter;
		MyRoomInfoAdd._LockObserver = m_LockObserver;
		MyRoomInfoAdd._LockTeamBalance = m_LockTeamBalance;
	}

	// AI전으로 설정 될 경우 0으로 재설정되기때문에 변경합니다.
	MyRoomInfo._WeaponFlag = 0x0F;

	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance	= 2;	// 기본 밸런스 (2번째)

	AIModeContext::i()->EnemyCount = 8;
	AIModeContext::i()->StartLevel = 1;
	AIModeContext::i()->CurrentLevel = 1;

	MyRoomInfoContext::i()->setMyRoomInfo(&MyRoomInfo);
	MyRoomInfoContext::i()->setMyRoomInfoAdd(&MyRoomInfoAdd);

	GameEventSender::i()->SetEventOnce( EVENT_CREATE_ROOM);

	i3UI::setDisableUIInput( true);

}

void UIPhaseLobby::EnterTutorial( void)
{
	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;

	if( m_bWaitingFromServer == false)
	{
		MyRoomInfoContext::i()->getMyRoomInfo(&MyRoomInfo);
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&MyRoomInfoAdd);


		i3::safe_string_copy(MyRoomInfo._Title, _TT("Tutorial"), NET_ROOM_NAME_SIZE );/*훈련전*/

		MyRoomInfo._StageID			= StageID::MakeStageID(STAGE_MODE_TUTORIAL, WEAPON_ALL, AI_USE_NO, false, false, 0, STAGE_UID_TUTORIAL);;
		MyRoomInfo._WeaponFlag		= 0x0F;	

		// Add
		MyRoomInfoAdd._SubType				= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10;
		MyRoomInfoAdd._NowRoundCount		= 0;
		MyRoomInfoAdd._NowIngTime			= 0;
		MyRoomInfoAdd._LockInterEnter		= 0x01;	// 기본 가능함
		MyRoomInfoAdd._LockTeamBalance		= 0x00;	// 기본 밸런스 있음

		MyRoomInfoContext::i()->setMyRoomInfo(&MyRoomInfo);
		MyRoomInfoContext::i()->setMyRoomInfoAdd(&MyRoomInfoAdd);

		GameEventSender::i()->SetEventOnce( EVENT_CREATE_ROOM);

		i3UI::setDisableUIInput( false);

		m_bWaitingFromServer = true;
	}
}

void UIPhaseLobby::OnCreateRoomCancel( void)
{

	m_bEnableCreateRoomMsgBox = false;

	// CreateRoom Window를 닫는다.
	//GetMainFrame()->ClosePopup( UPW_CREATEROOM );
	GetMainFrame()->ClosePopup( UPW_CREATEROOM_EX );
}

// 방에 입장
void UIPhaseLobby::EnterRoom( INT32 idxRoom)
{
	if( ((m_SelectedElement == idxRoom) && (idxRoom != -1)) ||
		((idxRoom == -1) && (m_SelectedElement != -1)) )
	{
		I3ASSERT( m_SelectedElement < (INT32)m_RoomList.size());

		LOBBY_ROOM_INFO* pLobbyRoom = m_RoomList[m_SelectedElement];
		ROOM_INFO_BASIC* pInfo = (ROOM_INFO_BASIC*) &pLobbyRoom->_RoomInfo;

		// 빈 슬롯 없음
		if (pInfo->_PersonNow >= pInfo->_PersonPlayerMax)
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_SLOT_FULL"));
			return;
		}

		// 공개여부에 따른 비밀번호 입력 팝업 출력. (단, GM , MOD 유저는 그냥 입장 가능.)
		if ( !UserInfoContext::i()->CheckAuthLevel() &&
			(GET_ROOM_INFO_PRIVATE_MODE( pInfo->_InfoFlag )) )
		{
			QPParam Param;

			Param.wstrMsg			= GAME_RCSTRING("STBL_IDX_EP_LOBBY_JOIN_ROOM_Q");
			Param.LimitCount	= NET_ROOM_PW;
			Param.OnlyNumber	= true;
			Param.ShowPassword	= true;
			Param.Callback		= JoinRoomPasswordCallback;
			Param.pUserContext	= this;

			m_JoinTargetRoom	= *pInfo;

			GetMainFrame()->TogglePopup( UPW_QUERY_PASSWORD, &Param);
		}
		else
		{
			if (
				 ClanGameContext::i()->getMyMercenaryState() == true &&
				 ChannelContext::i()->GetBattleChannel()
			   )
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MERC_JOIN_NORMAL_MATCH_ROOM"));
				return;
			}

			// 입장
			m_bAllowRefresh = false;
			I3TRACE( "--- SetEventOnce(EVENT_JOIN_ROOM)!!\n");
			if( MyRoomInfoContext::i()->JoinRoom(pInfo))
			{
				i3UI::setDisableUIInput( true);

				I3TRACE( "--- setDisableInput -> true\n");
			}
			else
				I3TRACE( "--- can not send event : EVENT_JOIN_ROOM !!\n");
		}
	}
	else
	{
		SelectRoom(idxRoom);
	}
}



void UIPhaseLobby::SendEventQuickJoin( void)
{
	if( GetMainFrame()->IsCreatingNick() == false)
	{
		m_bAllowRefresh = false;

		// Lock을 2번건이유 : 퀵조인같은경우는 광클을 할 경우 연속으로 2번 버튼 이벤트가 호출되기 때문에,
		// 처음 이벤트를 보낼때 강제로 입력을 막아줘야 합니다. 안그럴경우 서버와 Phase 동기화가 실패할 수 있습니다.
		if( GetMainFrame()->isLockedInput() == false && LocaleValue::Enable("EnableQuickJoin") == true)
		{
			UIMainFrame* f = g_pFramework->GetUIMainframe();
			I3ASSERT(f);
			f->OpenPopup(UPW_QUICK_JOIN);
		}
	}
}



void UIPhaseLobby::EventItemPopup()
{
	if( UserContext::i()->ReceivedEvent)
	{
		GetMainFrame()->TogglePopup(UPW_EVENT,nullptr);
	}
	else
	{
		ShowPcCafePopup();
	}
	

	m_bEvnetNotice = true;


}
// PcCafe용 Popup을 띄운다.
void UIPhaseLobby::ShowPcCafePopup(void)
{
	USER_INFO_BASIC	MyUserInfoBasic;

	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	// 자기정보가 설정전이면 처리해선 안된다.
	if( i3::generic_string_size(MyUserInfoBasic.m_strNick) == 0)
		return;

	// PC방전용 아이템 지급 팝업을 출력한다.
	if (MyUserInfoBasic.m_ui8PCCafe != PC_NOT_CAFE)
	{
		// Message를 UI에 띄운다. MustBeFix
		GetMainFrame()->OpenPopup(UPW_PCCAFE, &MyUserInfoBasic);
	}
	//else
	//{
	//	ShowNoticePopup();
	//}

	m_bPcCafeNotice = true;
}

// 공지 사항 창을 띄운다.
void UIPhaseLobby::ShowNoticePopup(void)
{
	if( true )
	{
		if(m_bNickNameCreate)
			TutorialPopup();
	}


	//INT32 nListCount = g_pFramework->getNoticeListCount();
	//i3::stack_wstring wstrTemp;

	//i3::rc_wstring	wstrMyNickName = UserInfoContext::i()->GetMyNickName();		// TODO : 아무래도 이 함수의 내부구현은 어느 정도 단순화가 되어야할것임..(2014.07.21.수빈)

	if( g_pFramework->getNoticeActivation())
	{
		// 서버에서 받은 값으로 출력합니다. 16.01.12
		g_pFramework->setNoticeActivation(false);

		i3::stack_wstring wstrText;
		i3::generic_string_cat(wstrText, g_pFramework->getChatNotice());

		GameUI::MsgBox(MSG_TYPE_GAME_OK, i3::rc_wstring(wstrText));

		// 160112 이전의 소스코드.
		//// 초기 1회만 노출시키도록 체크합니다! (파일 비교 체크)
		//CFileFind finder;
		//i3FileStream fileForCheck;

		//bool bWorking = finder.FindFile("Script/Etc/NoticeCheck.log");	
		//finder.Close();

		//if( bWorking)
		//{
		//	::DeleteFile("Script/Etc/NoticeCheck.log");
		//	fileForCheck.Close();

		//	if( fileForCheck.Create( "Script/Etc/NoticeCheck.log", STREAM_WRITE))
		//	{
		//		// BOM 추가하고 유니코드 버전으로 변경..(2014.07.21.수빈)
		//		WCHAR16 bom = 0xFEFF;
		//		fileForCheck.Write( &bom, sizeof( WCHAR16));
		//		fileForCheck.SetPosition( 0, STREAM_END);

		//		i3::sprintf( wstrTemp, L"%s_%s", g_pFramework->getNoticeVersion(), wstrMyNickName);
		//		fileForCheck.WriteLineW( wstrTemp.c_str() );
		//	}
		//}
		//else
		//{
		//	if( fileForCheck.Create( "Script/Etc/NoticeCheck.log", STREAM_WRITE))
		//	{
		//		// BOM 추가하고 유니코드 버전으로 변경..(2014.07.21.수빈)
		//		WCHAR16 bom = 0xFEFF;
		//		fileForCheck.Write( &bom, sizeof( WCHAR16));
		//		fileForCheck.SetPosition( 0, STREAM_END);

		//		i3::sprintf( wstrTemp, L"%s_%s", g_pFramework->getNoticeVersion(), wstrMyNickName);
		//		fileForCheck.WriteLineW( wstrTemp.c_str() );
		//	}
		//}				

		//fileForCheck.Close();

		//g_pFramework->setNoticeActivation( false);

		//if( nListCount > 0)
		//{
		//	INT32 i = 0;
		//	i3::stack_wstring wstrText;

		//	for( i=0; i<nListCount; i++)
		//	{
		//		i3::generic_string_cat(wstrText, g_pFramework->getNoticeList(i));
		//	}

		//	GameUI::MsgBox( MSG_TYPE_GAME_OK, i3::rc_wstring(wstrText) );
		//}
	}

	m_bNickNameCreate = false;
}

// 출석 체크 추가
void UIPhaseLobby::ShowAttendancePopup(void)
{
	// 출석체크가 현재는 2종류 뿐이나, 차후 확장 가능성을 고려하여
	// 현재 활성화되어있는(화면에서 진행중인) 출석체크를 차례로 띄움
	// ATTENDANCE_TYPE 에 정의된 순서대로 출력하며, 출력할 수 있는 상태가 아닌 것은 건너뜁니다
	// ATTENDANCE_TYPE 에 새로운 타입의 이벤트가 추가되었을 시, 아래 switch-case 문에 추가해주세요

	for (int i = AttendanceContext::i()->GetCurrrentType(); i < ATTENDANCE_TYPE_MAX; ++i)
	{
		const ATTENDANCE_USER * pUser = AttendanceContext::i()->GetAttendanceUser((ATTENDANCE_TYPE)i);
		if (pUser->m_ui8EventState == ATTENDANCE_STATE_RUNNING)
		{
			switch (i)
			{
			case ATTENDANCE_TYPE_DORMANT:
				GetMainFrame()->OpenPopup(UPW_DORMANT_EVENT);
				break;
			case ATTENDANCE_TYPE_NORMAL:
				GetMainFrame()->OpenPopup(UPW_MONTH_ATTENDANCE);
				break;
			default:
				break;
			}
			
			break;
		}
	}
}

//------------------------------------------------------------------------------//
//								Error Handler									//
//------------------------------------------------------------------------------//

void UIPhaseLobby::EhWrongQuickJoin(void)
{
	// 방 생성 Popup을 띄운다.
	bool isLobby = true;
#if 1
	GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, (void*) &isLobby );
#else
	GetMainFrame()->TogglePopup( UPW_SETUP_QUICK_ENTRANCE, (void*)&isLobby );
#endif
}

void UIPhaseLobby::EhSlotFull(void)
{
	// TTP # 292 - 슬롯없음이 암호틀림보다 우선되야한다고 합니다.
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_SLOT_FULL"));

	m_bCheckInvite = true;
}

void UIPhaseLobby::EhKickedByVote()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_FAIL_START_C"));

	m_bCheckInvite = true;
}

void UIPhaseLobby::EhWrongPW(void)
{
	QPParam	Param;

	Param.wstrMsg		= GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_PW_Q");
	Param.LimitCount	= NET_ROOM_PW;
	Param.OnlyNumber	= true;
	Param.ShowPassword	= true;
	Param.Callback		= JoinRoomPasswordCallback;
	Param.pUserContext	= this;

	GetMainFrame()->TogglePopup(UPW_QUERY_PASSWORD, &Param);
}

void UIPhaseLobby::EhNotExist(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));

	m_SelectedElement = -1;

	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitInterenter(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_LOBBY_LIMIT_ENTER_ROOM"));/*난입이 제한된 방입니다.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitLever(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_LOBBY_LIMIT_GRADE"));/*계급이 제한된 방입니다.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitTraining(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_LOBBY_WARING_TRANING_ENTER"));/*훈련전은 방장만 입장할 수 있습니다.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitMERC(void)
{
	GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_CANNOT_MERC_VISIT"));/*용병은 일반 클랜전 방 생성 및 입장이 불가능합니다.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhStageError(INT32 Event,INT32 Arg)
{	
	i3::rc_wstring wstr_bug;
	i3::sprintf( wstr_bug, GAME_RCSTRING("STR_TBL_GUI_LOBBY_DEBUG_POPUP_TEXT"), Event, Arg);/*디버그 팝업: Event(%d), Arg(0x%x)*/

	GameUI::MsgBox(MSG_TYPE_ERROR, wstr_bug);
	m_SelectedElement = -1;
}



//------------------------------------------------------------------------------//

void UIPhaseLobby::CbSameNick(void* pThis,INT32 nParam)
{
	g_pFramework->GetUIMainframe()->OpenPopup( UPW_CREATENICKNAME);
}

void UIPhaseLobby::CbBeInvited(void* pThis,INT32 nParam)
{
	INT32 i;

	UIPhaseLobby* pLobby = (UIPhaseLobby*)pThis;

	if (MBR_OK == nParam)
	{
		// 다른 팝업은 닫자.!!
		pLobby->GetMainFrame()->TogglePopup( UPW_NULL);

		// 방만들기 창 닫기
		//pLobby->OnCreateRoomCancel();

		// 초대 수락
		for( i = 0; i < LobbyContext::i()->getLobbyRoomCount(); i++)
		{
			const ROOM_INFO_BASIC* pInfo = LobbyContext::i()->getRoomInfo(i);
			if ( ! pInfo )
				continue;

			// 방 인덱스로 찾는다
			if (pInfo->_Idx == pLobby->m_InviteInfo._idxRoom)
			{
				
				MyRoomInfoContext::i()->SetMyRoomPW(reinterpret_cast<const char*>(pLobby->m_InviteInfo._pw), NET_ROOM_PW);

				MyRoomInfoContext::i()->JoinRoom(pInfo);
				return;
			}
		}

		// 찾을 수 없다면 에러팝업
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));
	}
	
	// 초대 메시지 검사 다시 시작
	pLobby->m_bCheckInvite = true;
}

void UIPhaseLobby::CbSuppliedConfirm(void* pThis,INT32 nParam)
{
	// 프로필화면 갱신
	((UIPhaseLobby*)pThis)->GetMainFrame()->EnableFloatWin( UFW_MINIPROFILE);
}

void UIPhaseLobby::CbEventCardSetConfirm(void* pThis,INT32 nParam)
{
	// 이벤트 카드셋 팝업 출력 후, PC방 팝업 출력
	((UIPhaseLobby*)pThis)->ShowPcCafePopup();
}
// 닉네임 생성 후 튜토리얼 모드로 진행할 것인지에 대한 팝업창
void UIPhaseLobby::TutorialPopup( void)
{
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_GUI_BASE_NEW_ENTRY_TUTORIAL_MODE"),
		&GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"), MAKE_CALLBACK(CbTutorialMode), this);
}

void UIPhaseLobby::CbTutorialMode(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
		((UIPhaseLobby*)pThis)->EnterTutorial();
}

void UIPhaseLobby::CbAIMode(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
		((UIPhaseLobby*)pThis)->EnterAIMode();
	//if( nParam == MBR_OK)
	//	EnterAIMode();
}

void UIPhaseLobby::CbKickedByVote( void * pThis, INT32 nParam)
{
	USER_INFO_BASIC basic;
	UserInfoContext::i()->GetMyUserInfoBasic(&basic);

	i3::rc_wstring wstrMsg;
	i3::sprintf(wstrMsg, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), basic.m_strNick);		// 형변환 (2014.07.21.수빈)

	g_pFramework->getChatBox()->PutSystemChatting(wstrMsg);

	// 팝업이 닫히면 해제되기에 초기화해줘야 한다.
	((UIPhaseLobby*)pThis)->m_pKickedMassageBox = nullptr;
}

void UIPhaseLobby::CbMercPenalty(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
		((UIPhaseLobby*)pThis)->m_bMyPhase = false;
	}
}

// Key : 출석 체크 추가
void UIPhaseLobby::CbShowAttendancePopup(void *pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		AttendanceContext::i()->SetNextType();
	}
	((UIPhaseLobby*)pThis)->ShowAttendancePopup();
}

void UIPhaseLobby::CbOnGotoBackPhase(void *pThis, INT32 nParam)
{
	((UIPhaseLobby*)pThis)->OnGotoBackPhase();
}

void UIPhaseLobby::CbLeaveTeam(void* pThis, INT32 nParam)
{
	// 용병 재참가
	if(nParam == MB_OK)
	{
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
	else
	{
		GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
}

void UIPhaseLobby::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_pBtnTutorial = (i3UIButton*) pScene->FindChildByName( "Btn_Tutorial");
	I3ASSERT( m_pBtnTutorial != nullptr);

	m_pScrollRoomList = i3::same_cast<i3UIScrollBar*>(pScene->FindChildByName("ScrollBar_RL"));
	I3ASSERT( m_pScrollRoomList != nullptr );

	m_pBtnTeamLIst = i3::same_cast<i3UIButton*>(pScene->FindChildByName("Btn_Squad"));
	I3ASSERT( m_pBtnTeamLIst != nullptr);

	m_bTutorialEnable = true;
}

void UIPhaseLobby::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseLobby::ReceiveGameEvent_Ready_Battle( INT32 arg, const i3::user_data&)
{
	// Key : User Guide System
	{
		UserGuideMgr::i()->DisableUserGuideAll();
	}

	OnExitEnd();
	
	m_bMyPhase = false;
	g_pFramework->JumpToBattle();
}

void UIPhaseLobby::ReceiveGameEvent_Join_Quick( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{	
		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);
	}
	else
	{
		m_bMyPhase = true;
		m_bAllowRefresh = true;
		i3UI::setDisableUIInput(false);
		GetMainFrame()->SetLockInput(false);

		if( EV_IS_ERROR(arg,EVENT_ERROR_FAIL))
			EhWrongQuickJoin();
		else
			EhStageError( EVENT_JOIN_QUICK,arg);
	}
}

void UIPhaseLobby::ReceiveGameEvent_Create_Room( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		// 방이 생성되었으면 AutoRefresh를 끈다.
		m_bAllowRefresh = false;
		m_bEnableCreateRoomMsgBox = false;

		// 서버에서 내려준 PersonMax 값을 받아, 나머지 슬롯을 닫아준다.
		{
			ROOM_INFO_BASIC RoomInfo;
			MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo);
			for( INT32 i = 0; i < USER_SLOT_COUNT; ++i)
			{
				if( i >= RoomInfo._PersonPlayerMax)
					BattleSlotContext::i()->setSlotState( i, SLOT_STATE_CLOSE);
			}
		}

		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);			
	}
	else
	{
		m_bMyPhase = true;
		m_bAllowRefresh = true;
		i3UI::setDisableUIInput(false);
		EhStageError( EVENT_CREATE_ROOM, arg);
	}

	m_bWaitingFromServer = false;
}

void UIPhaseLobby::ReceiveGameEvent_Join_Room( INT32 arg, const i3::user_data&)
{
	DisableRoomTooltip();

	//g_pFramework->GetUIMainframe()->SetLockInput(false);	// hansoft 11845. 레디룸으로 페이즈 변경 성공 시 PS_ENTRANCE_END 시점에서 초기화 되므로 주석 처리함.
	//i3UI::setDisableUIInput(false);

	CCommunityContext::i()->CompleteMovingToUser();

	// 빠른입장으로 타 채널로 변경할 경우 서버 정보도 변경한다.
	if (MyRoomInfoContext::i()->IsQuickJoin())
	{
		g_pServerInfo->SetCurGameServerType(static_cast<SChannelType>(QuickJoinContext::i()->getChannelType()));
		g_pServerInfo->SetCurChannelNumber(QuickJoinContext::i()->getChannelIndex());
		g_pServerInfo->SetGameServer(QuickJoinContext::i()->getServerIndex(), S_CHANNEL_TYPE_ALL);
		GetMainFrame()->UpdateLocation();
	}

	if( EV_SUCCESSED(arg))
	{
		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);
	}
	else
	{
		// 입장에 실패 할 경우 Phase 이동이 아니라서 초기화가 되지 않습니다.
		// 다시 한 번 초대가 뜨기 위해 다시 세팅합니다. Jinsik.kim(2014.12.9)
		m_bWaitingFromServer = false;
		m_bAllowRefresh = true;

		g_pFramework->GetUIMainframe()->SetLockInput(false);	// hansoft 11845. 레디룸으로 페이즈 변경 실패 시 SetEventOnce() 락을 해제하여 다시 요청 가능토록.
		i3UI::setDisableUIInput(false);
		m_bMyPhase = true;
		if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL))			EhSlotFull();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_ROOM_JOIN_VOTE))					EhKickedByVote();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM))	EhNotExist();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD))	EhWrongPW();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER))	EhLimitInterenter();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_LEVEL))		EhLimitLever();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_TRAINING))		EhLimitTraining();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_CLAN_WAR_MERCENAY))				EhLimitMERC();
		else
			EhStageError( EVENT_JOIN_ROOM,arg);
	}
}

void UIPhaseLobby::ReceiveGameEvent_Leave_Lobby( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
		m_PreClanMemberLevel = (CLAN_MEMBER_LEVEL_TYPE)UserInfoContext::i()->GetMyClanState();

		// 장성급 베레모 지급여부 확인
		if(UserInfoContext::i()->IsGeneralRecvData())
		{
			if(UserInfoContext::i()->IsGeneralMsgBox())
				return;
			else
				UserInfoContext::i()->SetGeneralMsgBox();

			INVEN_BUFFER itemInfo = UserInfoContext::i()->GetGeneralRankInfo();

			// 장착 체크
			if(usf::GetItemStatus(itemInfo._ui32ItemID, itemInfo._TItemWareDBIdx) == ITEM_STATUS_EQUIPPED)
			{
				g_pFramework->EquipWeaponToMyInven(itemInfo._ui32ItemID, true);
			}

			GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION4"));
		}
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();

		m_bMyPhase = true;
		m_bAllowRefresh = true;
		GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}
}

void UIPhaseLobby::ReceiveGameEvent_Get_RoomAddInfo( INT32 arg, const i3::user_data&)
{
	m_bQueRoomInfo = true;
}

void UIPhaseLobby::ReceiveGameEvent_DestroyedNetwork( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseLobby::ReceiveGameEvent_Update_UserList( INT32 arg, const i3::user_data&)
{
	// 방 리스트와 유저 리스트 정보가 동시에 온다.
	_UpdateRoomList();

	// 방 리스트 갱신이되면 AddInfo도 다시 설정해야된다.
	RefreshSelectedRoom();

	if( m_is_sort_room_list == false )
		_SortRoomList();

	// Key : User Guide System
	{
			UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}


void UIPhaseLobby::ReceiveGameEvent_Ready_Box_Message( INT32 arg, const i3::user_data&)
{
	//보급 상자 채팅 메세지를 띈운다.
	_ApplyMsgReadyBox();
}

void UIPhaseLobby::ReceiveGameEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseLobby::ReceiveGameEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseLobby::ReceiveGameEvent_Attendance_Clear(INT32 arg, const i3::user_data&)
{
	i3::rc_wstring wstrTemp;
	MSG_TYPE eType = MSG_TYPE_ERROR;

	switch (arg)
	{
	case EVENT_ERROR_ATTENDANCE_SUCCESS:

		if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
			wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS");
		else
			return;
		
		eType = MSG_TYPE_GAME_OK;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS");
		break;
	case EVENT_ERROR_ATTENDANCE_PARAMETER_USER:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_PARAMETER_USER");
		break;
	case EVENT_ERROR_ATTENDANCE_NOT_ENOUGH:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_NOT_ENOUGH");
		break;
	case EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_ALREADY_COMPLETED");
		break;
	case EVENT_ERROR_ATTENDANCE_WRONG_VERSION:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_WRONG_VERSION");
		break;
	case EVENT_ERROR_ATTENDANCE_UNKWON:
	default:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_UNKNOWN");
		break;
	}

	// 매일 & 연속 이벤트의 경우는 출석체크 팝업이 Close 된 후 다음 이벤트 팝업을 검색하지만.
	// 기간내 1회 이벤트 팝업의 경우는 메세지박스를 누른 후 다음 이벤트팝업을 검색한다.
	// 해당 팝업 종류에 따른 MessageBox CallBack 설정을 위해 다음 분기를 사용한다.
	GameUI::MsgBox(eType, wstrTemp, nullptr, MAKE_CALLBACK(CbShowAttendancePopup), this);
}

void UIPhaseLobby::AttendanceOpenPopup(INT32 EvtType)
{
	if ( AttendanceContext::i()->IsAttendanceEvent((ATTENDANCE_TYPE)EvtType) &&
		 AttendanceContext::i()->IsEventTarget((ATTENDANCE_TYPE)EvtType)		)
	{
		switch (EvtType)
		{
		case ATTENDANCE_TYPE_DORMANT:
			GetMainFrame()->OpenPopup(UPW_DORMANT_EVENT);
			break;
		case ATTENDANCE_TYPE_NORMAL:
			GetMainFrame()->OpenPopup(UPW_MONTH_ATTENDANCE);
			break;
		default:
			I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
			break;
		}
	}
	else
	{
		i3::rc_wstring wstrString;
		
		// 이벤트가 없는 경우와 이벤트 대상이 아닌 경우. 이벤트 존재 유무룰 먼저 검사해야 합니다 
		if (!AttendanceContext::i()->IsAttendanceEvent((ATTENDANCE_TYPE)EvtType))
		{
			switch (EvtType)
			{
			case ATTENDANCE_TYPE_DORMANT:
				wstrString = GAME_RCSTRING("STR_TBL_UI_DORMANT_NOT_ATTENDANCE");
			break;
			case ATTENDANCE_TYPE_NORMAL:
				wstrString = GAME_STRING("STR_TBL_UI_ATTENDANCE_NOT_ATTENDANCE");
				break;
			default:
				I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
				break;
			}
		}
		else if (!AttendanceContext::i()->IsEventTarget((ATTENDANCE_TYPE)EvtType))
		{
			switch (EvtType)
			{
			case ATTENDANCE_TYPE_DORMANT:
				wstrString = GAME_RCSTRING("STR_TBL_UI_DORMANT_NOTUSER_ATTENDANCE");
				break;
			default:
				I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
				break;
			}
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrString, &GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"));
	}
}
