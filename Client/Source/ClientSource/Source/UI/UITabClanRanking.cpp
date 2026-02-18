#include "pch.h"
#include "UITabClanRanking.h"

#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UIClanFunction.h"

#include "UIUtil.h"
#include "TextSet.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

#include "UserInfoContext.h"
#include "ClanSeasonContext.h"

namespace 
{
	enum { DEFAULT_SORT_TYPE = 3 };	// 누적 전적이 디폴트입니다. (CLAN_SORT_TOTAL_RECORD)
	CLAN_SOCORE_VIEW g_view_type = CSV_RECORD;
}

static UITabClanRanking * GetThis( void)
{
	UIPhaseClanMember* pPhase =	static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		return static_cast<UITabClanRanking*>(pPhase->GetTab(UTC_RANKING));
	}
	return nullptr;
}

//----------------------------------------------------------------------------------//
//								Lua Glue Functions									//
//----------------------------------------------------------------------------------//
extern "C" {
	int UITabClanRanking_SearchClan( LuaState * L)
	{
		char szStr[ MAX_PATH];
		if( i3Lua::GetStringArg( L, 1, szStr, sizeof(szStr)) != nullptr)
		{
			// UTF-8로 간주한다..(2014.08.26.수빈)
			i3::stack_wstring wstrRes;
			i3::utf8_to_utf16(szStr, wstrRes);

			UITabClanRanking * pTab = GetThis();
			if( pTab != nullptr)
				pTab->SearchClan( wstrRes );
		}
		return 0;
	}

	int UITabClanRanking_VisitClan( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);

		if(i32Slot > -1)
		{
			UITabClanRanking * pTab = GetThis();
			if( pTab != nullptr)
				pTab->VisitClan(i32Slot);
		}

		return 0;
	}

	int UITabClanRanking_SortClanList( LuaState * L)
	{
		INT32 i32SortType = i3Lua::GetIntegerArg(L , 1);

		UITabClanRanking * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SortClanList(i32SortType);

		return 0;
	}

	int UITabClanRanking_ChangeSlot( LuaState * L)
	{
		bool bAfter = i3Lua::GetBooleanArg(L , 1);

		UITabClanRanking * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeSlot(bAfter);

		return 0;
	}

	int UITabClanRanking_ChangeSlotToEnd( LuaState * L)
	{
		bool bAfter = i3Lua::GetBooleanArg(L , 1);

		UITabClanRanking * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeSlotToEnd(bAfter);

		return 0;
	}

	int UITabClanRanking_SelectedClan( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L , 1);

		UITabClanRanking * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SelectedClan(idx);

		return 0;
	}
}

LuaFuncReg UITabClanRanking_Glue[] = {
	{ "SearchClan",				UITabClanRanking_SearchClan},
	{ "VisitClan",				UITabClanRanking_VisitClan},
	{ "SortClanList",			UITabClanRanking_SortClanList},
	{ "ChangeSlot",				UITabClanRanking_ChangeSlot},
	{ "ChangeSlotToEnd",		UITabClanRanking_ChangeSlotToEnd},
	{"SelectedClan",			UITabClanRanking_SelectedClan		},
	{ nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UITabClanRanking);//, UITabClan_Normal);

UITabClanRanking::UITabClanRanking()
: m_iClanListidx	(-1)
, m_iBottomIdx		(0)
{

};

UITabClanRanking::~UITabClanRanking()
{

}

/*virtual*/ void UITabClanRanking::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabClan_Normal::_InitializeAtLoad( pScene);

	// Init Clan Slot
	{
		char szWndName[ MAX_NAME ];

		for( int i = 0; i < CLAN_RANK_COUNT; ++i )
		{
			i3::snprintf( szWndName, MAX_NAME, "FrameWnd_Item%d", i );
			i3UIFrameWnd* pWnd = (i3UIFrameWnd*) pScene->FindChildByName( szWndName );

			bool bRes = m_ClanSlot[i].Init( pWnd, i);
			I3ASSERT(bRes);
		}
	}
}

/*virtual*/ void UITabClanRanking::_ClearAtUnload( i3UIScene * pScene)
{
	for( INT32 i = 0; i < CLAN_RANK_COUNT; ++i )
	{
		m_ClanSlot[i].Init( nullptr, i);
	}

	UITabClan_Normal::_ClearAtUnload( pScene);
}

void UITabClanRanking::_SelectedSortType(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SelectedSortType");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, idx);
		_EndLuaFunction(L, 1);
	}
}

void UITabClanRanking::SelectedClan(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SelectedClanList");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, idx);
		_EndLuaFunction(L, 1);
	}
}

