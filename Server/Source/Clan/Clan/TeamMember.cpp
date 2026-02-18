#include "pch.h"
#include "TeamMember.h"

#include "Database.h"
#include "TaskProcessor.h"

#include "../../CommonServerSource/TrueSkill/TrueSkill.h"

//////////////////////////////////////////////////////////////////////////
// CTeamMember
CTeamMember::CTeamMember()
{
}

CTeamMember::~CTeamMember()
{
}


//////////////////////////////////////////////////////////////////////////
// CMemberClan
CClanMember::CClanMember(CLAN_MEMBER_DETAIL_INFO* pClanMember)
: m_pMember(pClanMember)
{
}

CClanMember::~CClanMember()
{
}

void CClanMember::JoinTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, T_TeamID TTeamID )
{
	if( !m_pMember )
	{
		g_pLog->WriteLog( L"[ERROR][CClanMember::JoinTeam] Not exist member.");
		return;
	}

	m_pMember->_state = g_pConfig->MakeStateMatch( m_pMember->_state, TTeamID );

	g_pTaskProcessor->SendClanMemberStateChange(pOwnerClan, m_pMember);
}

void CClanMember::LeaveTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan )
{
	if( !m_pMember )
	{
		g_pLog->WriteLog( L"[ERROR][CClanMember::LeaveTeam] Not exist member.");
		return;
	}

	// CLEAR_FRIEND_MATCH 호출하면 안됨.
	m_pMember->_state = g_pConfig->MakeStateMatch( m_pMember->_state, MATCH_TEAM_INVALID_IDX );

	g_pTaskProcessor->SendClanMemberStateChange(pOwnerClan, m_pMember);
}

void CClanMember::MakePacketMemeber(i3NetworkPacket* pPacket )
{
	MATCH_MEMBER_INFO MemberInfo;
	i3mem::FillZero(&MemberInfo, sizeof(MATCH_MEMBER_INFO));

	if( !m_pMember )
	{
		pPacket->WriteData(&MemberInfo, sizeof(MATCH_MEMBER_INFO));

		g_pLog->WriteLog( L"[ERROR][CClanMember::MakePacketMemeber] Not exist member.");
		return;
	}

	

	MemberInfo.m_TUID = m_pMember->_i64UID;
	MemberInfo.m_ui8MemberType	= (UINT8)MEMBER_CLAN;
	MemberInfo.m_i8Slot			= (INT8)GetSlot(); 
	MemberInfo.m_ui8NickColor	= m_pMember->_color;
	MemberInfo.m_ui8Rank		= m_pMember->_rank;
	MemberInfo.m_ui8MerClass	= WEAPON_CLASS_UNKNOWN;
	i3String::Copy( MemberInfo.m_strNick, m_pMember->_nick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );

	pPacket->WriteData(&MemberInfo,		sizeof(MATCH_MEMBER_INFO));
}


void CClanMember::MakePacketMemberUID(i3NetworkPacket* pPacket )
{
	if( !m_pMember )
	{
		T_UID	TUID = -1;
		INT32	i32SessionIdx = -1;
		pPacket->WriteData(&TUID, sizeof(T_UID));
		pPacket->WriteData(&i32SessionIdx, sizeof(INT32));

		g_pLog->WriteLog( L"[ERROR][CClanMember::MakePacketMemberUID] Not exist member.");
		return;
	}

	pPacket->WriteData(&m_pMember->_i64UID, sizeof(T_UID));
	pPacket->WriteData(&m_pMember->_session, sizeof(INT32));
}

