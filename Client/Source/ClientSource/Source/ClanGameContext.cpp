#include "pch.h"
#include "ClanGameContext.h"
#include "ClanStructDef.h"
#include "UserInfoContext.h"
#include "CommunityContext.h"
#include "./StageBattle/NetworkContext.h"
#include "i3Base/ColorString.h"

ClanGameContext::ClanGameContext()
{

	////////////////////////////////////////////////////////////////////////////////////
	//Clan

	i3mem::FillZero(&m_RequestDetailInfo, sizeof(m_RequestDetailInfo));

	m_bClanBattleLobbyEnable = false;

	i3mem::FillZero(&m_ClanContext,			sizeof(CLAN_CONTEXT));
	i3mem::FillZero(&m_ClanDetailInfoAll,	sizeof(CLAN_DETAIL_INFO_ALL));
	m_pClanList = nullptr;

	i3mem::FillZero(&m_MyClanInfo,			sizeof(CLAN_BASIC_INFO));
	i3mem::FillZero(&m_MyClanDetailInfoAll,	sizeof(CLAN_DETAIL_INFO_ALL));
	i3mem::FillZero(&m_MyClanMemberContext,	sizeof(CLAN_MEMBER_CONTEXT));
	m_pMyClanMemberList = nullptr;
	m_MyClanMemberMaxCount	= 0;

	i3mem::FillZero(&m_MyClanRequestContext, sizeof(CLAN_REQUEST_CONTEXT));
	m_pMyClanRequestList = nullptr;

	m_MyClanMatchTeamList.clear();
	m_ClanAllMatingInfo.Reset();
	m_MyMatchSquadInfo.Reset();

	for(INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
		m_ClanBattleTeamInfo[idx].Reset();

	m_bClanChannel = false;

	m_ClanMercenaryList.clear();
	m_bIsMercenary = false;
	m_SelectedMerID = 0;

	m_ClanMercenaryRecord.Reset();
	m_bMerceInfo = false;
	m_bMatchEnd = false;
	m_bMyMercDepartured = false;
	m_TeamIndex = 0;
	m_bForceLeaveChannel = false;
	for(int i = 0; i < SLOT_MAX_COUNT ; ++i)	 m_MercRank[i] = -1; 
}

ClanGameContext::~ClanGameContext()
{
	I3MEM_SAFE_FREE(m_pClanList);
	I3MEM_SAFE_FREE(m_pMyClanMemberList);
	I3MEM_SAFE_FREE(m_pMyClanRequestList);
}

CLAN_LIST_BASIC_INFO * ClanGameContext::CreateClanList(INT32 Count)
{
	if(Count <= 0) return nullptr;

	I3PRINTLOG(I3LOG_NOTICE, "클랜 리스트 생성 갯수 : %d", Count);

	I3MEM_SAFE_FREE(m_pClanList);
	m_pClanList = (CLAN_LIST_BASIC_INFO *)i3MemAlloc( sizeof(CLAN_LIST_BASIC_INFO) * Count);
	if (m_pClanList == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE, "클랜 리스트 MemAlloc 실패.");
		return nullptr;
	}

	i3mem::FillZero( m_pClanList, sizeof(CLAN_LIST_BASIC_INFO) * Count);

	return m_pClanList;
}

void ClanGameContext::CreateMyClanMemberList(INT32 Count)
{
	if(Count <= 0) return;

	I3MEM_SAFE_FREE(m_pMyClanMemberList);

	INT32 Size = sizeof(CLAN_MEMBER_BASIC_INFO) * Count;
	m_pMyClanMemberList = (CLAN_MEMBER_BASIC_INFO *)i3MemAlloc( Size );
	i3mem::FillZero( m_pMyClanMemberList, Size);

	m_MyClanMemberMaxCount = Count;
}

