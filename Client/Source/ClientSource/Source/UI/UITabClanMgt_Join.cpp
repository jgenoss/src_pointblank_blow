#include "pch.h"
#include "UITabClanMgt_Join.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UIPopupUserAction.h"
#include "UIUtil.h"

#include "../QuestTextDictionary.h"
#include "../CommunityContext.h"
#include "ClanDef.h"
#include "ClanContext.h"

const char * g_pszAdmissionLimitRankItem1[5] = { NULL, NULL, NULL, NULL, NULL};
const char * g_pszAdmissionLimitAgeLowItem1[ 4] = { NULL, NULL, NULL, NULL};
const char * g_pszAdmissionLimitAgeHighItem1[ 4] = { NULL, NULL, NULL, NULL};

I3_CLASS_INSTANCE( UITabClanMgt_Join, UITabClan_Mgt);

static UITabClanMgt_Join * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UIPhaseBase* pPhase = pFrame->GetCurrentSubPhase();
	UITabClan* pClan = static_cast<UIPhaseClanMember*>(pPhase)->GetCurrTab();
	return static_cast<UITabClanMgt_Join*>(pClan);
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int Clan_UserDetail_Info(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		GetThis()->Click_User_Detail(idx);
		return 0;
	}
	int Clan_InClan_Accept(LuaState * L)
	{
		GetThis()->Click_InClan_Accept();
		return 0;
	}
	int Clan_InClan_Reject(LuaState * L)
	{
		GetThis()->Click_InClan_Reject();
		return 0;
	}
	int Clan_Save_Condition(LuaState * L)
	{
		INT32 Rank = i3Lua::GetIntegerArg(L,1);
		INT32 HAValue = i3Lua::GetIntegerArg(L,2);
		INT32 LAValue = i3Lua::GetIntegerArg(L,3);
		INT32 KD = i3Lua::GetIntegerArg(L,4);
		GetThis()->Click_Save_Condition(Rank, HAValue, LAValue, KD);
		return 0;
	}
	int Clan_Combo_Check(LuaState * L)
	{
		INT32 type = i3Lua::GetIntegerArg(L,1);
		INT32 HAValue = i3Lua::GetIntegerArg(L,2);
		INT32 LAValue = i3Lua::GetIntegerArg(L,3);
	
		GetThis()->AgeLimitCheck((ADMISSION_LIMIT_TYPE)type, HAValue, LAValue);
		return 0;
	}
}

LuaFuncReg UITabClanMgt_Join_Glue[] = {
	{"UserDetail_Info",		Clan_UserDetail_Info	},
	{"InClan_Accept",		Clan_InClan_Accept		},
	{"InClan_Reject",		Clan_InClan_Reject		},
	{"Save_Condition",		Clan_Save_Condition		},
	{"CallComboCheck",		Clan_Combo_Check		},
	{NULL,					NULL					},
};

//------------------------------------------------------------------------------//


UITabClanMgt_Join::UITabClanMgt_Join()
{
	m_pClanContext = g_pFramework->GetClanContext();

	g_pszAdmissionLimitRankItem1[0] = GAME_STRING("STBL_IDX_CLAN_RANK_UNLIMIT");	//	"전체 허용",
	g_pszAdmissionLimitRankItem1[1] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_1");	//	"병장 이상",
	g_pszAdmissionLimitRankItem1[2] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_2");	//	"소위 이상",
	g_pszAdmissionLimitRankItem1[3] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_3");	//	"소령 이상",
	g_pszAdmissionLimitRankItem1[4] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_4");	//	"준장 이상",

	g_pszAdmissionLimitAgeLowItem1[0] = GAME_STRING("STBL_IDX_CLAN_AGE_UNLIMIT");	//	"제한 없음",
	g_pszAdmissionLimitAgeLowItem1[1] = GAME_STRING("STBL_IDX_CLAN_AGE_UPPER_1");	//	"15세 이상",
	g_pszAdmissionLimitAgeLowItem1[2] = GAME_STRING("STBL_IDX_CLAN_AGE_UPPER_2");	//	"20세 이상",
	g_pszAdmissionLimitAgeLowItem1[3] = GAME_STRING("STBL_IDX_CLAN_AGE_UPPER_3");	//	"30세 이상",

	g_pszAdmissionLimitAgeHighItem1[0] = GAME_STRING("STBL_IDX_CLAN_AGE_UNLIMIT");//	"제한 없음",
	g_pszAdmissionLimitAgeHighItem1[1] = GAME_STRING("STBL_IDX_CLAN_AGE_LOWER_1");//	"15세 이하",
	g_pszAdmissionLimitAgeHighItem1[2] = GAME_STRING("STBL_IDX_CLAN_AGE_LOWER_2");//	"20세 이하",
	g_pszAdmissionLimitAgeHighItem1[3] = GAME_STRING("STBL_IDX_CLAN_AGE_LOWER_3");//	"30세 이하",
}

