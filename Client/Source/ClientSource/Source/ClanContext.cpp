#include "pch.h"
#include "ClanContext.h"

#include "GlobalVariables.h"
#include "ClanDef.h"
#include "ClanMark.h"
#include "ServerInfo.h"
#include "UI/UIMainFrame.h"
#include "UI/UIPhaseNonClanMember.h"
#include "UI/UIUtil.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/ext/snprintf.h"
#include "ChannelContext.h"

#include "ClanGameContext.h"
#include "ClanSeasonContext.h"
#include "./StageBattle/NetworkContext.h"
#include "UserInfoContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"


#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif


namespace 
{
	enum CLAN_SORT_TYPE
	{
		CLAN_SORT_NONE = 0,	
		CLAN_SORT_CUR_SEASON,	// 클랜 포인트 (현재 시즌)
		CLAN_SORT_PREV_SEASON,	// 클랜 포인트 (이전 시즌)
		CLAN_SORT_TOTAL_RECORD,	// 총 전적
		CLAN_SORT_MY_CLAN		// 내 클랜 기준 (총 전적 기준 상하위 클랜 1개씩)
	};

	CLAN_SORT_TYPE g_sort_type = CLAN_SORT_NONE;

	bool sort_by_cur_season(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_SortInfo.m_clanpoint > pInfo2->_SortInfo.m_clanpoint ;
	}

	bool sort_by_prev_season(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_SortInfo.m_Preclanpoint > pInfo2->_SortInfo.m_Preclanpoint ;
	}

	bool sort_by_total_record(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_RecordInfo.m_totalWin > pInfo2->_RecordInfo.m_totalWin;
	}

	bool sort_by_my_clan(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_RecordInfo.m_totalWin > pInfo2->_RecordInfo.m_totalWin;
	}

	bool sort_by_clan_idx(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_info._idx < pInfo2->_info._idx ;
	}

}

I3_CLASS_INSTANCE( CClanContext);//, i3GameNode);


CClanContext::CClanContext(void)
{
	m_bEnter = false;
	m_bMember = false;		//	클랜원이면 true

	m_rUpdateClanMemberTime			= 0.0f; 
	m_nUpdateClanMemberSection		= 0; 
//	m_bListUpdateClanMemberFinish	= true; 

	m_rUpdateClanRequestTime		= 0.0f; 
	m_nUpdateClanRequestSection		= 0;
	m_bListUpdateClanRequestFinish	= true; 
	m_bInBattleTeam = false;
	m_bInClanBattleLobby = false;
	m_bVisibleLobbyPhase = true;

	m_nJoinClanIdx = 0;

	m_IndexBufferCount = 0;
	i3mem::FillZero(m_IndexBuffer, sizeof( m_IndexBuffer));

	m_nSortClanInfoCount		= 0;

	m_rUpdateClanListTime		= 0.f; 
	m_nUpdateClanListReq_BeginClanIdx	= 0;
	m_nUpdateClanListAck_BeginClanIdx = -1;
	m_nUpdateClanListAck_CurrRecvClanCount = 0;

	m_nUpdateClanListCount		= 0;
	m_nAppendClanListCount		= 0;
	m_bUpdateClanListFinish		= false;
	m_bNeedUpdateAppendClanList = true;

	m_HasClanCreationInfo	= false;
	m_bClanListUpdated			= false;

	m_ClanCreationRequirementPoint = 0;
	m_ClanCreationRequirementRank = 0;

	m_bEnableReservePhase = false;

	i3mem::FillZero(&m_ClanResultContext, sizeof(CLAN_MATCH_RESULT_CONTEXT));
	i3mem::FillZero(&m_ClanResultList, sizeof(CLAN_MATCH_RESULT) * MAX_MATCH_RESULT_COUNT);
	m_i32EmotionReqIndex = 0;
	m_rMatchListReqTime			= 0.0f; 
	m_ui8MatchListReqSection	= 0; 
	m_bMatchListReqFinish		= false;

	m_bIsClanVisit = false;
	m_bWaitMemberInfoReq = false;


	InitGameEventFunc();
}

CClanContext::~CClanContext(void)
{
	_ReleaseSortClanList();
}

void CClanContext::Create(void)
{

}
void CClanContext::SetEnter(bool bEnter)			
{
	m_bEnter = bEnter;		
}

void CClanContext::EnterServer(void * pArg)
{	

#if defined TEST_CLAN_MEMBER
	UINT16 year = 2007;
	UINT8 month = 10;
	UINT8 day = 1;	
	UINT32 date = ((year << 16) | (month << 8) | day);

	if( GetClanMemberCount() == 0)
	{
		for(INT32 i=0; i<16; i++)
		{
			char temp[256] = "";
			sprintf_s(temp, "TEST_%d", i);

			AddClanMember(temp, (UINT8)i, "TEST", date);

			AddClanSubscriber(temp, (UINT8)i, date);
		}
	}
#endif
	GameEventSender::i()->SetEvent(EVENT_CLAN_ENTER);

	m_nJoinClanIdx = 0;

	//m_bEnter = true;
}

void CClanContext::LeaveServer(void * pArg)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_LEAVE, pArg);

	ClearIndexBuffer();

	_ReleaseSortClanList();

	//m_bEnter = false;
}

void CClanContext::OnGameEvent(GAME_EVENT event,INT32 arg, const i3::user_data & userData)
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;

	it = m_EventFuncMap.find( event);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, userData);
	}
}

void	CClanContext::SetMemberInfoChange( INT64 i64UID, UINT64 ui64State )
{
	if( ClanGameContext::i()->GetMyClanMemberMaxCount() <= 0 )
		return;

	for( INT32 i = 0 ; i < GetMyClanMemberCount() ; i++ )
	{
		const CLAN_MEMBER_BASIC_INFO * pInfo = ClanGameContext::i()->getMyClanMember(i);
		if ( pInfo == nullptr )
			continue;

		if( pInfo->_i64UID == i64UID )
		{
			CLAN_MEMBER_BASIC_INFO NewMemberInfo = *pInfo;
			NewMemberInfo._state = ui64State;

			ClanGameContext::i()->setMyClanMember(i, NewMemberInfo);
			return;
		}
	}
}

void	CClanContext::SetMemberBasicInfo( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo )
{
	I3ASSERT(pMemberBasicInfo != nullptr);
	if ( pMemberBasicInfo == nullptr )	return;
	if( ClanGameContext::i()->GetMyClanMemberMaxCount() <= 0 )
		return;

	for( INT32 i = 0 ; i < GetMyClanMemberCount() ; i++ )
	{
		const CLAN_MEMBER_BASIC_INFO * pInfo = ClanGameContext::i()->getMyClanMember(i);
		if ( pInfo == nullptr )
			continue;

		if( pInfo->_i64UID == pMemberBasicInfo->_i64UID )
		{
			ClanGameContext::i()->setMyClanMember(i, *pMemberBasicInfo);
			return;
		}
	}
}

bool CClanContext::IsMaster(void)
{
	if( UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_MASTER)
		return true;

	return false;
}

CLAN_MEMBER_LEVEL_TYPE CClanContext::GetMyStaffLevel(void)
{
	return (CLAN_MEMBER_LEVEL_TYPE) UserInfoContext::i()->GetMyClanState();
}

INT64 CClanContext::GetMyDBIdx(void)			
{ 
	return NetworkContext::i()->UID;		
}

#define LIST_UPDATE_TIME	3.0f 
// 몇개의 예외 처리 필요함 20080103 성백 

//	클랜원 
void CClanContext::EnterClanMember(void)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

	//	체크 해제
	ClearIndexBuffer();
}

void CClanContext::RequestClanMember(void)
{
	m_rUpdateClanMemberTime			= LIST_UPDATE_TIME + FLT_MIN; // 1000.0f;  
	m_nUpdateClanMemberSection		= 0;  
//	m_bListUpdateClanMemberFinish	= false;  	

	//	체킹 리스트 다시 구성
	ClearIndexBuffer();
}

void CClanContext::UpdateClanMember(REAL32 rDeltaSeconds)
{
	CLAN_MEMBER_CONTEXT * pMemberContext = ClanGameContext::i()->getMyClanMemberContext(); 

	if ( m_nUpdateClanMemberSection < pMemberContext->_sectionCount )
	{
		m_rUpdateClanMemberTime += rDeltaSeconds;

		if( m_rUpdateClanMemberTime > LIST_UPDATE_TIME )
		{
			m_rUpdateClanMemberTime = 0.0f;
			GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_LIST_N, &m_nUpdateClanMemberSection);
			++m_nUpdateClanMemberSection;

			if (m_nUpdateClanMemberSection >= pMemberContext->_sectionCount)
				m_nUpdateClanMemberSection = UCHAR_MAX;
		}
	}
}

//	가입 대기자
void CClanContext::EnterClanSubscriber(void)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT);

	ClearIndexBuffer();
}

