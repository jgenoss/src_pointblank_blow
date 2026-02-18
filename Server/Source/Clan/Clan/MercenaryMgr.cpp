#include "pch.h"
#include "MercenaryMgr.h"

#include "DataBase.h"
#include "Gateway.h"
#include "MatchTeam.h"
#include "TeamMember.h"


const INT32 CMercenary::m_ai32MatchByRank[MER_RANK_COUNT] = { MER_RANK_0, MER_RANK_1, MER_RANK_2, MER_RANK_3, MER_RANK_4, MER_RANK_5, MER_RANK_6, MER_RANK_7, MER_RANK_8 };

CMercenary::CMercenary()
: m_TInviteTeamID(MATCH_TEAM_INVALID_IDX)
{
	m_stBaseInfo.m_ui8State = MER_STATE_NONE;
}

CMercenary::~CMercenary()
{
}

T_RESULT CMercenary::Invite(UINT32 ui32ServerIdx, CMatchTeam* pMatchTeam)
{
	TTCHAR* pClanName = pMatchTeam->GetClanName();
	if( !pClanName )
		return EVENT_ERROR_CLAN_WAR_INVITE_TEAM;

	T_TeamID	TTeamID		= pMatchTeam->GetTeamID();
	m_TInviteTeamID			= TTeamID;

	T_UID		TLeaderUID	= pMatchTeam->GetLeaderUID();	// 초대한 리더 UID
	T_RESULT	TResult		= EVENT_ERROR_SUCCESS;

	i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_INVITE_MERCENARY_ACK);
	stPacket.WriteData(&TLeaderUID,						sizeof(T_UID));			// 초대한 팀 리더 UID
	stPacket.WriteData(&TResult,						sizeof(T_RESULT)); 
	stPacket.WriteData(&m_TUID,							sizeof(T_UID));			// 용병 UID
	stPacket.WriteData(&m_stBaseInfo.m_TMercenaryID,	sizeof(T_MerID));		// 용병ID
	stPacket.WriteData(&TTeamID,						sizeof(T_TeamID));		// 팀 ID
	stPacket.WriteData(pClanName,						sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);

	g_pGateway->SendPacket(ui32ServerIdx, &stPacket);

	

	return EVENT_ERROR_SUCCESS;
}

void CMercenary::Resist(T_MerID TMerID, REGIST_MERCENARY* pMercenaryInfo, CLAN_USER_INFO *pUserInfo)
{
	m_TUID			=  pUserInfo->i64UID;
	m_i32SessionIdx =  pUserInfo->i32SessionIdx;

	m_stBaseInfo.m_TMercenaryID = TMerID;
	m_stBaseInfo.m_ui8Class		= pMercenaryInfo->m_ui8Class;
	m_stBaseInfo.m_ui8State		= MER_STATE_REGISERING;
	m_stBaseInfo.m_ui8NickColor = pMercenaryInfo->m_ui8NickColor;
	i3String::Copy(m_stBaseInfo.m_strNick, pMercenaryInfo->m_strNick, NET_NICK_NAME_SIZE);

	m_TInviteTeamID = MATCH_TEAM_INVALID_IDX;
	m_bSendMerRankUp	= false;
}

void CMercenary::Remove()
{ 
	m_stBaseInfo.m_ui8State		= MER_STATE_NONE;
	m_stBaseInfo.m_TMercenaryID = MERCENARY_INVALID_IDX;
	m_TUID						= INVALID_IDX;
	m_i32SessionIdx				= INVALID_IDX;
}	

UINT8 CMercenary::CheckMerRank()
{
	for( INT32 i = 0; i < MER_RANK_COUNT; ++i )
	{
		if( m_stRecord.m_i32MatchCount < m_ai32MatchByRank[i] )
		{
			return (UINT8)i;
		}
	}

	return MER_RANK_COUNT;
}

bool CMercenary::SetMercenaryRecord(MERCENARY_RECORD* pMercenaryInfo)
{
	m_stBaseInfo.m_ui8State	= MER_STATE_REGISTERED;

	i3mem::Copy( &m_stRecord, pMercenaryInfo, sizeof( MERCENARY_RECORD ));

	//SetMercenaryRank();
 
	return true;
}

void CMercenary::MakePacketMercenary(i3NetworkPacket* pPacket)
{
	pPacket->WriteData(&m_stBaseInfo,	sizeof(MERCENARY_INFO));
	//pPacket->WriteData(&m_stRecord,		sizeof(MERCENARY_RECORD));
}

void CMercenary::GetDetailInfo(MERCENARY_DETAIL_INFO& DetailInfo)
{
	i3mem::Copy(&DetailInfo.m_Recod,	&m_stRecord,	sizeof(MERCENARY_RECORD));
}

