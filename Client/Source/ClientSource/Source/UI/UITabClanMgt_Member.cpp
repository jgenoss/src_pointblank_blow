#include "pch.h"
#include "UITabClanMgt_Member.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UIUtil.h"

#include "ClanDef.h"
#include "../QuestTextDictionary.h"
#include "../CommunityContext.h"
#include "ClanContext.h"

I3_CLASS_INSTANCE( UITabClanMgt_Member, UITabClan_Mgt);

static UITabClanMgt_Member * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UIPhaseBase* pPhase = pFrame->GetCurrentSubPhase();
	UITabClan* pClan = static_cast<UIPhaseClanMember*>(pPhase)->GetCurrTab();
	return static_cast<UITabClanMgt_Member*>(pClan);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UISelect_UserAuth(LuaState * L)
	{
		INT32 type = i3Lua::GetIntegerArg(L, 1);
		GetThis()->OnManagementInstructionPopup(type);
		return 0;
	}

	int UISelect_Staff_Authority(LuaState * L)
	{
		GetThis()->OnManagementStaffAuthModify();
		return 0;
	}

	int UISelect_Select_UserExpell(LuaState * L)
	{
		GetThis()->OnManagementexpellend();
		return 0;
	}
}

LuaFuncReg UITabClanMgt_Member_Glue[] = {
	{"Select_UserAuth",			UISelect_UserAuth			},
	{"Staff_Authority",			UISelect_Staff_Authority	},
	{"Select_UserExpell",		UISelect_Select_UserExpell	},
	{NULL,						NULL						},
};

//------------------------------------------------------------------------------//


UITabClanMgt_Member::UITabClanMgt_Member()
{
	m_pClanContext = g_pFramework->GetClanContext();
}

UITabClanMgt_Member::~UITabClanMgt_Member()
{

}

void UITabClanMgt_Member::_UpdateClanManagementMemberPhase(REAL32 rDeltaSeconds)
{
	CLAN_DETAIL_INFO * pClanDetailInfo = m_pClanContext->GetMyClanDetailInfo();
	I3ASSERT( pClanDetailInfo);

	//	클랜원 정보 업데이트
	m_pClanContext->UpdateClanMember(rDeltaSeconds);
}

void UITabClanMgt_Member::_UpdateSortClanMemberList(void)
{
	INT32 nNewCount = m_pClanContext->GetMyClanMemberCount(); 
	INT32 nOldCount = (INT32)m_listManageMember.size();

	//	클랜원 정렬 리스트 재구성
	if( nOldCount != nNewCount)
	{
		m_listManageMember.clear();

		for(INT32 i=0; i<nNewCount; i++)
		{
			const CLAN_MEMBER_BASIC_INFO * pInfo = m_pClanContext->GetMyClanMember(i);

			if( pInfo->_i64UID > 0)
			{
				i3mem::Copy( &m_ManageMemberInfo[i]._info, pInfo, sizeof(CLAN_MEMBER_BASIC_INFO));

				m_listManageMember.push_back(&m_ManageMemberInfo[i]);
			}
			else
			{
				i3mem::FillZero( &m_ManageMemberInfo[i], sizeof(m_ManageMemberInfo[i]));
			}
		}	

		if( nOldCount > nNewCount)	//	자투리 부분을 초기화 한다.
		{
			for(INT32 i=nNewCount; i<nOldCount; i++)
				i3mem::FillZero( &m_ManageMemberInfo[i], sizeof(m_ManageMemberInfo[i]));
		}
	}
	//	변경된 정보만 갱신
	else
	{
		for(INT32 i=0; i<nNewCount; i++)
		{
			const CLAN_MEMBER_BASIC_INFO * pInfo = m_pClanContext->GetMyClanMember(i);
			i3mem::Copy( &m_ManageMemberInfo[i]._info, pInfo, sizeof(CLAN_MEMBER_BASIC_INFO));
		}
	}
}