void CClanContext::RequestClanSubscriber(void)
{
	m_rUpdateClanRequestTime		= 1000.0f; 
	m_nUpdateClanRequestSection		= 0; 
	m_bListUpdateClanRequestFinish	= false; 

	//	체킹 리스트 다시 구성
	ClearIndexBuffer();
}

void CClanContext::UpdateClanSubscriber(REAL32 rDeltaSeconds)
{
	if( !m_bListUpdateClanRequestFinish )
		m_rUpdateClanRequestTime += rDeltaSeconds;

	if( m_rUpdateClanRequestTime > LIST_UPDATE_TIME )
	{
		m_rUpdateClanRequestTime = 0.0f;

		PACKET_CS_NEW_REQUEST_LIST_REQ ReqPacket;

		ReqPacket.m_ui8Section = m_nUpdateClanRequestSection;

		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_LIST_N, &ReqPacket);			

		CLAN_REQUEST_CONTEXT * pClanRequestContext = ClanGameContext::i()->getMyClanRequestContext();
		m_nUpdateClanRequestSection++;
		if(m_nUpdateClanRequestSection >= pClanRequestContext->_sectionCount)
			m_bListUpdateClanRequestFinish = true;
	}
}

void CClanContext::RequsetSubscriberInfo(T_UID dbIdx)
{
	//	가입자 정보 요구
	GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_INFO_N, &dbIdx);	
}

bool CClanContext::ProcessSecessionFromClan(void)
{
	//	"클랜을 정말 탈퇴 하시겠습니까?"
	GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_Q"), nullptr , MAKE_CALLBACK(CbSecessionFromClan), this);

	return true;
}

bool CClanContext::ProcessAppointMember(INT64 * pdbIdx, INT32 count)
{ 
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//
		//	"클랜 멤버 스태프 임명"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_APPOINT_MEMBER"));

		return false;
	}
	else
	{
		//
		//	"해당 멤버를 스태프로 임명하겠습니까?"
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_APPOINT_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_APPOINT_MEMBER"), MAKE_CALLBACK(CbAppointMember), this);

		return true;
	}
}

bool CClanContext::ProcessDemoteMember(INT64 * pdbIdx, INT32 count)
{	// 클랜 스탭에서 일반으로 강등
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		// "최소한 한명 이상의 유저를 선택해야 합니다."
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DEMOTE_MEMBER"));

		return false;
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DEMOTE_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DEMOTE_MEMBER"), MAKE_CALLBACK(CbDemoteMember), this);

		return true;
	}
}

bool CClanContext::ProcessCommissionMaster(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	bool is_use_clan_xp_plus_150 = CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_CLAN_XP_PLUS_150);
	bool is_use_color_clan = CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_COLOR_CLAN);
	if( is_use_clan_xp_plus_150 || is_use_color_clan )
	{
		// 사용중인 아이템
	}

	if( count == 0)
	{
		//	"최소한 한명이상의 멤버를 선택해야 합니다."
		//	"클랜 마스터 위임"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISSION_MASTER"));
	}
	else if( count != 1)
	{
		//	"오직 한명의 멤버만 선택할 수 있습니다.
		//	"클랜 마스터 위임"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISSION_MASTER_ONLY"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISSION_MASTER"));
	}
	else
	{
		i3::rc_wstring wstrTemp;

		const CLAN_MEMBER_BASIC_INFO * pMember = FindMyClanMember(m_IndexBuffer[0]);
		I3ASSERT( pMember != nullptr);

		if (pMember != nullptr)
		{
			//	"해당 멤버에게 마스터를 위임하겠습니까?" (형변환 존재함.) 2014.07.10.수빈.
			i3::sprintf(wstrTemp, L"[%s] \n \n %s", pMember->_nick, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISSION_MASTER_Q"));
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISSION_MASTER"), MAKE_CALLBACK(CbCommissionMaster), this);	//	"클랜 마스터 위임"
		}
		else
		{
			//	"클랜 마스터 위임이 실패했습니다."
			//	"클랜 마스터 위임 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_FAIL"));
			return false;
		}		

		return true;
	}

	return false;
}

bool CClanContext::ProcessDismissalMember(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"최소한 한명이상의 멤버를 선택해야 합니다."
		//	"클랜 멤버 제명"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_MEMBER"));
	}
	else if( count > MAX_MEMBER_COUNT)
	{
		//	"선택할 수 있는 최대 허용 항목을 초과했습니다."
		//	"클랜 멤버 제명"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MEMBER_OVERFLOW"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_MEMBER"));
	}
	else
	{
		//	"선택된 멤버를 정말 클랜에서 제명 시킵니까?"
		//	"클랜 멤버 제명"	
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_MEMBER"), MAKE_CALLBACK(CbDismissalMember), this);
		return true;
	}

	return false;
}

bool CClanContext::ProcessViewMember(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"최소한 한명 이상의 유저를 선택해야 합니다."
		//	"상세 보기"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_VIEW_MEMBER"));

		return false;
	}
	else
	{
		return true;
	}
}

bool CClanContext::ProcessIntromissionMember(INT64 * pdbIdx, INT32 count)
{	
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"최소한 한명 이상의 유저를 선택해야 합니다."
		//	"클랜 가입 승인"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"),  &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_INTROMISSION_MEMBER"));

		return false;
	}
	else
	{
		CLAN_DETAIL_INFO * pClanInfo = GetMyClanDetailInfo();
		I3ASSERT( pClanInfo != nullptr);

		//	가입 인원 초과
		if(pClanInfo->m_maxPerson < pClanInfo->m_currentPerson + m_IndexBufferCount)
		{
			//	"가입을 승인할 수 없습니다.\n클랜 가입 허용 인원을 초과하였습니다."
			//	"클랜 가입 승인"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_INTROMISSION_MEMBER_OVERFLOW"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_INTROMISSION_MEMBER"));
		}
		else
		{
			/*char temp[256] = "";
			char message[1024] = "";

			UINT8 * pIdx = GetSlotCheckedIndexArray();

			//	선택된 모든 맴버 닉네임을 표시한다. 선택할 수 있는 맴버수에 제한이 없다면 문자 버퍼에 문제가 생길 수 있습니다. komet
			for(INT32 i=0; i<GetSlotCheckedIndexCount(); i++)
			{
			CLAN_REQUEST_BASIC_INFO * pSubscriber = GetMyClanSubscriber(*pIdx + i);
			I3ASSERT( pSubscriber != nullptr);

			sprintf_s(temp, "[%s] \n ",  pSubscriber->_nick);
			i3::generic_string_cat(message, temp);			
			}

			i3::generic_string_cat(message, GAME_RCSTRING("STBL_IDX_CLAN_Q_MEMBER_ADMISSION"));//\n 해당 유저의 클랜 가입을 승인하시겠습니까?*/

			//	"해당 유저의 클랜 가입을 승인하시겠습니까?"
			//	"클랜 가입 승인"	
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_INTROMISSION_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_INTROMISSION_MEMBER"), MAKE_CALLBACK(CbIntromissionMember), this);
		}

		return true;
	}
}

bool CClanContext::ProcessRejectionMember(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"최소한 한명이상의 유저를 선택해야 합니다."
		//	"클랜 가입 거부"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_MEMBER"));

		return false;
	}
	else
	{
		/*char temp[256] = "";
		char message[1024] = "";

		UINT8 * pIdx = GetSlotCheckedIndexArray();

		//	선택된 모든 맴버 닉네임을 표시한다. 선택할 수 있는 맴버수에 제한이 없다면 문자 버퍼에 문제가 생길 수 있습니다. komet
		for(INT32 i=0; i<GetSlotCheckedIndexCount(); i++)
		{
		CLAN_REQUEST_BASIC_INFO * pSubscriber = GetMyClanSubscriber(*pIdx + i);
		I3ASSERT( pSubscriber != nullptr);

		sprintf_s(temp, "[%s] \n ",  pSubscriber->_nick);
		i3::generic_string_cat(message, temp);			
		}

		i3::generic_string_cat(message, GAME_RCSTRING("STBL_IDX_CLAN_Q_MEMBER_REJECTION"));//\n 해당 유저의 클랜 가입을 거부하시겠습니까?*/

		//	"해당 유저의 클랜 가입을 거부하시겠습니까?"
		//	"클랜 가입 거부"
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REJECTION_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_MEMBER"), MAKE_CALLBACK(CbRejectionMember), this);

		return true;
	}
}

bool CClanContext::ProcessChangeClanName(INT64 authFlag, const i3::rc_wstring & clan_name)
{
	m_wstrClanName = clan_name;

	const i3::rc_wstring& result_str = clan_name;

	UINT8 ui8Size = (UINT8)(result_str.size()+1)*sizeof(TTCHAR);
	
	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &authFlag, &ui8Size,  result_str.c_str());
	
	return true;	
}

bool CClanContext::ProcessChangeClanMark(INT64 authFlag, UINT32 nClanMarkFlag)
{
	UINT8 ui8Size = sizeof(UINT32);
	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &authFlag, &ui8Size, &nClanMarkFlag);

	return true;	
}