CLAN_REQUEST_BASIC_INFO * ClanGameContext::CreateMyClanRequestList(INT32 Count)
{
	if(Count <= 0) return nullptr;

	I3MEM_SAFE_FREE(m_pMyClanRequestList);
	m_pMyClanRequestList = (CLAN_REQUEST_BASIC_INFO *)i3MemAlloc( sizeof(CLAN_REQUEST_BASIC_INFO) * Count);
	i3mem::FillZero( m_pMyClanRequestList, sizeof(CLAN_REQUEST_BASIC_INFO) * Count);

	return m_pMyClanRequestList;
}

const CLAN_MEMBER_BASIC_INFO *	ClanGameContext::getMyClanMember(INT32 Index)
{
	if ( Index < 0 || Index >= m_MyClanMemberMaxCount )
		return nullptr;

	return &m_pMyClanMemberList[Index];
}

void ClanGameContext::setMyClanMember( INT32 Index, const CLAN_MEMBER_BASIC_INFO & MemberInfo )
{
	if ( Index < 0 || Index >= m_MyClanMemberMaxCount )
		return;
	m_pMyClanMemberList[Index] = MemberInfo;
}

void ClanGameContext::setMyClanMatchTeamList(const CLAN_MATCH_TEAM_LIST_DATA & Data )
{
	CLAN_MATCH_TEAM_LIST_DATA & data = const_cast<CLAN_MATCH_TEAM_LIST_DATA &>(Data);
	m_MyClanMatchTeamList.clear();

	const MATCH_TEAM_INFO * pTeamInfo = &data.m_aTeamInfo;
	const UINT8 nCount = data.m_ui8MyClanTeam;
	INT8 ui8MyIdx = -1;

	for (UINT8 idx = 0; idx < nCount; ++idx)
	{
		if ( m_MyMatchSquadInfo._TMatchTeamID == pTeamInfo[idx].m_TTeamID )
			ui8MyIdx = (INT8)idx;
		m_MyClanMatchTeamList.push_back(CLAN_MATCH_TEAM_INFO(pTeamInfo[idx]));
	}
	
	// 내 매치 스쿼드 현재 인원에 대한 정보를 동기화 해줍니다.
	//INT8 i8MatchIdx = m_MyMatchSquadInfo. getMySlot();	
	if ( -1 < ui8MyIdx )
	{
		// 인덱스가 초과할 일은 없지만, 만약을 대비해서..
		if ( m_MyClanMatchTeamList.size() <= (size_t)ui8MyIdx)
		{
			i3::rc_wstring wstLog;
			i3::sprintf(wstLog, L"[CLAN_MATCH] Wrong Match Idx : %d", ui8MyIdx);
			I3PRINTLOG(I3LOG_FATAL, wstLog);
		}
		else
		{
			m_MyMatchSquadInfo._ui8CurPerson = m_MyClanMatchTeamList[ui8MyIdx]._ui8CurPerson;
		}
	}
}

const CLAN_MATCH_TEAM_INFO * ClanGameContext::getMyClanMatchTeam(const UINT8 TeamIdx) const
{
	INT32 count = m_MyClanMatchTeamList.size();

	for(INT32 idx = 0; idx < count ; ++idx)
	{
		const ClanMatchTeamList::const_iterator iter = &m_MyClanMatchTeamList.at(idx);
		
		if( iter->getClanTeamIdx() == TeamIdx )
			return iter;
	}

	return nullptr;
}

void ClanGameContext::setClanAllMatingInfo( const CLAN_ALL_MATCHING_DATA & Data )
{
	m_ClanAllMatingInfo.ParseData(Data);
}

void ClanGameContext::setMyMatchSquadInfo(const CLAN_WAR_CREATE_TEAM_DATA & Data)
{
	USER_INFO_BASIC 	Info;
	UserInfoContext::i()->GetMyUserInfoBasic(&Info);

	USER_INFO_INVITEM_DATA invData;
	UserInfoContext::i()->GetMyUserInfoInvItemData(&invData);

	MATCH_MEMBER_INFO Member;

	Member.m_TUID			= NetworkContext::i()->UID;
	Member.m_ui8MemberType	= 0;
	Member.m_i8Slot			= 0;
	Member.m_ui8Rank		= static_cast<UINT8>(Info.m_ui32Rank);
	Member.m_ui8NickColor	= invData._ui8NickColor;
	Member.m_ui8MerClass	= 0;
	i3::generic_string_copy(Member.m_strNick, Info.m_strNick);

	m_MyMatchSquadInfo.ParseData(Data, Member);

	// 팀 리스트에 자기 팀을 등록.
	const MATCH_TEAM_INFO & TeamInfo = const_cast<CLAN_WAR_CREATE_TEAM_DATA &>(Data).m_TeamlInfo;
	m_MyClanMatchTeamList.push_back( CLAN_MATCH_TEAM_INFO(TeamInfo) );

	m_TeamIndex = static_cast<UINT8>(m_MyClanMatchTeamList.size() - 1);
}