UITabClanMgt_Join::~UITabClanMgt_Join()
{

}

void UITabClanMgt_Join::_UpdateClanManagementSubscriberPhase(RT_REAL32 rDeltaSeconds)
{	
	//	가입 대기자 정보 업데이트
	m_pClanContext->UpdateClanSubscriber(rDeltaSeconds);
}

void UITabClanMgt_Join::_UpdateSortSubscriberList()
{

	//	가입 대기자 정렬 리스트 재구성
	INT32 nNewCount = m_pClanContext->GetMyClanSubscriberCount(); 
	INT32 nOldCount = (INT32)m_listManageSubscriber.size();

	if( nNewCount > 100 ) nNewCount = 100;
	if( nOldCount > 100 ) nOldCount = 100;

	if( nOldCount != nNewCount)
	{
		m_listManageSubscriber.clear();

		for(INT32 i=0; i<nNewCount; i++)
		{
			if( i >= MAX_REQUEST_COUNT)		
			{
				I3TRACE("[StepClanMember] Clan subscribers are over count 100 [%d]", i); 

				continue;				
			}

			CLAN_REQUEST_BASIC_INFO * pInfo = m_pClanContext->GetMyClanSubscriber(i);

			i3mem::Copy( &m_ManageSubscriberInfo[i]._info, pInfo, sizeof(CLAN_REQUEST_BASIC_INFO));

			if( pInfo->_i64UID > 0)
			{
				m_listManageSubscriber.push_back(&m_ManageSubscriberInfo[i]);
			}			
			else
			{
				i3mem::FillZero( &m_ManageSubscriberInfo[i], sizeof(m_ManageSubscriberInfo[i]));
			}
		}	

		if( nOldCount > nNewCount)	//	자투리 부분을 초기화 한다.
		{
			for(INT32 i=nNewCount; i<nOldCount; i++)
			{
				if( i >= MAX_REQUEST_COUNT)	
					continue;		

				i3mem::FillZero( &m_ManageSubscriberInfo[i], sizeof(m_ManageSubscriberInfo[i]));
			}
		}
	}
	//	변경된 정보만 갱신
	else
	{
		for(INT32 i=0; i<nNewCount; i++)
		{
			if( i >= MAX_REQUEST_COUNT)		
				continue;		

			CLAN_REQUEST_BASIC_INFO * pInfo = m_pClanContext->GetMyClanSubscriber(i);

			i3mem::Copy( &m_ManageSubscriberInfo[i]._info, pInfo, sizeof(CLAN_REQUEST_BASIC_INFO));
		}
	}
}

void UITabClanMgt_Join::_SetSubscriptionListSlot(INT32 idx, ManagementSubscriberInfo *pSubscriber)
{
	I3_BOUNDCHK(idx, SUBSCRIPTION_LIST_ROW_MAX);
	I3ASSERT( pSubscriber != NULL);

	char szEntry[MAX_STRING_COUNT] = "";
	INT32 c_state = 0;
	if(pSubscriber->_bCheck == TRUE)
		c_state = 1;
	else
		c_state = 2;

	_SetUserIndex(idx);
	_SetUserCheckBox(idx, c_state);
	_SetUserRank(idx, (INT32)(pSubscriber->_info._rank));
	_SetUserNick(idx, pSubscriber->_info._nick);
	//	"%04u년 %02u월 %02u일"
	sprintf_s(szEntry, GAME_STRING("STBL_IDX_CLAN_DECLARE_DATE"), 
		(INT32)GetEntryYear(pSubscriber->_info._date), 
		(INT32)GetEntryMonth(pSubscriber->_info._date), 
		(INT32)GetEntryDay(pSubscriber->_info._date));
	_SetUserJoinDay(idx, szEntry);

}