bool CClanContext::ProcessChangeClanMark(UINT32 nClanMarkFlag)
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_CHANGE_MARK, &nClanMarkFlag );

	return true;
}

bool CClanContext::ProcessDissolutionClan(void)
{
	CLAN_DETAIL_INFO * pClanInfo = GetMyClanDetailInfo();
	I3ASSERT( pClanInfo != nullptr);

	//	자기외에 다른 클랜 맴버가 남아 있다면 클랜을 해체할 수 없다.
	if( pClanInfo->m_currentPerson > 1)	
	{
		//	"클랜 멤버가 남은 상태로는 클랜 해체가 불가능 합니다."                                                                                                               
		//	"클랜 해체"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_REMAIN_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION"));
	}
	else
	{
		//	"정말 클랜을 해체하시겠습니까?"
		//	"클랜 해체"
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION"), MAKE_CALLBACK(CbDissolutionClan), this);

		return true;
	}

	return false;
}

bool CClanContext::ProcessManagementModify(CLAN_MANAGEMENT * pModify)
{
	if( pModify == nullptr) return false;

	GameEventSender::i()->SetEvent(EVENT_CLAN_MANAGEMENT, pModify );

	return true;
}

bool CClanContext::SendCreateClan(const i3::wliteral_range& wClanNameRng, const i3::wliteral_range& wIntroRng, const i3::wliteral_range& wURLRng, UINT32 nClanMarkFlag)
{
	// xgkick_thai
	// 클랜 생성이 올바르게 되지 않는 부분 수정. (아지트 URL 무시)
#if !defined (LOCALE_KOREA)
	if( i3::generic_string_size(wClanNameRng) == 0 )  
	{
		return false;
	}
#else
/*  클랜 URL 쓰지 않으므로 삭제 : 
	if( pszClanName == nullptr || i3::generic_string_size(pszClanName) == 0 ||
		pszURL == nullptr || i3::generic_string_size(pszURL) == 0 || pszIntro == nullptr)  
*/
	if( i3::generic_string_size(wClanNameRng) == 0 )  
	{
		return false;
	}
#endif

#ifdef CLAN_ADDRESS_ENABLE
	if( i3::generic_string_size(wURLRng) == 0)		return false;
#endif

	PACKET_CS_NEW_CREATE_CLAN_REQ info;
	i3mem::FillZero( &info, sizeof( PACKET_CS_NEW_CREATE_CLAN_REQ));

	info.m_ui32ClanMark = nClanMarkFlag;

	i3::safe_string_copy(info.m_strClanName, wClanNameRng, NET_CLAN_NAME_SIZE);
	i3::safe_string_copy(info.m_strClanIntro, wIntroRng, NET_CLAN_INTRO_SIZE);
	i3::safe_string_copy(info.m_strClanAzit, wURLRng, NET_CLAN_AZIT_URL_SIZE);

	GameEventSender::i()->SetEvent( EVENT_CLAN_CREATE, &info );

	return true;
}

bool CClanContext::SendOverlappingClanMark(UINT32 nClanMarkFlag)
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_DUPLICATE_MARK, &nClanMarkFlag );

	return true;
}

bool CClanContext::SendOverlappingClanName(const i3::wliteral_range& wTextRng)
{
	if( i3::generic_string_size(wTextRng) == 0) return false;

	//i3::safe_string_copy(m_szName,sizeof(m_szName),pszText);
	size_t size = wTextRng.size();

	GameEventSender::i()->SetEvent( EVENT_CLAN_DUPLICATE_NAME, wTextRng.c_str(), &size );		// TODO : 형변환 요구를 제거해야한다... 

	return true;
}

bool CClanContext::SendOverlappingClanAddress(const i3::wliteral_range& wTextRng)
{
	if( i3::generic_string_size(wTextRng) == 0) return false;

	GameEventSender::i()->SetEvent( EVENT_CLAN_DUPLICATE_ADDRESS, wTextRng.c_str());	// TODO : 형변환 요구를 제거해야한다... 	

	return true;
}

bool CClanContext::SendRequestJoining(UINT32 nClanIdx)
{
	m_nJoinClanIdx = nClanIdx;

	GameEventSender::i()->SetEvent(EVENT_CLAN_JOINCHECK_AUTHORITY, &nClanIdx );

	return true;
}

bool CClanContext::SendJoinToClanMember(UINT32 nClanIdx, const i3::rc_wstring& wstrMessage )
{

	GameEventSender::i()->SetEvent( EVENT_CLAN_JOIN_REQUEST, &nClanIdx, wstrMessage.c_str() );		

	return true;
}

bool CClanContext::SendCancelJoinFromClanMember()
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_CANCEL_REQUEST );

	return true;
}


void CClanContext::SendClanInvite()
{
	// revision 54949 유저찾기로 클랜초대 되지 않는 현상 수정(hansoft. 2929, 4388)

	GameEventSender::i()->SetEvent( EVENT_CLAN_INVITE);	
}

void CClanContext::SendClanInviteAccept( UINT32 wClanDBIdx, UINT8 cAccept )
{
	INT32 nServer	= g_pServerInfo->GetCurGameServerIndex();

	GameEventSender::i()->SetEvent( EVENT_CLAN_INVITE_ACCEPT_N, &wClanDBIdx, &nServer, &cAccept );	
}

void CClanContext::SendClanNote( UINT8 cType, const i3::wliteral_range& wNoteRng )
{
	UINT8 cSize = (UINT8)i3::generic_string_size( wNoteRng );
	GameEventSender::i()->SetEvent( EVENT_CLAN_NOTE, &cType, &cSize, wNoteRng.c_str() );
}

void CClanContext::SendClanDetailInfoNEvent(UINT32 ui32ClanUID, UINT8 uiClanType)
{
	PACKET_CS_NEW_DETAIL_INFO_REQ ReqPacket;

	ReqPacket.m_ui32CUID = ui32ClanUID;
	ReqPacket.m_ui8ClanType = uiClanType; // 0 == MyDetail , 1 == Detail

	GameEventSender::i()->SetEvent( EVENT_CLAN_DETAIL_INFO_N, &ReqPacket);
}

void CClanContext::CbSecessionFromClan(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		GameEventSender::i()->SetEventOnce(EVENT_CLAN_SECESSION);
	}
}