void ClanGameContext::setMyMatchSquadInfo(const CLAN_WAR_JOIN_TEAM_DATA & Data)
{
	m_MyMatchSquadInfo.ParseData(Data);
}

void ClanGameContext::modifyMyMatchSquadInfo(const CLAN_WAR_LEAVE_TEAM_DATA & Data)
{
	CLAN_WAR_LEAVE_TEAM_DATA & data = const_cast<CLAN_WAR_LEAVE_TEAM_DATA &>(Data);
	const INT8		i8LeaveSlot		= data.m_i8LeaveSlot;
	
	if( i8LeaveSlot < 0 || i8LeaveSlot >= MAX_CLAN_MATCH_MEMBER)
	{
		I3ASSERT_ONCE(i8LeaveSlot >= 0 && i8LeaveSlot < MAX_CLAN_MATCH_MEMBER);
		return;
	}

	if( m_MyMatchSquadInfo.isMySlot(i8LeaveSlot) )
	{
		// 자신이 나간거면 팀 정보를 초기화
		m_MyMatchSquadInfo.Reset();
	}
	else
	{
		// 다른 팀원이 나간거면 정보 갱신
		const INT8 i8LeaderSlot = data.m_i8LeaderSlot;

		// 방장이 나간 경우 다른 유저가 리더가 됨.
		if( m_MyMatchSquadInfo.isTeamLeader(i8LeaveSlot) )
			m_MyMatchSquadInfo.setLeaderSlot(i8LeaderSlot);

		m_MyMatchSquadInfo.resetMember(i8LeaveSlot);
	}

	if (m_MyMatchSquadInfo.isJoinTeam() == false)
		g_pFramework->GetClanContext()->SetInBattleTeam(false);
}

void ClanGameContext::modifyMyMatchSquadOperation(const CLAN_WAR_CHANGE_OPERATION_DATA & Data)
{
	CLAN_WAR_CHANGE_OPERATION_DATA & data = const_cast<CLAN_WAR_CHANGE_OPERATION_DATA &>(Data);

	i3::rc_wstring wstrOperation;

	wstrOperation = &data.m_strOperation;

	m_MyMatchSquadInfo.setOperation(wstrOperation);
}

void ClanGameContext::modifyMyMatchSquadMaxPerson(const CLAN_WAR_CHANGE_MAX_PERSON_DATA & Data)
{
	CLAN_WAR_CHANGE_MAX_PERSON_DATA & data = const_cast<CLAN_WAR_CHANGE_MAX_PERSON_DATA &>(Data);
	m_MyMatchSquadInfo.setMaxPerson(data.m_ui8MaxMemberCount);
}

const bool ClanGameContext::isMatchTeamLeader(void) const
{
	return m_MyMatchSquadInfo.isTeamLeader( m_MyMatchSquadInfo.getMySlot() );
}