void UITabClanMgt_Join::_SetUserCheckBox(INT32 UserIdx, INT32 c_State)
{
	LuaState * L = _CallLuaFunction("_SetUserCheckBox");
	i3Lua::PushInteger(L, UserIdx);
	i3Lua::PushInteger(L, c_State);
	_EndLuaFunction(L,2);
}

void UITabClanMgt_Join::_SetUserIndex(INT32 UserIdx)
{
	LuaState * L = _CallLuaFunction("_SetUserIndex");
	i3Lua::PushInteger(L, UserIdx);
	_EndLuaFunction(L,1);
}
void UITabClanMgt_Join::_SetUserRank(INT32 UserIdx, INT32 RankIdx)
{
	LuaState * L = _CallLuaFunction("_SetUserRank");
	i3Lua::PushInteger(L, UserIdx);
	i3Lua::PushInteger(L, RankIdx);
	_EndLuaFunction(L,2);
}
void UITabClanMgt_Join::_SetUserNick(INT32 UserIdx, char* NickName)
{
	LuaState * L = _CallLuaFunction("_SetUserNick");
	i3Lua::PushInteger(L, UserIdx);
	i3Lua::PushA2UTF8String(L, NickName);
	_EndLuaFunction(L,2);
}
void UITabClanMgt_Join::_SetUserJoinDay(INT32 UserIdx, char* JoinDay)
{
	LuaState * L = _CallLuaFunction("_SetUserJoinDay");
	i3Lua::PushInteger(L, UserIdx);
	i3Lua::PushA2UTF8String(L, JoinDay);
	_EndLuaFunction(L,2);
}

void UITabClanMgt_Join::_SetUserCount(INT32 Member, INT32 Max_Member)
{
	char Member_Count[MAX_STRING_COUNT];
	sprintf_s(Member_Count, "%d / %d", Member, Max_Member);

	LuaState * L = _CallLuaFunction("SetCurrentMember");
	i3Lua::PushA2UTF8String(L, Member_Count);
	_EndLuaFunction(L,1);

}

void UITabClanMgt_Join::_SetDefaultComboBox()
{
	LuaState * L = _CallLuaFunction("SetDefaultComboBox");
	_EndLuaFunction(L,0);
}

void UITabClanMgt_Join::SetUserValue(INT64 *i64UID, INT32 *count)
{
	
	for(size_t i=0; i<m_listManageSubscriber.size(); i++)
	{
		if( i >= MAX_REQUEST_COUNT)
			continue;

		ManagementSubscriberInfo * pSubscriber = m_listManageSubscriber[i];
		
		if (m_ListBox->isCheckedData(i,0) && pSubscriber->_info._i64UID > 0) //isCheckedData -> 체크박스의 CellIdx를 지정해주셔야됩니다.
		{
			I3_BOUNDCHK( *count, MAX_REQUEST_COUNT);
			i64UID[*count] = pSubscriber->_info._i64UID;
			(*count)++;
		}
	}
}

void UITabClanMgt_Join::Click_User_Detail(INT32 idx)
{
	if( idx == -1)	{	GameUI::MsgBox( MSG_TYPE_GAME_OK, "유저를 선택해 주시기 바랍니다."); return;}

	ManagementSubscriberInfo * pSubscriber = m_listManageSubscriber[idx];
	INT32 _Session = g_pGameContext->getUserSessionIdxByUserNickName(pSubscriber->_info._nick);
	LOBBY_USER_INFO_LIST * UserInfo = g_pGameContext->getLobbyUserInfoByNick(pSubscriber->_info._nick);
	if(UserInfo == NULL)
		return;

	//유저의 정보 요청.		
	//g_pCommunity->SetCommunityUserInfo( FALSE);
	g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_SESSION_N, &_Session);
	g_pGameContext->SetEvent(EVENT_USER_DETAIL_INFO, &_Session);
	//	미리 상세정보 팝업을 띄운다.
	if( GetMainFrame()->IsOpenedPopup( UPW_USERPROFILE) == FALSE)
		GetMainFrame()->TogglePopup(  UPW_USERPROFILE, (void*)USERLIST_TAB_CLAN, &UserInfo);
}