void CClanMember::MakePacketMatchMakingRecord(i3NetworkPacket* pPacket)
{
	if (!m_pMember)
	{
		T_UID	TUID = -1;
		INT32	i32SessionIdx = -1;
		UINT32	ui32Count = 0;					// 의미없음. 패킷 사이즈 맞추기 용.

		pPacket->WriteData(&TUID,			sizeof(T_UID));
		pPacket->WriteData(&i32SessionIdx,	sizeof(INT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		g_pLog->WriteLog(L"[ERROR][CClanMember::MakePacketMatchMakingRecord] Not exist member.");
		return;
	}

	pPacket->WriteData(&m_pMember->_i64UID, sizeof(T_UID));
	pPacket->WriteData(&m_pMember->_session,sizeof(INT32));
	pPacket->WriteData(&m_pMember->m_kill,	sizeof(UINT32));
	pPacket->WriteData(&m_pMember->m_death, sizeof(UINT32));
	pPacket->WriteData(&m_pMember->m_win,	sizeof(UINT32));
	pPacket->WriteData(&m_pMember->m_lose,	sizeof(UINT32));
	pPacket->WriteData(&m_pMember->m_Assist, sizeof(UINT32));
}

BOOL CClanMember::ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO *pMemberInfo) 
{
	if( !m_pMember )
	{
		g_pLog->WriteLog( L"[ERROR][CClanMember::ChangeMemberInfo] Not exist member.");
		return FALSE;
	}

	// 기존에 클랜원 정보 갱신에서 하므로 딱히 할필요는 없습니다.
	switch( eType )
	{
	case CHANGE_INFO_NICKNAME:
		i3String::Copy( m_pMember->_nick, pMemberInfo->m_strNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
		break;
	case CHANGE_INFO_NICKCOLOR:
		m_pMember->_color	= pMemberInfo->m_ui8NickColor;
		break;
	case CHANGE_INFO_RANK:
		m_pMember->_rank	= pMemberInfo->m_ui8Rank;
		break;
	default:
		return FALSE;
	}

	return TRUE;
} 

//////////////////////////////////////////////////////////////////////////
// CMemberClan
CMercenaryMember::CMercenaryMember(CMercenary* pMercenary)
: m_pMercenary(pMercenary)
{
	Initiailze();
}

CMercenaryMember::~CMercenaryMember()
{
}

void CMercenaryMember::Initiailze()
{
	if( !m_pMercenary )
	{
		return;
	}
}

void CMercenaryMember::MakePacketMemeber(i3NetworkPacket* pPacket )
{
	MATCH_MEMBER_INFO MemberInfo;
	i3mem::FillZero(&MemberInfo, sizeof(MATCH_MEMBER_INFO));

	if( !m_pMercenary )
	{
		pPacket->WriteData(&MemberInfo, sizeof(MATCH_MEMBER_INFO));
		g_pLog->WriteLog( L"[ERROR][CMercenaryMember::MakePacketMemeber] Not exist member.");
		return;
	}

	MemberInfo.m_TUID			= m_pMercenary->GetUserID();
	MemberInfo.m_ui8MemberType	= (UINT8)MEMBER_MERCENARY;
	MemberInfo.m_i8Slot			= (INT8)GetSlot();
	MemberInfo.m_ui8NickColor	= m_pMercenary->GetNickColor();
	MemberInfo.m_ui8Rank		= GetMerRank();
	MemberInfo.m_ui8MerClass	= GetMerClass();
	i3String::Copy( MemberInfo.m_strNick, m_pMercenary->GetNickname(), sizeof(TTCHAR) * NET_NICK_NAME_SIZE ); 

	pPacket->WriteData(&MemberInfo,		sizeof(MATCH_MEMBER_INFO));
}


void CMercenaryMember::MakePacketMemberUID(i3NetworkPacket* pPacket )
{
	T_UID TUID = -1;
	INT32 i32SessionIdx = -1;

	if( !m_pMercenary )
	{
		pPacket->WriteData(&TUID, sizeof(T_UID));
		pPacket->WriteData(&i32SessionIdx, sizeof(INT32));

		g_pLog->WriteLog( L"[ERROR][CMercenaryMember::MakePacketMemeber] Not exist member.");
		return;
	}

	TUID = m_pMercenary->GetUserID();
	i32SessionIdx = m_pMercenary->GetSessionIdx();
	pPacket->WriteData(&TUID,				sizeof(T_UID));  
	pPacket->WriteData(&i32SessionIdx,		sizeof(INT32)); 
}

void CMercenaryMember::MakePacketMatchMakingRecord(i3NetworkPacket* pPacket )
{
	T_UID TUID = -1;
	INT32 i32SessionIdx = -1;
	
	if (!m_pMercenary)
	{
		UINT32	ui32Count = 0;					// 의미없음. 패킷 사이즈 맞추기 용.

		pPacket->WriteData(&TUID,			sizeof(T_UID));
		pPacket->WriteData(&i32SessionIdx,	sizeof(INT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));
		pPacket->WriteData(&ui32Count,		sizeof(UINT32));

		g_pLog->WriteLog(L"[ERROR][CMercenaryMember::MakePacketMatchMakingRecord] Not exist member.");
		return;
	}

	TUID = m_pMercenary->GetUserID(); 
	i32SessionIdx = m_pMercenary->GetSessionIdx();

	MERCENARY_RECORD	MerRecord;
	m_pMercenary->GetRecordInfo(MerRecord);

	pPacket->WriteData(&TUID,						sizeof(T_UID));
	pPacket->WriteData(&i32SessionIdx,				sizeof(INT32));
	pPacket->WriteData(&MerRecord.m_i32KillCount,	sizeof(UINT32));
	pPacket->WriteData(&MerRecord.m_i32DeathCount,	sizeof(UINT32));
	pPacket->WriteData(&MerRecord.m_i32WinMatchCnt, sizeof(UINT32));
	pPacket->WriteData(&MerRecord.m_i32LoseMatchCnt,sizeof(UINT32));
	pPacket->WriteData(&MerRecord.m_i32AssistCount, sizeof(UINT32));
}

void CMercenaryMember::JoinTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, T_TeamID TTeamID )
{
	if( !m_pMercenary )
	{
		g_pLog->WriteLog( L"[ERROR][CMercenaryMember::JoinTeam] Not exist member.");
		return;
	}

	m_pMercenary->SetInviteTeamID(TTeamID);
	m_pMercenary->SetState(MER_STATE_JOIN_TEAM);
}

void CMercenaryMember::LeaveTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan )
{
	if( !m_pMercenary )
	{
		g_pLog->WriteLog( L"[ERROR][CMercenaryMember::LeaveTeam] Not exist member.");
		return;
	}
	m_pMercenary->SetInviteTeamID(MATCH_TEAM_INVALID_IDX);
	m_pMercenary->SetState(MER_STATE_REGISTERED);
}

BOOL CMercenaryMember::ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO *pMemberInfo ) 
{
	if( !m_pMercenary )
	{
		g_pLog->WriteLog( L"[ERROR][CMercenaryMember::ChangeMemberInfo] Not exist member.");
		return FALSE;
	}

	m_pMercenary->ChangeMemberInfo(eType, pMemberInfo);

	return TRUE;
}