void ClanGameContext::setClanBattleTeamInfo(const CLAN_BATTLE_TEAM_DATA & Data)
{
	CLAN_BATTLE_TEAM_DATA & data = const_cast<CLAN_BATTLE_TEAM_DATA &>(Data);
	const CLAN_MATCHING_TEAM_INFO * pClanInfo = &data.m_aClanInfo;
	MATCHMAKING_RECORD* pRecord = &data.m_aMatchMakingRecord;

	int count = data.m_aMatchMakingRecord.GetCount();

	for (INT32 idx = 0; idx < count; ++idx)
	{
		if(pRecord[idx].m_i8RoomSlotIdx >= 0)
		{
			INT32 slotIdx = pRecord[idx].m_i8RoomSlotIdx;
			m_pMatchRecord[slotIdx].ParseData(pRecord[idx]);
		}
	}

	for(INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
		m_ClanBattleTeamInfo[idx].ParseData(pClanInfo[idx]);
}

CLAN_PERSONAL_MATCH_RECORD* ClanGameContext::getMathRecord(INT32 slotidx)
{
	if (slotidx < 0 || slotidx >= SLOT_MAX_COUNT)
		return nullptr;

	return &m_pMatchRecord[slotidx];
}

void ClanGameContext::resetClanBattleTeamInfo(void)
{
	for(INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
		m_ClanBattleTeamInfo[idx].Reset();
}

/************************************************************************/
/*								Clan Mercenary							*/
/************************************************************************/
void ClanGameContext::setClanMercenaryList( const CLAN_WAR_MERCENARY_LIST_DATA & Data )
{
	CLAN_WAR_MERCENARY_LIST_DATA & data = const_cast<CLAN_WAR_MERCENARY_LIST_DATA &>(Data);

	// 첫 데이터인 경우 리스트 초기화
	if( data.m_ui16StartIdx == 0 )
		m_ClanMercenaryList.clear();


	const MERCENARY_INFO * pMercenaryInfo = nullptr; 
	const UINT16 nCount = data.m_ui16CurCount;

	if(nCount > 0 )
		pMercenaryInfo = &data.m_aMerInfo;

	for (UINT16 idx = 0; idx < nCount; ++idx)
	{
		if(pMercenaryInfo != nullptr )
			m_ClanMercenaryList.push_back(CLAN_MERCENARY_INFO(pMercenaryInfo[idx]));
	}

	struct fn_sort
	{	
		bool operator()(const CLAN_MERCENARY_INFO & lhs, const CLAN_MERCENARY_INFO & rhs) const
		{
			return lhs.getMecenaryID() < rhs.getMecenaryID();
		}
	};

	i3::sort( m_ClanMercenaryList.begin(), m_ClanMercenaryList.end(), fn_sort() );
}

const CLAN_MERCENARY_INFO * ClanGameContext::getClanMercenaryInfo( const INT32 slot_idx ) const
{
	INT32 count = m_ClanMercenaryList.size();

	if( slot_idx > -1 && slot_idx < count )
		return &m_ClanMercenaryList.at(slot_idx);

	return nullptr;
}

const CLAN_MERCENARY_INFO * ClanGameContext::getClanMercenaryInfo( const T_MerID MerID ) const
{
	INT32 count = m_ClanMercenaryList.size();

	for(INT32 idx = 0; idx < count ; ++idx)
	{
		const ClanMercenaryList::const_iterator iter = &m_ClanMercenaryList.at(idx);

		if( iter->getMecenaryID() == MerID )
			return iter;
	}

	return nullptr;
}

const CLAN_MERCENARY_INFO * ClanGameContext::getClanMercenaryInfoByName(const i3::rc_wstring str) const
{	
	INT32 count = m_ClanMercenaryList.size();

	for(INT32 idx = 0; idx < count; ++idx)
	{
		const ClanMercenaryList::const_iterator iter = &m_ClanMercenaryList.at(idx);

		if( i3::generic_is_equal( iter->getNickName(), str))
			return iter;
	}

	return nullptr;
}

void ClanGameContext::setClanMercenaryRecord( const CLAN_WAR_MERCENARY_RECORD_DATA & Data)
{
	// UserProfile을 띄우기 위해 세팅. 기획의 요청에 따라 PACKET_H_BASE_USER_DETAIL_INFO_ACK 에 일반 전적이 아닌 용병 전적을 세팅합니다.

	CLAN_WAR_MERCENARY_RECORD_DATA &data = const_cast<CLAN_WAR_MERCENARY_RECORD_DATA &>(Data);
	
	// 용병 전적
	MERCENARY_DETAIL_INFO MercenaryInfo = data.m_DetailInfo;
	m_ClanMercenaryRecord = CLAN_MERCENARY_RECORD(MercenaryInfo.m_Recod);

	PACKET_H_BASE_USER_DETAIL_INFO_ACK Info;
	S2_USER_DETAIL_INFO UserInfo = Data.m_UserDetailInfo;
	Info.m_i32Rv = data.m_TResult;
	Info.m_UserInfoLastGame = UserInfo.m_LastGame;
	Info.m_iCount = UserInfo.m_i8CharCount;
	
	Info.m_UserInfoRecord._match = m_ClanMercenaryRecord.getMatchCount(); 
	Info.m_UserInfoRecord._win = m_ClanMercenaryRecord.getWinMatchCnt();
	Info.m_UserInfoRecord._lose = m_ClanMercenaryRecord.getLoseMatchCnt();
	Info.m_UserInfoRecord._draw = m_ClanMercenaryRecord.getMatchCount() - m_ClanMercenaryRecord.getWinMatchCnt() - m_ClanMercenaryRecord.getLoseMatchCnt();
	Info.m_UserInfoRecord._killcount = m_ClanMercenaryRecord.getKillCount();
	Info.m_UserInfoRecord._headshot = m_ClanMercenaryRecord.getHeadshotCount();
	Info.m_UserInfoRecord._death = m_ClanMercenaryRecord.getDeathCount();
	Info.m_UserInfoRecord._dmatch = UserInfo.m_ui32Match; // ?? 
	Info.m_UserInfoRecord._dkillcount = 0;
	Info.m_UserInfoRecord._dis = m_ClanMercenaryRecord.getDisconMatchCnt();
	Info.m_UserInfoRecord._AssistCount = m_ClanMercenaryRecord.getAssistCount();

	Info.m_UserInfoRecord._smatch = m_ClanMercenaryRecord.getMatchCount();
	Info.m_UserInfoRecord._swin = m_ClanMercenaryRecord.getWinMatchCnt();
	Info.m_UserInfoRecord._slose = m_ClanMercenaryRecord.getLoseMatchCnt();
	Info.m_UserInfoRecord._sdraw = m_ClanMercenaryRecord.getMatchCount() - m_ClanMercenaryRecord.getWinMatchCnt() - m_ClanMercenaryRecord.getLoseMatchCnt();
	Info.m_UserInfoRecord._skillcount = m_ClanMercenaryRecord.getKillCount();
	Info.m_UserInfoRecord._sheadshot = m_ClanMercenaryRecord.getHeadshotCount();
	Info.m_UserInfoRecord._sdeath = m_ClanMercenaryRecord.getDeathCount();
	Info.m_UserInfoRecord._sdmatch = UserInfo.m_ui32Match;
	Info.m_UserInfoRecord._sdkillcount = 0;
	Info.m_UserInfoRecord._sdis = m_ClanMercenaryRecord.getDisconMatchCnt();
	Info.m_UserInfoRecord._sAssistCount = m_ClanMercenaryRecord.getAssistCount();
	
	CCommunityContext::i()->SetUserDetailInfo(&Info);

	if (UserInfo.m_i8CharCount > 0)
	{
		CCommunityContext::i()->SetUserDetailInfo()->_Clear();

		for (INT32 idx = 0; idx < UserInfo.m_i8CharCount; ++idx)
		{
			PACKET_D_BASE_USER_DETAIL_INFO_CHARA ack_Chara;
			ack_Chara.m_ui8CharaState = CHARA_STATE_POS;
			ack_Chara.m_ui8SlotNum = UserInfo.m_aCharInfo[idx].m_ui8SlotIdx;
			ack_Chara.m_ui32CharaItemID = UserInfo.m_aCharInfo[idx].m_TItemID;
			i3::generic_string_copy(ack_Chara.m_strCharaName, UserInfo.m_aCharInfo[idx].m_strCharName);
			ack_Chara.m_ui32CreateDate = UserInfo.m_aCharInfo[idx].m_ui32CreateDate;
			ack_Chara.m_CharaSkill.m_PrimarySkill = (CHARA_CLASS)UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8Class;
			ack_Chara.m_CharaSkill.m_CommonSkill = (COMMON_SKILL_TYPE)UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8CommonSkill;
			ack_Chara.m_CharaSkill.m_MainSkill = (MAIN_SKILL_TYPE)UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8MainSkill;
			ack_Chara.m_CharaSkill.m_AssistSkill = (ASSIST_SKILL_TYPE)UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8AssistSkill;
			ack_Chara.m_CharaSkill.m_ui8CommonSkillLv = UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8CommonSkillLv;
			ack_Chara.m_CharaSkill.m_ui8MainSkillLv = UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8MainSkillLv;
			ack_Chara.m_CharaSkill.m_ui8AssistSkillLv = UserInfo.m_aCharInfo[idx].m_SkillInfo.m_ui8AssistSkillLv;

			CCommunityContext::i()->AddUserDetailInfoChara(ack_Chara);
		}
	}



	PACKET_BASE_USER_BASIC_INFO_ACK BasicInfo;

	BasicInfo.m_i32Rv = data.m_TResult ;
	BasicInfo.m_i64FindUID = UserInfo.m_TUID;
	BasicInfo.m_clanMark = UserInfo.m_ui32ClanMark;
	BasicInfo.m_rank = UserInfo.m_ui8Rank;
	BasicInfo.m_exp = UserInfo.m_ui32Exp;
	BasicInfo.m_killCount = UserInfo.m_ui32Kill;
	BasicInfo.m_deathCount = UserInfo.m_ui32Death;
	BasicInfo.m_State = UserInfo.m_ui32State;
	BasicInfo.m_ServerIdx = UserInfo.m_ui32ServerIdx;
	BasicInfo.m_ChannelNum = UserInfo.m_ui32ChannelNum;
	BasicInfo.m_RoomIdx = UserInfo.m_ui32RoomIdx;
	BasicInfo.m_nickColor = UserInfo.m_InvenData._ui8NickColor;
	BasicInfo.m_DisguiseRank = UserInfo.m_InvenData._wRank;
	BasicInfo.m_AprilFoolRank = UserInfo.m_InvenData._wAprilFoolRank;

	//BasicInfo.m_ui32TotalBattleTime	= Recv.m_UserDetailInfo.;

	i3::generic_string_copy(BasicInfo.m_strClanName, UserInfo.m_strClanName);

	i3::safe_string_copy(BasicInfo.m_nickName, UserInfo.m_strNickName, NET_NICK_NAME_SIZE);

	CCommunityContext::i()->SetUserSimpleInfo(&BasicInfo);

	CCommunityContext::i()->SetCommunityUserInfo(false);
}

void ClanGameContext::ProcessMatchResult(const CLAN_MATCH_MAKING_RESULT & Data)
{
	CLAN_MATCH_MAKING_RESULT &data = const_cast<CLAN_MATCH_MAKING_RESULT &>(Data);

	i3::rc_wstring wstrLabel;				// 전체 출력할 buf. 
	i3::rc_wstring wstrMyClanName;			// 내 클랜명
	i3::rc_wstring wstTargetClanName;		// 상대 클랜명
	i3::rc_wstring wstrResult;				// 승리/패배/무승부 출력할 buf.

	i3::sprintf(wstrMyClanName, getMyClanDetailInfo()->m_name); // getMyClanDetailInfo에만 정보가 담겨있음.
	i3::sprintf(wstTargetClanName, data.m_strClanName);
	
	if (data.m_Result == ROOM_ENDTYPE_WIN)
		i3::sprintf(wstrResult, GAME_RCSTRING("STBL_IDX_CLAN_CHATMESSAGE_MATCH_WIN"));
	else if (data.m_Result == ROOM_ENDTYPE_LOSE)
		i3::sprintf(wstrResult, GAME_RCSTRING("STBL_IDX_CLAN_CHATMESSAGE_MATCH_LOSE"));
	else if (data.m_Result == ROOM_ENDTYPE_DRAW)
		i3::sprintf(wstrResult, GAME_RCSTRING("STBL_IDX_CLAN_CHATMESSAGE_MATCH_DRAW"));
	else
		return; // 값이 제대로 안옴. 아예 띄우질 말자

	/* "wstrMyClanName"클랜이 "wstTargetClanName"클랜에게 
	data.m_MyClanScore:data.m_OtherClanScore으로 wstrResult하였습니다.*/
	i3::sprintf(wstrLabel, GAME_RCSTRING("STBL_IDX_CLAN_CHATMESSAGE_MATCH"), wstrMyClanName, wstTargetClanName,
		data.m_MyClanScore, data.m_OtherClanScore, wstrResult);

	// 결과메세지 색상 적용 추가 
	::ColorString(wstrLabel, I3COLOR(80, 255, 60, 255));
 
	// 패킷 받은 모든 클랜원에게 출력
	g_pFramework->getChatBox()->PutSystemChatting(wstrLabel);

}

const CLAN_MERCENARY_RECORD * ClanGameContext::getClanMercenaryRecord() const
{
	return &m_ClanMercenaryRecord; 
}

INT32 ClanGameContext::GetMercIconIdx(const CLAN_MERCENARY_INFO *pInfo)
{
	if(pInfo == nullptr) return 0;
	INT32 classType = 0;

	switch((INT32)pInfo->getClassType())
	{
		case WEAPON_CLASS_ASSAULT:
			classType = 0; 
			break;
		case WEAPON_CLASS_MG:
			classType = 1; 
			break;
		case WEAPON_CLASS_SNIPER:
			classType = 2;
			break;
		case WEAPON_CLASS_SMG:
			classType = 3;
			break;
		case WEAPON_CLASS_SHOTGUN:
			classType = 4;
			break;
	}
	return classType * 10 + pInfo->getRank() + MERCENARY_RANK_STARTIDX;
}

bool ClanGameContext::IsMaxMerceMyTeam()
{
	int MercenaryCount = 0; 
	for(int i=0 ; i< MAX_CLAN_MATCH_MEMBER; ++i)
	{
		if(m_MyMatchSquadInfo._Member[i]._MemberType == MEMBER_MERCENARY)
			MercenaryCount++;

		if(MercenaryCount >= 2)
			return true;
	}

	return false;
}

void ClanGameContext::SetMercRank(int slotidx, UINT8 classType, UINT8 rank)
{
	if(classType == INIT_UINT8 || rank == INIT_UINT8) 
	{
		m_MercRank[slotidx] = -1; // 용병아님
		return;
	}

	if(slotidx < 0 || slotidx >= SLOT_MAX_COUNT) return; // 잘못된 슬롯인덱스

	switch((INT32)classType)
	{
		case WEAPON_CLASS_ASSAULT:
			classType = 0; 
			break;
		case WEAPON_CLASS_MG:
			classType = 1; 
			break;
		case WEAPON_CLASS_SNIPER:
			classType = 2;
			break;
		case WEAPON_CLASS_SMG:
			classType = 3;
			break;
		case WEAPON_CLASS_SHOTGUN:
			classType = 4;
			break;
	}
	
	m_MercRank[slotidx] = classType * 10 + rank + MERCENARY_RANK_STARTIDX;
}

bool	ClanGameContext::ChangeMercRank(UINT8 iOld, UINT8 iNew)
{
	if (iOld >= SLOT_MAX_COUNT) return false; // 잘못된 슬롯인덱스
	if (iNew >= SLOT_MAX_COUNT) return false; // 잘못된 슬롯인덱스

	INT32 itemp = m_MercRank[iOld];

	m_MercRank[iOld] = m_MercRank[iNew];

	m_MercRank[iNew] = itemp;

	return true;
}

INT32	ClanGameContext::GetMercRank(INT32 slotidx) 
{
	if(slotidx < 0 || slotidx >= SLOT_MAX_COUNT) return -1; // 잘못된 슬롯인덱스

	return m_MercRank[slotidx];
}

void	ClanGameContext::ResetMercRank()
{
	for(int i = 0; i < SLOT_MAX_COUNT ; ++i)
		m_MercRank[i] = -1; 
}