void CClanContext::CbAppointMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		UINT8 userCount = 0;

		CClanContext * pContext = (CClanContext*) pThis;

		for( UINT8 i = 0 ; i < pContext->GetIndexBufferCount(); i++ ) 
		{
			INT64 dbIdx = pContext->GetIndexBuffer(i);

			const CLAN_MEMBER_BASIC_INFO * pMember = pContext->FindMyClanMember( dbIdx );
			I3ASSERT(pMember != nullptr);

			if( pMember == nullptr || pMember->_level != CLAN_MEMBER_LEVEL_REGULAR )
				continue;

			userCount++;
		}

		if( userCount == 0 )
		{
			//	"이미 스태프로 임명된 멤버입니다."
			//	"스태프 임명 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_STAFF_ALREADY"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_STAFF_FAIL"));
			return;
		}

		INT32 count = pContext->GetIndexBufferCount();

		GameEventSender::i()->SetEvent(EVENT_CLAN_COMMISSIONSTAFF, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbDemoteMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		UINT8 userCount = 0;

		CClanContext * pContext = (CClanContext*) pThis;

		for( UINT8 i = 0 ; i < pContext->GetIndexBufferCount(); i++ ) 
		{
			INT64 dbIdx = pContext->GetIndexBuffer(i);

			const CLAN_MEMBER_BASIC_INFO * pMember = pContext->FindMyClanMember( dbIdx );
			I3ASSERT(pMember != nullptr);

			if( pMember == nullptr || pMember->_level != CLAN_MEMBER_LEVEL_STAFF )
				continue;

			userCount++;
		}

		if( userCount == 0 )
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_REGULAR_ALREADY"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_REGULAR_FAIL"));
			return;
		}

		INT32 count = pContext->GetIndexBufferCount();

		GameEventSender::i()->SetEvent(EVENT_CLAN_COMMISSION_REGULAR, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbCommissionMaster(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis;

		//	마스터 위임은 오직 한명입니다.
		if( pContext->GetIndexBufferCount() > 1 || pContext->GetIndexBufferCount() == 0)
			return;

		INT64 dbIdx = pContext->GetIndexBuffer(0);

		const CLAN_MEMBER_BASIC_INFO * pMember = pContext->FindMyClanMember(dbIdx);
		I3ASSERT( pMember != nullptr);

		GameEventSender::i()->SetEvent( EVENT_CLAN_COMMISSIONMASTER, &pMember->_i64UID );
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbDismissalMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis;
		I3ASSERT( i3::kind_of<CClanContext*>(pContext));

		//	맴버가 여러명일 수 있다.
		for( UINT8 i = 0 ; i < pContext->GetIndexBufferCount() ; i++ ) 
		{
			INT64 dbIdx = pContext->GetIndexBuffer(i);

			if( dbIdx == NetworkContext::i()->UID) 
			{
				//	"자기 자신을 제명 시킬 수 없습니다."
				//	"클랜 멤버 제명 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MYSELF"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_FAIL"));
				return;
			}
		}

		INT32 count = pContext->GetIndexBufferCount();
		GameEventSender::i()->SetEvent(EVENT_CLAN_DISMISSALMEMBER, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}	

void CClanContext::CbIntromissionMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis; 
		I3ASSERT( i3::kind_of<CClanContext*>(pContext));

		INT32 count = pContext->GetIndexBufferCount();

		//	가입 승인
		GameEventSender::i()->SetEvent(EVENT_CLAN_ACCEPT_REQUEST, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}	

void CClanContext::CbRejectionMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis;
		I3ASSERT( i3::kind_of<CClanContext*>(pContext));

		INT32 count = pContext->GetIndexBufferCount();

		//	가입 거절
		GameEventSender::i()->SetEvent(EVENT_CLAN_DENIAL_REQUEST, &count, pContext->GetIndexBuffer() );
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbDissolutionClan(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		//I3ASSERT( pThis != nullptr);

		//CClanContext * pContext = (CClanContext*) pThis;
		GameEventSender::i()->SetEvent(EVENT_CLAN_DISSOLUTION);
	}
}

// 공지사항 수정
void CClanContext::BoardNoticeModify( const i3::wliteral_range& wNoticeRng )
{
	UINT8 wLenght = (UINT8)i3::generic_string_size( wNoticeRng );
	GameEventSender::i()->SetEvent(EVENT_CLAN_NOTICE_MODIFY, &wLenght, wNoticeRng.c_str() );
	SetWaitMemberinfoReq(true);
}

// 클랜소개 수정
void CClanContext::BoardGuideModify( const i3::wliteral_range& wGuideRng )
{
	UINT8 wLenght = (UINT8)i3::generic_string_size( wGuideRng );

	GameEventSender::i()->SetEvent(EVENT_CLAN_GUIDE_MODIFY, &wLenght, wGuideRng.c_str() );
	SetWaitMemberinfoReq(true);
}

// 클랜 가입 승인 상태 수정
void CClanContext::JoinOptionModify( UINT32 ui32State )
{
	PACKET_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ ReqPacket;

	ReqPacket.m_ui32AutoAcceptOption = ui32State;

	GameEventSender::i()->SetEvent(EVENT_CLAN_ACCEPT_OPTION_MODIFY, &ReqPacket);
	SetWaitMemberinfoReq(true);
}

CLAN_DETAIL_INFO * CClanContext::GetMyClanDetailInfo(void)
{ 
	return ClanGameContext::i()->getMyClanDetailInfo();			
}

CLAN_BASIC_INFO * CClanContext::GetMyClanBasicInfo(void)
{ 
	return ClanGameContext::i()->getMyClanInfo();				
}

CLAN_MEMBER_CONTEXT * CClanContext::GetMyClanMemberContext(void)
{
	return ClanGameContext::i()->getMyClanMemberContext();
}


const CLAN_MEMBER_BASIC_INFO * CClanContext::GetMyClanMember(INT32 idx)
{
	INT32 count = GetMyClanMemberCount();

	if( count == 0) return nullptr;

	I3_BOUNDCHK(idx, count);

	return ClanGameContext::i()->getMyClanMember(idx);
}

INT32 CClanContext::GetMyClanMemberCount(void)
{
	return (INT32)GetMyClanMemberContext()->_totalCount;
}

const CLAN_MEMBER_BASIC_INFO * CClanContext::FindMyClanMember(INT64 dbIdx)
{
	for(INT32 i=0; i<GetMyClanMemberCount(); i++)
	{
		const CLAN_MEMBER_BASIC_INFO * pMember = GetMyClanMember(i);

		if( pMember->_i64UID == dbIdx)
			return pMember;
	}

	return nullptr;
}

CLAN_REQUEST_CONTEXT* CClanContext::GetMyClanSubscriberContext(void)
{
	return ClanGameContext::i()->getMyClanRequestContext();
}

CLAN_REQUEST_BASIC_INFO* CClanContext::GetMyClanSubscriberList(void)
{
	return ClanGameContext::i()->getMyClanRequest();
}

CLAN_REQUEST_BASIC_INFO * CClanContext::GetMyClanSubscriber(INT32 idx)
{
	INT32 count = GetMyClanSubscriberCount();

	if( count == 0) return nullptr;

	I3_BOUNDCHK(idx, count);

	return GetMyClanSubscriberList() + idx;
}

CLAN_REQUEST_DETAIL_INFO* CClanContext::GetMyClanSubscriberDetail(void)
{
	return ClanGameContext::i()->getClanRequestDetailInfo();
}

INT32 CClanContext::GetMyClanSubscriberCount(void)
{
	//return (INT32) GetMyClanSubscriberContext()->_sectionCount * GetMyClanSubscriberContext()->_sectionSize;
	return (INT32)GetMyClanSubscriberContext()->_totalCount;
}

void CClanContext::ClearIndexBuffer(void)
{
	m_IndexBufferCount = 0;

	i3mem::FillZero(m_IndexBuffer, sizeof( m_IndexBuffer));
}

void CClanContext::CopyIndexBuffer(INT64 * pIdxArray, INT32 count)
{
	I3ASSERT( pIdxArray != nullptr);
	I3_BOUNDCHK( count, MAX_ARRAY_COUNT);

	i3mem::FillZero(m_IndexBuffer, sizeof( m_IndexBuffer));
	m_IndexBufferCount = count;

	if( count > 0)
	{	
		i3mem::Copy(&m_IndexBuffer, pIdxArray, sizeof(INT64) * count);	
	}
}

bool	CClanContext::CheckMenagementAuth( CLAN_MEMBER_LEVEL_TYPE eEnumType, CLAN_MENAGE_AUTHORITY_TYPE eMenageType )
{
	if( CLAN_MEMBER_LEVEL_MASTER == eEnumType )		return true;
	if( CLAN_MEMBER_LEVEL_STAFF != eEnumType )		return false;

	switch( eMenageType )
	{
	case CLAN_MENAGE_TYPE_WRITE:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_BOARD)		return true;
		break;
	case CLAN_MENAGE_TYPE_INVITED:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_INVITE)		return true;
		break;
	case CLAN_MENAGE_TYPE_DEPORTATION:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_EJECT)	return true;
		break;
	case CLAN_MENAGE_TYPE_ACCEPT:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_JOIN)		return true;
		break;
	}
	return false;
}

// 클랜 배틀 팀 리스트 받기
void	CClanContext::MatchTeamList( void )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_GET_TEAM_LIST );
}

// 배틀 팀 생성
void	CClanContext::MatchTeamCreate( UINT8 cPerCount )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_CREATE_TEAM, &cPerCount );
}

// 배틀 팀 참여
void	CClanContext::MatchTeamJoin( UINT32 wTeamIdx )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_JOIN_TEAM, &wTeamIdx );
}

// 배틀 팀 떠나기
void	CClanContext::MatchTeamLeave( void )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_LEAVE_TEAM );
}

// 작전명 변경
void	CClanContext::MatchTeamChangeOperation( const i3::rc_wstring & strOperation)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_CHANGE_OPERATION, &strOperation);
}

// 배틀 팀 인원수 변경
void	CClanContext::MatchTeamChangeMaxPerson( UINT8 ui8MaxPerson )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_CHANGE_MAX_PERSON, &ui8MaxPerson );
}

//	매치매이킹 팀 등록
void	CClanContext::MatchTeamRegisterMatchMaking( void)
{
	if ( ClanGameContext::i()->isMatchTeamLeader() )
	{
		GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_REGISTER_MATCHMAKING );
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_PROPOSE_ONLY_LEADER"));/*리더만 클랜전 신청이 가능합니다.*/
	}
}

//   매치매이킹 팀 등록취쇠
void	CClanContext::MatchTeamCancelRegisterMatchMaking(void)
{
	if (ClanGameContext::i()->isMatchTeamLeader())
	{
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING);
	}
}