void UITabClanRanking::AllUnselectedClanList()
{
	LuaState * L = _CallLuaFunction("AllUnselectedClanList");
	if (L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
}

void UITabClanRanking::_UpdateClanList( void)
{
	INT32 i32ClanCnt = g_pFramework->GetClanContext()->GetClanListCount();

// 	if(i32ClanCnt <= 0)
// 		return;

	// 나누어 떨어질 경우 이전 페이지 표시해야 하므로 ..
	if ( i32ClanCnt % CLAN_RANK_COUNT == 0 )
	{
		m_iBottomIdx = (i32ClanCnt / CLAN_RANK_COUNT) - 1;
	}
	else
	{
		m_iBottomIdx = i32ClanCnt / CLAN_RANK_COUNT;
	}

	LuaState * L =_CallLuaFunction("UpdatePages");
	if ( L != nullptr )
	{
		i3Lua::PushInteger(L,m_iClanListidx + 1);
		i3Lua::PushInteger(L,m_iBottomIdx + 1);
		_EndLuaFunction(L , 2);
	}

	for(int i = 0 ; i < CLAN_RANK_COUNT ; i++)
	{
		SORT_CLAN_INFO * pSortClanInfo = nullptr;
		INT32 iRank = (m_iClanListidx * CLAN_RANK_COUNT) + i;
		if (iRank >= i32ClanCnt )
		{
			m_ClanSlot[i].SetSlotData(pSortClanInfo, i, iRank, g_view_type);
		}
		else
		{
			pSortClanInfo = g_pFramework->GetClanContext()->GetClanList(iRank);
			m_ClanSlot[i].SetSlotData(pSortClanInfo, i, iRank, g_view_type);
		}
	}
}

bool UITabClanRanking::_SearchClanListView( const i3::wliteral_range& wKeywordRng)
{	
	return g_pFramework->GetClanContext()->SearchClanListView( wKeywordRng) ? true : false;
}

void UITabClanRanking::_InitClanSortType( void)
{
	g_pFramework->GetClanContext()->InitSortClanList(DEFAULT_SORT_TYPE);

	LuaState * L = _CallLuaFunction("InitSortState");
	_EndLuaFunction(L , 0);

	m_iClanListidx = 0;
	g_view_type = CSV_RECORD;

	_UpdateClanList();
}

bool UITabClanRanking::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	bool bRv = UITabClan_Normal::OnEvent( event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_RETURN:
			{
				i3UIEditBox* pEditBox = (i3UIEditBox*)GetScene(0)->FindChildByName("Edit_Search");
				SearchClan(pEditBox->getText());
			}
			break;
		}
	}

	return bRv;
}

bool UITabClanRanking::OnKey_Enter()
{
	if( GetScene(0) )
	{
		if( g_pEnvSet->IsBaseOnUIVersion_1_0() )
		{
			i3UIEditBox* pEditBox = (i3UIEditBox*)GetScene(0)->FindChildByName("Edit_Search");
			if( pEditBox )
			{
				if( !pEditBox->isFocused() )
					pEditBox->OnSetFocus();
			}
		}
	}
	return true;
}

void UITabClanRanking::_InitRadioToolTip(void)
{
	i3::rc_wstring wstrToolTip[4];

	{
		const CLAN_SEASON_INFO * pInfo = ClanSeasonContext::i()->GetCurrentSeason();
		const SYSTEMTIME _StartTime = pInfo->GetSeasonStartDate();
		const SYSTEMTIME _EndTime = pInfo->GetSeasonEndDate();

		i3::sprintf(wstrToolTip[0], GAME_STRING("STBL_IDX_CLAN_RANKING_CURRENT_SCORE"), 
			_StartTime.wYear, _StartTime.wMonth, _StartTime.wDay, _EndTime.wYear, _EndTime.wMonth, _EndTime.wDay);
	}

	{
		const CLAN_PRESEASON_INFO * pInfo = ClanSeasonContext::i()->GetPreSeasonInfo();
		const SYSTEMTIME _StartTime = pInfo->GetSeasonStartDate();
		const SYSTEMTIME _EndTime = pInfo->GetSeasonEndDate();

		i3::sprintf(wstrToolTip[1], GAME_STRING("STBL_IDX_CLAN_RANKING_BEFORE_SCORE"), 
			_StartTime.wYear, _StartTime.wMonth, _StartTime.wDay, _EndTime.wYear, _EndTime.wMonth, _EndTime.wDay);
	}

	wstrToolTip[2] = GAME_STRING("STBL_IDX_CLAN_RANKING_TOTAL_SCORE");
	wstrToolTip[3] = GAME_STRING("STBL_IDX_CLAN_RANKING_MYCLAN_SCORE");

	LuaState * L = _CallLuaFunction("InitToolTip");

	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrToolTip[0]);
		i3Lua::PushStringUTF16To8(L, wstrToolTip[1]);
		i3Lua::PushStringUTF16To8(L, wstrToolTip[2]);
		i3Lua::PushStringUTF16To8(L, wstrToolTip[3]);
		_EndLuaFunction(L , 4);
	}
}

