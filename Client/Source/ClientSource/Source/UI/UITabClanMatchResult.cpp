#include "pch.h"
#include "UITabClanMatchResult.h"
#include "UIClanFunction.h"
#include "GameGUI.h"
#include "UIMainFrame.h"

#include "UIUtil.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/substr.h"

#include "ClanGameContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClientStageInfoUtil.h"

static UITabClanMatchResult* GetThis( void)
{
	UIPhaseClanMember* pPhase =	static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		return static_cast<UITabClanMatchResult*>(pPhase->GetTab(UTC_RESULT));
	}
	return nullptr;
}


extern "C"
{
	int UITabClanMatchResult_MRListScroll( LuaState* L)
	{
		UITabClanMatchResult * pTab = GetThis();
		if( pTab != nullptr)
			pTab->MatchResultListScroll();
		return 0;
	}

	int UITabClanMatchResult_ClanListWheel( LuaState * L)
	{
		INT32 zDelta = i3Lua::GetIntegerArg( L, 1);
		UITabClanMatchResult * pTab = GetThis();
		if( pTab != nullptr)
			pTab->MatchResultListWheel(zDelta);
		return 0;
	}
}

LuaFuncReg UITabClanMatchResult_Glue[] =
{
	{ "MRListScroll",		UITabClanMatchResult_MRListScroll },
	{ "MRListWheel",		UITabClanMatchResult_ClanListWheel },
	{nullptr,	nullptr}
};

I3_CLASS_INSTANCE( UITabClanMatchResult);//, UITabClan_Normal);

UITabClanMatchResult::UITabClanMatchResult()
{
	m_pMRListScroll = nullptr;
	m_hdc = nullptr;

	m_nSubjectWidth = 0;
	m_nEllipseWidth = 0;

	m_MatchList.clear();

	RegisterGameEventFunc(EVENT_CLAN_WAR_RESULT, &UITabClanMatchResult::ReceiveGameEvent_MatchResult);
}

UITabClanMatchResult::~UITabClanMatchResult()
{
	m_pMRListScroll = nullptr;
	
	if( nullptr != m_hdc )
	{
		::ReleaseDC(g_hWnd, m_hdc);
	}
}

/*virtual*/ void UITabClanMatchResult::_InitializeAtLoad(i3UIScene * pScene)
{
	UITabClan_Normal::_InitializeAtLoad(pScene);

	// Scroll Bar
	{
		m_pMRListScroll = (i3UIScrollBar *) pScene->FindChildByName("ScrollBar_MatchResult");
		m_pMRListScroll->setTickSize( 1 );
		m_pMRListScroll->EnableCtrl( false);
	}
}

/*virtual*/ void UITabClanMatchResult::_ClearAtUnload(i3UIScene * pScene)
{
	m_pMRListScroll = nullptr;

	{
		if( nullptr != m_hdc )
			::ReleaseDC(g_hWnd, m_hdc);

		m_hdc = nullptr;

	}

	UITabClan_Normal::_ClearAtUnload(pScene);
}

void UITabClanMatchResult::_InitUI(void)
{
	LuaState * L = _CallLuaFunction("InitUI");

	if(L != nullptr)
	{
		_EndLuaFunction(L , 0);
	}
}

void UITabClanMatchResult::_InitScrollBar(void)
{
	I3ASSERT(m_pMRListScroll != nullptr);

	INT32 i32Count = m_MatchList.size();

	if(i32Count > CLAN_MATCH_RESULT_SLOT_COUNT )
	{
		m_pMRListScroll->setScrollRange( 0, i32Count-3);
		m_pMRListScroll->setPageSize( 0 );
		m_pMRListScroll->setScrollPos( 0 );

		i3UIManager* pMgr = i3UI::getManager();

		if( pMgr )
			pMgr->SetFocusControl( m_pMRListScroll );

		m_pMRListScroll->EnableCtrl( true);	
	}
	else
	{
		m_pMRListScroll->setScrollRange( 0, 0);
		m_pMRListScroll->setPageSize( 0 );
		m_pMRListScroll->setScrollPos( 0 );
		m_pMRListScroll->EnableCtrl( false);	
	}

	
}

void UITabClanMatchResult::_UpdateMatchResultList(void)
{
	INT32 i32Count = m_MatchList.size();

	INT32 idx = 0;


	for(INT32 i = 0 ; i < CLAN_MATCH_RESULT_SLOT_COUNT ; ++i)
	{
		idx = (m_pMRListScroll->getScrollPos() + i);

		if(i < i32Count)
		{
			_SetMatchResultInfo(i, &m_MatchList[idx]);
			_SetMatchResultStageInfo(i, &m_MatchList[idx]);
		}
		else
		{
			_SetMatchResultInfo(i, nullptr);
			_SetMatchResultStageInfo(i, nullptr);
		}
	}
}

void UITabClanMatchResult::_SetMatchResultInfo(INT32 i32Slot, const CLAN_WAR_RESULT * pInfo )
{
	LuaState * L = _CallLuaFunction("SetMRInfo");

	if (L == nullptr)
		return;

	if ( pInfo != nullptr)
	{
		// 필요한 계산 먼저
		INT32 i32ResultIdx = 0;
		switch (pInfo->m_Result)
		{
		case ROOM_ENDTYPE_WIN: i32ResultIdx = 1; break;
		case ROOM_ENDTYPE_DRAW: i32ResultIdx = 2; break;
		case ROOM_ENDTYPE_LOSE: i32ResultIdx = 0; break;
		}

		i3::stack_wstring wstrScore;
		i3::sprintf(wstrScore, L"%d:%d", pInfo->m_MyClanScore, pInfo->m_OtherClanScore);

		i3Lua::PushInteger(L, i32Slot);
		i3Lua::PushBoolean(L, true);
		i3Lua::PushInteger(L, i32ResultIdx);
		i3Lua::PushStringUTF16To8(L, pInfo->m_strClanName);
		i3Lua::PushStringUTF16To8(L, wstrScore);

		_EndLuaFunction(L, 5);		
	}
	else
	{
		i3Lua::PushInteger(L , i32Slot);
		i3Lua::PushBoolean(L , false);
		i3Lua::PushInteger(L , -1);
		i3Lua::PushStringUTF16To8(L , L"");
		i3Lua::PushStringUTF16To8(L , L"");
		_EndLuaFunction(L, 5);
	}


}