void CClanContext::cbClanCreate(void* pThis, INT32 nParam)
{
	if( nParam == MB_OK)
		GameEventSender::i()->SetEvent(EVENT_CLAN_CREATION_INFO);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	클랜 리스트
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClanContext::EnterClanList(bool bReqToServerForList)
{	
	_ReleaseSortClanList();

	//	default sorting type
	m_wstrCurSearchClan.clear(); 

	m_rUpdateClanListTime		= 0.f; 
	m_nUpdateClanListReq_BeginClanIdx	= 0;
	m_nUpdateClanListAck_BeginClanIdx	= -1;
	m_nUpdateClanListAck_CurrRecvClanCount = 0;

	m_nUpdateClanListCount		= 0;
	m_nAppendClanListCount		= 0;
	m_bUpdateClanListFinish		= false;
	m_bNeedUpdateAppendClanList	= true;

	if( bReqToServerForList)
		GameEventSender::i()->SetEvent(EVENT_CLAN_LIST_CONTEXT);
}

#define CLAN_LIST_UPDATE_TIME	3.0f
bool CClanContext::UpdateClanList(REAL32 rDeltaSeconds)
{
	bool bRet = false;
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

	//몇개의 예외 처리 필요함 20080102 성백 
#if defined TEST_CLAN_LIST
	if( m_sortClanList.GetCount() == 0)
	{
		_BuildSortClanListDummy();

		return true;
	}
#else
	
	// 14376 : 클랜채널에서 빠르게 클랜해체를 시도하면 Lobby Leave 된 상태에서
	// EVENT_CLAN_LIST_N 를 보내는 버그 발생. UI 이동중에는 요청하지 않도록 수정.
	if (!g_pFramework->GetUIMainframe()->GetCurrentPhaseType())
		m_bUpdateClanListFinish = false;

	if( m_bUpdateClanListFinish == false) 
	{
		if( m_bNeedUpdateAppendClanList )
		{
			m_rUpdateClanListTime += rDeltaSeconds;

			// 일정시간되면 서버에 클랜 리스트를 요청한다.
			if( m_rUpdateClanListTime > CLAN_LIST_UPDATE_TIME)
			{
				m_rUpdateClanListTime	= 0.0f;

				// 클랜채널에서 (탈퇴/제명/해체) 등의 이유로 강제로 이동중이면 처리하지 않습니다
				if(!ClanGameContext::i()->IsForceLeaveChannel())
					GameEventSender::i()->SetEvent(EVENT_CLAN_LIST_N, &m_nUpdateClanListReq_BeginClanIdx);

				m_bNeedUpdateAppendClanList = false;
				
			}
		}
	}

	// 서버로부터 받은 새로운 섹션의 클랜 리스트를 받아서 새로 리스트 추가.
	{
		if( m_nUpdateClanListCount >= (INT32) pClanContext->_totalCount)
			m_bUpdateClanListFinish = true;
		else
			m_bUpdateClanListFinish = false;

		// 업데이트 페이지가 갱신되면 리스트에 추가.
		if( !m_bUpdateClanListFinish)
		{
			if( m_nUpdateClanListReq_BeginClanIdx == m_nUpdateClanListAck_BeginClanIdx)
			{
				//	클랜 정보가 없다면 서버에 계속 요청하도록. 클랜 정보가 있다면 리스트를 갱신한다
				if( pClanContext->_sectionCount > 0)
				{
					m_nUpdateClanListCount += m_nUpdateClanListAck_CurrRecvClanCount;

					// 클랜 리스트 추가
					_BuildSortClanAppendList( m_wstrCurSearchClan);
					SortClanList(g_sort_type);

					bRet = true;	//	리스트가 갱신됐습니다.
				}
			}

			if( m_nUpdateClanListAck_BeginClanIdx + m_nUpdateClanListAck_CurrRecvClanCount < (INT32)pClanContext->_totalCount &&
				m_nUpdateClanListReq_BeginClanIdx == m_nUpdateClanListAck_BeginClanIdx)
			{
				m_nUpdateClanListReq_BeginClanIdx += m_nUpdateClanListAck_CurrRecvClanCount; // 서버 요청 섹션

			//	m_nUpdateClanListReqSection++;	// 서버 요청 섹션

				m_bNeedUpdateAppendClanList = true;
			}
		}
	}
#endif

	return bRet;
}

bool CClanContext::SearchClanListView(const i3::wliteral_range& wKeywordRng)
{
	i3::stack_wstring wstrKeyword;

	UINT32 size = i3::generic_string_size(wKeywordRng);

	for( UINT32 i = 0; i < size; i++)
	{
		if( wKeywordRng[i] != L' ')
			wstrKeyword.push_back(wKeywordRng[i]);
	}

	if( i3::generic_is_iequal( m_wstrCurSearchClan, wstrKeyword  ) )
	{
		return false;
	}

	m_wstrCurSearchClan = wstrKeyword;
	SortClanList(g_sort_type);

	return true;
}

void CClanContext::InitSortClanList(INT32 type)
{
	SortClanList(type);
}

//-----------------------------------------------------------------------------------
//	List에서 20000개의 클랜을 기준으로 sort된 시간을 측정했습니다. 
//	(측정 방법은 timeGetTime 함수를 이용해 start와 end 시간을 비교한것임.)
//
// [linear sort in List]
//	- name (string)				: about 22 s
//	- entry date (UINT32)		: about 9.3 s
//	- rank or member (UINT8)	: about 8.9 s
//
// [quick sort in List]
//	- name (string)				: about 16-17 s
//	- entry date (UINT32)		: about 9 s
//	- rank or member (UINT8)	: about 8.6 s
//-----------------------------------------------------------------------------------
void CClanContext::SortClanList(INT32 type)
{
	g_sort_type = static_cast<CLAN_SORT_TYPE>(type);

	_DynamicBuildSortClanList();

	switch( g_sort_type)
	{
	case CLAN_SORT_CUR_SEASON:
		{
			if( ClanSeasonContext::i()->GetCurrentSeason()->IsValidSeason() )
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_cur_season);
			else
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_total_record);
		}
		break;
	case CLAN_SORT_PREV_SEASON:
		{
			if( ClanSeasonContext::i()->GetPreSeasonInfo()->IsValidSeason() )
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_prev_season);
			else
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_total_record);
		}
		break;
	case CLAN_SORT_TOTAL_RECORD:
		{
			std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_total_record);
		}
		break;
	case CLAN_SORT_MY_CLAN:
		{
			std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_my_clan);

			// 정렬 후, 내 클랜 기준으로 위아래 한개 클랜씩만 표시되도록 설정합니다.
			{
				size_t count = m_sortClanList.size(); 

				for(size_t idx = 0 ; idx < count ; ++idx)
				{
					SORT_CLAN_INFO * pInfo = m_sortClanList[idx];
					if( pInfo->_info._idx == GetMyClanDetailInfo()->m_ui32CID )
					{
						i3::vector<SORT_CLAN_INFO*>	_list;

						if( idx > 0 )
							_list.push_back( m_sortClanList[idx - 1] );

						_list.push_back( m_sortClanList[idx] );

						if( idx < (count - 1))
							_list.push_back( m_sortClanList[idx + 1] );
						
						m_sortClanList = _list;
						break;
					}
				}
			}

		}
		break;
	default:		
		{
			std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_clan_idx);
		}
		break;
	}
}

INT32 CClanContext::GetClanSerchRate(void)
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

//	if( pClanContext->_sectionCount > 0 && m_nUpdateClanListAckSection >= 0)
	if ( pClanContext->_totalCount > 0 )
	{

		REAL32 rate = (REAL32)(m_nUpdateClanListCount) / (REAL32)(pClanContext->_totalCount);
					//(REAL32) (m_nUpdateClanListAckSection +1) / (REAL32)(pClanContext->_sectionCount);

		if( rate > 1.0f) rate = 1.0f;

		return (INT32)(rate * 100.0f);
	}

	return 0;
}

void CClanContext::_DynamicBuildSortClanList(void)
{
	bool build_all_clan = false;

	if( g_sort_type == CLAN_SORT_MY_CLAN )
		build_all_clan = true;

	if( m_wstrCurSearchClan.length() == 0)
		build_all_clan = true;

	// 전체 클랜 리스트 구축.
	if( build_all_clan == true )
	{
#if defined TEST_CLAN_LIST
		_BuildSortClanListDummy();
#else
		_BuildSortClanList();
#endif
	}
	else
	{
		//	설정된 키워드로 클랜 리스트 구축.
#if defined TEST_CLAN_LIST
		_BuildSearchClanListDummy( m_wstrCurSearchClan);
#else
		_BuildSearchClanList( m_wstrCurSearchClan);
#endif 
	}
}

void CClanContext::_BuildSortClanList(void)
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO  * pClanBaseInfo = ClanGameContext::i()->getClanList();
	INT32 count = pClanContext->_totalCount;

	if( pClanBaseInfo != nullptr)
		_BuildSortClanList(pClanBaseInfo, count);
}

void CClanContext::_BuildSearchClanList(const i3::wliteral_range& wKeywordRng)
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO * pClanBaseInfo = ClanGameContext::i()->getClanList();	
	INT32 count = pClanContext->_totalCount;

	_BuildSearchClanList(wKeywordRng, pClanBaseInfo, count);
}