void CMercenary::AddRecord(CLAN_RESULT_USER* pResult)
{
	++m_stRecord.m_i32MatchCount;

	switch( pResult->_WinLose )
	{
	case ROOM_ENDTYPE_WIN			: ++m_stRecord.m_i32WinMatchCnt;	break;
	case ROOM_ENDTYPE_LOSE			: ++m_stRecord.m_i32LoseMatchCnt;	break;
	case ROOM_ENDTYPE_DISCONNECT	: ++m_stRecord.m_i32DisconMatchCnt;	break;
	default : break;
	}

	m_stRecord.m_i32KillCount	+= pResult->_Kill;
	m_stRecord.m_i32DeathCount	+= pResult->_Death;
	m_stRecord.m_i32Headshot	+= pResult->_Head;
	m_stRecord.m_i32AssistCount += pResult->_Assist;

	UINT8 ui8CheckRank = CheckMerRank();
	if( m_stBaseInfo.m_ui8MerRank < ui8CheckRank ) 
	{	
		m_stBaseInfo.m_ui8MerRank = ui8CheckRank;
		m_bSendMerRankUp	= true;
	}
}


void	CMercenary::ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO* pMemberInfo)
{ 
	switch(eType)
	{
	case CHANGE_INFO_NICKNAME:
		i3String::Copy( m_stBaseInfo.m_strNick, pMemberInfo->m_strNick, NET_NICK_NAME_SIZE); 
		break;
	case CHANGE_INFO_NICKCOLOR:
		m_stBaseInfo.m_ui8NickColor = pMemberInfo->m_ui8NickColor;
		break;

	// 용병은 일반 랭크를 반영하지 않습니다.
	//case CHANGE_INFO_RANK:
	//	m_stBaseInfo.m_ui8MerRank = pMemberInfo->m_ui8Rank; 
	//	break;
	default:
		return;
	}
}


 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// CMercenaryManager

CMercenaryManager::CMercenaryManager()
{
}


CMercenaryManager::~CMercenaryManager()
{
}

T_MerID CMercenaryManager::GetEmptyBuffer()
{
	for( UINT16 i = 0; i < MAX_MERCENARY_COUNT; ++i )
	{
		if( m_Mercenary[i].GetState() == MER_STATE_NONE )
			return i;
	}

	return MERCENARY_INVALID_IDX;
}

INT32 CMercenaryManager::GetMercenaryCount(INT32 i32MultiState)
{
	INT32 i32MercenaryCnt = 0;
	for( INT32 i = 0; i < MAX_MERCENARY_COUNT; ++i )
	{
		if( ! ( m_Mercenary[i].GetState() | i32MultiState) ) continue;

		++i32MercenaryCnt;
	}

	return i32MercenaryCnt;
}