void UITabClanMgt_Member::_SetManagementListSlot(INT32 idx, ManagementClanMemberInfo *pMember)
{
	I3ASSERT( pMember != NULL);
	char szTemp[MAX_STRING_COUNT] = "";

	_SetUserIndex(idx);

	_SetNameUsingColorTable(idx, pMember->_info._nick, pMember->_info._color); // 닉네임

	GetMemberLevelName2(pMember->_info._level, szTemp, MAX_STRING_COUNT);
	_SetUserClass(idx, szTemp); // 멤버 계급
	//m_slotManagementList[nIdx].pClass->SetText(szTemp);

	//서버쪽 미 구현 된 상태 그래서 주석 처리 햇음
	//클랜원 최종 접속 날짜 표시 
	//if(pMember->_info._date > 0)
	//{		
	//	//	"%04u년 %02u월 %02u일"
	//	sprintf_s(szTemp, GAME_STRING("STBL_IDX_CLAN_DECLARE_DATE"), 
	//		(INT32)GetEntryYear(pMember->_info._date), 
	//		(INT32)GetEntryMonth(pMember->_info._date), 
	//		(INT32)GetEntryDay(pMember->_info._date));
	//	_SetEntryDay(idx, szTemp); // 가입일
	//}
	//else
	//{
	//	_SetEntryDay(idx, "");
	//}

	_SetRankImage(idx, (INT32)(pMember->_info._rank));

	INT32 p_State = FRIEND_STATE_64_TO_32(pMember->_info._state);
	switch( GET_FRIEND_STATE( p_State ) )
	{
	case FRIEND_STATE_OFFLINE:
		_SetConnectState(idx,"Offline");
		break;
	case FRIEND_STATE_ONLINE:
	case FRIEND_STATE_ROOM:
		_SetConnectState(idx,"Online");
		break;
	}
}


BOOL UITabClanMgt_Member::SetUserValue(INT64 *i64UID, INT32 *count)
{
	for(size_t i=0; i<m_listManageMember.size(); i++)
	{
		if( i >= MAX_MEMBER_COUNT)
			continue;

		ManagementClanMemberInfo * pMember = m_listManageMember[i];

		if( m_ListBox->isSelectedData(i) && pMember->_info._i64UID > 0 && pMember->_info._i64UID == m_pClanContext->GetMyDBIdx())
			return FALSE;
		//isSelectedData
		if( m_ListBox->isSelectedData(i) && pMember->_info._i64UID > 0 && pMember->_info._i64UID != m_pClanContext->GetMyDBIdx() &&
			pMember->_info._level != (UINT8) CLAN_MEMBER_LEVEL_MASTER)
		{
			//m_List[i]
			I3_BOUNDCHK( *count, MAX_MEMBER_COUNT);

			i64UID[*count] = pMember->_info._i64UID;
			(*count)++;
		}
	}
	return TRUE;
}


void UITabClanMgt_Member::OnManagementexpellend()
{
	INT64 i64UID[MAX_MEMBER_COUNT] = {0,};
	INT32 count = 0;

	if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_DEPORTATION ) ) 
	{
		//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));
	}
	else
	{
		if(SetUserValue(i64UID, &count))
			m_pClanContext->ProcessDismissalMember(i64UID, count);
		else
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MYSELF"));
	}
}

void UITabClanMgt_Member::OnManagementInstructionPopup(INT32 nItemIdx)
{	
#if defined(TEST_CLAN_MEMBER_LIST)
	GameUI::MsgBox( MSG_TYPE_GAME_OK, "TEST_CLAN_MEMBER_LIST");
	return;
#endif

	INT64 i64UID[MAX_MEMBER_COUNT] = {0,};
	INT32 count = 0;


	switch(nItemIdx)
	{
	case 1:		//	스텝 임명
		{
			if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"마스터 권한이 필요합니다."
			}
			else
			{
				if(SetUserValue(i64UID, &count))
					m_pClanContext->ProcessAppointMember(i64UID, count);
				else
					;
			}
		}			 
		break;
	case 2:		//	마스터 위임	
		{
			if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));		//	"마스터 권한이 필요합니다."
			}
			else
			{
				if(SetUserValue(i64UID, &count))
					m_pClanContext->ProcessCommissionMaster(i64UID, count);
				else
					;
			}
		}
		break;
	case 3:		//	일반으로 강등
		{
			if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"마스터 권한이 필요합니다."
			}
			else
			{
				if(SetUserValue(i64UID, &count))
					m_pClanContext->ProcessDemoteMember(i64UID, count);
				else
					;
			}
		}
		break;
	}
}