void CClanContext::_BuildSortClanListDummy(void)
{
	//	테스트 클랜 임시 생성
	UINT32 cnt = 3000;
	CLAN_LIST_BASIC_INFO * pDummy = (CLAN_LIST_BASIC_INFO*) i3MemAlloc(sizeof( CLAN_LIST_BASIC_INFO) * cnt);
	I3ASSERT( pDummy != nullptr);
	i3mem::FillZero(pDummy, sizeof( CLAN_LIST_BASIC_INFO) * cnt);

	for(UINT32 i=0; i<cnt; i++)
	{
		CLAN_LIST_BASIC_INFO * pInfo = pDummy+i;

		TTCHAR temp[256] = _TT("");
		i3::snprintf(temp, _countof(temp), _TT("DUMMY %d"), i+1);
		pInfo->m_ClanBasicInfo._idx = i+1;
		i3::safe_string_copy( pInfo->m_ClanBasicInfo._name, temp, NET_CLAN_NAME_SIZE);
		pInfo->m_ClanBasicInfo._rank = (UINT8)(i3Math::Rand() % 50 + 1);
		pInfo->m_ClanBasicInfo._person = (UINT8)(i3Math::Rand() % 200 + 1);    
		pInfo->m_ClanBasicInfo._date = 0;

		INT32 a1 = rand() % CClanMarkManager::GetMarkBackImageCount() + 1;
		INT32 b1 = rand() % CClanMarkManager::GetMarkShapeImageCount() + 1;
		INT32 a2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;
		INT32 b2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;

		pInfo->m_ClanBasicInfo._mark = MAKE_CLANMARK_FLAG(a1, b1, a2, b2);
	}

	_BuildSortClanList(pDummy, cnt);

	I3MEM_SAFE_FREE( pDummy);
}

void CClanContext::_BuildSearchClanListDummy(const i3::wliteral_range& wKeywordRng)
{
	//	테스트 클랜 임시 생성
	UINT32 cnt = 3000;
	CLAN_LIST_BASIC_INFO * pDummy = (CLAN_LIST_BASIC_INFO*) i3MemAlloc(sizeof( CLAN_LIST_BASIC_INFO) * cnt);
	I3ASSERT( pDummy != nullptr);
	i3mem::FillZero(pDummy, sizeof( CLAN_LIST_BASIC_INFO) * cnt);

	for(UINT32 i=0; i<cnt; i++)
	{
		CLAN_LIST_BASIC_INFO * pInfo = pDummy+i;

		TTCHAR temp[256] = _TT("");
		i3::snprintf(temp, _countof(temp), _TT("DUMMY %d"), i+1);
		pInfo->m_ClanBasicInfo._idx = i+1;
		i3::safe_string_copy( pInfo->m_ClanBasicInfo._name, temp, NET_CLAN_NAME_SIZE);
		pInfo->m_ClanBasicInfo._rank = (UINT8)(i3Math::Rand() % 50 + 1);
		pInfo->m_ClanBasicInfo._person = (UINT8)(i3Math::Rand() % 200 + 1);    
		pInfo->m_ClanBasicInfo._date = 0;

		INT32 a1 = rand() % CClanMarkManager::GetMarkBackImageCount() + 1;
		INT32 b1 = rand() % CClanMarkManager::GetMarkShapeImageCount() + 1;
		INT32 a2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;
		INT32 b2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;

		pInfo->m_ClanBasicInfo._mark = MAKE_CLANMARK_FLAG(a1, b1, a2, b2);
	}

	_BuildSearchClanList(wKeywordRng, pDummy, cnt);

	I3MEM_SAFE_FREE( pDummy);
}

bool CClanContext::_CreateSortClanListStorage( INT32 nCreateCount)
{
	if( (INT32) m_nSortClanInfoCount != nCreateCount)
	{//이전 카운트와 다시받은 카운트가 다르다면 메모리 재할당
		i3::cu::for_each_delete_clear(m_pSortClanInfo);

		if( nCreateCount > 0)
		{
			m_pSortClanInfo.resize(nCreateCount);
			for (int i = 0; i < nCreateCount; i++)
				m_pSortClanInfo[i] = (new SORT_CLAN_INFO());
		}

		m_nSortClanInfoCount = nCreateCount;

		return true;
	}

	return false;
}

void CClanContext::_BuildSortClanAppendList( const i3::wliteral_range& wKeywordRng )
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO * pClanBaseInfo = ClanGameContext::i()->getClanList();	
	INT32 nTotalCnt = pClanContext->_totalCount;
	INT32 nNeedUpdateCnt = (m_nUpdateClanListCount - m_nAppendClanListCount);
	INT32 nNeedUpdateStart = m_nAppendClanListCount;

	I3ASSERT( pClanBaseInfo != nullptr);

	// 리스트 메모리 할당
	if( _CreateSortClanListStorage( nTotalCnt))
	{
		//초기화 
		m_sortClanList.clear();

		//I3TRACE( "SortClanList clear.\n");
	}

	if( nTotalCnt <= 0 || nNeedUpdateCnt <= 0 )
		return;

	m_bClanListUpdated = true;

	//	리스트 뒤로 새로운 클랜만 추가
	for(INT32 i=nNeedUpdateStart; i< m_nUpdateClanListCount; i++)
	{
		if( i >= (INT32) m_nSortClanInfoCount) break;

		m_nAppendClanListCount++;

		CLAN_LIST_BASIC_INFO * pData = pClanBaseInfo + i;

		if( pData->m_ClanBasicInfo._idx > 0)
		{
			SORT_CLAN_INFO * pNewInfo = m_pSortClanInfo[i];

			//	클랜 정보 복사본을 만든다.
			pNewInfo->CopyFromClanListInfo(pData);

			if( i3::generic_string_size( wKeywordRng) > 0)
			{
				//	업데이트되는 클랜 리스트를 받아오는 중에 검색명이 있다면 이후 받을때마다 자동으로 검색해서 클랜 리스트에 추가한다. komet
				i3::wliteral_range clan_name = pNewInfo->_info._name;
				if( i3::icontain_string( clan_name, wKeywordRng) != -1)
				{
					m_sortClanList.push_back(pNewInfo);

					//I3TRACE( "SortClanList search add %s (%d)\n", pNewInfo->_info._name, i);
				}
			}
			else
			{
				m_sortClanList.push_back(pNewInfo);

				//I3TRACE( "SortClanList add %s (%d)\n", pNewInfo->_info._name, i);
			}
		}

		//I3TRACE( "add (%d) - %s\n", i, pData->_name);
	}
}

void CClanContext::_BuildSortClanList(const i3::vector<CLAN_LIST_BASIC_INFO*>& InfoList)
{
	
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

	const size_t count = InfoList.size();

	// 리스트 메모리 할당
	_CreateSortClanListStorage(pClanContext->_totalCount);

	//초기화 
	m_sortClanList.clear();

	if( count <= 0)
		return;

	//	클랜 리스트 생성
	for( size_t i=0; i<count; i++)
	{
		if( i >= (INT32) m_nSortClanInfoCount) break;

		CLAN_LIST_BASIC_INFO * pData = InfoList[i];

		if( pData->m_ClanBasicInfo._idx > 0)
		{
			SORT_CLAN_INFO * pNewInfo = m_pSortClanInfo[i];

			//	클랜 정보 값을 복사해 둔다.
			pNewInfo->CopyFromClanListInfo(pData);

			m_sortClanList.push_back(pNewInfo);
		}
	}

}

void CClanContext::_BuildSortClanList(CLAN_LIST_BASIC_INFO * pInfo, INT32 count)
{
	I3ASSERT( pInfo != nullptr);

	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

	// 리스트 메모리 할당
	_CreateSortClanListStorage(pClanContext->_totalCount);
	
	//초기화 
	m_sortClanList.clear();

	if( count <= 0)
		return;

	//	클랜 리스트 생성
	for(INT32 i=0; i<count; i++)
	{
		if( i >= (INT32) m_nSortClanInfoCount) break;

		CLAN_LIST_BASIC_INFO * pData = pInfo + i;

		if( pData->m_ClanBasicInfo._idx > 0)
		{
			SORT_CLAN_INFO * pNewInfo = m_pSortClanInfo[i];

			//	클랜 정보 복사본을 만든다.
			pNewInfo->CopyFromClanListInfo(pData);

			m_sortClanList.push_back( pNewInfo);
		}
	}

}

void CClanContext::_BuildSearchClanList(const i3::wliteral_range& wKeywordRng, CLAN_LIST_BASIC_INFO * pInfo, INT32 count)
{	
	i3::vector<CLAN_LIST_BASIC_INFO*> searchList;
	
	for(INT32 i=0; i<count; i++) 
	{
		CLAN_LIST_BASIC_INFO * pClanInfo = pInfo + i;

		//	keyword를 포함하는 클랜 이름이 있다면
		i3::wliteral_range clan_name = pClanInfo->m_ClanBasicInfo._name;
		if( i3::icontain_string(clan_name, wKeywordRng) != -1)
		{
			searchList.push_back(pClanInfo);
		}
	} 

	_BuildSortClanList(searchList);
}

void CClanContext::_ReleaseSortClanList(void)
{
	i3::cu::for_each_delete_clear(m_pSortClanInfo);

	m_sortClanList.clear();

	m_nSortClanInfoCount	= 0;	
	m_nAppendClanListCount  = 0;
}