void UITabClanRanking::_SetScoreLabel(INT32 sort_idx)
{
	if( sort_idx == 1 )			g_view_type = CSV_CUR_CLANPOINT;
	else if( sort_idx == 2 )	g_view_type = CSV_PRE_CLANPOINT;
	else						g_view_type = CSV_RECORD;

	i3::rc_wstring wstr_label = GAME_RCSTRING("STBL_IDX_CLAN_USER_RECORD");
	if( g_view_type != CSV_RECORD )
		wstr_label = GAME_RCSTRING("STR_CLAN_RANK_SORT_CLAN_POINT");

	LuaState * L = _CallLuaFunction("SetScoreLabel");

	if( L != nullptr)	
	{
		i3Lua::PushStringUTF16To8(L , wstr_label);
		_EndLuaFunction(L , 1);
	}
}

/*virtual*/ void UITabClanRanking::OnCreate( i3GameNode * pParent)
{
	UITabClan_Normal::OnCreate( pParent);

	AddScene("Scene/Clan/PointBlankRe_Clan_Ranking.i3UIs", UITabClanRanking_Glue);
}

/*virtual*/ void UITabClanRanking::OnUpdate( REAL32 rDeltaSeconds)
{
	UITabClan_Normal::OnUpdate( rDeltaSeconds);


	//  Client 클랜리스트 정보
	if( g_pFramework->GetClanContext()->IsUpdatedClanList())
	{
		_UpdateClanList();
		g_pFramework->GetClanContext()->SetUpdatedClanList( false);
	}

}

/*virtual*/ void UITabClanRanking::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UITabClan_Normal::ProcessGameEvent( event, arg, UserData);

	switch( event)
	{
	case EVENT_CLAN_DETAIL_INFO :	
		break;
	}
}

/*virtual*/ bool UITabClanRanking::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UITabClan_Normal::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_SearchClanListView(L"");
	_InitClanSortType();

	m_iClanListidx = 0;
	_UpdateClanList();

	LuaState * L = _CallLuaFunction("InitFrames");
	_EndLuaFunction(L , 0);

	_InitRadioToolTip();

	_SelectedSortType(DEFAULT_SORT_TYPE);

	return true;
}

void UITabClanRanking::SearchClan( const i3::wliteral_range& wKeywordRng)
{
	if( _SearchClanListView(wKeywordRng))
	{
//		_InitClanSortType();
		_UpdateClanList();
	}
}

void UITabClanRanking::VisitClan(INT32 i32Slot)
{
	CClanContext * pClanCxt = g_pFramework->GetClanContext();
	
	// 실제 인덱스가 클랜 수보다 크면 리턴
	if(i32Slot >= pClanCxt->GetClanListCount())
		return;

	SORT_CLAN_INFO * pSortClanInfo = pClanCxt->GetClanList( (m_iClanListidx * CLAN_RANK_COUNT) + i32Slot);
	
	// 자기 클랜일 경우에는 방문 버튼 비활성화합니다.
	USER_INFO_BASIC stUserInfo;
	UserInfoContext::i()->GetMyUserInfoBasic( &stUserInfo);

	if(pSortClanInfo->_info._idx == stUserInfo.m_ui32Clanidx)
		return;

	pClanCxt->SendClanDetailInfoNEvent(pSortClanInfo->_info._idx, 1);
	pClanCxt->SetClanVisit(true);
}

void UITabClanRanking::SortClanList(INT32 i32SortType)
{
	g_pFramework->GetClanContext()->SortClanList(i32SortType);

	_SelectedSortType(i32SortType);

	m_iClanListidx = 0;

	_SetScoreLabel(i32SortType);
	_UpdateClanList();

	AllUnselectedClanList();
}

void UITabClanRanking::ChangeSlot(bool bAfter)
{
	if ( bAfter )
	{
		if( m_iBottomIdx > (UINT32)m_iClanListidx)
		{
			m_iClanListidx ++;
		}
		else
		{
			m_iClanListidx =  m_iBottomIdx;
		}
	}
	else
	{
		m_iClanListidx --;
		if ( m_iClanListidx < 0)
			m_iClanListidx = 0;
	}

	_UpdateClanList();
	AllUnselectedClanList();
}

void UITabClanRanking::ChangeSlotToEnd(bool bAfter)
{
	if ( bAfter )
	{
		m_iClanListidx =  m_iBottomIdx;
	}
	else
	{
		m_iClanListidx = 0;
	}

	_UpdateClanList();
	AllUnselectedClanList();
}