T_RESULT CMercenaryManager::RegistMercenary(REGIST_MERCENARY* pRegistInfo, CLAN_USER_INFO* pUserInfo )
{
	T_MerID TMerID = GetEmptyBuffer();
	if( TMerID == MERCENARY_INVALID_IDX ) return EVENT_ERROR_EVENT_BUFFER_FULL;

	m_Mercenary[TMerID].Resist(TMerID, pRegistInfo, pUserInfo);

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_LOAD_MERCENARY;
	stRingInDB._i64UID			= pRegistInfo->m_TUID;
	stRingInDB._i32SessionIdx	= pUserInfo->i32SessionIdx;
	stRingInDB._ui32ServerIdx	= pUserInfo->ui32ServerIdx;

	stRingInDB.Write( &TMerID,	sizeof( T_MerID) );

	if( g_pDataBase->InsertWorkingUID(stRingInDB._i64UID, &stRingInDB) != RINGBUFFER_SUCCESS )
	{
		wchar_t wstrTemp[ MAX_PATH ];			// 추후 사용 
		wstrTemp[0] = '\0';
		g_pLog->WriteLog( wstrTemp );
	}

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CMercenaryManager::InviteMercenary(UINT32 ui32ServerIdx, T_MerID TMerID, CMatchTeam* pMatchTeam)
{
	if( !CheckValidMercenary(TMerID) )
		return EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY;

	CMercenary* pMercenary = &m_Mercenary[TMerID];

	if( pMercenary->GetState() != MER_STATE_REGISTERED )
		return EVENT_ERROR_CLAN_WAR_WRONG_STATE;

	if( pMercenary->GetInviteTeamID() != MATCH_TEAM_INVALID_IDX ) 
		return EVENT_ERROR_CLAN_WAR_INVITE_TEAM;

	return pMercenary->Invite(ui32ServerIdx, pMatchTeam);
}

BOOL CMercenaryManager::RemoveMercenary(T_MerID TMerID)
{
	CMercenary* pMercenary = FindMercenary(TMerID);
	if( !pMercenary ) return FALSE;

	pMercenary->Remove();

	return TRUE;
}

CMercenary* CMercenaryManager::SetMercenaryRecord(T_MerID TMerID, MERCENARY_RECORD* pMercenaryInfo)
{
	if( !CheckValidMercenary(TMerID) )
		return NULL;

	if( !m_Mercenary[TMerID].SetMercenaryRecord(pMercenaryInfo) )
		return NULL;

	return &m_Mercenary[TMerID];
}

CMercenary* CMercenaryManager::FindMercenary(INT64 i64UserID)
{
	for( INT32 i = 0; i < MAX_MERCENARY_COUNT; ++i )
	{
		if( m_Mercenary[i].GetState() < MER_STATE_REGISTERED )
			continue;

		if( m_Mercenary[i].GetUserID() == i64UserID )
			return &m_Mercenary[i];
	}

	return NULL;
}

CMercenary* CMercenaryManager::FindMercenary(T_MerID TMerID)
{
	if( !CheckValidMercenary( TMerID ) ) return NULL;

	if( m_Mercenary[TMerID].GetState() == MER_STATE_NONE ) return NULL;

	return &m_Mercenary[TMerID];
}

void CMercenaryManager::ChangeMemberInfo(CHANGE_INFO	eType, CLAN_WAR_INFO *pUserInfo, MATCH_MEMBER_INFO* pMemberInfo)
{
	if( !CheckValidMercenary(pUserInfo->m_TMerID) ) return;

	CMercenary* pMercenary = &m_Mercenary[pUserInfo->m_TMerID];

	if( MER_STATE_NONE == pMercenary->GetState()  ) return;

	pMercenary->ChangeMemberInfo(eType, pMemberInfo);

	// 용병리스트를 통해 전달되므로 따로 패킷 전달은 필요없다.
}



void CMercenaryManager::SendMercenaryList(CLAN_USER_INFO* pUserInfo)
{
	MERCENARY_INFO	aMerInfo[MAX_MERCENARY_COUNT];
	UINT16 ui16TotalMerCount = 0;

	for( INT32 i = 0; i < MAX_MERCENARY_COUNT; ++i )
	{	// 등록이 완료된 용병만 추출한다.

		if( m_Mercenary[i].GetState() != MER_STATE_REGISTERED ) continue;

		i3mem::Copy( &aMerInfo[ui16TotalMerCount], m_Mercenary[i].GetBaseInfo(), sizeof( MERCENARY_INFO ) );

		++ui16TotalMerCount;
	}

	//if (0 == ui16TotalMerCount) return;

	// MTU 사이즈에 맞게 분할해서 패킷을 보냅니다.
	UINT16 ui16RemainCount	= ui16TotalMerCount;
	UINT16 ui16CurCount		= 0;
	UINT16 ui16StartIdx		= 0;
	UINT16 ui16PacketSize	= PACKET_BUFFERSIZE_MTU - PACKETHEADERSIZE - sizeof( T_UID ) - sizeof( INT32 ) - sizeof( UINT16 ) - sizeof( UINT16 ) - sizeof( UINT16 );
	
	bool	bSendContinue	= true;	
	UINT16	ui16MaxCount	= (ui16PacketSize / sizeof( MERCENARY_INFO ));
	ui16MaxCount = MIN ( MAX_MERCENARY_LIST_PER_PACKET, ui16MaxCount );

	while( bSendContinue )
	{	
		ui16StartIdx			+= ui16CurCount;
		if( ui16RemainCount > ui16MaxCount )	{	ui16CurCount	= ui16MaxCount;								}
		else									{	ui16CurCount	= ui16RemainCount; bSendContinue = false;	}
		ui16RemainCount -= ui16CurCount;

		i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK);
		SendPacket.WriteData(&pUserInfo->i32SessionIdx,	sizeof(INT32));
		SendPacket.WriteData(&pUserInfo->i64UID,		sizeof(T_UID)); 
		SendPacket.WriteData(&ui16TotalMerCount,		sizeof(UINT16));
		SendPacket.WriteData(&ui16CurCount,				sizeof(UINT16));
		SendPacket.WriteData(&ui16StartIdx,				sizeof(UINT16));
		SendPacket.WriteData(&aMerInfo[ui16StartIdx],	(UINT16)(sizeof( MERCENARY_INFO )*ui16CurCount) );

		g_pGateway->SendPacket(pUserInfo->ui32ServerIdx, &SendPacket);			
	}
}

BOOL CMercenaryManager::GetDetailInfo(T_MerID TMerID, MERCENARY_DETAIL_INFO& DetailInfo)
{
	CMercenary* pMercenary = FindMercenary(TMerID);
	if( !pMercenary ) return FALSE;

	pMercenary->GetDetailInfo(DetailInfo);

	return TRUE;
}


void CMercenaryManager::ReleaseAllMercenary()
{
	for (INT32 i = 0; i < MAX_MERCENARY_COUNT; ++i)
	{	
		CMercenary* pMercenary= &m_Mercenary[i];

		if (pMercenary->GetState() <= MER_STATE_NONE) continue;

		pMercenary->Remove();
	}
}