bool CClanContext::PossibleChangeClanName(void)
{	//	클랜명 변경 아이템 소유 여부
	return CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_CHANGE_CLAN_NAME);
}

bool CClanContext::PossibleChangeClanMark(void)
{	//	클랜 마크 아이템 변경 소유 여부
	return CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_CHANGE_CLAN_MARK);
}

// 자신이 클랜 초대권한 스탭인지 확인
bool CClanContext::IsPossibleClanInvite(void)
{
	return CheckMenagementAuth( GetMyStaffLevel(), CLAN_MENAGE_TYPE_INVITED );
}

void CClanContext::SetClanCreationRequirementInfo( UINT8 Rank, UINT32 Point )
{
	m_ClanCreationRequirementRank	= Rank;
	m_ClanCreationRequirementPoint	= Point;
	m_HasClanCreationInfo	= true;
}

bool CClanContext::GetClanCreationRequirementInfo( UINT8 * Rank, UINT32 * Point )
{
	if ( ! m_HasClanCreationInfo )
		return false;

	if ( Rank )
		*Rank	= m_ClanCreationRequirementRank;

	if ( Point )
		*Point	= m_ClanCreationRequirementPoint;

	return true;
}


//////////////////	클랜 매치 결과 // ///////////////

void CClanContext::_ClearMatchResultList(void)
{
	i3mem::FillZero(m_ClanResultList, sizeof(m_ClanResultList));
}

//void CClanContext::RequestClanMatchResultList(void)
//{
//	m_rMatchListReqTime			= 1000.0f; 
//	m_ui8MatchListReqSection	= 0; 
//	m_bMatchListReqFinish		= false;
//
//	//	매치 결과 리스트 다시 구성
//	_ClearMatchResultList();
//}
//
//void CClanContext::UpdateClanMatchRsultList(REAL32 rDeltaSeconds)
//{
//	if( m_bMatchListReqFinish == true )
//		return;
//
//	m_rMatchListReqTime += rDeltaSeconds;
//
//	if( m_rMatchListReqTime > LIST_UPDATE_TIME )
//	{
//		m_rMatchListReqTime = 0.0f;
//
//		PACKET_CS_CLAN_MATCH_RESULT_LIST_REQ ReqPacket;
//
//		// 자기 클랜 or 타 클랜(방문자)에 따라 클랜 UID 를 설정합니다.
//		if(IsClanVisit() == true)
//			ReqPacket.m_ui32CUID = ClanGameContext::i()->getClanDetailInfo()->m_ui32CID;
//		else
//			ReqPacket.m_ui32CUID = ClanGameContext::i()->getMyClanDetailInfo()->m_ui32CID;
//
//		ReqPacket.m_ui8Section = m_ui8MatchListReqSection;
//
//		GameEventSender::i()->SetEvent(EVENT_CLAN_MATCH_RESULT_LIST, &ReqPacket);
//
//		m_ui8MatchListReqSection++;
//
//		if(m_ui8MatchListReqSection >= m_ClanResultContext.m_ui8SectionCount )
//			m_bMatchListReqFinish = true;
//	}
//}

void CClanContext::ResetMatchResultContext( void)
{
	i3mem::FillZero(&m_ClanResultContext, sizeof(m_ClanResultContext));
	_ClearMatchResultList();
}

/************************************************************************/
/*								Game Event Func                         */
/************************************************************************/
void CClanContext::InitGameEventFunc(void)
{
	RegisterGameEventFunc(EVENT_CLAN_ENTER,						&CClanContext::ReceiveGameEvent_Clan_Enter					);		// 클랜 페이지 입장
	RegisterGameEventFunc(EVENT_CLAN_LEAVE,						&CClanContext::ReceiveGameEvent_Clan_Leave					);		// 클랜 페이지 퇴장
	RegisterGameEventFunc(EVENT_CLAN_MEMBER_CONTEXT,			&CClanContext::ReceiveGameEvent_Clan_MemberContext			);		// 클랜원 컨텍스트 요청
	RegisterGameEventFunc(EVENT_CLAN_MEMBER_LIST,				&CClanContext::ReceiveGameEvent_Clan_MemberList				);		// 클랜원 리스트 요청
	RegisterGameEventFunc(EVENT_CLAN_REQUEST_CONTEXT,			&CClanContext::ReceiveGameEvent_Clan_RequestContext			);		// 가입 대기자 리스트 요청
	RegisterGameEventFunc(EVENT_CLAN_ACCEPT_REQUEST,			&CClanContext::ReceiveGameEvent_Clan_AcceptRequest			);		// 클랜 가입 승인
	RegisterGameEventFunc(EVENT_CLAN_DENIAL_REQUEST,			&CClanContext::ReceiveGameEvent_Clan_DenialRequest			);		// 클랜 가입 거부
	RegisterGameEventFunc(EVENT_CLAN_SECESSION,					&CClanContext::ReceiveGameEvent_Clan_Secession				);		// 클랜 탈퇴
	RegisterGameEventFunc(EVENT_CLAN_COMMISSIONMASTER,			&CClanContext::ReceiveGameEvent_Clan_CommissionMaster		);		// 클랜 마스터 위임
	RegisterGameEventFunc(EVENT_CLAN_COMMISSIONSTAFF,			&CClanContext::ReceiveGameEvent_Clan_CommissionStaff		);		// 클랜 스태프 임명
	RegisterGameEventFunc(EVENT_CLAN_COMMISSION_REGULAR,		&CClanContext::ReceiveGameEvent_Clan_CommissionRegular		);		// 일반 클랜원으로 강등
	RegisterGameEventFunc(EVENT_CLAN_DISMISSALMEMBER,			&CClanContext::ReceiveGameEvent_Clan_DismissalMember		);		// 클랜원 제명
	RegisterGameEventFunc(EVENT_CLAN_NOTICE_MODIFY,				&CClanContext::ReceiveGameEvent_Clan_ModifyOption			);		// 공지사항 변경
	RegisterGameEventFunc(EVENT_CLAN_GUIDE_MODIFY,				&CClanContext::ReceiveGameEvent_Clan_ModifyOption			);		// 클랜 소개글 변경
	RegisterGameEventFunc(EVENT_CLAN_ACCEPT_OPTION_MODIFY,		&CClanContext::ReceiveGameEvent_Clan_ModifyOption			);		// 클랜 가입 자동/수동 승인 변경
	RegisterGameEventFunc(EVENT_CLAN_CANCEL_REQUEST,			&CClanContext::ReceiveGameEvent_Clan_CancelRequest			);		// 가입 신청서 취소
	RegisterGameEventFunc(EVENT_CLAN_DISSOLUTION,				&CClanContext::ReceiveGameEvent_Clan_Dissolution			);		// 클랜 해체
	RegisterGameEventFunc(EVENT_CLAN_MANAGEMENT,				&CClanContext::ReceiveGameEvent_Clan_Management				);		// 클랜 관리 페이지 수정

	RegisterGameEventFunc(EVENT_CLAN_WAR_CREATE_TEAM,			&CClanContext::ReceiveGameEvent_ClanWar_CreateTeam			);		// 클랜전 팀 생성
	RegisterGameEventFunc(EVENT_CLAN_WAR_JOIN_TEAM,				&CClanContext::ReceiveGameEvent_ClanWar_JoinTeam			);		// 클랜전 팀 입장
	RegisterGameEventFunc(EVENT_CLAN_WAR_LEAVE_TEAM,			&CClanContext::ReceiveGameEvent_ClanWar_LeaveTeam			);		// 클랜전 팀 퇴장
	RegisterGameEventFunc(EVENT_CLAN_WAR_CHANGE_MAX_PERSON,		&CClanContext::ReceiveGameEvent_ClanWar_ChangeMaxPerson		);		// 클랜전 팀 구성 변경
}


void CClanContext::RegisterGameEventFunc(GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert( i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f) );
}

void CClanContext::ReceiveGameEvent_Clan_Enter(INT32 arg, const i3::user_data & )
{
	if( EV_FAILED(arg))
	{
		GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_CLAN_ENTER_FAIL_E") );
		return;
	}

	m_bEnter = true;
	
	if( arg == 1 )		m_bMember = true;		//	클랜에 가입되어 있습니다.
	else if (arg == 0)	m_bMember = false;		//	클랜 미가입 상태입니다.
	else				I3ASSERT_0;
}

void CClanContext::ReceiveGameEvent_Clan_Leave(INT32 arg, const i3::user_data & )
{
	if (EV_FAILED(arg))
	{
		GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_CLAN_LEAVE_FAIL_E") );
		return;
	}

	m_bEnter = false;
}

void CClanContext::ReceiveGameEvent_Clan_MemberContext(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg) )
		RequestClanMember();
}

void CClanContext::ReceiveGameEvent_Clan_MemberList(INT32 arg, const i3::user_data & )
{

}