void UITabClanMgt_Member::OnManagementStaffAuthModify()
{
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"마스터 권한이 필요합니다."
		InitCheckBox();
		return;
	}
	CLAN_MANAGEMENT modify;

	modify._limitRank = m_pClanContext->GetMyClanDetailInfo()->m_ranklock;
	modify._limitAgeLow = m_pClanContext->GetMyClanDetailInfo()->m_lagelock;
	modify._limitAgeHigh = m_pClanContext->GetMyClanDetailInfo()->m_hagelock;

	UINT8 wr = (UINT8)(Box[0]->isState(I3UI_STATE_SELECTED));
	UINT8 in = (UINT8)(Box[1]->isState(I3UI_STATE_SELECTED));
	UINT8 de = (UINT8)(Box[2]->isState(I3UI_STATE_SELECTED));
	UINT8 ac = (UINT8)(Box[3]->isState(I3UI_STATE_SELECTED));

	modify._authority = CLAN_MENAGE_AUTHORITY( wr, in, de, ac);

	if( m_pClanContext->GetMyClanDetailInfo()->m_authstaff	!= modify._authority	||
		m_pClanContext->GetMyClanDetailInfo()->m_ranklock	!= modify._limitRank	||
		m_pClanContext->GetMyClanDetailInfo()->m_lagelock	!= modify._limitAgeLow	||
		m_pClanContext->GetMyClanDetailInfo()->m_hagelock	!= modify._limitAgeHigh	)
	{
		m_pClanContext->GetMyClanDetailInfo()->m_authstaff	= modify._authority;
		m_pClanContext->GetMyClanDetailInfo()->m_ranklock	= modify._limitRank;
		m_pClanContext->GetMyClanDetailInfo()->m_lagelock	= modify._limitAgeLow;
		m_pClanContext->GetMyClanDetailInfo()->m_hagelock	= modify._limitAgeHigh;	
		//	서버로 변경 사항을 보냄
		m_pClanContext->ProcessManagementModify( &modify);
	}
}

void UITabClanMgt_Member::InitState()
{
	InitCheckBox();
}