void UITabClanMgt_Join::Click_InClan_Accept()
{
	if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_ACCEPT ) ) 
	{
		//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));

	}
	else
	{
		INT64 i64UID[MAX_REQUEST_COUNT] = {0,};
		INT32 count = 0;
		SetUserValue(i64UID, &count);
		if(count)
			m_pClanContext->ProcessIntromissionMember(i64UID, count);
	}
}

void UITabClanMgt_Join::Click_InClan_Reject()
{
	if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_ACCEPT ) )
	{
		//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));

	}
	else
	{
		INT64 i64UID[MAX_REQUEST_COUNT] = {0,};
		INT32 count = 0;
		SetUserValue(i64UID, &count);
		if(count)
			m_pClanContext->ProcessRejectionMember(i64UID, count);
	}
}

void UITabClanMgt_Join::Click_Save_Condition(INT32 Rank, INT32 HighAgeValue, INT32 LowAgeValue, INT32 KILL_DEATH)
{
	CLAN_MANAGEMENT modify;
	//	가입 계급 제한
	switch(Rank)
	{
	case 0:		modify._limitRank = CLAN_LIMIT_RANK_1;		break;		//	전체 허용
	case 1:		modify._limitRank = CLAN_LIMIT_RANK_2;		break;		//	사관 계급
	case 2:		modify._limitRank = CLAN_LIMIT_RANK_3;		break;		//	위관 계급
	case 3:		modify._limitRank = CLAN_LIMIT_RANK_4;		break;		//	영관 계급
	case 4:		modify._limitRank = CLAN_LIMIT_RANK_5;		break;		//	장성 계급
	default:	modify._limitRank = CLAN_LIMIT_RANK_1;		break;
	}	
	//	가입 나이 제한 하한
	switch(LowAgeValue)
	{
	case 0:		modify._limitAgeLow = CLAN_LIMIT_AGE_1;		break;		//	전체 허용
	case 1:		modify._limitAgeLow = CLAN_LIMIT_AGE_2;		break;		//	15세 이상
	case 2:		modify._limitAgeLow = CLAN_LIMIT_AGE_3;		break;		//	20세 이상
	case 3:		modify._limitAgeLow = CLAN_LIMIT_AGE_4;		break;		//	30세 이상
	default:	modify._limitAgeLow = CLAN_LIMIT_AGE_1;		break;
	}
	//	가입 나이 제한 상한
	switch(HighAgeValue)
	{
	case 0:		modify._limitAgeHigh = CLAN_LIMIT_AGE_1;	break;		//	전체 허용
	case 1:		modify._limitAgeHigh = CLAN_LIMIT_AGE_2;	break;		//	15세 이하
	case 2:		modify._limitAgeHigh = CLAN_LIMIT_AGE_3;	break;		//	20세 이하
	case 3:		modify._limitAgeHigh = CLAN_LIMIT_AGE_4;	break;		//	30세 이하
	default:	modify._limitAgeHigh = CLAN_LIMIT_AGE_1;	break;
	}

	INT32 ClanAuthority[STAFF_AUTHORITY];
	ClanAuthority[0] = GET_CLAN_MENAGE_WRITE( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	ClanAuthority[1] = GET_CLAN_MENAGE_INVITED( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	ClanAuthority[2] = GET_CLAN_MENAGE_DEPORTATION( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	ClanAuthority[3] = GET_CLAN_MENAGE_ACCEPT( m_pClanContext->GetMyClanDetailInfo()->m_authstaff);
	UINT8 wr = (UINT8)(ClanAuthority[0]);
	UINT8 in = (UINT8)(ClanAuthority[1]);
	UINT8 de = (UINT8)(ClanAuthority[2]);
	UINT8 ac = (UINT8)(ClanAuthority[3]);

	modify._authority = CLAN_MENAGE_AUTHORITY( wr, in, de, ac);

	m_pClanContext->GetMyClanDetailInfo()->m_authstaff	= modify._authority;
	m_pClanContext->GetMyClanDetailInfo()->m_ranklock	= modify._limitRank;
	m_pClanContext->GetMyClanDetailInfo()->m_lagelock	= modify._limitAgeLow;
	m_pClanContext->GetMyClanDetailInfo()->m_hagelock	= modify._limitAgeHigh;	
	//	서버로 변경 사항을 보냄
	m_pClanContext->ProcessManagementModify( &modify);
}

void UITabClanMgt_Join::InitState()
{
	INT32 Rank,HighAge,LowAge;
	Rank = HighAge = LowAge = 0;
	switch(m_pClanContext->GetMyClanDetailInfo()->m_ranklock)
	{
	case CLAN_LIMIT_RANK_1:	Rank = 0;	break;
	case CLAN_LIMIT_RANK_2: Rank = 1;	break;
	case CLAN_LIMIT_RANK_3: Rank = 2;	break;
	case CLAN_LIMIT_RANK_4: Rank = 3;	break;
	case CLAN_LIMIT_RANK_5: Rank = 4;	break;
	}
	switch(m_pClanContext->GetMyClanDetailInfo()->m_lagelock)
	{
	case CLAN_LIMIT_AGE_1: LowAge = 0;	break;
	case CLAN_LIMIT_AGE_2: LowAge = 1;	break;
	case CLAN_LIMIT_AGE_3: LowAge = 2;	break;
	case CLAN_LIMIT_AGE_4: LowAge = 3;	break;
	}
	switch(m_pClanContext->GetMyClanDetailInfo()->m_hagelock)
	{
	case CLAN_LIMIT_AGE_1: HighAge = 0;	break;
	case CLAN_LIMIT_AGE_2: HighAge = 1;	break;
	case CLAN_LIMIT_AGE_3: HighAge = 2;	break;
	case CLAN_LIMIT_AGE_4: HighAge = 3;	break;
	}
	LuaState * L = _CallLuaFunction("InitState");
	i3Lua::PushInteger(L,Rank);
	i3Lua::PushInteger(L,LowAge);
	i3Lua::PushInteger(L,HighAge);
	_EndLuaFunction(L,3);

}

void UITabClanMgt_Join::AgeLimitCheck( ADMISSION_LIMIT_TYPE type, INT32 HAValue,INT32 LAValue)
{
	switch( type)
	{
	case ADMISSION_LIMIT_RANK:	//	계급 제한
		{
			//I3_BOUNDCHK( nItemIdx, 5);
			//m_pManagementOthersAdmissionLimitRankCombo->SetText( g_pszAdmissionLimitRankItem[nItemIdx]);
		}
		break;
	case ADMISSION_LIMIT_AGE1:	//	나이 하한 제한
		{
			char temp[256] = "";

			//	상한 값
			INT32 idxHigh = HAValue;
			const char * pszHigh = g_pszAdmissionLimitAgeLowItem1[LAValue];

			//	상한이 제한되어 있을 경우
			switch(LAValue)
			{
			case 0:		//	전체 허용
				break;
			case 1:		//	15세 이상
				break;
			case 2:		//	20세 이상
				{
					if( idxHigh == 1)
					{
						//	"불가능한 설정입니다. \n 나이 제한 상한이 %s로 되어있습니다."
						i3String::Format( temp, sizeof(temp), GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_UPPER"), pszHigh);
						GameUI::MsgBox( MSG_TYPE_GAME_OK, temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"가입 제한 나이" 
						_SetDefaultComboBox();
					}
				}
				break;
			case 3:		//	30세 이상
				{
					if( idxHigh == 1 || idxHigh == 2)
					{
						//	"불가능한 설정입니다. \n 나이 제한 상한이 %s로 되어있습니다."
						i3String::Format( temp, sizeof(temp), GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_UPPER"), pszHigh);
						GameUI::MsgBox( MSG_TYPE_GAME_OK, temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"가입 제한 나이"
						_SetDefaultComboBox();
					}
				}
				break;
			}

		}
		break;
	case ADMISSION_LIMIT_AGE2:	//	나이 상한 제한
		{
			char temp[256] = "";

			//	하한 값
			INT32 idxLow = LAValue;
			const char * pszLow = g_pszAdmissionLimitAgeHighItem1[HAValue];;

			//	상한이 제한되어 있을 경우
			switch(HAValue)
			{
			case 0:		//	전체 허용
				break;
			case 1:		//	15세 이하
				{
					if( idxLow == 2 || idxLow == 3)
					{
						//	"불가능한 설정입니다. \n 나이 제한 하한이 %s로 되어있습니다."
						i3String::Format( temp, sizeof(temp), GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_LOWER"), pszLow);
						GameUI::MsgBox( MSG_TYPE_GAME_OK, temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"가입 제한 나이"
						_SetDefaultComboBox();
					}
				}
				break;
			case 2:		//	20세 이하
				{
					if( idxLow == 3)
					{
						//	"불가능한 설정입니다. \n 나이 제한 하한이 %s로 되어있습니다."
						i3String::Format( temp, sizeof(temp), GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_LOWER"), pszLow);
						GameUI::MsgBox( MSG_TYPE_GAME_OK, temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"가입 제한 나이" 
						_SetDefaultComboBox();
					}
				}
				break;
			case 3:		//	30세 이하
				{
				}
				break;
			}
		}
		break;
	}
}

void UITabClanMgt_Join::InitAuthority()
{
	LuaState * L = _CallLuaFunction("InitAuthority");
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER )
		i3Lua::PushBoolean(L, true);
	else
		i3Lua::PushBoolean(L, false);
	_EndLuaFunction(L,1);
}

/*virtual*/ void UITabClanMgt_Join::OnCreate(i3GameNode * pParent)
{
	base::OnCreate(pParent);
	//LoadScene
	char * pszPath = "UIRe/PBRe_ClanMgt_Join.i3UIe";
	AddScene( pszPath,UITabClanMgt_Join_Glue);
}

/*virtual*/ void UITabClanMgt_Join::ProcessGameEvent(INT32 event, INT32 arg)
{
	base::ProcessGameEvent(event,arg);
	switch(event)
	{
		case EVENT_CLAN_MEMBER_LIST_N:
		{
			if(SUCCEEDED(arg))
			{
				//	맴버 리스트 재구성
				_UpdateSortSubscriberList();

				LuaState * L = _CallLuaFunction("DeleteAllList");
				i3Lua::PushInteger(L, m_pClanContext->GetMyClanSubscriberCount());
				_EndLuaFunction( L, 1);

				const size_t count = m_listManageSubscriber.size();
				BOOL bValid = FALSE;
				for(size_t i=0; i < count; i++)
				{
					ManagementSubscriberInfo * pSubscriber = m_listManageSubscriber[i];

					if( pSubscriber != NULL)
					{
						if( pSubscriber->_info._i64UID > 0)
							bValid = TRUE;
					}

					if(bValid)
						_SetSubscriptionListSlot(i, pSubscriber);	//	슬롯 값 삽입
				}
				_SetUserCount((INT32)m_pClanContext->GetMyClanDetailInfo()->m_currentPerson, (INT32)m_pClanContext->GetMyClanDetailInfo()->m_maxPerson);
			}
		}
		break;
	}
}

/*virtual*/ void UITabClanMgt_Join::OnUpdate(REAL32 rDeltaSeconds)
{
	base::OnUpdate(rDeltaSeconds);
	_UpdateClanManagementSubscriberPhase(rDeltaSeconds);
}

/*virtual*/ void UITabClanMgt_Join::OnEntranceEnd(void)
{
	base::OnEntranceEnd();
}

/*virtual*/ bool UITabClanMgt_Join::OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */)
{
	if( base::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	return true;	
}

/*virtual*/ void UITabClanMgt_Join::OnLoadAllScenes()
{
	base::OnLoadAllScenes();
	m_listManageSubscriber.clear();
	i3mem::FillZero( m_ManageSubscriberInfo, sizeof(m_ManageSubscriberInfo));
	m_pClanContext->EnterClanSubscriber();
	InitState();
	InitAuthority();

	LuaState * L = _CallLuaFunction("OnLoadScene");
	UILua::EndLuaFunction(L , 0);
}

/*virtual*/ void UITabClanMgt_Join::OnUnloadAllScenes()
{
	base::OnUnloadAllScenes();
}

/*virtual*/ void UITabClanMgt_Join::_InitializeAtLoad(i3UIScene * pScene)
{
	m_ListBox = (i3UIListView_Box *)(pScene->FindChildByName("i3UIListView_Box"));
}

/*virtual*/ void UITabClanMgt_Join::OnExitEnd()
{
	base::OnExitEnd();
}