void CClanContext::ReceiveGameEvent_Clan_RequestContext(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg) )
		RequestClanSubscriber();
}

void CClanContext::ReceiveGameEvent_Clan_AcceptRequest(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		// "%d명의 클랜 가입 처리가 승인되었습니다."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_PROCESS_SUBSCRIPTION"), arg);
		// 클랜 가입 승인
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp , &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_SUCCESS"));
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}
	else
	{
		switch( arg ) 
		{
		case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
			{
				//	"\n클랜 가입 처리가 실패했습니다.\n클랜을 찾을수 없습니다."
				//	클랜 가입 승인 실패
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_NOMEMBER:
			{
				//	"\n클랜 가입 처리가 실패했습니다.\n클랜원이 아닙니다."
				//	클랜 가입 승인 실패
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
			{
				//	"\n클랜 가입 처리가 실패했습니다.\n클랜 마스터가 아닙니다."
				//	클랜 가입 승인 실패
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_MASTER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_MEMBER:
			{
				//	"\n클랜 가입 처리가 실패했습니다.\n이미 클랜에 가입되어있습니다."
				//	클랜 가입 승인 실패
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ALREADY_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_MAXMEMBER:
			{
				//	"\n클랜 가입 처리가 실패했습니다.\n더이상 가입 할 수 없습니다."
				//	클랜 가입 승인 실패
				GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		default:
			{
				i3::rc_wstring wstrTemp;
				//	""\n클랜 가입 처리가 실패했습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오."
				i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ASK_FOR_INFO"), arg );
				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	//	클랜 가입 승인 실패
			}
			break;
		}
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT); 
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_DenialRequest(INT32 arg, const i3::user_data & )
{
	if( EV_FAILED(arg))			
	{
		//	"클랜 거부 처리가 실패했습니다.
		//	"클랜 가입 거부"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REJECTION_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_FAIL"));
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT); 
	}
	else
	{	
		//	"%d명의 클랜 가입을 거부하였습니다."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REJECTION_SUCCESS"), arg);
		//	"클랜 가입 거부"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_FAIL"));
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_Secession(INT32 arg, const i3::user_data & )
{
	if( EV_FAILED( arg ) )
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_LEVEL_MASTER:
			//	"클랜 마스터는 클랜을 탈퇴할 수 없습니다.\n탈퇴하기 위해선 클랜 마스터를 위임해야 합니다."
			//	"클랜 탈퇴 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_MASTER_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_FAIL"));
			break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL:
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_MATCH_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_FAIL"));
			break;
		case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
			if(g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_CLAN_MEMBER) == true)
			{
				g_pFramework->GetUIMainframe()->ClosePopup(UPW_CLAN_MEMBER);
			}
			break;
		default:
			//	"클랜 탈퇴가 실패했습니다."
			//	"클랜 탈퇴 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_FAIL"));
			break;
		}
	}
}

void CClanContext::ReceiveGameEvent_Clan_CommissionMaster(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		//	"클랜 마스터 위임이 정상적으로 처리되었습니다."
		//	"클랜 마스터 위임 완료"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_SUCCESS"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_SUCCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32ClanState = CLAN_MEMBER_LEVEL_STAFF;
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}
	else 
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_USE_ITEM_FAIL:
			//	"사용중인 아이템이 있어\n클랜 마스터 위임에 실패했습니다."
			//	"클랜 마스터 위임 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_ITEM_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_FAIL"));
			break;
		default:
			//	"클랜 마스터 위임이 실패했습니다."
			//	"클랜 마스터 위임 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_FAIL"));
			break;
		}
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_CommissionStaff(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		//	"1명을 스태프로 임명 하였습니다."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_STAFF_SUCESS"), arg);
		//	"스태프 임명 완료"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_STAFF_SUCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);
	}
	else {
		//	"스태프 임명에 실패했습니다."
		//	"스태프 임명 실패"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_STAFF_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_STAFF_FAIL"));
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_CommissionRegular(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		i3::rc_wstring wstrTemp;

		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_REGULAR_SUCCESS"), arg);
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_REGULAR_SUCCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);
	}
	else {
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_REGULAR_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_REGULAR_FAIL"));
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_DismissalMember(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		//	"%d명을 제명 하였습니다."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_SUCCESS"), arg);
		//	"클랜 멤버 제명 완료"
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_SUCCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}
	else 
	{
		//	"클랜 멤버 제명에 실패했습니다."
		//	"클랜 멤버 제명 실패"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_FAIL"));
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_ModifyOption(INT32 arg, const i3::user_data & )
{	
	if( EV_SUCCESSED( arg ) )
	{
		//	"수정된 내용의 저장이 완료되었습니다."
		//	"수정 완료"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_MODIFY_SUCCESS") );
	}
	else 
	{
		if( EV_IS_ERROR( arg, EVENT_ERROR_CLAN_LEVEL_NOSTAFF))
		{
			//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
			//	"수정 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_MODIFY_FAIL"));
		}
		else
		{
			//	"수정된 내용의 저장에 실패했습니다."
			//	"수정 실패"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_MODIFY_FAIL") );
		}
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_CancelRequest(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		ResetClanCreationInfo();
		//	"가입신청이 취소되었습니다."
		//	"가입신청 취소 완료"
		// MessageBox 종료 후 클랜 가입 경고문 팝업이 나타나야 되므로 
		// 종료될때까지 대기한다.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANCEL_REQUEST_SUCCESS"), 
			&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_CANCEL_REQUEST_SUCCESS"));//, /*MAKE_CALLBACK(cbClanCreate)*/nullptr , this);
	}
	else 
	{
		//	"가입신청이 취소가 실패했습니다."
		//	"가입신청 취소 실패"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANCEL_REQUEST_FAIL"), 
			&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_CANCEL_REQUEST_FAIL"));
	}
}

void CClanContext::ReceiveGameEvent_Clan_Dissolution(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{

	}
	else
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
			{
				//	"클랜을 찾을수 없습니다."
				//	"클랜 해체 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_NO_FINDING"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;

		case EVENT_ERROR_CLAN_NOMEMBER:
			{
				//	"클랜의 멤버가 아닙니다."
				//	"클랜 해체 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_NO_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
			{
				//	"클랜 해체는 마스터만 가능합니다."
				//	"클랜 해체 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_ONLY_MASTER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_CLOSECLAN_MEMBER_FAIL:
			{
				//	 "클랜 멤버가 남은 상태로는 클랜 해체가 불가능 합니다."
				//	"클랜 해체 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_REMAIN_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_CLOSECLAN_FAIL:
			{
				//	"클랜 해체에 실패했습니다."
				//	"클랜 해체 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		default:
			{
				i3::rc_wstring wstrTemp;
				//	"\n클랜 해체에 실패하였습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오."
				i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", 
					GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_ASK_FOR_INFO"), arg );

				//	"클랜 해체 실패"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		}
	}
}

void CClanContext::ReceiveGameEvent_Clan_Management(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg))
	{
		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
		//	"수정된 내용의 저장이 완료되었습니다."
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_COMPLETE"), &GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY")); 
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY"));
	}
}

void CClanContext::ReceiveGameEvent_ClanWar_CreateTeam(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg))
		m_bInBattleTeam = true;
	else
	{
		m_bInBattleTeam = false;
	}
}

void CClanContext::ReceiveGameEvent_ClanWar_JoinTeam(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg))
		m_bInBattleTeam = true;
	else
	{
		m_bInBattleTeam = false;

		switch( arg)
		{				
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL:
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL:
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_JOIN"));/*팀 참가에 실패했습니다.\n 잘못된 팀입니다.*/
			}
			break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL:
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FULL_MEMBER"));/*팀의 정원이 모두 찼기 때문에 해당 팀에 참여할 수 없습니다.*/
			}
			break;	
		default:
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FAILED_JOIN"));/*네트워크 문제로 팀에 참가하지 못했습니다.\n 다시 시도해 주세요.*/
			}
			break;
		}
	}
}

void CClanContext::ReceiveGameEvent_ClanWar_LeaveTeam(INT32 arg, const i3::user_data & )
{
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	if( pMySquadInfo->isJoinTeam() == false )
		m_bInBattleTeam = false;
}

void CClanContext::ReceiveGameEvent_ClanWar_ChangeMaxPerson(INT32 arg, const i3::user_data & )
{
	//	팀 구성 변경을 시도
	if( EV_SUCCESSED(arg))
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CHANGE_ORGANIZATION"));/*팀 구성이 변경되었습니다.*/
	}
	else
	{
		if( ClanGameContext::i()->isMatchTeamLeader() )	//	리더에게만 보여준다.
		{
			switch(arg)
			{
			case EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL:
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_FIND"));/*팀을 찾을 수 없습니다.\n 다시 시도해 주세요.*/
				break;
			case EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_FAIL:
			case EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_PERMAX_FAIL:
			case EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_NOMASTER_FAIL:
			default:
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_CHANGE"));/*팀 구성을 변경할 수 없습니다.*/
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////