void UITabClanMgt_Member::InitCheckBox()
{
	INT32 ClanAuthority[STAFF_AUTHORITY];
	ClanAuthority[0] = GET_CLAN_MENAGE_WRITE( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	ClanAuthority[1] = GET_CLAN_MENAGE_INVITED( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	ClanAuthority[2] = GET_CLAN_MENAGE_DEPORTATION( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	ClanAuthority[3] = GET_CLAN_MENAGE_ACCEPT( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);

	LuaState * L = _CallLuaFunction("InitCheckBox");
	for(INT i=0; i<STAFF_AUTHORITY; i++)
	{
		i3Lua::PushInteger(L, ClanAuthority[i]);
	}
	_EndLuaFunction(L, STAFF_AUTHORITY);
}

void UITabClanMgt_Member::InitAuthority()
{
	LuaState * L = _CallLuaFunction("InitAuthority");
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER )
		i3Lua::PushBoolean(L, true);
	else
		i3Lua::PushBoolean(L, false);
	_EndLuaFunction(L,1);
}

/*virtual*/ void UITabClanMgt_Member::_InitializeAtLoad( i3UIScene * pScene)
{
	base::_InitializeAtLoad(pScene);
	Box[0] = (i3UICheckBox *)pScene->FindChildByName("i3UICheckBox");
	Box[1] = (i3UICheckBox *)pScene->FindChildByName("i3UICheckBox0");
	Box[2] = (i3UICheckBox *)pScene->FindChildByName("i3UICheckBox1");
	Box[3] = (i3UICheckBox *)pScene->FindChildByName("i3UICheckBox2");

	m_ListBox = (i3UIListView_Box *)pScene->FindChildByName("i3UIMemberList");
}

void UITabClanMgt_Member::_SetUserIndex(INT32 idx)
{	
	LuaState * L = _CallLuaFunction("SetUserIndex");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L,1);
}

void UITabClanMgt_Member::_SetNameUsingColorTable(INT32 idx, char *pszName, UINT8 color)
{
	LuaState * L = _CallLuaFunction("SetNameUsingColorTable");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushA2UTF8String(L, pszName);
	i3Lua::PushInteger(L, (INT32)color);
	_EndLuaFunction(L,3);
}

void UITabClanMgt_Member::_SetUserClass(INT32 idx, char *userClass)
{
	LuaState * L = _CallLuaFunction("SetUserClass");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushA2UTF8String(L, userClass);
	_EndLuaFunction(L,2);
}

void UITabClanMgt_Member::_SetEntryDay(INT32 idx, char *entryDay)
{
	LuaState * L = _CallLuaFunction("SetEntryDay");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushA2UTF8String(L, entryDay);
	_EndLuaFunction(L,2);
}

void UITabClanMgt_Member::_SetRankImage(INT32 idx, INT32 ImageIndex)
{
	LuaState * L = _CallLuaFunction("SetRankImage");
	i3Lua::PushInteger(L,idx);
	i3Lua::PushInteger(L,ImageIndex);
	_EndLuaFunction(L,2);
}

void UITabClanMgt_Member::_SetConnectState(INT32 idx, char *userState)
{
	LuaState * L = _CallLuaFunction("SetConnectState");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushA2UTF8String(L, userState);
	_EndLuaFunction(L,2);
}

/*virtual*/ void UITabClanMgt_Member::OnCreate(i3GameNode * pParent)
{
	base::OnCreate(pParent);
	//LoadScene
	char * pszPath = "UIRe/PBRe_ClanMgt_Member.i3UIe";
	AddScene( pszPath,UITabClanMgt_Member_Glue);
}

/*virtual*/ void UITabClanMgt_Member::OnUpdate(REAL32 rDeltaSeconds)
{
	base::OnUpdate( rDeltaSeconds);
	_UpdateClanManagementMemberPhase(rDeltaSeconds);
}

/*virtual*/ void UITabClanMgt_Member::ProcessGameEvent(INT32 event, INT32 arg)
{
	base::ProcessGameEvent(event,arg);

	switch(event)
	{
		case EVENT_CLAN_MEMBER_LIST:
		{
			if( EV_SUCCESSED(arg))
			{
				//  멤버 리스트 정보 재구성
				_UpdateSortClanMemberList();
				const size_t count = m_listManageMember.size();

				LuaState * L = _CallLuaFunction("DeleteAllList");
				i3Lua::PushInteger(L, m_pClanContext->GetMyClanMemberCount());
				_EndLuaFunction( L, 1);

				for(size_t i=0; i < count; i++)
				{
					ManagementClanMemberInfo * pMember = m_listManageMember[i];
					_SetManagementListSlot(i, pMember);	//	슬롯 값 삽입
				}
			}
		}
		break;
	
	}
	LuaState * L = _CallLuaFunction("SetStaffCombo_Default");
	_EndLuaFunction(L,0);
}

/*virtual*/ void UITabClanMgt_Member::OnLoadAllScenes()
{
	base::OnLoadAllScenes();
	i3mem::FillZero( m_ManageMemberInfo, sizeof(m_ManageMemberInfo));
	m_listManageMember.clear();
	//	서버에 클랜 멤버 리스트 요청
	m_pClanContext->EnterClanMember();

	InitState();
	InitAuthority();
}

/*virtual*/ void UITabClanMgt_Member::OnUnloadAllScenes()
{
	base::OnUnloadAllScenes();

}

/*virtual*/ void UITabClanMgt_Member::OnExitEnd()
{
	base::OnExitEnd();
}

CLAN_DETAIL_INFO * UITabClanMgt_Member::GetMyClanDetailInfo()
{
	return g_pGameContext->getMyClanDetailInfo();
}