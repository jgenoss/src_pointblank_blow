#include "pch.h"
#include "UIFloatLobbyMapInfo_V11.h"

#include "UI/UIMainFrame.h"
#include "UI_V11/UIPhaseLobby_V11.h"
#include "UI/UIUtil.h"

#include "../TextSet.h"

#include "LobbyContext.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"

extern "C" {
	int UIFloatLobbyMapInfo_EnterRoom( LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame != nullptr)
		{
			UIPhaseBase * pLobby = pFrame->GetCurrentPhase();
			if( i3::same_of<UIPhaseLobby_V11*>(pLobby))
			{
				((UIPhaseLobby*)pLobby)->EnterRoom( -1);
			}
		}
		return 0;
	}

	int UIFloatLobbyMapInfo_Close( LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame != nullptr)
		{
			pFrame->DisableFloatWin(UFW_LOBBYMAPINFO);
			UIPhaseLobby_V11* pLobby = static_cast<UIPhaseLobby_V11*>(pFrame->GetCurrentPhase());
			pLobby->ResetSelectedElement(); // 닫기 버튼으로 누르면 룸 리스트에 셀렉트 된 인덱스를 초기화 해주어야 함
		}
		return 0;
	}
}

LuaFuncReg UIFloatLobbyMapInfo_Glue[] = {
	{"ClickEnter",				UIFloatLobbyMapInfo_EnterRoom},
	{"ClickClose",				UIFloatLobbyMapInfo_Close}, // 닫기 버튼 클릭으로 호출. V11에만 존재합니다.
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatLobbyMapInfo_V11);

UIFloatLobbyMapInfo_V11::UIFloatLobbyMapInfo_V11()
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;
	m_UIStyle = UI_STYLE_REOPENTYPE;

#if defined( I3_DEBUG)
	SetName( "LobbyMapInfo");
#endif
}

UIFloatLobbyMapInfo_V11::~UIFloatLobbyMapInfo_V11()
{
}

/*virtual*/ void UIFloatLobbyMapInfo_V11::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);
	
	// 랜덤맵 부스트 이벤트 스트링 세팅.
	LuaState * L = _CallLuaFunction("InitToolTip");

	if (L != NULL)
	{
		const i3::rc_wstring& wstrRMEvent = ClientStageInfoContext::i()->GetRandomMapEventString();
		i3Lua::PushStringUTF16To8(L, wstrRMEvent);	
		_EndLuaFunction(L, 1);
	}
}

/*virtual*/ void UIFloatLobbyMapInfo_V11::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PointBlankRe_Lobby_MapInfo_ToolTip.i3UIs", UIFloatLobbyMapInfo_Glue);
}

/*virtual*/ bool UIFloatLobbyMapInfo_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	
	SetMapInfo( static_cast<ROOM_INFO_BASIC*>(pArg1), static_cast<ROOM_INFO_ADD*>(pArg2));

	return true;
}
void	 UIFloatLobbyMapInfo_V11::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}