void UITabClanMatchResult::_SetMatchResultStageInfo(INT32 i32Slot, const CLAN_WAR_RESULT * pInfo)
{
	LuaState * L = _CallLuaFunction("SetMRStageInfo");

	if (L == nullptr)
		return;

	if( pInfo != nullptr)
	{
		i3::rc_wstring wstrMapName  = L" ";
		i3::rc_wstring wstrModeName = L" ";
		if(CStageInfoUtil::GetMapData((UINT32)pInfo->m_ui32StageID))	wstrMapName = CStageInfoUtil::GetMapData((UINT32)pInfo->m_ui32StageID)->getDisplayName();
		if(CStageInfoUtil::GetRuleData(pInfo->m_ui32StageID))			wstrModeName = CStageInfoUtil::GetRuleData(pInfo->m_ui32StageID)->getRuleName();
		
		INT32 i32Idx = g_pFramework->GetMapShapeIdx(StageID::GetStageUID(pInfo->m_ui32StageID));

		i3Lua::PushInteger(L , i32Slot);
		i3Lua::PushInteger(L , i32Idx);
		i3Lua::PushStringUTF16To8(L , wstrMapName);
		i3Lua::PushStringUTF16To8(L , wstrModeName);
		_EndLuaFunction(L, 4);
	}
	else
	{
		i3Lua::PushInteger(L , i32Slot);
		i3Lua::PushInteger(L , -1);
		i3Lua::PushStringUTF16To8(L , L"");
		i3Lua::PushStringUTF16To8(L , L"");
		_EndLuaFunction(L, 4);
	}
}

/*virtual*/ void UITabClanMatchResult::OnCreate( i3GameNode * pParent)
{
	UITabClan_Normal::OnCreate(pParent);
	
	AddScene("Scene/Clan/PointBlankRe_Clan_Match.i3UIs", UITabClanMatchResult_Glue);
}

/*virtual*/ bool UITabClanMatchResult::OnEntranceStart(void * pArg1 /* = nullptr  */, void * pArg2 /* = nullptr */ )
{
	if(UITabClan_Normal::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	_InitUI();

	GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_RESULT);
	
	return true;
}

/*virtual*/ void UITabClanMatchResult::OnEntranceEnd(void)
{
	UITabClan_Normal::OnEntranceEnd();

	_InitScrollBar();
}

void UITabClanMatchResult::RegisterGameEventFunc(GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type(evt, f));
}

/*virtual*/ void UITabClanMatchResult::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UITabClan_Normal::ProcessGameEvent( event, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find(event);

	if (it != m_EventFuncMap.end())
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

/*virtual*/ void UITabClanMatchResult::OnUpdate(REAL32 rDeltaSeconds)
{
	UITabClan_Normal::OnUpdate(rDeltaSeconds);
}


void UITabClanMatchResult::MatchResultListScroll(void)
{
	_UpdateMatchResultList();
}

void UITabClanMatchResult::MatchResultListWheel(INT32 zDelta)
{
	I3ASSERT_RETURN(m_pMRListScroll);

	INT32 i32Count = m_MatchList.size();
	INT32 icurrentpos = m_pMRListScroll->getScrollPos();
	INT32 iAddRange = MINMAX( -1, zDelta, 1 );

	if((i32Count-3 < icurrentpos) && (iAddRange < 0))
		return;

	if( i32Count > CLAN_MATCH_RESULT_SLOT_COUNT )
	{
		int nPos = icurrentpos - iAddRange;//(int)(zDelta / 120);
		nPos = MINMAX( 0, nPos, i32Count );

		m_pMRListScroll->setScrollPos( nPos );
		
		_UpdateMatchResultList();
	}	
}

void UITabClanMatchResult::ReceiveGameEvent_MatchResult(INT32 arg, const i3::user_data& data)
{
	if (EV_SUCCESSED(arg))
	{
		m_MatchList.clear();

		PACKET_CLAN_WAR_RESULT_ACK RecvData = i3::value_cast<PACKET_CLAN_WAR_RESULT_ACK>(data);
		CLAN_WAR_RESULT* Result = &RecvData.m_aClanWarResult;
		
		int count = RecvData.m_aClanWarResult.GetCount();
		
		for (int i = 0; i < count; ++i)
		{	
			m_MatchList.push_back(Result[i]);
		}

		// m_i32MatchIdx 값이 큰 게 최근 전적. 혹시 모르니 정렬해준다
		struct fn_sort
		{
			bool operator()(const CLAN_WAR_RESULT & lhs, const CLAN_WAR_RESULT & rhs) const
			{
				return lhs.m_i32MatchIdx > rhs.m_i32MatchIdx;
			}
		};

		i3::sort(m_MatchList.begin(), m_MatchList.end(), fn_sort());
		
		_UpdateMatchResultList();
		_InitScrollBar();
	}
}