void UIFloatLobbyMapInfo_V11::SetMapInfo( ROOM_INFO_BASIC * pInfo, ROOM_INFO_ADD * pAddInfo)
{
	i3::rc_wstring wstageName;

	if( nullptr == pInfo || nullptr == pAddInfo )
	{	// 해당 방은 없어진경우..
		return;
	}
	
	STAGE_MODE	stageType	= StageID::GetStageMode( pInfo->_StageID);

	UINT8 GameSetPrim	= pAddInfo->_SubType & 0x0F;
	UINT8 GameSetSec	= pAddInfo->_SubType & 0xF0;

	// Map Image
	INT32 stageImageIdx;
	INT32 newMark = 0;

//#if defined (USE_RANDOMMAP_MODE)
	bool Random;
	bool bRandomMapEventShow = false;

	Random = (pInfo->_InfoFlag & ROOM_INFO_FLAG_RANDOM_MODE) ? true : false;
	if (Random)
	{
		stageImageIdx = 26;
		bRandomMapEventShow = ClientStageInfoContext::i()->IsRandomMapEventOpened();
	}
	else
		stageImageIdx = g_pFramework->GetMapShapeIdx( StageID::GetStageUID( pInfo->_StageID));
//#else	
//	stageImageIdx = g_pFramework->GetMapShapeIdx( (STAGE_ID) pInfo->_StageID);
//#endif

	//맵 이름 및 New 마크에 대한 표시를 설정 한다.
	INT32 typeImageIdx = -1;
	
	switch(stageType)
	{
	case STAGE_MODE_DEATHMATCH:			typeImageIdx = 2;	break;		//단체전
	case STAGE_MODE_BOMB:				typeImageIdx = 0;	break;		//폭파 미션
	case STAGE_MODE_DESTROY:			typeImageIdx = 3;	break;		//파괴 미션
	case STAGE_MODE_DEFENCE:			typeImageIdx = 6;	break;		//방어미션
	case STAGE_MODE_ANNIHILATION:		typeImageIdx = 1;	break;		//섬멸전
	case STAGE_MODE_ESCAPE:				typeImageIdx = 5;	break;		//탈출미션
	default:
		//챌린지(AI전)은 다른 형식으로 판단한다.
		if(StageID::GetAIMode(pInfo->_StageID) != AI_USE_NO)
		{
			typeImageIdx = 4;
		}
		break;
	}

	//New 마크 표시
	if ( CStageInfoUtil::GetStageData( pInfo->_StageID )->m_ui8Mark == GOODS_MARK_NEW )
	{
		newMark = 1;
	}

	
//#if defined (USE_RANDOMMAP_MODE)
	if(Random)
	{
		i3::safe_string_copy( wstageName, GAME_STRING("STR_POPUP_RANDOMMAP"), MAX_STRING_COUNT);
	}
	else
	{
		wstageName = CStageInfoUtil::GetMapData((UINT32)(pInfo->_StageID))->getDisplayName();
	}
//#else
//	getStageDisplayName(stageName, (STAGE_ID) pInfo->_StageID);
//#endif

// 게임모드
	i3::rc_wstring strModeName;

	{
		strModeName = CStageInfoUtil::GetRuleData(pInfo->_StageID)->getRuleName();
	}

	// 승리 조건
	i3::rc_wstring strWinObject;
	i3::safe_string_copy(strWinObject, TextSet::VictoryCondition(stageType, GameSetPrim), MAX_STRING_COUNT);
	strWinObject += L" /";
	strWinObject += TextSet::GameTime( GameSetSec);

	// 상태 모드
	i3::rc_wstring strState;
	if (ROOM_STATE_READY == pInfo->_State)
	{
		i3::safe_string_copy( strState, GAME_STRING("STR_TBL_GUI_LOBBY_WAIT_STATE_TEXT"), MAX_STRING_COUNT );/*대기중*/
	}	
	else
	{
		// 게임모드
		switch( stageType)
		{
		case STAGE_MODE_DEATHMATCH:// 단체전
			i3::sprintf(strState, L"%02d:%02d", (INT32)(pAddInfo->_NowIngTime / 60), (INT32)(pAddInfo->_NowIngTime % 60));
			break;
		case STAGE_MODE_ESCAPE			:// 보스미션전
		case STAGE_MODE_CROSSCOUNT		:// CC모드
		case STAGE_MODE_ANNIHILATION	:// 섬멸전
		case STAGE_MODE_DESTROY	: 
		case STAGE_MODE_DEFENCE	: 
		case STAGE_MODE_BOMB	: 
		default:			//미션
			{
				if (pAddInfo->_NowRoundCount == 0)
					i3::sprintf(strState, GAME_RCSTRING("STR_TBL_GUI_LOBBY_CURRENT_ROUND_COUNT"), 1);
				else
					i3::sprintf(strState, GAME_RCSTRING("STR_TBL_GUI_LOBBY_CURRENT_ROUND_COUNT"), pAddInfo->_NowRoundCount);
			}
			break;
		}
	}
    
	// 난입 제한
	i3::rc_wstring szInterEnter;
	if (0 == pAddInfo->_LockInterEnter)
		i3::safe_string_copy( szInterEnter, GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"), MAX_STRING_COUNT);/*없음*/
	else
		i3::safe_string_copy( szInterEnter, GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"), MAX_STRING_COUNT);/*제한*/

	// 무기 제한
	i3::rc_wstring szWeaponLimit;
	if (pInfo->_WeaponFlag == LOCK_BASIC)
		i3::safe_string_copy( szWeaponLimit, GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"), MAX_STRING_COUNT);/*없음*/
	else
		i3::safe_string_copy( szWeaponLimit, GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"), MAX_STRING_COUNT);/*제한*/
	
	// 관전제한
	i3::rc_wstring szObserverLimit;
	if( TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_CHASE) &&
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_FREE) &&
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY) &&
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		i3::safe_string_copy( szObserverLimit, GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER1"), MAX_STRING_COUNT );// 모두사용
	}
	else if( TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_1PVONLY) ||
			TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_CHASE) ||
			TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_FREE) ||
			TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY) ||
			TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		i3::safe_string_copy( szObserverLimit, GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER2"), MAX_STRING_COUNT );//사용
	}
	else
	{
		i3::safe_string_copy( szObserverLimit, GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER3"), MAX_STRING_COUNT );//사용하지 않음
	}

    // 팀밸런스
	i3::rc_wstring szBalance;
	if (pAddInfo->_LockTeamBalance == 0)
	{
		i3::safe_string_copy( szBalance, GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_OFF"), MAX_STRING_COUNT );/*없음*/
	}
	else if (pAddInfo->_LockTeamBalance == 1)
	{
		i3::safe_string_copy( szBalance, GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON"), MAX_STRING_COUNT );/*인원*/
	}
	else
	{
		i3::safe_string_copy( szBalance, GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"), MAX_STRING_COUNT ); /*실력*/
	}

	LuaState * L = _CallLuaFunction( "SetInfo");

	//  X-Mas Mark
	bool bXMasEnable = false;

	if(TESTBIT(CStageInfoUtil::GetStageData(pInfo->_StageID)->m_ui8Mark, GOODS_MARK_XMAS))
		bXMasEnable = true;

	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstageName);	// Map Name
		i3Lua::PushInteger( L, stageImageIdx);		// Map Image Idx
		i3Lua::PushInteger( L, newMark);			// New Mark
		i3Lua::PushBoolean( L , bXMasEnable ? true : false );	// X-Mas Mark
		i3Lua::PushBoolean(L, bRandomMapEventShow ? TRUE : FALSE);	// 랜덤맵 부스트 이벤트
		i3Lua::PushStringUTF16To8( L, strModeName);		// 모드
		i3Lua::PushStringUTF16To8( L, strWinObject);	// 승리 조건
		i3Lua::PushStringUTF16To8( L, pAddInfo->getNick());// 방장 Nick
		i3Lua::PushStringUTF16To8( L, strState);
		i3Lua::PushStringUTF16To8( L, szInterEnter);	// 난입 설정
		i3Lua::PushStringUTF16To8( L, szWeaponLimit);	// 무기 설정
		i3Lua::PushStringUTF16To8( L, szObserverLimit);	// 관전 설정
		i3Lua::PushStringUTF16To8( L, szBalance);	// 팀 밸런스 설정
		// 방장	NIck Color
	#ifdef USE_COLOR_NICK2
		i3Lua::PushColor( L, GetNickColor(pAddInfo->_NicklColor));
	#else
		COLOR color;
		i3Color::Set( &color, (UINT8) 255, 255, 255, 255);
		i3Lua::PushColor( L, &color);
	#endif
		_EndLuaFunction( L, 17);
	}
}
