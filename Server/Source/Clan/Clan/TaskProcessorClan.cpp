#include "pch.h"

#include <math.h>

#include "TrackBuffer.h"
#include "DataBase.h"
#include "Gateway.h"
#include "TaskProcessor.h"
#include "TeamMember.h"
#include "MatchingManager.h"
#include "../../CommonServerSource/TrueSkill/TrueSkill.h"


// ------------------------------------------------------------------------- //
// NAME: GetClanPersonRank()
// DESC: 버전
// ------------------------------------------------------------------------- //
UINT8	CTaskProcessor::GetClanPersonRank( UINT8 ui8Person )
{
	if		(DF_CLAN_UNIT_CORPS		<= ui8Person)	return CLAN_UNIT_CORPS;
	else if (DF_CLAN_UNIT_DIVISION	<= ui8Person)	return CLAN_UNIT_DIVISION;
	else if (DF_CLAN_UNIT_BRIGADE	<= ui8Person)	return CLAN_UNIT_BRIGADE;
	else if (DF_CLAN_UNIT_REGIMENT	<= ui8Person)	return CLAN_UNIT_REGIMENT;
	else if (DF_CLAN_UNIT_BATTALION <= ui8Person)	return CLAN_UNIT_BATTALION;
	else if (DF_CLAN_UNIT_COMPANY	<= ui8Person)	return CLAN_UNIT_COMPANY;
	else if (DF_CLAN_UNIT_PLATOON	<= ui8Person)	return CLAN_UNIT_PLATOON;

	//DF_CLAN_UNIT_SQUARD
	return CLAN_UNIT_SQUARD;
}

// ------------------------------------------------------------------------- //
// NAME: GetClanExpRank()
// DESC: 버전
// ------------------------------------------------------------------------- //
UINT8	CTaskProcessor::GetClanExpRank( UINT32 ui32Exp )
{
	if		(DF_CLAN_RANK_L5 <= ui32Exp)	return CLAN_RANK_L5;
	else if (DF_CLAN_RANK_L4 <= ui32Exp)	return CLAN_RANK_L4;
	else if (DF_CLAN_RANK_L3 <= ui32Exp)	return CLAN_RANK_L3;
	else if (DF_CLAN_RANK_L2 <= ui32Exp)	return CLAN_RANK_L2;
	else if (DF_CLAN_RANK_L1 <= ui32Exp)	return CLAN_RANK_L1;
	else if (DF_CLAN_RANK_K5 <= ui32Exp)	return CLAN_RANK_K5;
	else if (DF_CLAN_RANK_K4 <= ui32Exp)	return CLAN_RANK_K4;
	else if (DF_CLAN_RANK_K3 <= ui32Exp)	return CLAN_RANK_K3;
	else if (DF_CLAN_RANK_K2 <= ui32Exp)	return CLAN_RANK_K2;
	else if (DF_CLAN_RANK_K1 <= ui32Exp)	return CLAN_RANK_K1;
	else if (DF_CLAN_RANK_J5 <= ui32Exp)	return CLAN_RANK_J5;
	else if (DF_CLAN_RANK_J4 <= ui32Exp)	return CLAN_RANK_J4;
	else if (DF_CLAN_RANK_J3 <= ui32Exp)	return CLAN_RANK_J3;
	else if (DF_CLAN_RANK_J2 <= ui32Exp)	return CLAN_RANK_J2;
	else if (DF_CLAN_RANK_J1 <= ui32Exp)	return CLAN_RANK_J1;
	else if (DF_CLAN_RANK_I5 <= ui32Exp)	return CLAN_RANK_I5;
	else if (DF_CLAN_RANK_I4 <= ui32Exp)	return CLAN_RANK_I4;
	else if (DF_CLAN_RANK_I3 <= ui32Exp)	return CLAN_RANK_I3;
	else if (DF_CLAN_RANK_I2 <= ui32Exp)	return CLAN_RANK_I2;
	else if (DF_CLAN_RANK_I1 <= ui32Exp)	return CLAN_RANK_I1;
	else if (DF_CLAN_RANK_H5 <= ui32Exp)	return CLAN_RANK_H5;
	else if (DF_CLAN_RANK_H4 <= ui32Exp)	return CLAN_RANK_H4;
	else if (DF_CLAN_RANK_H3 <= ui32Exp)	return CLAN_RANK_H3;
	else if (DF_CLAN_RANK_H2 <= ui32Exp)	return CLAN_RANK_H2;
	else if (DF_CLAN_RANK_H1 <= ui32Exp)	return CLAN_RANK_H1;
	else if (DF_CLAN_RANK_G5 <= ui32Exp)	return CLAN_RANK_G5;
	else if (DF_CLAN_RANK_G4 <= ui32Exp)	return CLAN_RANK_G4;
	else if (DF_CLAN_RANK_G3 <= ui32Exp)	return CLAN_RANK_G3;
	else if (DF_CLAN_RANK_G2 <= ui32Exp)	return CLAN_RANK_G2;
	else if (DF_CLAN_RANK_G1 <= ui32Exp)	return CLAN_RANK_G1;
	else if (DF_CLAN_RANK_F5 <= ui32Exp)	return CLAN_RANK_F5;
	else if (DF_CLAN_RANK_F4 <= ui32Exp)	return CLAN_RANK_F4;
	else if (DF_CLAN_RANK_F3 <= ui32Exp)	return CLAN_RANK_F3;
	else if (DF_CLAN_RANK_F2 <= ui32Exp)	return CLAN_RANK_F2;
	else if (DF_CLAN_RANK_F1 <= ui32Exp)	return CLAN_RANK_F1;
	else if (DF_CLAN_RANK_E4 <= ui32Exp)	return CLAN_RANK_E4;
	else if (DF_CLAN_RANK_E3 <= ui32Exp)	return CLAN_RANK_E3;
	else if (DF_CLAN_RANK_E2 <= ui32Exp)	return CLAN_RANK_E2;
	else if (DF_CLAN_RANK_E1 <= ui32Exp)	return CLAN_RANK_E1;
	else if (DF_CLAN_RANK_D4 <= ui32Exp)	return CLAN_RANK_D4;
	else if (DF_CLAN_RANK_D3 <= ui32Exp)	return CLAN_RANK_D3;
	else if (DF_CLAN_RANK_D2 <= ui32Exp)	return CLAN_RANK_D2;
	else if (DF_CLAN_RANK_D1 <= ui32Exp)	return CLAN_RANK_D1;
	else if (DF_CLAN_RANK_C3 <= ui32Exp)	return CLAN_RANK_C3;
	else if (DF_CLAN_RANK_C2 <= ui32Exp)	return CLAN_RANK_C2;
	else if (DF_CLAN_RANK_C1 <= ui32Exp)	return CLAN_RANK_C1;
	else if (DF_CLAN_RANK_B2 <= ui32Exp)	return CLAN_RANK_B2;
	else if (DF_CLAN_RANK_B1 <= ui32Exp)	return CLAN_RANK_B1;
	else if (DF_CLAN_RANK_A2 <= ui32Exp)	return CLAN_RANK_A2;
	else if (DF_CLAN_RANK_A1 <= ui32Exp)	return CLAN_RANK_A1;

	//DF_CLAN_RANK_NEW
	return CLAN_RANK_NEW;
}

// ------------------------------------------------------------------------- //
// NAME: GetClanExp()
// DESC: 버전
// ------------------------------------------------------------------------- //
UINT32	CTaskProcessor::GetClanExp( UINT8 ui8Rank )
{
	switch( ui8Rank )
	{
		case CLAN_RANK_L5:	return DF_CLAN_RANK_L5;
		case CLAN_RANK_L4:	return DF_CLAN_RANK_L4;
		case CLAN_RANK_L3:	return DF_CLAN_RANK_L3;
		case CLAN_RANK_L2:	return DF_CLAN_RANK_L2;
		case CLAN_RANK_L1:	return DF_CLAN_RANK_L1;
		case CLAN_RANK_K5:	return DF_CLAN_RANK_K5;
		case CLAN_RANK_K4:	return DF_CLAN_RANK_K4;
		case CLAN_RANK_K3:	return DF_CLAN_RANK_K3;
		case CLAN_RANK_K2:	return DF_CLAN_RANK_K2;
		case CLAN_RANK_K1:	return DF_CLAN_RANK_K1;
		case CLAN_RANK_J5:	return DF_CLAN_RANK_J5;
		case CLAN_RANK_J4:	return DF_CLAN_RANK_J4;
		case CLAN_RANK_J3:	return DF_CLAN_RANK_J3;
		case CLAN_RANK_J2:	return DF_CLAN_RANK_J2;
		case CLAN_RANK_J1:	return DF_CLAN_RANK_J1;
		case CLAN_RANK_I5:	return DF_CLAN_RANK_I5;
		case CLAN_RANK_I4:	return DF_CLAN_RANK_I4;
		case CLAN_RANK_I3:	return DF_CLAN_RANK_I3;
		case CLAN_RANK_I2:	return DF_CLAN_RANK_I2;
		case CLAN_RANK_I1:	return DF_CLAN_RANK_I1;
		case CLAN_RANK_H5:	return DF_CLAN_RANK_H5;
		case CLAN_RANK_H4:	return DF_CLAN_RANK_H4;
		case CLAN_RANK_H3:	return DF_CLAN_RANK_H3;
		case CLAN_RANK_H2:	return DF_CLAN_RANK_H2;
		case CLAN_RANK_H1:	return DF_CLAN_RANK_H1;
		case CLAN_RANK_G5:	return DF_CLAN_RANK_G5;
		case CLAN_RANK_G4:	return DF_CLAN_RANK_G4;
		case CLAN_RANK_G3:	return DF_CLAN_RANK_G3;
		case CLAN_RANK_G2:	return DF_CLAN_RANK_G2;
		case CLAN_RANK_G1:	return DF_CLAN_RANK_G1;
		case CLAN_RANK_F5:	return DF_CLAN_RANK_F5;
		case CLAN_RANK_F4:	return DF_CLAN_RANK_F4;
		case CLAN_RANK_F3:	return DF_CLAN_RANK_F3;
		case CLAN_RANK_F2:	return DF_CLAN_RANK_F2;
		case CLAN_RANK_F1:	return DF_CLAN_RANK_F1;
		case CLAN_RANK_E4:	return DF_CLAN_RANK_E4;
		case CLAN_RANK_E3:	return DF_CLAN_RANK_E3;
		case CLAN_RANK_E2:	return DF_CLAN_RANK_E2;
		case CLAN_RANK_E1:	return DF_CLAN_RANK_E1;
		case CLAN_RANK_D4:	return DF_CLAN_RANK_D4;
		case CLAN_RANK_D3:	return DF_CLAN_RANK_D3;
		case CLAN_RANK_D2:	return DF_CLAN_RANK_D2;
		case CLAN_RANK_D1:	return DF_CLAN_RANK_D1;
		case CLAN_RANK_C3:	return DF_CLAN_RANK_C3;
		case CLAN_RANK_C2:	return DF_CLAN_RANK_C2;
		case CLAN_RANK_C1:	return DF_CLAN_RANK_C1;
		case CLAN_RANK_B2:	return DF_CLAN_RANK_B2;
		case CLAN_RANK_B1:	return DF_CLAN_RANK_B1;
		case CLAN_RANK_A2:	return DF_CLAN_RANK_A2;
		case CLAN_RANK_A1:	return DF_CLAN_RANK_A1;
		case CLAN_RANK_NEW:	return DF_CLAN_RANK_NEW;
	}
	return DF_CLAN_RANK_NEW;
}

// ------------------------------------------------------------------------- //
// NAME: SendPacket()
// DESC: 팻킷 전송
// ------------------------------------------------------------------------- //
void	CTaskProcessor::SendPacket( PROTOCOL ProtocolID, CLAN_USER_INFO* pstUserInfo, INT32 i32Success )
{
	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,			sizeof(T_UID));
	SendPacket.WriteData(&i32Success,					sizeof(INT32));

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::SendPacket( PROTOCOL ProtocolID, UINT32 ui32ServerIdx, INT32 i32SessionIdx, T_UID i64UID, INT32 i32Success )
{
	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&i64UID,			sizeof(T_UID));
	SendPacket.WriteData(&i32Success,		sizeof(INT32));

	g_pGateway->SendPacket( ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::SendPacketInvitedResult( PROTOCOL ProtocolID, UINT32 ui32ServerIdx, INT32 i32SessionIdx, T_UID i64UID, T_UID i64SUID )
{
	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&i64UID,			sizeof(T_UID));
	SendPacket.WriteData(&i64SUID,			sizeof(T_UID));

	g_pGateway->SendPacket( ui32ServerIdx, &SendPacket);
}

// ------------------------------------------------------------------------- //
// NAME: SetClanListContect()
// DESC: ClanContextList 갱신
// ------------------------------------------------------------------------- //
void	CTaskProcessor::SetClanListContect()
{
	m_ClanListContext._sectionCount = (UINT16)(m_ClanListContext._totalCount / CLAN_SECTION_SIZE);
	if( m_ClanListContext._totalCount % CLAN_SECTION_SIZE )
	{
		m_ClanListContext._sectionCount++;
	}

	m_ClanListContext._version = i3ThreadTimer::StandTime().GetDate();
}

// ------------------------------------------------------------------------- //
// NAME: SetMemberListContect()
// DESC: MemberListContect 갱신
// ------------------------------------------------------------------------- //
void	CTaskProcessor::SetMemberListContect( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	pstClan->_stMemberContext._sectionCount	= pstClan->_stMemberContext._totalCount / MEMBER_SECTION_SIZE;
	// 올림
	if( pstClan->_stMemberContext._totalCount % MEMBER_SECTION_SIZE )
	{
		pstClan->_stMemberContext._sectionCount++;
	}

	pstClan->_stMemberContext._version = i3ThreadTimer::StandTime().GetDate();
}

// ------------------------------------------------------------------------- //
// NAME: SetRequestListContect()
// DESC: RequestListContect 갱신
// ------------------------------------------------------------------------- //
void	CTaskProcessor::SetRequestListContect( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	pstClan->_stRequestContext._sectionCount = (UINT8)(pstClan->_stRequestContext._totalCount / REQUEST_SECTION_SIZE);
	// 올림
	if( pstClan->_stRequestContext._totalCount % REQUEST_SECTION_SIZE )
	{
		pstClan->_stRequestContext._sectionCount++;
	}

	pstClan->_stRequestContext._version = i3ThreadTimer::StandTime().GetDate();
}

// ------------------------------------------------------------------------- //
// NAME: DeleteRequest()
// DESC: Request 삭제
// ------------------------------------------------------------------------- //
void	CTaskProcessor::DeleteRequest( CLAN_DETAIL_INFO_SERVER* pstClan, T_UID i64UID )
{
	CLAN_REQUEST_SERVER_INFO* pstTempRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->DeleteEx( i64UID );
	if( NULL == pstTempRequest ) return;
	PushTempRequest( pstTempRequest );
	pstClan->_stRequestContext._totalCount--;
	SetRequestListContect( pstClan );
}

// ------------------------------------------------------------------------- //
// NAME: 클랜 상태 변화()
// DESC: SetClanChangeStateDate 삭제
// ------------------------------------------------------------------------- //
void	CTaskProcessor::SetClanChangeStateDate( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	UINT32 ui32NowDay = i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD();

	if( ui32NowDay == pstClan->_statedate ) return;

	pstClan->_statedate = ui32NowDay;

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_SET_ACTIVE_STATE;
	stRingInDB._ui32ClanDBIdx	= pstClan->_ClanDetailInfo.m_ui32CID;
	stRingInDB.Write( &ui32NowDay, sizeof( UINT32) );

	g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );
}

// ------------------------------------------------------------------------- //
// NAME: GetMenagement()
// DESC: 클랜 관리 권한 설정
// ------------------------------------------------------------------------- //
BOOL	CTaskProcessor::GetMenagement( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstMember, CLAN_MENAGE_AUTHORITY_TYPE eMenageType )
{
	// 마스터이면 무조건 TRUE 
	if( CLAN_MEMBER_LEVEL_MASTER == pstMember->_level )				return TRUE;
	// 스탭이 아니면 무조건 FALSE
	if( CLAN_MEMBER_LEVEL_STAFF != pstMember->_level )				return FALSE;

	// STAFF 일경우 권한 체크
	switch( eMenageType )
	{
		case CLAN_MENAGE_TYPE_WRITE:
			if( GET_CLAN_MENAGE_WRITE( pstClan->_ClanDetailInfo.m_authstaff ))		return TRUE;
			break;
		case CLAN_MENAGE_TYPE_INVITED:
			if( GET_CLAN_MENAGE_INVITED( pstClan->_ClanDetailInfo.m_authstaff ))		return TRUE;
			break;
		case CLAN_MENAGE_TYPE_DEPORTATION:
			if( GET_CLAN_MENAGE_DEPORTATION( pstClan->_ClanDetailInfo.m_authstaff ))	return TRUE;
			break;
		case CLAN_MENAGE_TYPE_ACCEPT:
			if( GET_CLAN_MENAGE_ACCEPT( pstClan->_ClanDetailInfo.m_authstaff ))		return TRUE;
			break;
	}
	return FALSE;
}

// ------------------------------------------------------------------------- //
// NAME: ClanChangePerson()
// DESC: 클랜 인원 랭크 변화
// ------------------------------------------------------------------------- //
void	CTaskProcessor::ClanChangePerson( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	UINT8 ui8PersonRank = GetClanPersonRank( pstClan->_ClanDetailInfo.m_currentPerson );

	if( pstClan->_ClanDetailInfo.m_personRank != ui8PersonRank )
	{
		pstClan->_ClanDetailInfo.m_personRank = ui8PersonRank;

		// 바뀐 정보 보내주기
		ClanSendConnectMemberChangeInfo( PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK, pstClan, &pstClan->_ClanDetailInfo.m_personRank, sizeof(UINT8) );
	}

	SendClanBasicInfo( pstClan );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	CTaskProcessor::SendMemberInfo( CLAN_USER_INFO* pstUserInfo, CLAN_DETAIL_INFO_SERVER* pstClan )
{
	// 맵버 정보를 보내준다이~
	UINT8 ui8Count = 0;

	i3NetworkPacket SendPacket(PROTOCOL_CS_MEMBER_INFO_ACK);
	SendPacket.WriteData( &pstUserInfo->i32SessionIdx,	sizeof(INT32) );
	SendPacket.WriteData( &pstUserInfo->i64UID,			sizeof(T_UID) );
	SendPacket.WriteData( &ui8Count,					sizeof(UINT8) );

	INT32	i32MemberCount = pstClan->_pMember->GetCount();

	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	UINT32 ui32PacketSize  = 0;
	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( NULL == pstMember ) continue;
		if( pstUserInfo->i64UID == pstMember->_i64UID ) continue;

		UINT8 ui8Size = (UINT8)i3String::Length( pstMember->_nick ) + 1;

		ui32PacketSize = SendPacket.GetPacketSize() + sizeof(UINT8) + sizeof(TTCHAR) * ui8Size + sizeof(CLANMEMBER_INFO);

		if( PACKETBUFFERSIZE <= ui32PacketSize )
		{
			SendPacket.WriteData( sizeof(INT32) + sizeof(T_UID), &ui8Count, sizeof(UINT8) );

			g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket );

			ui8Count = 0;
			SendPacket.Clear();
			SendPacket.SetProtocol(PROTOCOL_CS_MEMBER_INFO_ACK);
			SendPacket.WriteData( &pstUserInfo->i32SessionIdx,	sizeof(INT32) );
			SendPacket.WriteData( &pstUserInfo->i64UID,			sizeof(T_UID) );
			SendPacket.WriteData( &ui8Count,					sizeof(UINT8) );			
		}
		
		CLANMEMBER_INFO Info;
		Info._fuid	= pstMember->_i64UID;
		Info._state = pstMember->_state;
		Info._rank	= pstMember->_rank;
		Info.m_ui8NickColor	= pstMember->_color;

		SendPacket.WriteData( &ui8Size,			sizeof(UINT8) );
		SendPacket.WriteData( pstMember->_nick,	sizeof(TTCHAR) * ui8Size );
		SendPacket.WriteData( &Info,			sizeof(CLANMEMBER_INFO) );
		ui8Count++;
	}
	SendPacket.WriteData( sizeof(INT32) + sizeof(T_UID), &ui8Count, sizeof(UINT8) );
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket );
}

void CTaskProcessor::SendClanMemberStateChange(CLAN_DETAIL_INFO_SERVER* pClanDetail, CLAN_MEMBER_DETAIL_INFO* pMember)
{
	if( !pClanDetail )
		return;

	i3mem::FillZero(m_pui8SendUserCount, sizeof(UINT8) * g_pConfig->GetServerCount());

	for( INT32 i = 0 ; i < pClanDetail->_pMember->GetCount(); ++i )
	{
		CLAN_MEMBER_DETAIL_INFO* pClanMember = static_cast<CLAN_MEMBER_DETAIL_INFO*>( pClanDetail->_pMember->GetItem(i) );
		if( !pClanMember )
			continue;

		if( pClanMember->_i64UID == pMember->_i64UID )
			continue;

		if( FRIEND_STATE_OFFLINE == GET_FRIEND_STATE(pClanMember->_state) )
			continue;

		if( -1 == pClanMember->_session )
			continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pClanMember->_state );

		// 중남미에서 2011.12.13에 ui32Server값이 224(-32)가 들어와서 클랜서버 크래쉬 났습니다. 크래쉬 안나도록 예외처리
		if( ui32Server >= g_pConfig->GetServerCount() )						continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pClanMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pClanMember->_i64UID;

		m_pui8SendUserCount[ ui32Server ]++;
	}
	
	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount(); i++ )
	{
		if( 0 == m_pui8SendUserCount[i] )
			continue;

		i3NetworkPacket stSendPacket( PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK );
		stSendPacket.WriteData( &m_pui8SendUserCount[ i ],	sizeof(UINT8) );
		stSendPacket.WriteData( m_ppi32SendSession[ i ],	sizeof(INT32) * m_pui8SendUserCount[ i ] );
		stSendPacket.WriteData( m_ppi64SendDBIdx[ i ],		sizeof(T_UID) * m_pui8SendUserCount[ i ] );
		stSendPacket.WriteData( &pMember->_i64UID,			sizeof(T_UID) );
		stSendPacket.WriteData( &pMember->_state,			sizeof(UINT64) );

		g_pGateway->SendPacket( i, &stSendPacket );
	}
}

void	CTaskProcessor::SendMemberInfo_Insert( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser )
{
	UINT8 ui8Lenght = (UINT8)i3String::Length( pstUser->_nick ) + 1;

	CLANMEMBER_INFO Info;
	Info._fuid	= pstUser->_i64UID;
	Info._rank	= pstUser->_rank;
	Info._state = pstUser->_state;
	Info.m_ui8NickColor = pstUser->_color;

	i3mem::FillZero( m_pui8SendUserCount, sizeof( UINT8 )*g_pConfig->GetServerCount());

	INT32	i32MemberCount = pstClan->_pMember->GetCount();
	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( NULL == pstMember )														continue;
		if( pstUser->_i64UID == pstMember->_i64UID )								continue;
		if( GET_FRIEND_STATE( (UINT32)pstMember->_state ) == FRIEND_STATE_OFFLINE )	continue;
		if( -1 == pstMember->_session )												continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		if( ui32Server >= g_pConfig->GetServerCount() )								continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pstMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pstMember->_i64UID;
		m_pui8SendUserCount[ ui32Server ]++;
	}

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		if( m_pui8SendUserCount[ i ] == 0 ) continue;

		i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_INSERT_ACK );
		SendPacket.WriteData( &m_pui8SendUserCount[ i ],		sizeof(UINT8) );
		SendPacket.WriteData( m_ppi32SendSession[ i ],			(UINT16)(sizeof(INT32) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( m_ppi64SendDBIdx[ i ],			(UINT16)(sizeof(T_UID) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( &ui8Lenght,						sizeof(UINT8));
		SendPacket.WriteData( pstUser->_nick,					sizeof(TTCHAR) * ui8Lenght );
		SendPacket.WriteData( &Info,							sizeof(CLANMEMBER_INFO));
		g_pGateway->SendPacket( i, &SendPacket );
	}
}

void	CTaskProcessor::SendMemberInfo_Delete( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser )
{
	i3mem::FillZero( m_pui8SendUserCount, sizeof( UINT8 )*g_pConfig->GetServerCount());

	INT32	i32MemberCount = pstClan->_pMember->GetCount();
	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( NULL == pstMember )														continue;
		if( pstUser->_i64UID == pstMember->_i64UID )								continue;
		if( GET_FRIEND_STATE( (UINT32)pstMember->_state ) == FRIEND_STATE_OFFLINE )	continue;
		if( -1 == pstMember->_session )												continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		if( ui32Server >= g_pConfig->GetServerCount() )								continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pstMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pstMember->_i64UID;
		m_pui8SendUserCount[ ui32Server ]++;
	}

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		if( m_pui8SendUserCount[ i ] == 0 ) continue;

		i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_DELETE_ACK );
		SendPacket.WriteData( &m_pui8SendUserCount[ i ],		sizeof(UINT8) );
		SendPacket.WriteData( m_ppi32SendSession[ i ],			(UINT16)(sizeof(INT32) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( m_ppi64SendDBIdx[ i ],			(UINT16)(sizeof(T_UID) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( &pstUser->_i64UID,				sizeof(T_UID));
		g_pGateway->SendPacket( i, &SendPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Request Message
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------
// Name : CheckHaveItem
// Desc : 클랜 상용화 체크
// -----------------------------------------------------------
void	CTaskProcessor::CheckHaveItem( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	// 클랜 칼라 체크
	if( pstClan->_ItemColorNameDate != 0 )
	{
		if( pstClan->_ItemColorNameDate < i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )
		{
			pstClan->_ClanDetailInfo.m_color = 0;
			//SendClanBasicInfo( nClanIdx );

			pstClan->_ItemColorNameDate = 0;	// 0으로 초기화
		}
	}
	// 클랜 경험치 업 체크
	if( pstClan->_ItemExpUpDate != 0 )
	{
		if( pstClan->_ItemExpUpDate < i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )
		{
			pstClan->_ItemExpUpDate = 0;		// 0으로 초기화
		}
	}
}

// -----------------------------------------------------------
// Name : CheckUseItem
// Desc : 클랜 상용화 체크
// -----------------------------------------------------------
BOOL	CTaskProcessor::CheckUseItem( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	// 클랜 칼라 체크
	if( pstClan->_ItemColorNameDate >= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )	return TRUE;
	// 클랜 경험치 업 체크
	if( pstClan->_ItemExpUpDate >= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )		return TRUE;
	
	return FALSE;
}
// -----------------------------------------------------------
// Name : UserLogIn
// Desc : 로그인 처리
// -----------------------------------------------------------
void	CTaskProcessor::UserLogIn( CLAN_USER_INFO* pstUserInfo )
{
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_USER_LOGIN; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	//stRingInDB._ui8Rank			= pstUserInfo->ui8Rank;

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorkingUID( pstUserInfo->i64UID, &stRingInDB ) )
	{
		//EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
		// 로그만 남깁니다. -- 현우 일해라 --
	}
}

// -----------------------------------------------------------
// Name : UserLogOut
// Desc : 로그아웃 처리
// -----------------------------------------------------------
void	CTaskProcessor::UserLogOut( CLAN_WAR_INFO* pstUserInfo )
{
	ClanWarLogout(pstUserInfo);

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_OFFLINE, 0, 0, 0 );
		pstMember->_session		= -1;
		pstMember->_pagein		= 0;

		SendClanMemberStateChange( pstClan, pstMember );
		
		return;
	}
	CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		pstRequest->_state		= MAKE_FRIEND_MATCH_STATE(MATCH_TEAM_INVALID_IDX, FRIEND_STATE_OFFLINE, 0, 0, 0);
		pstRequest->_sessionIdx	= -1;
	}
}
// -----------------------------------------------------------
// Name : UserStateChange
// Desc : 유저의 상태 변경
// -----------------------------------------------------------
void	CTaskProcessor::UserStateChange(CLAN_USER_INFO* pstUserInfo, UINT64 ui64State)
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2(pstUserInfo->ui32ClanDBIdx);
	if (NULL == pstClan) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx(pstUserInfo->i64UID);
	if (pstMember)
	{
		pstMember->_state = ui64State;
		pstMember->_session = pstUserInfo->i32SessionIdx;

		SendClanMemberStateChange(pstClan, pstMember);
		return;
	}
	CLAN_REQUEST_SERVER_INFO*	pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx(pstUserInfo->i64UID);
	if (pstRequest)
	{
		pstRequest->_state = MAKE_FRIEND_STATE(GET_FRIEND_STATE(ui64State), GET_FRIEND_SERVER(ui64State), GET_FRIEND_CHANNEL(ui64State), GET_FRIEND_ROOMIDX(ui64State));
		pstRequest->_sessionIdx = pstUserInfo->i32SessionIdx;
	}
}
// -----------------------------------------------------------
// Name : UserServerEnter
// Desc : 서버 입장
// -----------------------------------------------------------
void	CTaskProcessor::UserServerEnter( CLAN_USER_INFO* pstUserInfo )
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pstUserInfo->ui32ServerIdx, 0, 0 );
		pstMember->_session		= pstUserInfo->i32SessionIdx;
		return;
	}
	CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pstUserInfo->ui32ServerIdx, 0, 0 );
		pstRequest->_sessionIdx	= pstUserInfo->i32SessionIdx;
	}
}

// -----------------------------------------------------------
// Name : UserServerLeave
// Desc : 서버 퇴장
// -----------------------------------------------------------
void	CTaskProcessor::UserServerLeave( CLAN_WAR_INFO* pstUserInfo )
{
	ClanWarLogout(pstUserInfo);

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, 0, 0, 0 );
		pstMember->_session		= -1;
		pstMember->_pagein		= 0;
		return;
	}
	CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest ) 
	{
        pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, 0, 0, 0 );
		pstRequest->_sessionIdx	= -1;
	}
}

// -----------------------------------------------------------
// Name : UserChannelEnter
// Desc : 채널 입장
// -----------------------------------------------------------
void CTaskProcessor::UserChannelEnter(CLAN_USER_INFO* pstUserInfo, UINT32 ui32Channel)
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		UINT32 ui32Match	= (UINT32)GET_FRIEND_MATCH( pstMember->_state );		// 방에서 나올때 ChannelEnter 을 받기때문에 Match 방을 저장해둔다.
		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( ui32Match, FRIEND_STATE_LOBBY, pstUserInfo->ui32ServerIdx, ui32Channel, 0 );
		pstMember->_session		= pstUserInfo->i32SessionIdx;

		SendClanMemberStateChange( pstClan, pstMember );
		return;
	}
	CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_LOBBY, pstUserInfo->ui32ServerIdx, ui32Channel, 0 );
		pstRequest->_sessionIdx	= pstUserInfo->i32SessionIdx;
	}
}

// -----------------------------------------------------------
// Name : UserChannelLeave
// Desc : 채널 퇴장
// -----------------------------------------------------------
void	CTaskProcessor::UserChannelLeave( CLAN_WAR_INFO* pstUserInfo )
{
	ClanWarLogout(pstUserInfo );

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pstUserInfo->ui32ServerIdx, 0, 0 );
		pstMember->_session		= pstUserInfo->i32SessionIdx;
		pstMember->_pagein		= 0;

		SendClanMemberStateChange( pstClan, pstMember );
		return;

	}
	CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pstUserInfo->ui32ServerIdx, 0, 0 );
		pstRequest->_sessionIdx	= pstUserInfo->i32SessionIdx;
	}
}

// -----------------------------------------------------------
// Name : UserRoomEnter
// Desc : 방에 입장
// -----------------------------------------------------------
void	CTaskProcessor::UserRoomEnter( CLAN_USER_INFO* pstUserInfo, UINT32 ui32RoomIdx, bool bIsMatchMaking )
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		UINT32 ui32Match		= (UINT32)GET_FRIEND_MATCH( pstMember->_state );
		UINT32 ui32Channel		= GET_FRIEND_CHANNEL( (UINT32)pstMember->_state );

		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( ui32Match, FRIEND_STATE_ROOM, pstUserInfo->ui32ServerIdx, ui32Channel, ui32RoomIdx );
		pstMember->_session		= pstUserInfo->i32SessionIdx;

		SendClanMemberStateChange( pstClan, pstMember );
		 
		return;
	}
	CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		UINT32 ui32Channel		= GET_FRIEND_CHANNEL( pstRequest->_state );

		pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ROOM, pstUserInfo->ui32ServerIdx, ui32Channel, ui32RoomIdx );
		pstRequest->_sessionIdx	= pstUserInfo->i32SessionIdx;
	}
}

// -----------------------------------------------------------
// Name : UserRoomLeave
// Desc : 방 퇴장
// -----------------------------------------------------------
void	CTaskProcessor::UserRoomLeave( CLAN_USER_INFO* pstUserInfo )
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		UINT32 ui32Match		= (UINT32)GET_FRIEND_MATCH( pstMember->_state );
		UINT32 ui32Channel		= GET_FRIEND_CHANNEL( (UINT32)pstMember->_state );

		pstMember->_state		= MAKE_FRIEND_MATCH_STATE( ui32Match, FRIEND_STATE_LOBBY, pstUserInfo->ui32ServerIdx, ui32Channel, 0 );
		pstMember->_session		= pstUserInfo->i32SessionIdx;

		SendClanMemberStateChange( pstClan, pstMember );
		return;
	}

	CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		UINT32 ui32Channel		= GET_FRIEND_CHANNEL( (UINT32)pstRequest->_state );

		pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_LOBBY, pstUserInfo->ui32ServerIdx, ui32Channel, 0 );
		pstRequest->_sessionIdx	= pstUserInfo->i32SessionIdx;
	}
}

// -----------------------------------------------------------
// Name : UserClanPageEnter
// Desc : 클랜 페이지 입장
// -----------------------------------------------------------
void	CTaskProcessor::UserClanPageEnter( CLAN_USER_INFO* pstUserInfo )
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) return;

	pstMember->_pagein = 1;

	ClanDetailInfo( pstUserInfo, 0 );
}

// -----------------------------------------------------------
// Name : UserClanPageLeave
// Desc : 클랜 페이지 퇴장
// -----------------------------------------------------------
void	CTaskProcessor::UserClanPageLeave( CLAN_USER_INFO* pstUserInfo )
{
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) return;

	pstMember->_pagein = 0;
}

// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void CTaskProcessor::StreamingClanList( UINT32 ui32ServerIdx, UINT32 ui32StartIdx )
{
	if (ui32StartIdx >= m_ClanListContext._totalCount )	return;

	// 요구한 섹션을 전달한다.
	UINT8 ui8Size = 0;

	i3NetworkPacket sendPacket(PROTOCOL_CS_CLAN_LIST_STREAM_ACK);
	sendPacket.WriteData(&ui32StartIdx,		sizeof(UINT32));
	sendPacket.WriteData(&ui8Size,			sizeof(UINT8));

	CLAN_LIST_BASIC_INFO	ClanListBasicInfo;
	for( UINT32 i = ui32StartIdx; i < m_ClanListContext._totalCount ; i++ )
	{	
		//설정된 클랜 최대 생성수를 초과하였습니다.
		if( i >= g_pConfig->m_ui32ClanMax)
		{
				g_pGateway->SendPacket( ui32ServerIdx, &sendPacket );
				return;
		}
	
		CLAN_DETAIL_INFO_SERVER		ClanDetailInfoServer = m_pClanDetailInfo[i];
		ClanDetailInfoServer.SetClanListBaiscInfo(&ClanListBasicInfo);

		//실제 Write 할 크기를 계산한다
		INT32	i32Realsize	=	sizeof(CLAN_BASIC_INFO) +		
								sizeof(CLAN_BASIC_RECORD) +
								sizeof(CLAN_BASIC_SORT_INFO) +
								sizeof(UINT16) +
								sizeof(TTCHAR) * ClanListBasicInfo.m_ui16introsize;

		// 8192 byte 넘어가면 루프를 빠져나가고 다음패킷에 쓴다
		if(PACKETBUFFERSIZE < sendPacket.GetPacketSize() + i32Realsize )	break;

		sendPacket.WriteData( &ClanListBasicInfo.m_ClanBasicInfo,		sizeof(CLAN_BASIC_INFO));
		sendPacket.WriteData( &ClanListBasicInfo.m_ClanBasicRecord,		sizeof(CLAN_BASIC_RECORD));
		sendPacket.WriteData( &ClanListBasicInfo.m_ClanBsicSortInfo,	sizeof(CLAN_BASIC_SORT_INFO));
		sendPacket.WriteData( &ClanListBasicInfo.m_ui16introsize,		sizeof(UINT16));
		sendPacket.WriteData( &ClanListBasicInfo.m_intro,				sizeof(TTCHAR) * ClanListBasicInfo.m_ui16introsize);

		ui32StartIdx++;
		ui8Size++;
	}

	sendPacket.WriteData( sizeof(UINT32), &ui8Size, sizeof(UINT8) );

	g_pGateway->SendPacket( ui32ServerIdx, &sendPacket );
	//I3TRACE("Normal Send / ServerIdx:%d / StartIdx : %d \n", ui32ServerIdx, ui32StartIdx);

	// 다음 섹션을 요청하는 재귀패킷을 생성한다.
	if (ui32StartIdx < m_ClanListContext._totalCount )
	{
		i3NetworkPacket SendPacket(PROTOCOL_CS_CLAN_LIST_STREAM_REQ);
		SendPacket.WriteData(&ui32ServerIdx,		sizeof(UINT32));
		SendPacket.WriteData(&ui32StartIdx,			sizeof(UINT32));
		
		g_pTaskProcessor->SendRecursivePacket( &SendPacket );

		//I3TRACE("RecurSend / ServerIdx:%d / StartIdx : %d \n", ui32ServerIdx, ui32StartIdx);
	}
}

// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void	CTaskProcessor::SendClanBasicInfo( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	SetClanListContect();

	//i3NetworkPacket sendPacket( PROTOCOL_CS_CLAN_LIST_GS_REQ );
	//sendPacket.WriteData( &pstClan->_ui32CID,	sizeof(UINT32) );
	//sendPacket.WriteData( pstClan,				sizeof(CLAN_BASIC_INFO) );

	//// 모든 서버에 보냄
	//for( UINT32 i = 0 ; i < g_pConfig->m_ServerCount ; i++ )
	//{
	//	if( g_pGateway->IsConnectedGameServer( i ) )
	//		g_pGateway->SendPacket( i, &sendPacket );
	//}
}
// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void	CTaskProcessor::SendMemberContext( CLAN_USER_INFO* pstUserInfo )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_MEMBER_CONTEXT_ACK;
	INT32		i32Success	= EVENT_ERROR_SUCCESS;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	UINT8 ui8SectionSize = MEMBER_SECTION_SIZE;

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,				sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,						sizeof(T_UID));
	SendPacket.WriteData(&i32Success,								sizeof(INT32));
	SendPacket.WriteData(&pstClan->_stMemberContext._totalCount,	sizeof(UINT8));
	SendPacket.WriteData(&ui8SectionSize,							sizeof(UINT8));
	SendPacket.WriteData(&pstClan->_stMemberContext._sectionCount,	sizeof(UINT8));
	SendPacket.WriteData(&pstClan->_stMemberContext._version,		sizeof(UINT32));

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void	CTaskProcessor::SendRequestContext( CLAN_USER_INFO* pstUserInfo )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_REQUEST_CONTEXT_ACK;
	INT32		i32Success	= EVENT_ERROR_SUCCESS;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	UINT8	ui8SectionSize = REQUEST_SECTION_SIZE;

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,				sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,						sizeof(T_UID));
	SendPacket.WriteData(&i32Success,								sizeof(INT32));
	SendPacket.WriteData(&pstClan->_stRequestContext._totalCount,	sizeof(UINT8));
	SendPacket.WriteData(&ui8SectionSize,							sizeof(UINT8));
	SendPacket.WriteData(&pstClan->_stRequestContext._sectionCount,	sizeof(UINT8));
	SendPacket.WriteData(&pstClan->_stRequestContext._version,		sizeof(UINT32));

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void CTaskProcessor::SendMemberList( CLAN_USER_INFO* pstUserInfo, UINT8 ui8SectionIdx )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_MEMBER_LIST_ACK;
	INT32		i32Success	= EVENT_ERROR_SUCCESS;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 존재하는 Section 인지
	if ( ui8SectionIdx >= pstClan->_stMemberContext._sectionCount )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_SECTION );

	UINT8 ui8Size = MEMBER_SECTION_SIZE;

	if ( ui8SectionIdx + 1 == pstClan->_stMemberContext._sectionCount )
	{
		ui8Size = (UINT8)( pstClan->_stMemberContext._totalCount % MEMBER_SECTION_SIZE );

		if (0 == ui8Size)
		{
			ui8Size = MEMBER_SECTION_SIZE;
		}
	}

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,			sizeof(T_UID));
	SendPacket.WriteData(&i32Success,					sizeof(INT32));			
	SendPacket.WriteData(&ui8SectionIdx,				sizeof(UINT8));
	SendPacket.WriteData(&ui8Size,						sizeof(UINT8));

	INT32 i32Idx = ui8SectionIdx * MEMBER_SECTION_SIZE;

	for( INT32 i = 0 ; i < ui8Size ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i32Idx+i );
		if( NULL == pstMember ) continue;

		SendPacket.WriteData( pstMember, sizeof(CLAN_MEMBER_BASIC_INFO));
	}
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket );
}

// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void CTaskProcessor::SendRequestList( CLAN_USER_INFO* pstUserInfo, UINT8 ui8SectionIdx )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_REQUEST_LIST_ACK;
	INT32		i32Success	= EVENT_ERROR_SUCCESS;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 존재하는 Section 인지
	if ( ui8SectionIdx >= pstClan->_stRequestContext._sectionCount )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_SECTION );
	
	UINT8 ui8Size = REQUEST_SECTION_SIZE;

	if ( ui8SectionIdx + 1 == pstClan->_stRequestContext._sectionCount )
	{
		ui8Size = ( pstClan->_stRequestContext._totalCount % REQUEST_SECTION_SIZE );

		if (0 == ui8Size)
		{
			ui8Size = REQUEST_SECTION_SIZE;
		}
	}

	i3NetworkPacket sendPacket( ProtocolID );
	sendPacket.WriteData(&pstUserInfo->i32SessionIdx,	sizeof(INT32));
	sendPacket.WriteData(&pstUserInfo->i64UID,			sizeof(T_UID));
	sendPacket.WriteData(&i32Success,					sizeof(INT32));
	sendPacket.WriteData(&ui8SectionIdx,				sizeof(UINT8));
	sendPacket.WriteData(&ui8Size,						sizeof(UINT8));

	INT32 i32Idx = ui8SectionIdx * REQUEST_SECTION_SIZE;

	for( INT32 i = 0 ; i < ui8Size ; i++ )
	{
		CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->GetItem( i32Idx+i );
		if( NULL == pstRequest ) continue;

		sendPacket.WriteData( pstRequest, sizeof(CLAN_REQUEST_BASIC_INFO) );
	}
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &sendPacket);
}

// 여기
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------
// Name : ClanDetailInfo
// Desc : 클랜 상세정보_클랜정보
// -----------------------------------------------------------
void	CTaskProcessor::ClanSendConnectMemberChangeInfo( PROTOCOL ProtocolID, CLAN_DETAIL_INFO_SERVER* pstClan, void* pArg, UINT16 ui16Size, PROTOCOL TeamProtocolID)
{
	i3mem::FillZero( m_pui8SendUserCount, sizeof( UINT8 )*g_pConfig->GetServerCount());

	INT32	i32MemberCount = pstClan->_pMember->GetCount();
	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( FRIEND_STATE_OFFLINE == GET_FRIEND_STATE( (UINT32)pstMember->_state )  )	continue;
		if( -1 == pstMember->_session )													continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		if( ui32Server >= g_pConfig->GetServerCount() )									continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pstMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pstMember->_i64UID;

		m_pui8SendUserCount[ ui32Server ]++;
	}

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		if( m_pui8SendUserCount[ i ] == 0 ) continue;

		i3NetworkPacket SendPacket( ProtocolID );
		SendPacket.WriteData( &m_pui8SendUserCount[ i ],	sizeof(UINT8) );
		SendPacket.WriteData( m_ppi32SendSession[ i ],		(UINT16)(sizeof(INT32) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( m_ppi64SendDBIdx[ i ],		(UINT16)(sizeof(T_UID) * m_pui8SendUserCount[ i ]) );
		if( (NULL != pArg) && (0 < ui16Size) )
		{
			SendPacket.WriteData( pArg,						ui16Size );
		}
		g_pGateway->SendPacket( i, &SendPacket );
	}

	// 해당 클랜의 매치팀이 매치메이킹 룸에 있다면 알려줍니다.
	if ( pstClan->GetMatchTeamCount() > 0 && TeamProtocolID > 0 && pArg && ui16Size > 0)
	{
		for (INT32 k = 0; k < MAX_CLAN_MATCH_TEAM_COUNT; ++k)
		{
			CMatchTeam* pMatchTeam = pstClan->GetMatchTeam(k);
			if (!pMatchTeam) continue;
			if (((UINT32)pMatchTeam->GetClanID()) != pstClan->_ClanDetailInfo.m_ui32CID) continue;
			if (TEAM_STATE_MATCHED != pMatchTeam->GetTeamState()) continue;

			UINT32		ui32ServerIdx	= pMatchTeam->GetServerIdx(); 
			T_TeamID	TTeamID			= pMatchTeam->GetTeamID();

			i3NetworkPacket TeamPacket(TeamProtocolID);
			TeamPacket.WriteData(&TTeamID, sizeof(T_TeamID));	// 변경 할 매치팀 ID
			TeamPacket.WriteData(pArg, ui16Size);
			
			pMatchTeam->MakePacketMemberUIDList(&TeamPacket);	// 자기팀

			CTeamManager* pTeamManager = m_pTeamManager[ui32ServerIdx];
			CMatchTeam* pEnemyTeam = pTeamManager->FindMatchTeam(pMatchTeam->GetMatchedTeamID());
			if (pEnemyTeam)
			{
				pEnemyTeam->MakePacketMemberUIDList(&TeamPacket);	// 상대팀 
			}
			else
			{
				INT32 i32CurMember = 0;
				TeamPacket.WriteData(&i32CurMember, sizeof(INT32));
			}

			g_pGateway->SendPacket(ui32ServerIdx, &TeamPacket);
		}
	}	
}

// -----------------------------------------------------------
// Name : ClanListDetailInfo
// Desc : 클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 요청
// -----------------------------------------------------------
void	CTaskProcessor::ClanListDetailInfo( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Type )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK;
	INT32		i32Success	= EVENT_ERROR_SUCCESS;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	i32Success = (INT32)ui8Type;

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,			sizeof(T_UID));
	SendPacket.WriteData(&i32Success,					sizeof(INT32));
	SendPacket.WriteData(pstClan,						sizeof(CLAN_DETAIL_INFO));

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

// -----------------------------------------------------------
// Name : ClanDetailInfo
// Desc : 클랜 상세정보_클랜정보
// -----------------------------------------------------------
void	CTaskProcessor::ClanDetailInfo(CLAN_USER_INFO* pstUserInfo, UINT8 ui8Type)
{
	PROTOCOL	ProtocolID = PROTOCOL_CS_DETAIL_INFO_ACK;
	INT32		i32Success = EVENT_ERROR_SUCCESS;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2(pstUserInfo->ui32ClanDBIdx);
	if (NULL == pstClan) EV_FAILED_CLAN(ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX);

	i32Success = (INT32)ui8Type;

	i3NetworkPacket SendPacket(ProtocolID);
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx, sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID, sizeof(T_UID));
	SendPacket.WriteData(&i32Success, sizeof(INT32));

	SendPacket.WriteData(&pstClan->_ClanDetailInfo, sizeof(CLAN_DETAIL_INFO));
	SendPacket.WriteData(&pstClan->_ClanTopMember, sizeof(CLAN_TOP_MEMBER));
	SendPacket.WriteData(&pstClan->_stRequestContext._totalCount, sizeof(UINT8));

	INT32 i32ArrIdx = pstClan->FindNewMatchArrIdx();

	SendPacket.WriteData(&pstClan->m_ClanWarResult[i32ArrIdx].m_Result, sizeof(UINT8));
	SendPacket.WriteData(pstClan->m_aClanWarPlayerUID, sizeof(T_UID)*CLAN_TEAM_MEMBER_COUNT);

	g_pGateway->SendPacket(pstUserInfo->ui32ServerIdx, &SendPacket);
}

// -----------------------------------------------------------
// Name : ClanDetailInfoMember
// Desc : 클랜 상세정보_멤버 정보
// -----------------------------------------------------------
void	CTaskProcessor::ClanDetailInfoMember( CLAN_USER_INFO* pstUserInfo, T_UID i64UID )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_GET_MEMBER_INFO;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &i64UID,	sizeof(T_UID) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanDetailInfoRequest
// Desc : 클랜 상세정보_가입신청서
// -----------------------------------------------------------
void	CTaskProcessor::ClanDetailInfoRequest( CLAN_USER_INFO* pstUserInfo, T_UID i64UID )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_REQUEST_INFO_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( i64UID );
	if( NULL == pstRequest ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_GET_REQUEST_INFO;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &i64UID,	sizeof(T_UID) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanCreate
// Desc : 클랜 생성
// -----------------------------------------------------------
void	CTaskProcessor::ClanCreate( CLAN_USER_INFO* pstUserInfo, CLAN_CS_CREATE_INFO* pClanCreateInfo )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_CREATE_CLAN_ACK;

	if( m_ClanListContext._totalCount >= g_pConfig->m_ui32ClanMax )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MAXCLAN );

	if( m_i32ClanTempCount >= (INT32)g_pConfig->m_ui32ClanMax )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MAXCLAN );

	UINT16 ui16ClanIdx;
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_CREATE; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= 0;
	stRingInDB.Write( &ui16ClanIdx,		sizeof(UINT16) );
	stRingInDB.Write( pClanCreateInfo,	sizeof(CLAN_CS_CREATE_INFO) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorkingUID( pstUserInfo->i64UID, &stRingInDB ) )	// 클랜이 존재하지 않으므로 UserIdx 로 쓰레드를 나눠줍니다.
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
	m_i32ClanTempCount++;

	//m_pListClanTemp
}

// -----------------------------------------------------------
// Name : ClanClose
// Desc : 클랜 해채
// -----------------------------------------------------------
void	CTaskProcessor::ClanClose( CLAN_USER_INFO* pstUserInfo )
{
	// 클랜 삭제는 현재 작동 안합니다. 어차피 이부분 싹 바꿔야 하기 때문에 우선 주석 처리합니다.
	// 20100115 -- 김현우 -- 대폭 수정 예정중입니다.

	PROTOCOL ProtocolID = PROTOCOL_CS_CLOSE_CLAN_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstMember->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	// 매치팀 있는지
	if( pstClan->HaveMatchTeam() ) 
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_WAR_HAVE_MATCH_TEAM );


	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_DEATROY; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( pstUserInfo->ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanRequestJoin
// Desc : 클랜 가입 신청서
// -----------------------------------------------------------
void	CTaskProcessor::ClanRequestJoin( CLAN_USER_INFO* pstUserInfo, CLAN_CS_JOIN_INFO* pClanJoinInfo )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_JOIN_REQUEST_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) 
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// Request 개수 확인
	if( MAX_REQUEST_COUNT <= pstClan->_stRequestContext._totalCount )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MAXREQUEST );

	if( MAX_REQUEST_COUNT <= pstClan->_pRequest->GetCount() )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MAXREQUEST );

	// 계급 제한에 걸리는지
	if( pClanJoinInfo->wRank < pstClan->_ClanDetailInfo.m_ranklock )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL );

	// 나이 제한에 걸리는지
	if( pstClan->_ClanDetailInfo.m_lagelock > 0 && (pClanJoinInfo->wAge < pstClan->_ClanDetailInfo.m_lagelock) )// 연령제한 하한
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL );
	
	if( pstClan->_ClanDetailInfo.m_hagelock > 0 && (pClanJoinInfo->wAge > pstClan->_ClanDetailInfo.m_hagelock) )// 연령제한 상한
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL );

	// Memory 관리를 위하여 미리 메모리를 생성합니다.
	CLAN_REQUEST_SERVER_INFO* pstRequest = PopTempRequest();//new CLAN_REQUEST_SERVER_INFO;
	if( NULL == pstRequest )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMORY_FULL );

	i3String::Copy( pstRequest->_nick, pClanJoinInfo->strNick, NET_NICK_NAME_SIZE );
	pstRequest->_i64UID		= pstUserInfo->i64UID;
	pstRequest->_rank		= pClanJoinInfo->wViewRank;
	pstRequest->_ui8NickColor = pClanJoinInfo->ui8NickColor;
	pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pstUserInfo->ui32ServerIdx, pClanJoinInfo->wChannelNum, pClanJoinInfo->nRoomIdx );
	pstRequest->_sessionIdx	= pstUserInfo->i32SessionIdx;
	if( 0 > pstClan->_pRequest->Add( pstRequest ) )
	{
		I3_SAFE_DELETE( pstRequest );
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMORY_FULL );
	}

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REQUEST_JOIN; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( pClanJoinInfo,	sizeof(CLAN_CS_JOIN_INFO) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanRequestAccept
// Desc : 클랜 가입 신청 수락
// -----------------------------------------------------------
void	CTaskProcessor::ClanRequestAccept( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Count, T_UID* pi64UID )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_ACCEPT_REQUEST_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 권한이 있는지
	if( FALSE == GetMenagement( pstClan, pstMember, CLAN_MENAGE_TYPE_ACCEPT ) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOSTAFF );

	// 인원수 확인
	if( (pstClan->_ClanDetailInfo.m_currentPerson + ui8Count) > pstClan->_ClanDetailInfo.m_maxPerson )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MAXMEMBER );

	for( INT32 i = 0 ; i < ui8Count ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstFind = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pi64UID[i] );
		if( pstFind )
		{	// 이미 클랜원이다.
			EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMBER );
		}

		CLAN_MEMBER_DETAIL_INFO* pstNewMember = PopTempMember();//new CLAN_MEMBER_DETAIL_INFO;
		if( NULL == pstNewMember )
		{
			for( INT32 j = 0 ; j < i ; j++ )
			{	// 그동안 만들었던 메모리를 삭제합니다.
				CLAN_MEMBER_DETAIL_INFO* pstTempMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->DeleteEx( pi64UID[j] );
				PushTempMember( pstTempMember );
			}
			EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMORY_FULL );
		}

		pstNewMember->_i64UID		= pi64UID[i];
		pstNewMember->_session		= -1;
		pstNewMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_OFFLINE, 0, 0, 0 );
		//pstNewMember->_battleteam	= MATCH_TEAM_INVALID_IDX;
		pstNewMember->_level		= CLAN_MEMBER_LEVEL_REGULAR;

		if( 0 > pstClan->_pMember->Add( pstNewMember ) )
		{
			for( INT32 j = 0 ; j < i ; j++ )
			{	// 그동안 만들었던 메모리를 삭제합니다.
				CLAN_MEMBER_DETAIL_INFO* pstTempMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->DeleteEx( pi64UID[j] );
				PushTempMember( pstTempMember );
			}
			EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMORY_FULL );
		}
	}

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REQUEST_ACCEPT; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui8Count,	sizeof(UINT8) );
	stRingInDB.Write( pi64UID,		sizeof(T_UID)*ui8Count );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanRequestCancel
// Desc : 클랜 가입 신청 취소
// -----------------------------------------------------------
void	CTaskProcessor::ClanRequestCancel( CLAN_USER_INFO* pstUserInfo )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_CANCEL_REQUEST_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstRequest ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REQUEST_CANCEL; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanRequestDenial
// Desc : 클랜 가입 신청 거절
// -----------------------------------------------------------
void	CTaskProcessor::ClanRequestDenial( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Count, T_UID* pi64UID )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_DENIAL_REQUEST_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 권한이 있는지
	if( FALSE == GetMenagement( pstClan, pstMember, CLAN_MENAGE_TYPE_ACCEPT ) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOSTAFF );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REQUEST_DENIAL; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui8Count,	sizeof(UINT8) );
	stRingInDB.Write( pi64UID,		sizeof(T_UID)*ui8Count );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanInvitedAccept
// Desc : 클랜 클랜 가입 신청 수락
// -----------------------------------------------------------
void	CTaskProcessor::ClanInviteAccept( CLAN_USER_INFO* pstUserInfo, UINT32 ui32State, UINT8 ui8Accept, UINT32 ui32Rank, TTCHAR* pstrNick,  UINT8 ui8NickColor  )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_INVITE_ACCEPT_ACK;
	INT32 i32Success = EVENT_ERROR_SUCCESS;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 이미 클랜원 인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMBER );

	if( ui8Accept == 0 )
	{
		UINT8 ui8NameSize = (UINT8)i3String::Length( pstClan->_ClanDetailInfo.m_name ) + 1;
		// 거절 메시지.
		i32Success = EVENT_ERROR_CLAN_INVITED_REFUSAL;
		i3NetworkPacket SendPacket( ProtocolID );
		SendPacket.WriteData( &pstUserInfo->i32SessionIdx,				sizeof(INT32) );
		SendPacket.WriteData( &pstUserInfo->i64UID,						sizeof(T_UID) );
		SendPacket.WriteData( &i32Success,								sizeof(INT32) );
		SendPacket.WriteData( &pstClan->_ClanDetailInfo.m_ui32CID,		sizeof(UINT32) );
		SendPacket.WriteData( &pstClan->_ClanDetailInfo.m_i64masterUID,	sizeof(INT64) );
		SendPacket.WriteData( &ui8NameSize,								sizeof(UINT8) );
		SendPacket.WriteData( pstClan->_ClanDetailInfo.m_name,			sizeof(TTCHAR) * ui8NameSize );

		g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx , &SendPacket );
		return;
	}

	// 인원수 확인
	if( pstClan->_ClanDetailInfo.m_currentPerson >= pstClan->_ClanDetailInfo.m_maxPerson )
			EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MAXMEMBER );

	CLAN_MEMBER_DETAIL_INFO* pstNewMember = PopTempMember();//new CLAN_MEMBER_DETAIL_INFO;
	if( NULL == pstNewMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMORY_FULL );

	pstNewMember->_i64UID	= pstUserInfo->i64UID;
	pstNewMember->_state	= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, GET_FRIEND_STATE(ui32State), pstUserInfo->ui32ServerIdx, GET_FRIEND_CHANNEL(ui32State), GET_FRIEND_ROOMIDX(ui32State) );
	pstNewMember->_session	= pstUserInfo->i32SessionIdx;
	pstNewMember->_rank		= (UINT8)ui32Rank;
	pstNewMember->_color	= ui8NickColor;
	pstNewMember->_level	= CLAN_MEMBER_LEVEL_REGULAR;
	i3String::Copy( pstNewMember->_nick, pstrNick, NET_NICK_NAME_SIZE );

	if( 0 > pstClan->_pMember->Add( pstNewMember ) )
	{
		I3_SAFE_DELETE( pstNewMember );
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_MEMORY_FULL );
	}

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_INVITE_ACCEPT; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( pstrNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
	stRingInDB.Write( &ui32Rank,	sizeof( UINT32 ) );
	stRingInDB.Write(&ui8NickColor, sizeof(UINT8));

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanCommissionMaster
// Desc : 클랜 마스터 위임
// -----------------------------------------------------------
void	CTaskProcessor::ClanCommissionMaster( CLAN_USER_INFO* pstUserInfo, T_UID i64UID, UINT8 ui8CommisionRank)
{
	PROTOCOL ProtocolID = PROTOCOL_CS_COMMISSION_MASTER_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 현재 사용중인 아이템이 있는지
	if( CheckUseItem( pstClan ) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_USE_ITEM_FAIL );

	// 클랜원 인지
	CLAN_MEMBER_DETAIL_INFO* pstMaster = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMaster ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOMEMBER );

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOMEMBER );

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstMaster->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	// 대상이 마스터인지
	if( CLAN_MEMBER_LEVEL_MASTER == pstMember->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_MASTER );

	// 대상의 랭크가 위임기준에 맞지 않으면 실패
	if (ui8CommisionRank > pstMember->_rank)
		EV_FAILED_CLAN(ProtocolID, pstUserInfo, EVENT_ERROR_PBV15_CLAN_COMMISION_FAIL_RANK);

	//if( wRank < CLAN_RANK_A2 || m_UserInfoBasic._point < 5000 )

	//if( pstMember->_rank )
	//	EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_MASTER );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_COMMISSION_MASTER; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &i64UID, sizeof(T_UID));

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanCommissionStaff
// Desc : 클랜 스태프 임명
// -----------------------------------------------------------
void	CTaskProcessor::ClanCommissionStaff( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Count, T_UID* pi64UID )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_COMMISSION_STAFF_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜원 인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstMember->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_COMMISSION_STAFF; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui8Count,	sizeof(UINT8));
	stRingInDB.Write( pi64UID,		sizeof(T_UID)*ui8Count);

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanCommissionRegular
// Desc : 클랜 일반 임명
// -----------------------------------------------------------
void	CTaskProcessor::ClanCommissionRegular( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Count, T_UID* pi64UID )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_COMMISSION_REGULAR_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜원 인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstMember->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_COMMISSION_REGULAR; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui8Count,	sizeof(UINT8));
	stRingInDB.Write( pi64UID,		sizeof(T_UID)*ui8Count);

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanSecession
// Desc : 클랜 탈퇴
// -----------------------------------------------------------
void	CTaskProcessor::ClanSecession( CLAN_USER_INFO* pstUserInfo )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_SECESSION_CLAN_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	 
	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 마스터는 탈퇴 불가
	if( CLAN_MEMBER_LEVEL_MASTER == pstMember->_level ) 
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_MASTER );

	// 배틀 중이면 탈퇴 불가. 기획 변경으로 인해 가능함.2015.01.22
	//if( MATCH_TEAM_INVALID_IDX != pstMember->_battleteam )	
	//	EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_MEMBER_SECESSION; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanDeportation
// Desc : 클랜 제명
// -----------------------------------------------------------
void	CTaskProcessor::ClanDeportation( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Count, T_UID* pi64MemberDBIdx )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_DEPORTATION_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 권한이 있는지
	if( FALSE == GetMenagement( pstClan, pstMember, CLAN_MENAGE_TYPE_DEPORTATION ) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOSTAFF );
	
	for( UINT8 i = 0 ; i < ui8Count ; i++ ) 
	{
		CLAN_MEMBER_DETAIL_INFO* pstNewMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pi64MemberDBIdx[i] );
		if( NULL == pstNewMember )									EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOMEMBER );
		if( pstMember == pstNewMember )								EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX );		// 나 자신은 탈퇴 불가
		if( CLAN_MEMBER_LEVEL_MASTER == pstNewMember->_level )		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_MASTER );				// 마스터 탈퇴 불가
		//if( MATCH_TEAM_INVALID_IDX != pstNewMember->_battleteam )	EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOMEMBER );					// 클랜전중 탈퇴 불가. 기획 변경으로 가능 2015.01.22
	}

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_MEMBER_DEPORTATION; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID	= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui8Count,		sizeof(UINT8) );
	stRingInDB.Write( pi64MemberDBIdx,	sizeof(T_UID)*ui8Count );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanMemChatting
// Desc : 클랜 맴버 체팅
// -----------------------------------------------------------
void	CTaskProcessor::ClanMemChatting( CLAN_USER_INFO* pstUserInfo, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_CHATTING_ACK;	

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	i3mem::FillZero( m_pui8SendUserCount, sizeof( UINT8 )*g_pConfig->GetServerCount());

	UINT8 ui8NickSize = (UINT8)i3String::Length( pstUser->_nick )+1;

	INT32	i32MemberCount = pstClan->_pMember->GetCount();
	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( NULL == pstMember ) continue;
		if( GET_FRIEND_STATE( pstMember->_state ) == FRIEND_STATE_OFFLINE )	continue;
		if( -1 == pstMember->_session  )									continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		if( ui32Server >= g_pConfig->GetServerCount() )						continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pstMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pstMember->_i64UID;

		m_pui8SendUserCount[ ui32Server ]++;
	}

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		if( m_pui8SendUserCount[ i ] == 0 ) continue;

		i3NetworkPacket SendPacket( ProtocolID );
		SendPacket.WriteData( &m_pui8SendUserCount[ i ],	sizeof(UINT8) );
		SendPacket.WriteData( m_ppi32SendSession[ i ],		(UINT16)(sizeof(INT32) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( m_ppi64SendDBIdx[ i ],		(UINT16)(sizeof(T_UID) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( &ui8NickSize,					sizeof(UINT8) );
		SendPacket.WriteData( pstUser->_nick,				sizeof(TTCHAR) * ui8NickSize );
		SendPacket.WriteData( &bIsGMUser,					sizeof(bool) );
		SendPacket.WriteData( &ui8NickColor,				sizeof(UINT8) );
		SendPacket.WriteData( &ui8ChatSize,					sizeof(UINT8) ); 
		SendPacket.WriteData( pstrChating,					sizeof(TTCHAR) * ui8ChatSize );
		g_pGateway->SendPacket( i, &SendPacket );
	}
}

// -----------------------------------------------------------
// Name : ClanPageChatting
// Desc : 클랜 맴버 페이지 체팅
// -----------------------------------------------------------
void	CTaskProcessor::ClanPageChatting( CLAN_USER_INFO* pstUserInfo, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_PAGE_CHATTING_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	i3mem::FillZero( m_pui8SendUserCount, sizeof( UINT8 )*g_pConfig->GetServerCount());

	UINT8 ui8NickSize = (UINT8)i3String::Length( pstUser->_nick )+1;
	
	INT32	i32MemberCount = pstClan->_pMember->GetCount();
	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( NULL == pstMember ) continue;
		if( GET_FRIEND_STATE( pstMember->_state ) == FRIEND_STATE_OFFLINE )	continue;
		if( -1 == pstMember->_session  )									continue;
		if( 0 == pstMember->_pagein  )										continue;
		if( MATCH_TEAM_INVALID_IDX != GET_FRIEND_MATCH(pstMember->_state) )				continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		if( ui32Server >= g_pConfig->GetServerCount() )						continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pstMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pstMember->_i64UID;

		m_pui8SendUserCount[ ui32Server ]++;
	}

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		if( m_pui8SendUserCount[ i ] == 0 ) continue;

		i3NetworkPacket SendPacket( ProtocolID );
		SendPacket.WriteData( &m_pui8SendUserCount[ i ],	sizeof(UINT8) );
		SendPacket.WriteData( m_ppi32SendSession[ i ],		(UINT16)(sizeof(INT32) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( m_ppi64SendDBIdx[ i ],		(UINT16)(sizeof(T_UID) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( &ui8NickSize,					sizeof(UINT8) );
		SendPacket.WriteData( pstUser->_nick,				sizeof(TTCHAR) * ui8NickSize );
		SendPacket.WriteData( &bIsGMUser,					sizeof(bool) );
		SendPacket.WriteData( &ui8NickColor,				sizeof(UINT8) );
		SendPacket.WriteData( &ui8ChatSize,					sizeof(UINT8) );
		SendPacket.WriteData( pstrChating,					sizeof(TTCHAR) * ui8ChatSize );

		g_pGateway->SendPacket( i, &SendPacket );
	}
}

// -----------------------------------------------------------
// Name : ClanCheckMark
// Desc : 클랜 체크_마크 중복
// -----------------------------------------------------------
void	CTaskProcessor::ClanCheckMark( CLAN_USER_INFO* pstUserInfo, UINT32 ui32Mark )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_CHECK_MARK_ACK;
	INT32 i32Success = _ClanCheckMark( ui32Mark );
	
	SendPacket( ProtocolID, pstUserInfo, i32Success );
}

// -----------------------------------------------------------
// Name : ClanCheckMark
// Desc : 클랜 체크_마크 중복
// -----------------------------------------------------------
INT32	CTaskProcessor::_ClanCheckMark( UINT32 ui32Mark )
{
	for( UINT32 i = 0 ; i < m_ClanListContext._totalCount ; i++ )
	{
		if( m_pClanDetailInfo[i]._ClanDetailInfo.m_mark == ui32Mark ) 
		{
			return EVENT_ERROR_FAIL;
		}
	}

	// 훈련전 클랜을 위해 클랜 마크를 비워둡니다.
	if( ui32Mark == MAKE_CLANMARK_FLAG( 9, 2, 9, 7 ) )		return EVENT_ERROR_FAIL;
	else if( ui32Mark == MAKE_CLANMARK_FLAG( 3, 4, 6, 4 ) )	return EVENT_ERROR_FAIL;
	else if( ui32Mark == MAKE_CLANMARK_FLAG( 5, 3, 1, 9 ) )	return EVENT_ERROR_FAIL;

	return EVENT_ERROR_SUCCESS;
}

// -----------------------------------------------------------
// Name : ClanCheckJoinAuthority
// Desc : 클랜 체크_가입 조건
// -----------------------------------------------------------
void	CTaskProcessor::ClanCheckJoinAuthority( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Age, UINT8 ui8Rank )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK;
	INT32 i32Success = EVENT_ERROR_SUCCESS;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 계급 제한에 걸리는지
	if( ui8Rank < pstClan->_ClanDetailInfo.m_ranklock )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL );

	// 나이 제한에 걸리는지
	if( pstClan->_ClanDetailInfo.m_lagelock > 0 && ui8Age < pstClan->_ClanDetailInfo.m_lagelock )// 연령제한 하한
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL );
	
	if( pstClan->_ClanDetailInfo.m_hagelock > 0 &&	ui8Age > pstClan->_ClanDetailInfo.m_hagelock )// 연령제한 상한
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL );

	SendPacket( ProtocolID, pstUserInfo, i32Success );
}

// -----------------------------------------------------------
// Name : ClanReplaceNotice
// Desc : 클랜 교체_공지사항
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceNotice( CLAN_USER_INFO* pstUserInfo, TTCHAR* pstrNotice )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_REPLACE_NOTICE_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 권한이 있는지
	if( FALSE == GetMenagement( pstClan, pstUser, CLAN_MENAGE_TYPE_WRITE ) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOSTAFF );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REPLACE_NOTICE;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( pstrNotice,	sizeof(TTCHAR) * NET_CLAN_INTRO_SIZE );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceIntro
// Desc : 클랜 교체_소개
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceIntro( CLAN_USER_INFO* pstUserInfo, TTCHAR* pstrIntro )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_REPLACE_INTRO_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 권한이 있는지
	if( FALSE == GetMenagement( pstClan, pstUser, CLAN_MENAGE_TYPE_WRITE ) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOSTAFF );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REPLACE_INTRO;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( pstrIntro,	sizeof(TTCHAR) * NET_CLAN_INTRO_SIZE );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceName
// Desc : 클랜 교체_이름
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceClanName( CLAN_USER_INFO* pstUserInfo, TTCHAR* pstrName )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_REPLACE_NAME_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );
	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REPLACE_CLANNAME;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( pstrName,	sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceMark
// Desc : 클랜 교체_마크
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceMark( CLAN_USER_INFO* pstUserInfo, UINT32 ui32Mark )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_REPLACE_MARK_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );
	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	// 이미 사용중인 클랜 마크인지
	if( EV_FAILED( _ClanCheckMark( ui32Mark ) ))
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_FAIL );
	}
	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REPLACE_MARK;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui32Mark,	sizeof(UINT32) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanReplacePersonMax
// Desc : 클랜 교체_최대인원
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplacePersonMax( CLAN_USER_INFO* pstUserInfo, UINT16 ui16AddPersonCount )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_REPLACE_PERSONMAX_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	if( ( pstClan->_ClanDetailInfo.m_maxPerson + ui16AddPersonCount ) > MAX_MEMBER_COUNT )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_INCREASE_MAXPERSON_FAIL );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REPLACE_PERSONMAX;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui16AddPersonCount, sizeof( UINT16 ) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceManagement
// Desc : 클랜 교체_가입조건
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceManagement( CLAN_USER_INFO* pstUserInfo, CLAN_MANAGEMENT* pManagement )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_REPLACE_MANAGEMENT_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REPLACE_MANAGEMENT;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID	= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( pManagement, sizeof( CLAN_MANAGEMENT ) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceMemberNick
// Desc : 클랜 교체_멤버 닉네임
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceMemberNick( CLAN_WAR_INFO* pstUserInfo, MATCH_MEMBER_INFO* pMemberInfo )
{
	ClanWarChangeMemberInfo(CHANGE_INFO_NICKNAME, pstUserInfo, pMemberInfo );

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember ) 
	{
		// 마스터 인지
		if( CLAN_MEMBER_LEVEL_MASTER == pstMember->_level )
		{	// 클랜 마스터 이름을 바꿔줍니다.
			i3String::Copy( pstClan->_ClanDetailInfo.m_masterNick, pMemberInfo->m_strNick, NET_NICK_NAME_SIZE );
		}
		// 맴버의 이름을 바꿔줍니다.
		i3String::Copy( pstMember->_nick, pMemberInfo->m_strNick, NET_NICK_NAME_SIZE );

		// 바뀐 정보 보내주기
		ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK, pstClan, pstMember, sizeof( CLAN_MEMBER_BASIC_INFO ) );

		// DB 에 저장합니다.
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_REPLACE_MEMBER_NICK;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID	= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &pstMember->_level,	sizeof(UINT8) );
		stRingInDB.Write( pMemberInfo->m_strNick,				sizeof(TTCHAR) * NET_NICK_NAME_SIZE );

		g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );

		return;
	}
    CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		i3String::Copy( pstRequest->_nick, pMemberInfo->m_strNick, NET_NICK_NAME_SIZE );

		UINT8 ui8Level = 0;
		// DB 에 저장합니다.
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_REPLACE_MEMBER_NICK;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID			= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &ui8Level,	sizeof(UINT8) );
		stRingInDB.Write( pMemberInfo->m_strNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE );

		g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceMemberColorNick
// Desc : 클랜 교체_멤버 닉네임 칼라
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceMemberColorNick( CLAN_WAR_INFO* pstUserInfo, MATCH_MEMBER_INFO* pMemberInfo )
{
	ClanWarChangeMemberInfo(CHANGE_INFO_NICKCOLOR, pstUserInfo, pMemberInfo );

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember ) 
	{
		// 마스터 인지
		if( CLAN_MEMBER_LEVEL_MASTER == pstMember->_level )
		{	// 클랜 마스터 닉네임컬러를 바꿔줍니다.
			pstClan->_ClanDetailInfo.m_masterNickColor = pMemberInfo->m_ui8NickColor;
		}

		// 맴버의 닉네임컬러을 바꿔줍니다.
		pstMember->_color = pMemberInfo->m_ui8NickColor;

		// 바뀐 정보 보내주기
		ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK, pstClan, pstMember, sizeof( CLAN_MEMBER_BASIC_INFO ) );

		// DB 에 저장합니다.
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_REPLACE_MEMBER_NICKCOLOR;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID	= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &pstMember->_level,	sizeof(UINT8) );
		stRingInDB.Write( &pMemberInfo->m_ui8NickColor,			sizeof(UINT8) );

		g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );

		return;
	}
    CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		pstRequest->_ui8NickColor = pMemberInfo->m_ui8NickColor;

		UINT8 ui8Level = 0;
		// DB 에 저장합니다.
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_REPLACE_MEMBER_NICKCOLOR;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID			= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &ui8Level,	sizeof(UINT8) );
		stRingInDB.Write( &pMemberInfo->m_ui8NickColor,	sizeof(UINT8) );

		g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );
	}
}

// -----------------------------------------------------------
// Name : ClanReplaceMemberRank
// Desc : 클랜 교체_멤버 랭크
// -----------------------------------------------------------
void	CTaskProcessor::ClanReplaceMemberRank( CLAN_WAR_INFO* pstUserInfo, MATCH_MEMBER_INFO* pMemberInfo )
{
	ClanWarChangeMemberInfo(CHANGE_INFO_RANK, pstUserInfo, pMemberInfo );

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( pstMember )
	{
		// 마스터 인지
		if( CLAN_MEMBER_LEVEL_MASTER == pstMember->_level )
		{	// 클랜 마스터 계급을 바꿔줍니다. 
			pstClan->_ClanDetailInfo.m_masterRank = pMemberInfo->m_ui8Rank;
		}
		// 맴버의 계급을 바꿔줍니다.
		pstMember->_rank = pMemberInfo->m_ui8Rank;
		
		// 바뀐 정보 보내주기
		ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK, pstClan, pstMember, sizeof( CLAN_MEMBER_BASIC_INFO ) );

		// DB 에 저장합니다.
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_REPLACE_MEMBER_RANK;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID	= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &pstMember->_level,	sizeof(UINT8) );
		stRingInDB.Write( &pMemberInfo->m_ui8Rank,				sizeof(UINT8) );

		g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );
		return;
	}
	CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pstUserInfo->i64UID );
	if( pstRequest )
	{
		pstRequest->_rank = pMemberInfo->m_ui8Rank;

		UINT8 ui8Level = 0;
		// DB 에 저장합니다.
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_REPLACE_MEMBER_RANK;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID	= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &ui8Level,			sizeof(UINT8) );
		stRingInDB.Write( &pMemberInfo->m_ui8Rank,				sizeof(UINT8) );

		g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );
	}
}

// -----------------------------------------------------------
// Name : ClanMemberInvitedRoom
// Desc : 클랜 멤버 방에 초대
// -----------------------------------------------------------
void	CTaskProcessor::ClanMemberInvitedRoom( CLAN_USER_INFO* pstUserInfo, T_UID i64UID )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_ROOM_INVITED_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	INT32 i32CurrentServerIdx = GET_FRIEND_SERVER( (UINT32)pstMember->_state );

	if (0 >= i32CurrentServerIdx)
	{
		// Send Game Server
		SendPacket(PROTOCOL_CS_ROOM_INVITED_ACK, pstUserInfo, EVENT_ERROR_EVENT_FAIL_INVITED_USER);
		return;
	}
	
	// 요청 전달
	if( (FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstMember->_state )) && (-1 != pstMember->_session) )
	{
		SendPacketInvitedResult( PROTOCOL_CS_ROOM_INVITED_RESULT_ACK, i32CurrentServerIdx, pstMember->_session, i64UID, pstUserInfo->i64UID );
	}
}

// -----------------------------------------------------------
// Name : ClanNote
// Desc : 클랜 전체 쪽지
// -----------------------------------------------------------
void	CTaskProcessor::ClanNote( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Type, UINT8 ui8NoteSize, TTCHAR* pstrNote )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_NOTE_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	INT32	i32MessageCount = 0;
	T_UID	ai64UID[ MAX_MEMBER_COUNT ];
	INT32	i32MemberCount = pstClan->_pMember->GetCount();

	if( MAX_MEMBER_COUNT < i32MemberCount )
	{
		g_pLog->WriteLog( L"[ERROR] Clan Member Over Count:%d, CID:%d", i32MemberCount, pstClan->_ClanDetailInfo.m_ui32CID );

		i32MemberCount = MAX_MEMBER_COUNT;
	}

	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( NULL == pstMember ) continue;

		switch ( ui8Type )
		{
			case CLAN_NOTE_STAFF:
				if( CLAN_MEMBER_LEVEL_STAFF != pstMember->_level )		continue;
				break;
			case CLAN_NOTE_REGULAR:
				if( CLAN_MEMBER_LEVEL_REGULAR != pstMember->_level )	continue;
				break;
		}

		ai64UID[ i32MessageCount ] = pstMember->_i64UID;
		i32MessageCount++;
	}

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData( &pstUserInfo->i32SessionIdx,		sizeof(INT32) );
	SendPacket.WriteData( &pstUserInfo->i64UID,				sizeof(T_UID) );
	SendPacket.WriteData( &i32MessageCount,					sizeof(INT32) );
	if( 0 < i32MessageCount )
	{
		SendPacket.WriteData( ai64UID,						(UINT16)(sizeof(T_UID)*i32MessageCount));
		SendPacket.WriteData( &ui8NoteSize,					sizeof(UINT8) );
		SendPacket.WriteData( pstrNote,						sizeof(TTCHAR) * ui8NoteSize );
	}

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket );
}

// -----------------------------------------------------------
// Name : ClanCheckName
// Desc : 클랜 이름 중복 체크
// -----------------------------------------------------------
void	CTaskProcessor::ClanCheckName( CLAN_USER_INFO* pstUserInfo, TTCHAR* pstrClanName )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_CHECK_DUPLICATE_ACK;

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_CHECK_NAME;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= rand()%DB_THREAD_COUNT;
	stRingInDB.Write( pstrClanName, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanCheckAzit
// Desc : 클랜 아지트 중복 체크
// -----------------------------------------------------------
void	CTaskProcessor::ClanCheckAzit( CLAN_USER_INFO* pstUserInfo, TTCHAR* pstrAzit )
{
	PROTOCOL	ProtocolID	= PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK;
	
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_CHECK_AZIT;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= rand()%DB_THREAD_COUNT;
	stRingInDB.Write( pstrAzit, sizeof(TTCHAR) * NET_CLAN_AZIT_URL_SIZE );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}

// -----------------------------------------------------------
// Name : ClanBattleExpUp
// Desc : 클랜 경험치 증가권
// -----------------------------------------------------------
void	CTaskProcessor::ClanBattleExpUp( CLAN_USER_INFO* pstUserInfo, UINT32 ui32ItemID, UINT32 ui32Date )
{
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )	return;

	// 성공
	// Log 남기기
	g_pLog->WriteLog( L"Clan ClanBattleExpUp - User Idx:%I64d, Clan Name:%s, Clan Idx:%d", 
		pstUserInfo->i64UID, pstClan->_ClanDetailInfo.m_name, pstUserInfo->ui32ClanDBIdx );
	// Log 남기기

	pstClan->_ItemExpUpDate =	ui32Date;

	UINT32 ui32Arg = 0;
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_ITEM_PERIOD;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui32ItemID,	sizeof(UINT32) );
	stRingInDB.Write( &ui32Date,	sizeof(UINT32) );
	stRingInDB.Write( &ui32Arg,		sizeof(UINT32) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		return;
	}
}

// -----------------------------------------------------------
// Name : ClanBattleRecordReset
// Desc : 클랜 전적 초기화
// -----------------------------------------------------------
void	CTaskProcessor::ClanBattleRecordReset( CLAN_USER_INFO* pstUserInfo )
{
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )	return;

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_ITEM_RESETRECORD;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		return;
	}
}

// -----------------------------------------------------------
// Name : ClanBattlePointReset
// Desc : 클랜 포인트 초기화
// -----------------------------------------------------------
void	CTaskProcessor::ClanBattlePointReset( CLAN_USER_INFO* pstUserInfo )
{
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )	return;

	//-------------------------------------------------------------------------------------------------
	// Error 확인 끝
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_ITEM_RESETPOINT;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		return;
	}
}


// -----------------------------------------------------------
// Name : ClanBattleReplaceColorName
// Desc : 컬러 클랜명
// -----------------------------------------------------------
void	CTaskProcessor::ClanBattleReplaceColorName( CLAN_USER_INFO* pstUserInfo, UINT32 ui32ItemID, UINT32 ui32Date, UINT8 ui8Color )
{
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )	return;

	// 성공
	// Log 남기기
	g_pLog->WriteLog( L"Clan ClanBattleColorName - User Idx:%I64d, Clan Name:%s, Clan Idx:%d", 
		pstUserInfo->i64UID, pstClan->_ClanDetailInfo.m_name, pstUserInfo->ui32ClanDBIdx );
	// Log 남기기

	if( 0 == ui32Date )		
	{	// 인증 받기 전 보낸 패킷
		pstClan->_ItemColorNameDate		= 0;
		pstClan->_ClanDetailInfo.m_color	= ui8Color;

		UINT32 ui32Arg = ui8Color;
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_ITEM_PERIOD;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID			= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &ui32ItemID,	sizeof(UINT32) );
		stRingInDB.Write( &ui32Date,	sizeof(UINT32) );
		stRingInDB.Write( &ui32Arg,		sizeof(UINT32) );

		if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
		{
			return;
		}
	}
	else if( 0 == ui8Color )
	{	// 인증 후 보낸 패킷
		pstClan->_ItemColorNameDate	= ui32Date;

		SendClanBasicInfo( pstClan );

		ClanDetailInfo( pstUserInfo, 0 );

		// 바뀐 정보 보내주기
		ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK, pstClan, &pstClan->_ClanDetailInfo.m_color, sizeof( UINT8 ), PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK);

		UINT32 ui32Arg = pstClan->_ClanDetailInfo.m_color;
		RING_IN_DB stRingInDB;
		stRingInDB._ui8Type			= RING_DB_ITEM_PERIOD;
		stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
		stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
		stRingInDB._i64UID			= pstUserInfo->i64UID;
		stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
		stRingInDB.Write( &ui32ItemID,	sizeof(UINT32) );
		stRingInDB.Write( &ui32Date,	sizeof(UINT32) );
		stRingInDB.Write( &ui32Arg,		sizeof(UINT32) );

		if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
		{
			return;
		}
	}
}

void	CTaskProcessor::ClanInvite( CLAN_USER_INFO* pstUserInfo, T_UID i64UID )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_INVITE_ACK;

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_INVITE;
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pstUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &i64UID, sizeof(T_UID) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );
	}
}
/*
// -----------------------------------------------------------
// Name : ClanBattleDeleteItem
// Desc : 컬러 클랜명
// -----------------------------------------------------------
void	CTaskProcessor::ClanBattleDeleteItem( CLAN_USER_INFO* pstUserInfo, UINT32 wItemIdx )
{
	//PROTOCOL ProtocolID = PROTOCOL_CS_DELETE_ITEM_ACK;
	//INT32 nSuccess = EVENT_ERROR_SUCCESS;

	// 클랜이 존재하는지
	INT32 nClanIdx = FindClan( pstUserInfo->ui32ClanDBIdx );
	if( nClanIdx < 0 ) return;//EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 원인지
	INT32 nUserIdx = FindUserInClan( pstUserInfo->i64UID, nClanIdx );
	if( nUserIdx < 0 ) return;//EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOMEMBER );

	// 마스터 인지
	if( m_ppMemberDetailInfo[ nClanIdx ][ nUserIdx ]._level != CLAN_MEMBER_LEVEL_MASTER )
		return;//EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_LEVEL_NOMASTER );

	switch( wItemIdx )
	{
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_CLAN_XP_PLUS_150, CASHITEM_MAINTENANCE_CLAN_XP_PLUS_150):
			m_pClanDetailInfo[ nClanIdx ]._ItemExpUpDate		= 0;
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_COLOR_CLAN, CASHITEM_MAINTENANCE_COLOR_CLAN):
			m_pClanDetailInfo[ nClanIdx ]._color				= 0;
			m_pClanDetailInfo[ nClanIdx ]._ItemColorNameDate	= 0;

			// 바뀐 정보 보내주기
			ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK, nClanIdx, &m_pClanDetailInfo[ nClanIdx ]._color, sizeof( UINT8 ) );
			break;
		default:
			return;
	}
}*/

void	CTaskProcessor::ClanItemDelete( CLAN_USER_INFO *pUserInfo, UINT32 ui32DeleteItemID )
{
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	// 클랜 원인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pUserInfo->i64UID );
	if( NULL == pstUser ) return;

	// 마스터 인지
	if( CLAN_MEMBER_LEVEL_MASTER != pstUser->_level )	return;

	// 삭제하려는 클랜아이템을 사용중인지
	switch( ui32DeleteItemID )
	{
	case CASHITEM_ITEMID_CLAN_XP_PLUS_150:
		// 클랜경험치150%
		if( pstClan->_ItemExpUpDate == 0 )	return;
		pstClan->_ItemExpUpDate = 0;
		break;
	case CASHITEM_ITEMID_COLOR_CLAN:
		// 클랜 칼라
		if( pstClan->_ItemColorNameDate == 0 )	return;
		pstClan->_ItemColorNameDate = 0;
		pstClan->_ClanDetailInfo.m_color				= 0;
		break;
	}

	// 성공
	// Log 남기기
	g_pLog->WriteLog( L"Clan ClanItemDelete - User Idx:%I64d, Clan Name:%s, Clan Idx:%d, DeleteItem: %d", 
		pUserInfo->i64UID, pstClan->_ClanDetailInfo.m_name, pUserInfo->ui32ClanDBIdx, ui32DeleteItemID );
	// Log 남기기

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type			= RING_DB_ITEM_DELETE;
	stRingInDB._ui32ServerIdx	= pUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pUserInfo->i64UID;
	stRingInDB._ui32ClanDBIdx	= pUserInfo->ui32ClanDBIdx;
	stRingInDB.Write( &ui32DeleteItemID,	sizeof(UINT32) );

	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
	{
		g_pLog->WriteLog( L"[ItemError]ClanItemDelete Fail - User Idx:%I64d, Clan Name:%s, Clan Idx:%d, DeleteItem: %d",
			pUserInfo->i64UID, pstClan->_ClanDetailInfo.m_name, pUserInfo->ui32ClanDBIdx, ui32DeleteItemID );

		return;
	}
}

// 여기
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------
// Name : 
// Desc : 
// -----------------------------------------------------------
void	CTaskProcessor::BattleResultUser( UINT8 ui8Count, CLAN_RESULT_USER* pResultUserList, CMercenaryManager*	pMercenaryMgr )
{
	for( UINT8 i = 0 ; i < ui8Count ; i++ )
	{
		CLAN_RESULT_USER* pResultUser = &pResultUserList[i]; 

		RING_IN_DB stRingInDB;
		stRingInDB._i64UID		= pResultUser->_i64UID;

		CMercenary* pMercenary = pMercenaryMgr->FindMercenary( pResultUser->m_TMerID );
		if( NULL != pMercenary ) 
		{	// 용병 
			pMercenary->AddRecord( pResultUser );

			INT32 i32MerRank = (INT32)pMercenary->GetMerRank();

			stRingInDB._ui8Type	= RING_DB_UPDATE_MERCENARY_MATCH_RECORD;
			stRingInDB.Write(&i32MerRank, sizeof(INT32));
			stRingInDB.Write(pResultUser, sizeof(CLAN_RESULT_USER));

			if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorkingUID(pResultUser->_i64UID, &stRingInDB) )
			{
				g_pLog->WriteLog( L"[ERROR] Failed to insert Mercenary match result. (UID : %I64d, MerID : %d, Result : %d, Exp : %d, Kill : %d, Death : %d, Headshot : %d)",
					pResultUser->_i64UID, pResultUser->m_TMerID , pResultUser->_WinLose, pResultUser->_Exp, pResultUser->_Kill, pResultUser->_Death, pResultUser->_Head);
			}

			continue;
		}

		// 클랜원 
		CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pResultUser->_ClanIdx );
		if( NULL == pstClan )
		{	
			g_pLog->WriteLog( L"[ERROR] Failed to Find Clan. (UID : %I64d, ClanIdx : %d)", pResultUser->_i64UID, pResultUser->_ClanIdx);
			continue;
		}

		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pResultUser->_i64UID );
		if( NULL == pstMember )
		{	
			g_pLog->WriteLog( L"[ERROR] Failed to Find ClanMember. (UID : %I64d, ClanIdx : %d)", pResultUser->_i64UID, pResultUser->_ClanIdx);
			continue;
		}

		pstMember->AddRecord( pResultUser );

		stRingInDB._ui8Type			= RING_DB_BATTLE_RESULT_USER;
		stRingInDB._ui32ClanDBIdx	= pResultUser->_ClanIdx; 
		stRingInDB.Write(pResultUser, sizeof(CLAN_RESULT_USER));

		if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
		{	
			g_pLog->WriteLog( L"[ERROR] Failed to insert ClanMember match result. (UID : %I64d, CID : %d,  Result : %d, Exp : %d, Kill : %d, Death : %d, Headshot : %d)",
				pResultUser->_i64UID, pResultUser->_ClanIdx,  pResultUser->_WinLose, pResultUser->_Exp, pResultUser->_Kill, pResultUser->_Death, pResultUser->_Head);
		}

	}
}

void	CTaskProcessor::BattleResult( UINT8 ui8ClanCount, CLAN_RESULT* pClanResult, UINT8 ui8UserCount, CLAN_RESULT_USER* pClanResultUser, CTeamManager* pTeamManager, UINT8 ui8IsMatchMaking  )
{
	if( ui8ClanCount == 0 ) return;

	CLAN_DETAIL_INFO_SERVER* ppstClan[ TEAM_COUNT ];

	for( INT32 i = 0 ; i < ui8ClanCount ; i++ )
	{
		ppstClan[ i ] = FindClan2( pClanResult[i]._ClanIdx );
		if( NULL == ppstClan[i] ) continue;

		ppstClan[ i ]->_bClanOrder = TRUE;

		SetClanChangeStateDate( ppstClan[i] );
	}
	
	if( pClanResult[0]._ClanIdx != pClanResult[1]._ClanIdx )
	{	// 클랜 누적 전적 반영
		for( INT32 i = 0 ; i < ui8ClanCount ; i++ )
		{
			if( NULL == ppstClan[i] ) continue;

			// 경험치 증가권을 사용했을시
			if( ppstClan[i]->_ItemExpUpDate >= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )
			{
				UINT32 ui32UpExp = (UINT32)((REAL32)pClanResult[i]._Exp * 0.5f);
				pClanResult[i]._Exp += ui32UpExp;
			}

			ppstClan[i]->_ClanDetailInfo.m_exp		+= pClanResult[i]._Exp;
			ppstClan[i]->_ClanDetailInfo.m_match++;
			ppstClan[i]->_ClanDetailInfo.m_win		+= pClanResult[i]._Win;
			ppstClan[i]->_ClanDetailInfo.m_lose		+= pClanResult[i]._Lose;

			ppstClan[i]->_ClanDetailInfo.m_Totalmatch++;
			ppstClan[i]->_ClanDetailInfo.m_Totalwin		+= pClanResult[i]._Win;
			ppstClan[i]->_ClanDetailInfo.m_Totallose	+= pClanResult[i]._Lose;

			UINT8 ui8ExpRank = GetClanExpRank( ppstClan[i]->_ClanDetailInfo.m_exp );
			if( ppstClan[i]->_ClanDetailInfo.m_expRank != ui8ExpRank )
			{
				ppstClan[i]->_ClanDetailInfo.m_expRank	= ui8ExpRank;
				// 바뀐 정보 보내주기
				ClanSendConnectMemberChangeInfo( PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK, ppstClan[i], &ppstClan[i]->_ClanDetailInfo.m_expRank, sizeof(UINT8) );
			}

			// DB 밀어넣기
			CLAN_RECORD	stRecord;

			RING_IN_DB stRingInDB;
			stRingInDB._ui8Type			= RING_DB_BATTLE_RESULT_CLAN;
			stRingInDB._ui32ClanDBIdx	= ppstClan[i]->_ClanDetailInfo.m_ui32CID;

			stRecord.m_i32Experience	= ppstClan[i]->_ClanDetailInfo.m_exp;
			stRecord.m_i32MatchCount	= 1;
			stRecord.m_i32WinMatch		= pClanResult[i]._Win;
			stRecord.m_i32LoseMatch		= pClanResult[i]._Lose;

			stRingInDB.Write( &stRecord,		sizeof( CLAN_RECORD ) );

			if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB ) )
			{	// 밀어넣기 실패
				g_pLog->WriteLog( L"[ERROR] InsertWorking / BattleResultTeam : Type:%d, ClanDB:%d, Exp:%d, Win:%d, Lose:%d, Match:%d ",
					stRingInDB._ui8Type, stRingInDB._ui32ClanDBIdx, ppstClan[i]->_ClanDetailInfo.m_exp, ppstClan[i]->_ClanDetailInfo.m_win,
					ppstClan[i]->_ClanDetailInfo.m_lose, ppstClan[i]->_ClanDetailInfo.m_match );
			}
		}
	}

	SetClanListContect() ;
}

void	CTaskProcessor::BattleMatchTeamLeaveRoom( CLAN_RESULT* pClanResult, BATTLE_INFO* pBattleInfo, CLAN_DETAIL_INFO_SERVER* pstClan )
{
	if( ( NULL == pBattleInfo ) || ( NULL == pstClan ) ) return;

	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ;

	// 달라진것이 있으면 바로 갱신
	if( i3String::Compare( pBattleInfo->_Clan._ClanName, pstClan->_ClanDetailInfo.m_name ) != 0 )
		i3String::Copy( pBattleInfo->_Clan._ClanName, pstClan->_ClanDetailInfo.m_name, NET_CLAN_NAME_SIZE );

	if( pBattleInfo->_Clan._ClanMark != pstClan->_ClanDetailInfo.m_mark )
		pBattleInfo->_Clan._ClanMark = pstClan->_ClanDetailInfo.m_mark;

	if( pBattleInfo->_Clan._ClanRank != pstClan->_ClanDetailInfo.m_expRank )
		pBattleInfo->_Clan._ClanRank = (UINT8)pstClan->_ClanDetailInfo.m_expRank;

	if( pBattleInfo->_Clan._ClanPoint != pstClan->_ClanDetailInfo.GetClanPoint() )
		pBattleInfo->_Clan._ClanPoint = pstClan->_ClanDetailInfo.GetClanPoint();

	if( pBattleInfo->_Clan._ClanColor != pstClan->_ClanDetailInfo.m_color )
		pBattleInfo->_Clan._ClanColor = pstClan->_ClanDetailInfo.m_color;

	UINT32	ui32ServerIdx = 0;
	UINT8	ui8UserCount = 0;
	INT32	ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
	T_UID	ai64UID[ CLAN_TEAM_MEMBER_COUNT ];

	CLAN_MEMBER_DETAIL_INFO* pstMember;
	for( UINT8 i = 0 ; i < pBattleInfo->_Team._PerMax ; i++ )
	{
		if( pBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY )	continue;
		
		pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		ui32ServerIdx				= GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		ai32Session[ ui8UserCount ]	= pstMember->_session;
		ai64UID[ ui8UserCount ]		= pstMember->_i64UID;

		ui8UserCount++;
	}

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData( &ui8UserCount,		sizeof(UINT8));
	SendPacket.WriteData( ai32Session,			sizeof(INT32) * ui8UserCount );
	SendPacket.WriteData( ai64UID,				sizeof(T_UID) * ui8UserCount );
	SendPacket.WriteData(&pBattleInfo->_Clan,	sizeof(CLAN_BATTLE_CLAN));
	g_pGateway->SendPacket( ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::BattleMatchTeamContext( CLAN_USER_INFO* pstUserInfo )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK;

	CLAN_BATTLE_TEAM_CONTEXT  stTeamContext;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	BATTLE_CLAN_INFO* pBattleClanInfo = m_pBattleTeam->FindClanTeam( pstUserInfo->ui32ClanDBIdx );
	if( pBattleClanInfo )
	{	
		stTeamContext._totalcount		= pBattleClanInfo->_stClanContext._ui8Totalcount;
		stTeamContext._sectionSize		= BATTLE_TEAM_SECTION_SIZE;
		stTeamContext._sectionCount		= pBattleClanInfo->_stClanContext._ui8SectionCount;
	}
	else
	{
		stTeamContext._totalcount		= 0;
		stTeamContext._sectionSize		= BATTLE_TEAM_SECTION_SIZE;
		stTeamContext._sectionCount		= 0;
	}

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,		sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,				sizeof(T_UID));
	SendPacket.WriteData(&stTeamContext,					sizeof(CLAN_BATTLE_TEAM_CONTEXT));

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::BattleMatchAllTeamContext( CLAN_USER_INFO* pstUserInfo, UINT32 ui32Channel )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK;

	BATTLE_CHANNE_INFO* pBattleChannelInfo = m_pBattleTeam->FindChannelTeam( ui32Channel );
	if( NULL == pBattleChannelInfo ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
 
	CLAN_BATTLE_ALL_TEAM_CONTEXT stAllTeamContext;

	stAllTeamContext._totalcount	= pBattleChannelInfo->_stChannelContext._ui16Totalcount;
	stAllTeamContext._sectionSize	= BATTLE_ALL_TEAM_SECTION_SIZE;
	stAllTeamContext._sectionCount	= pBattleChannelInfo->_stChannelContext._ui16SectionCount;	

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,			sizeof(T_UID));
	SendPacket.WriteData(&stAllTeamContext,				sizeof(CLAN_BATTLE_ALL_TEAM_CONTEXT));

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

void CTaskProcessor::RequestMatchTeamList(CTeamManager* pTeamManager, CLAN_USER_INFO* pstUserInfo)
{
	CLAN_DETAIL_INFO_SERVER* pClanDetail = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( !pClanDetail )
	{
		return;
	}

	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK);
	SendPacket.WriteData( &pstUserInfo->i32SessionIdx,	sizeof(INT32) );
	SendPacket.WriteData( &pstUserInfo->i64UID,			sizeof(T_UID) );

	pTeamManager->MakePacketMatchTeamList( pstUserInfo->ui32ClanDBIdx, &SendPacket );

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::BattleMatchGetClanRoomInfo( CLAN_USER_INFO* pstUserInfo, UINT32 ui32Channel )
{
		//PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_ACK;

	//BATTLE_CHANNE_INFO* pBattleChannelInfo = m_pBattleTeam->FindChannelTeam( ui32Channel );

	//INT16	i16AllTeamCount;
	//PACKET_CS_MATCH_GET_CLAN_ROOM_INFO_ACK		PacketAckGC;
	//PacketAckGC.m_i32SessionIdx					= pstUserInfo->i32SessionIdx;
	//PacketAckGC.m_i64UID						= pstUserInfo->i64UID;
	//PacketAckGC.m_PacketCSD.m_WaitingRoomCount	= 0;
	//PacketAckGC.m_PacketCSD.m_PlayingRoomCount	= 0; 
	//
	//if( NULL != pBattleChannelInfo )	
	//{
	//	i16AllTeamCount	= pBattleChannelInfo->_stChannelContext._ui16Totalcount; 

	//	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	//	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, 0 );

	//	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	//	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, 0 );

	//	if( 0 < i16AllTeamCount )
	//	{ 	
	//		INT32 i32Idx = 0;
	//		BATTLE_INFO* pBattleInfo;
	//		INT16 i16Count = 0;

	//		while( i16Count < i16AllTeamCount )
	//		{
	//			if( i32Idx >= pBattleChannelInfo->_pChannelList->GetCount() ) break;
	//			pBattleInfo = (BATTLE_INFO*)pBattleChannelInfo->_pChannelList->GetItem( i32Idx );
	//			i32Idx++;

	//			if( NULL == pBattleInfo ) continue;
	//			if( (pstMember->_battleteam == pBattleInfo->_ui32Idx) && (pstUserInfo->ui32ClanDBIdx == pBattleInfo->_Clan._ClanIdx) ) continue;
	//			i16Count++;

	//			if( CLAN_ROOM_STAY >= pBattleInfo->_Team._State)
	//			{
	//				PacketAckGC.m_PacketCSD.m_WaitingRoomCount++;
	//			}
	//			else
	//			{
	//				PacketAckGC.m_PacketCSD.m_PlayingRoomCount++;
	//			}
	//		}
	//	}
	//}	

	//// 매치 클랜 정보 전송
	//i3NetworkPacket SendPacket( ProtocolID );
	//SendPacket.WriteData(	&PacketAckGC,	sizeof(PACKET_CS_MATCH_GET_CLAN_ROOM_INFO_ACK) );
	//g_pGateway->SendPacket(	pstUserInfo->ui32ServerIdx, &SendPacket );
}

void	CTaskProcessor::BattleMatchAllTeamList( CLAN_USER_INFO* pstUserInfo, UINT32 ui32Channel, UINT16 ui16Section )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK;
	
	BATTLE_CHANNE_INFO* pBattleChannelInfo = m_pBattleTeam->FindChannelTeam( ui32Channel );
	if( NULL == pBattleChannelInfo ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, 0 );

	// 오류
	if( 0 == pBattleChannelInfo->_stChannelContext._ui16Totalcount )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, 0 );

	UINT8 ui8Size = 0;
	UINT16 ui16TotalCount = pBattleChannelInfo->_stChannelContext._ui16Totalcount-1;

	if( 0 != ui16TotalCount )
	{
		if( ui16Section >= pBattleChannelInfo->_stChannelContext._ui16SectionCount )	return;

		ui8Size = BATTLE_ALL_TEAM_SECTION_SIZE;

		if ( ui16Section + 1 == pBattleChannelInfo->_stChannelContext._ui16SectionCount )
		{
			ui8Size = (UINT8)ui16TotalCount % BATTLE_ALL_TEAM_SECTION_SIZE;

			if ( 0 == ui8Size )
			{
				ui8Size = BATTLE_ALL_TEAM_SECTION_SIZE;
			}
		}
	}

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, 0 );

	CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstMember ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, 0 );

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,									sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,											sizeof(T_UID));
	SendPacket.WriteData(&ui16TotalCount,												sizeof(UINT16));
	if( 0 < ui16TotalCount )
	{ 
		SendPacket.WriteData(&pBattleChannelInfo->_stChannelContext._ui16SectionCount,	sizeof(UINT16));
		SendPacket.WriteData(&ui16Section,												sizeof(UINT16));
		SendPacket.WriteData(&ui8Size,													sizeof(UINT8));

		INT32 i32Idx = ui16Section * BATTLE_ALL_TEAM_SECTION_SIZE;

		BATTLE_INFO* pBattleInfo;
		INT32 i32Count = 0;
		while( i32Count < ui8Size )
		{
			if( i32Idx >= pBattleChannelInfo->_pChannelList->GetCount() ) break;
			pBattleInfo = (BATTLE_INFO*)pBattleChannelInfo->_pChannelList->GetItem( i32Idx );
			i32Idx++;

			if( NULL == pBattleInfo ) continue;
			if( (GET_FRIEND_MATCH(pstMember->_state) == pBattleInfo->_ui32Idx) && (pstUserInfo->ui32ClanDBIdx == pBattleInfo->_Clan._ClanIdx) ) continue;
			i32Count++;

			SendPacket.WriteData(&pBattleInfo->_ui32Idx,								sizeof( UINT32 ) );
			SendPacket.WriteData(&pBattleInfo->_Team,									sizeof( CLAN_BATTLE_TEAM ) );
			SendPacket.WriteData(&pBattleInfo->_Clan,									sizeof( CLAN_BATTLE_CLAN ) );
			SendPacket.WriteData(&pBattleInfo->_Member[pBattleInfo->_Team._MasterIdx],	sizeof( CLAN_BATTLE_MEMBER ) );
		}
	}
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::BattleTeamDetailInfo( CLAN_USER_INFO* pstUserInfo, UINT32 ui32TeamIdx )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK;
	INT32 i32Success = EVENT_ERROR_SUCCESS;

	BATTLE_INFO* pBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pBattleInfo ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	// 비어있는 팀
	if( CLAN_ROOM_EMPTY == pBattleInfo->_Team._State )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pBattleInfo->_Clan._ClanIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,		sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,				sizeof(T_UID));
	SendPacket.WriteData(&i32Success,						sizeof(INT32));
	SendPacket.WriteData(pstClan,							sizeof(CLAN_DETAIL_INFO));
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::BattleTeamJoin( CLAN_USER_INFO* pstUserInfo, UINT32 ui32TeamIdx )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_TEAM_JOIN_ACK;
	INT32 i32Success = EVENT_ERROR_SUCCESS; 

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

    // 이미 팀에 등록 되어있는지.
	if( MATCH_TEAM_INVALID_IDX != GET_FRIEND_MATCH(pstUser->_state) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL );

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	// 대기중인 팀인지 체크
	if( CLAN_ROOM_STAY != pstBattleInfo->_Team._State )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL );

	// 같은 클랜인지
 	if( pstUserInfo->ui32ClanDBIdx != pstBattleInfo->_Clan._ClanIdx )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );
	
	// 채널이 같은지
 	//if( ( pstBattleInfo->_Team._Channel % VIEW_CHANNEL_COUNT ) != m_BattleTeamContext[ sServer ][ sChannel ]._cChannel )
	//	EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL );

	// 인원 체크
	if( pstBattleInfo->_Team._PerCount >= pstBattleInfo->_Team._PerMax ) 
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL );
	
	// 맴버 인덱스 체크
	INT32 i32MemberIdx = -1;
	for( UINT8 i = 0 ; i < pstBattleInfo->_Team._PerMax ; i++ )
	{
		if( CLAN_ROOM_EMPTY == pstBattleInfo->_Member[ i ]._State )
		{
			i32MemberIdx = i;
			break;
		}
	}
	// 남는 인원이 없으면
	if( -1 == i32MemberIdx ) 
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL );

	// 성공
	pstBattleInfo->_Member[ i32MemberIdx ]._State	= CLAN_ROOM_STAY;
	pstBattleInfo->_Member[ i32MemberIdx ]._Rank	= pstUser->_rank;
	pstBattleInfo->_Member[ i32MemberIdx ]._i64UID	= pstUser->_i64UID;
	i3String::Copy( pstBattleInfo->_Member[ i32MemberIdx ]._Name, pstUser->_nick, NET_NICK_NAME_SIZE );
	pstBattleInfo->_Team._PerCount++;

	// Member 변경
	pstUser->_state						= SET_FRIEND_MATCH( pstUser->_state, pstBattleInfo->_Team._Naming+1 );
	SendClanMemberStateChange( pstClan, pstUser );

	// 성공여부 전달
	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,								sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,										sizeof(T_UID));
	SendPacket.WriteData(&i32Success,												sizeof(INT32));
	SendPacket.WriteData(&ui32TeamIdx,												sizeof(UINT32));
	SendPacket.WriteData(&pstBattleInfo->_Team,										sizeof(CLAN_BATTLE_TEAM));
	SendPacket.WriteData(&pstBattleInfo->_Clan,										sizeof(CLAN_BATTLE_CLAN));
	SendPacket.WriteData(&pstBattleInfo->_Member[pstBattleInfo->_Team._MasterIdx],	sizeof( CLAN_BATTLE_MEMBER ) );
	SendPacket.WriteData(pstBattleInfo->_Member,									sizeof(CLAN_BATTLE_MEMBER) * pstBattleInfo->_Team._PerMax);

	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);

	//SendMatchTeamChanges(pstUserInfo->ui32ServerIdx, pstBattleInfo);
}

void	CTaskProcessor::BattleTeamJoinNaming( CLAN_USER_INFO* pstUserInfo, UINT8 ui8Naming )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_TEAM_JOIN_ACK;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	BATTLE_CLAN_INFO* pBattleClanInfo = m_pBattleTeam->FindClanTeam( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pBattleClanInfo ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	for( INT32 i = 0 ; i < pBattleClanInfo->_pClanList->GetCount() ; i++ )
	{
		BATTLE_INFO* pstBattleInfo = (BATTLE_INFO*)pBattleClanInfo->_pClanList->GetItem( i );

		if( ui8Naming != pstBattleInfo->_Team._Naming ) continue;

		BattleTeamJoin( pstUserInfo, pstBattleInfo->_ui32Idx );
		return;
	}

	SendPacket( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL );
}

void	CTaskProcessor::BattleTeamLeave( CLAN_USER_INFO* pstUserInfo )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK;
	INT32 i32Success		= EVENT_ERROR_SUCCESS;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 맴버 인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 이미 팀에 등록 되어있는지.
	if( MATCH_TEAM_INVALID_IDX == GET_FRIEND_MATCH(pstUser->_state) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL );

	i32Success = BattleTeamLeaveUser(pstClan, pstUser, pstUserInfo->ui32ServerIdx);

	SendPacket( ProtocolID, pstUserInfo, i32Success );
}

INT32	CTaskProcessor::BattleTeamLeaveUser(CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser, INT32 i32ServerIdx)
{ 
	BATTLE_INFO* pMatchTeam = m_pBattleTeam->FindTeam( GET_FRIEND_MATCH(pstUser->_state) );
	if( NULL == pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_EMPTY_FAIL;

	// 활성화 되어있는 팀이 맞는지
	if( pMatchTeam->_Team._State == CLAN_ROOM_EMPTY )
	{
		g_pLog->WriteLog( L"Error GetTeamIdx 01" );
		return EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_EMPTY_FAIL;
	}

	INT32 i32MemberIdx = -1;
	for( INT32 i = 0 ; i < pMatchTeam->_Team._PerMax ; i++ )
	{
		if( pMatchTeam->_Member[i]._i64UID == pstUser->_i64UID )
		{
			i32MemberIdx = i;
			break;
		}
	}
	// 찾을수 없음
	if( -1 == i32MemberIdx )
	{
		g_pLog->WriteLog( L"Error GetTeamIdx 02" );
		return EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL;
	}

	pMatchTeam->_Member[ i32MemberIdx ]._State	= CLAN_ROOM_EMPTY;
	pMatchTeam->_Member[ i32MemberIdx ]._i64UID	= 0;
	pMatchTeam->_Member[ i32MemberIdx ]._Rank	= 0;
	pMatchTeam->_Member[ i32MemberIdx ]._Name[0]	= '\0';

	pstUser->_state			= CLEAR_FRIEND_MATCH( pstUser->_state );
	SendClanMemberStateChange( pstClan, pstUser );

	pMatchTeam->_Team._PerCount--;
	
	BOOL bReturn = FALSE;
	// 방에대한 처리
	if( pMatchTeam->_Team._PerCount > 0 ) 
	{
		// 방장이었을때 방장 교체	
		if( pMatchTeam->_Team._MasterIdx == i32MemberIdx )
		{
			INT32 i32MasterMemberIdx = -1;
			for( INT32 i = 0 ; i < pMatchTeam->_Team._PerMax ; i++ )
			{
				if( pMatchTeam->_Member[i]._State != CLAN_ROOM_EMPTY )
				{
					i32MasterMemberIdx = i;
					break;
				}
			}

			if( -1 == i32MasterMemberIdx )
			{
				g_pLog->WriteLog( L"BattleLeave Error Master, Clan:%d, Name:%s", pstClan->_ClanDetailInfo.m_ui32CID, pstClan->_ClanDetailInfo.m_name );
				return EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL;
			}
			else
			{
				pMatchTeam->_Team._MasterIdx = (UINT8)i32MasterMemberIdx;
			}
		}
		bReturn = TRUE;
	}

	if( bReturn ) 
	{
		//SendMatchTeamChanges(i32ServerIdx, pMatchTeam);

		if( pMatchTeam->_Team._PerCount != 0 )
		{
			g_pLog->WriteLog( L"[ERROR] pstBattleInfo->_Team._PerCount != 0" );
		}

		return EVENT_ERROR_SUCCESS;
	}
	
	//방에 더이상 인원이 없으면 방 삭제
	pMatchTeam->_Team._State = CLAN_ROOM_EMPTY;

	m_pBattleTeam->DestroyTeam( pMatchTeam );

	return EVENT_ERROR_SUCCESS;
}

void	CTaskProcessor::BattleTeamChangePer( CLAN_USER_INFO* pstUserInfo, UINT8 i8PerMax )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK;
	INT32 i32Success = EVENT_ERROR_SUCCESS;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 맴버 인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 이미 팀에 등록 되어있는지.
	UINT32	ui32TeamIdx = GET_FRIEND_MATCH(pstUser->_state);
	if( MATCH_TEAM_INVALID_IDX == ui32TeamIdx )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	// 비어있는 팀
	if( CLAN_ROOM_EMPTY == pstBattleInfo->_Team._State )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );	

	// 마스터 인지 체크
	if( pstBattleInfo->_Member[ pstBattleInfo->_Team._MasterIdx ]._i64UID != pstUserInfo->i64UID )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_NOMASTER_FAIL );

	// 인원이 이상함
	if( i8PerMax < 4 || i8PerMax > 8 )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_FAIL );

	// 현재 인원수가 더 많아서 줄이기 실패
	if( pstBattleInfo->_Team._PerCount > i8PerMax )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_PERMAX_FAIL );
	
	pstBattleInfo->_Team._PerMax = i8PerMax;

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(&pstUserInfo->i32SessionIdx,		sizeof(INT32));
	SendPacket.WriteData(&pstUserInfo->i64UID,				sizeof(T_UID));
	SendPacket.WriteData(&i32Success,						sizeof(INT32));
	SendPacket.WriteData(&pstBattleInfo->_Team._PerMax,		sizeof(UINT8));
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket);

	// 모두에게 쏴주기	
	for( UINT8 i = 0 ; i < pstBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY )			continue;
		if( pstUserInfo->i64UID == pstBattleInfo->_Member[ i ]._i64UID )	continue;

		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		i3NetworkPacket SendPacket2( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK );
		SendPacket2.WriteData(&pstMember->_session,			sizeof(INT32));
		SendPacket2.WriteData(&pstMember->_i64UID,			sizeof(T_UID));
		SendPacket2.WriteData(&pstBattleInfo->_Team._PerMax,	sizeof(UINT8));
		g_pGateway->SendPacket( GET_FRIEND_SERVER( (UINT32)pstMember->_state ), &SendPacket2 );
	}
}

void CTaskProcessor::BattleClanRequset( CLAN_USER_INFO* pstUserInfo, UINT32 ui32RequsetTeamIdx )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK;

	// 클랜이 존재하는지
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 맴버 인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 이미 팀에 등록 되어있는지.
	UINT32 ui32TeamIdx = GET_FRIEND_MATCH(pstUser->_state);
	if( MATCH_TEAM_INVALID_IDX == ui32TeamIdx )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );
	 
	// 자기 자신에게 신청했는지.
	if( ui32TeamIdx == ui32RequsetTeamIdx )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_SELF_FAIL );

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	BATTLE_INFO* pstReqBattleInfo = m_pBattleTeam->FindTeam( ui32RequsetTeamIdx );
	if( NULL == pstReqBattleInfo )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );
	
	// 내가 대기중인 팀인지
	if( pstBattleInfo->_Team._State != CLAN_ROOM_STAY )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL );

	// 상대가 대기중인 팀인지
	if( pstReqBattleInfo->_Team._State != CLAN_ROOM_STAY )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL );

	// 게임에 참여할 인원이 맞는지	
	if( pstBattleInfo->_Team._PerMax != pstReqBattleInfo->_Team._PerMax )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_NOPER_FAIL );

#ifndef CLAN_MATCH_SOLO_PLAY
	// 게임에 참여할 인원이 맞는지
	if( pstBattleInfo->_Team._PerMax != pstBattleInfo->_Team._PerCount )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_NOPER_FAIL );

	if( pstReqBattleInfo->_Team._PerMax != pstReqBattleInfo->_Team._PerCount )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_NOPER_FAIL );
#endif	

	// 상대방 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstRequestClan = FindClan2( pstReqBattleInfo->_Clan._ClanIdx );
	if( NULL == pstRequestClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	
	UINT8	ui8UserCount = 0;
	INT32	ai32SessionIdx[ CLAN_TEAM_MEMBER_COUNT ];
	T_UID	ai64UID[ CLAN_TEAM_MEMBER_COUNT ];
	UINT32	ui32ServerIdx = 0;
	// 모두 클랜 원인지, 체크하면서 sess, DB 넣기
	for( INT32 i = 0 ; i < pstReqBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstReqBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY ) continue;

		// 클랜 맴버 인지
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstRequestClan->_pMember->FindItemEx( pstReqBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		ui32ServerIdx					= GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		ai32SessionIdx[ ui8UserCount ]	= pstMember->_session;
		ai64UID[ ui8UserCount ]			= pstMember->_i64UID;

		ui8UserCount++;
	}

	i3NetworkPacket NewPacket( PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK );
	NewPacket.WriteData(&ui8UserCount,											sizeof(UINT8));
	NewPacket.WriteData(ai32SessionIdx,											sizeof(INT32) * ui8UserCount );
	NewPacket.WriteData(ai64UID,												sizeof(T_UID) * ui8UserCount );
	NewPacket.WriteData(&ui32TeamIdx,											sizeof(UINT32));
	NewPacket.WriteData(&pstBattleInfo->_Team,									sizeof(CLAN_BATTLE_TEAM));
	NewPacket.WriteData(&pstBattleInfo->_Clan,									sizeof(CLAN_BATTLE_CLAN));
	NewPacket.WriteData(&pstBattleInfo->_Member[pstBattleInfo->_Team._MasterIdx],	sizeof(CLAN_BATTLE_MEMBER));
	g_pGateway->SendPacket( ui32ServerIdx, &NewPacket);	

	// 전투 신청 성공
	SendPacket( ProtocolID, pstUserInfo, EVENT_ERROR_SUCCESS );
}

void	CTaskProcessor::BattleClanAccept( CLAN_USER_INFO* pstUserInfo, UINT32 ui32SpIdx, UINT32 ui32TeamIdx, UINT8 ui8Accept )
{
	// sTeamIdx;			<-- 신청한팀
	// sRequsetTeamIdx;		<-- 신청 당한 팀 == pstUserInfo

	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	// 클랜 맴버 인지
	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	// 이미 팀에 등록 되어있는지.
	if( MATCH_TEAM_INVALID_IDX == GET_FRIEND_MATCH(pstUser->_state) )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );

	UINT32 ui32RequsetTeamIdx = GET_FRIEND_MATCH(pstUser->_state);

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	BATTLE_INFO* pstReqBattleInfo = m_pBattleTeam->FindTeam( ui32RequsetTeamIdx );
	if( NULL == pstReqBattleInfo )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	if( CLAN_ROOM_STAY != pstBattleInfo->_Team._State )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL );

	if( CLAN_ROOM_STAY != pstReqBattleInfo->_Team._State )
		EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL );

	//if( pTeamInfo->_Team._SpIdx != wSpIdx )
	//	EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL );

	// 상대편 팀장의 정보
	T_UID i64UID = pstBattleInfo->_Member[ pstBattleInfo->_Team._MasterIdx ]._i64UID;

	pstClan = FindClan2( pstBattleInfo->_Clan._ClanIdx );
	if( NULL == pstClan ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );

	pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( i64UID );
	if( NULL == pstUser ) EV_FAILED_CLAN( ProtocolID, pstUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX );
	
	UINT32	ui32ServerIdx	= GET_FRIEND_SERVER( (UINT32)pstUser->_state );
	INT32	i32SessionIdx	= pstUser->_session;

	// 수락버튼누른사람에게 확인 메시지 보냄
	SendPacket( ProtocolID, pstUserInfo, EVENT_ERROR_SUCCESS );

	if( ui8Accept == 0 )
	{	// 배틀 신청한 유저에게 거절 메시지
		SendPacket( PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK, ui32ServerIdx, i32SessionIdx, i64UID, EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_REFUSAL_FAIL );
		return;
	}

	// 게임 서버로 보내는 방 만들기 메시지
	pstBattleInfo->_Team._State			= CLAN_ROOM_READY;
	pstReqBattleInfo->_Team._State		= CLAN_ROOM_READY;

	// 수락시에 방장에게 바로 방을 만들라고 요청
	//ROOM_INFO_BASIC		RoomInfoBasic;
	//RoomInfoBasic.Reset();

	//i3String::Copy( RoomInfoBasic._Title, "Clan Battle");

	//RoomInfoBasic._PersonMax	= pstBattleInfo->_Team._PerMax * 2;
	//RoomInfoBasic._StageID		= MAKE_STAGEID( STAGE_MODE_BOMB, STAGE_SLOT_MODE_NORMAL, STAGE_ORD_DOWNTOWN );	
	//RoomInfoBasic._StageMask	= SET_STAGEMASK_CLANMATCH( RoomInfoBasic._StageMask, 1 );
	//RoomInfoBasic._InfoFlag		= ROOM_INFO_FLAG_INTER_ENTER | ROOM_INFO_FLAG_OBSERVER;
	//RoomInfoBasic._WeaponFlag	= 0x0F;

	UINT8 ui8PerMax = pstBattleInfo->_Team._PerMax * 2;

	i3NetworkPacket NewPacket( PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ );
	NewPacket.WriteData( &i64UID,						sizeof(T_UID));
	NewPacket.WriteData( &ui32TeamIdx,					sizeof(UINT32));
	NewPacket.WriteData( &ui32RequsetTeamIdx,			sizeof(UINT32));
	NewPacket.WriteData( &ui8PerMax,					sizeof(UINT8));

	//NewPacket.WriteData(&RoomInfoBasic,				sizeof(ROOM_INFO_BASIC));
	NewPacket.WriteData(&pstReqBattleInfo->_Team,		sizeof(CLAN_BATTLE_TEAM));
	NewPacket.WriteData(&pstReqBattleInfo->_Clan,		sizeof(CLAN_BATTLE_CLAN));
	g_pGateway->SendPacket( ui32ServerIdx, &NewPacket);
}

void	CTaskProcessor::BattleClanRoomJoin( CLAN_USER_INFO* pstUserInfo, UINT32 ui32TeamIdx, UINT32 ui32RequsetTeamIdx, UINT32 ui32RoomIdx )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ;

	// 신청 요청한 팀 방으로 입장
	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo )		return;

	BATTLE_INFO* pstReqBattleInfo = m_pBattleTeam->FindTeam( ui32RequsetTeamIdx );
	if( NULL == pstReqBattleInfo )	return;

	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstBattleInfo->_Clan._ClanIdx );
	if( NULL == pstClan )			return;  

	CLAN_DETAIL_INFO_SERVER* pstReqClan = FindClan2( pstReqBattleInfo->_Clan._ClanIdx );
	if( NULL == pstReqClan )		return;

	UINT8	ui8UserCount = 0;
	INT32	ai32SessionIdx[ CLAN_TEAM_MEMBER_COUNT ];
	T_UID	ai64UID[ CLAN_TEAM_MEMBER_COUNT ];
	UINT32	ui32ServerIdx = 0;
	CLAN_MEMBER_DETAIL_INFO* pstMember;

	// 한개의 팀 보내기
	for( UINT8 i = 0 ; i < pstBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY )	continue;
		pstBattleInfo->_Member[ i ]._State = CLAN_ROOM_READY;

		if( pstBattleInfo->_Team._MasterIdx == i )					continue;

		// 클랜 맴버 인지
		pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		ui32ServerIdx					= GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		ai32SessionIdx[ ui8UserCount ]	= pstMember->_session;
		ai64UID[ ui8UserCount ]			= pstMember->_i64UID;

		ui8UserCount++;
	}
	i3NetworkPacket NewPacket( ProtocolID );
	NewPacket.WriteData(&ui8UserCount,						sizeof(UINT8));
	NewPacket.WriteData(ai32SessionIdx,						sizeof(INT32) * ui8UserCount);
	NewPacket.WriteData(ai64UID,							sizeof(T_UID) * ui8UserCount);
	NewPacket.WriteData(&ui32RoomIdx,						sizeof(UINT32));
	NewPacket.WriteData(&ui32TeamIdx,						sizeof(UINT32));
	NewPacket.WriteData(&pstReqBattleInfo->_Team,			sizeof(CLAN_BATTLE_TEAM));
	NewPacket.WriteData(&pstReqBattleInfo->_Clan,			sizeof(CLAN_BATTLE_CLAN));
	g_pGateway->SendPacket( ui32ServerIdx, &NewPacket);

	// 신청 요청당한 팀 방으로 입장 

	ui8UserCount = 0;
	for( UINT8 i = 0 ; i < pstReqBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstReqBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY )	continue;
		pstReqBattleInfo->_Member[ i ]._State = CLAN_ROOM_READY;

		// 클랜 맴버 인지
		pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstReqClan->_pMember->FindItemEx( pstReqBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		ui32ServerIdx					= GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		ai32SessionIdx[ ui8UserCount ]	= pstMember->_session;
		ai64UID[ ui8UserCount ]			= pstMember->_i64UID;

		ui8UserCount++;
	}
	i3NetworkPacket NewPacket2( ProtocolID );
	NewPacket2.WriteData(&ui8UserCount,						sizeof(UINT8));
	NewPacket2.WriteData(ai32SessionIdx,					sizeof(INT32) * ui8UserCount);
	NewPacket2.WriteData(ai64UID,							sizeof(T_UID) * ui8UserCount);
	NewPacket2.WriteData(&ui32RoomIdx,						sizeof(UINT32));
	NewPacket2.WriteData(&ui32RequsetTeamIdx,				sizeof(UINT32));
	NewPacket2.WriteData(&pstBattleInfo->_Team,				sizeof(CLAN_BATTLE_TEAM));
	NewPacket2.WriteData(&pstBattleInfo->_Clan,				sizeof(CLAN_BATTLE_CLAN));
	g_pGateway->SendPacket( ui32ServerIdx, &NewPacket2);
}

void	CTaskProcessor::BattleClanRoomLeave( CLAN_USER_INFO* pstUserInfo )
{
	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	UINT32 ui32TeamIdx = GET_FRIEND_MATCH(pstUser->_state);

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo ) return;

	INT32 i32PerCount = 0;
	for( UINT8 i = 0 ; i < pstBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstBattleInfo->_Member[ i ]._State <= CLAN_ROOM_STAY )
		{ 
			i32PerCount++;
			continue;
		}

		if( pstBattleInfo->_Member[ i ]._i64UID != pstUserInfo->i64UID )	continue;

		pstBattleInfo->_Member[ i ]._State = CLAN_ROOM_STAY;
		i32PerCount++;
	}

	if( CLAN_ROOM_READY == pstBattleInfo->_Team._State && pstBattleInfo->_Team._PerMax == i32PerCount )
	{
		pstBattleInfo->_Team._State = CLAN_ROOM_STAY;
	}

	//SendMatchTeamChanges(pstUserInfo->ui32ServerIdx, pstBattleInfo);
}

void	CTaskProcessor::MatchUserChatting( CLAN_USER_INFO* pstUserInfo, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_CHATING_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pstUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstUserInfo->i64UID );
	if( NULL == pstUser ) return;

	UINT32 ui32TeamIdx = GET_FRIEND_MATCH(pstUser->_state);

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo ) return;

	// 성공
	UINT8 ui8NickSize = (UINT8)i3String::Length( pstUser->_nick )+1;

	UINT8	ui8UserCount = 0;
	INT32	ai32SessionIdx[ CLAN_TEAM_MEMBER_COUNT ];
	T_UID	ai64UID[ CLAN_TEAM_MEMBER_COUNT ];

	CLAN_MEMBER_DETAIL_INFO* pstMember;
	for( UINT8 i = 0 ; i < pstBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY ) continue;

		pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		ai32SessionIdx[ ui8UserCount ]	= pstMember->_session;
		ai64UID[ ui8UserCount ]			= pstMember->_i64UID;

		ui8UserCount++;
	}

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData( &ui8UserCount,		sizeof(UINT8) );
	SendPacket.WriteData( ai32SessionIdx,		sizeof(INT32) * ui8UserCount);
	SendPacket.WriteData( ai64UID,				sizeof(T_UID) * ui8UserCount);
	SendPacket.WriteData( &ui8NickSize,			sizeof(UINT8) );
	SendPacket.WriteData( pstUser->_nick,		sizeof(TTCHAR) * ui8NickSize );
	SendPacket.WriteData( &bIsGMUser,			sizeof(bool) );
	SendPacket.WriteData( &ui8NickColor,		sizeof(UINT8) );
	SendPacket.WriteData( &ui8ChatSize,			sizeof(UINT8) );
	SendPacket.WriteData( pstrChating,			sizeof(TTCHAR) * ui8ChatSize );
	g_pGateway->SendPacket( pstUserInfo->ui32ServerIdx, &SendPacket );
}

void	CTaskProcessor::BattleClanMatchClanInfo( CLAN_USER_INFO* pUserInfo, PACKET_GC_D_CS_MATCH_NEW_CLAN_INFO_REQ* pPacketReqGCD )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK;

	// Red Team 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstRedClan = FindClan2( pPacketReqGCD->m_ui32CUID[TEAM_RED] );
	if( NULL == pstRedClan ) return;
	// Red Team 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstBlueClan = FindClan2( pPacketReqGCD->m_ui32CUID[TEAM_BLUE] );
	if( NULL == pstBlueClan ) return;

	PACKET_GC_H_CLAN_ACK					PacketAckGCH;
	PACKET_GC_D_CS_MATCH_CLAN_INFO			PacketAckGCD = {0,};

	PacketAckGCH.m_i32SessionIdx			= pUserInfo->i32SessionIdx;
	PacketAckGCH.m_i64UID					= pUserInfo->i64UID;
	PacketAckGCH.m_i32Result				= EVENT_ERROR_SUCCESS;

	PacketAckGCD.m_ui32ChannelNum			= pPacketReqGCD->m_ui32ChannelNum;
	PacketAckGCD.m_ui32RoomIdx				= pPacketReqGCD->m_ui32RoomIdx;

	//pstRedClan->SetClanMatchInfo( &PacketAckGCD.m_ClanMatchInfo[TEAM_RED] );
	//pstBlueClan->SetClanMatchInfo( &PacketAckGCD.m_ClanMatchInfo[TEAM_BLUE] );

	// 매치 클랜 정보 전송
	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData(	&PacketAckGCH,	sizeof(PACKET_GC_H_CLAN_ACK) );
	SendPacket.WriteData(	&PacketAckGCD,	sizeof(PACKET_GC_D_CS_MATCH_CLAN_INFO) );
	g_pGateway->SendPacket(	pUserInfo->ui32ServerIdx, &SendPacket );
}

void	CTaskProcessor::BattleClanOperation( CLAN_USER_INFO* pUserInfo, PACKET_GC_D_CS_MATCH_NEW_OPERATION* pPacketReqGCD )
{
	PROTOCOL ProtocolID = PROTOCOL_CS_MATCH_NEW_OPERATION_ACK;

	// 클랜이 존재하는지
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pUserInfo->ui32ClanDBIdx );
	if( NULL == pstClan ) return;

	CLAN_MEMBER_DETAIL_INFO* pstUser = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pUserInfo->i64UID );
	if( NULL == pstUser ) return;

	// 팀에 등록 되어있는지 않으면
	if( MATCH_TEAM_INVALID_IDX == GET_FRIEND_MATCH(pstUser->_state) )
	{	
		EV_FAILED_CLAN( ProtocolID, pUserInfo, EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL );
	}

	UINT32 ui32TeamIdx = GET_FRIEND_MATCH(pstUser->_state);

	BATTLE_INFO* pstBattleInfo = m_pBattleTeam->FindTeam( ui32TeamIdx );
	if( NULL == pstBattleInfo ) return;

	// 작전명 변경자가 리더인가?
	if( pUserInfo->i64UID != pstBattleInfo->_Member[pstBattleInfo->_Team._MasterIdx]._i64UID )
	{		
		EV_FAILED_CLAN( ProtocolID, pUserInfo, EVENT_ERROR_NO_CLAN_TEAM );
	}

	i3String::Copy(pstBattleInfo->_Team.m_strOperation, pPacketReqGCD->m_strOperation, NET_CLAN_OPERATION_SIZE);
	pstBattleInfo->_Team.m_strOperation[NET_CLAN_OPERATION_SIZE-1] = '\0';

	// 성공
	PACKET_GC_CS_MATCH_OPERATION_ACK		PacketAckGC;
	PacketAckGC.m_ui8UserCount					= 0;
	i3String::Copy(PacketAckGC.m_strOperation, pPacketReqGCD->m_strOperation, NET_CLAN_OPERATION_SIZE);
	PacketAckGC.m_strOperation[NET_CLAN_OPERATION_SIZE-1] = '\0';

	CLAN_MEMBER_DETAIL_INFO* pstMember;
	for( UINT8 i = 0 ; i < pstBattleInfo->_Team._PerMax ; i++ )
	{
		if( pstBattleInfo->_Member[ i ]._State == CLAN_ROOM_EMPTY ) continue;			

		pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pstBattleInfo->_Member[ i ]._i64UID );
		if( NULL == pstMember ) continue;

		PacketAckGC.m_i32SessionIdx[ PacketAckGC.m_ui8UserCount ]	= pstMember->_session;
		PacketAckGC.m_i64UID[ PacketAckGC.m_ui8UserCount ]			= pstMember->_i64UID;

		PacketAckGC.m_ui8UserCount++;
	}

	i3NetworkPacket SendPacket( ProtocolID );
	SendPacket.WriteData( &PacketAckGC, sizeof(PACKET_GC_CS_MATCH_OPERATION_ACK) );
	g_pGateway->SendPacket(	pUserInfo->ui32ServerIdx, &SendPacket );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLAN CS

// -----------------------------------------------------------
// Name : ClanRequestAutoAcceptOption
// Desc : 클랜 가입신청서 (자동,수동) 승인 설정 요청(클랜마스터만 가능)
// -----------------------------------------------------------
void	CTaskProcessor::ClanRequestAutoAcceptOption( UINT32 ui32ServerIdx, PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ* pPacketReqGC )
{
	//-------------------------------------------------------------------------------------------------	
	RING_IN_DB	stRingInDB;
	stRingInDB._ui8Type			= RING_DB_REQUEST_AUTO_OPTION; 
	stRingInDB._ui32ServerIdx	= ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pPacketReqGC->m_i32SessionIdx;
	stRingInDB._i64UID			= pPacketReqGC->m_i64UID;
	stRingInDB._ui32ClanDBIdx	= pPacketReqGC->m_ui32CUID;
	// 클랜 찾기
	CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( stRingInDB._ui32ClanDBIdx );
	if( NULL == pstClan ) 
	{
		CLAN_USER_INFO stUserInfo;
		stUserInfo.ui32ServerIdx	= ui32ServerIdx;
		stUserInfo.i32SessionIdx	= pPacketReqGC->m_i32SessionIdx;
		stUserInfo.i64UID			= pPacketReqGC->m_i64UID;
		stUserInfo.ui32ClanDBIdx	= pPacketReqGC->m_ui32CUID;
		EV_FAILED_CLAN( PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK, &stUserInfo, EVENT_ERROR_CLAN_NOSEARCH_CLANIDX );
	}

	// 조건 검사
	if(MAX_CLAN_AUTO_OPTION > pPacketReqGC->m_ui32AutoAcceptOption)
	{
		stRingInDB.Write( &pPacketReqGC->m_ui32AutoAcceptOption,	sizeof(UINT32) );
		
		if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorking( pPacketReqGC->m_ui32CUID, &stRingInDB ) )
		{
			g_pLog->WriteLog( L"ClanMatchResultEmotionSave(EVENT_ERROR_EVENT_BUFFER_FULL)" );

			CLAN_USER_INFO stUserInfo;
			stUserInfo.ui32ServerIdx	= ui32ServerIdx;
			stUserInfo.i32SessionIdx	= pPacketReqGC->m_i32SessionIdx;
			stUserInfo.i64UID			= pPacketReqGC->m_i64UID;
			stUserInfo.ui32ClanDBIdx	= pPacketReqGC->m_ui32CUID;
			EV_FAILED_CLAN( PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK, &stUserInfo, EVENT_ERROR_EVENT_BUFFER_FULL );			
		}
		return;
	}

	// 해킹(버그)일 가능성 있음.
	CLAN_USER_INFO stUserInfo;
	stUserInfo.ui32ServerIdx	= ui32ServerIdx;
	stUserInfo.i32SessionIdx	= pPacketReqGC->m_i32SessionIdx;
	stUserInfo.i64UID			= pPacketReqGC->m_i64UID;
	stUserInfo.ui32ClanDBIdx	= pPacketReqGC->m_ui32CUID;
	EV_FAILED_CLAN( PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK, &stUserInfo, EVENT_ERROR_GAME_WRONG_DATA );		
}

void	CTaskProcessor::SendClanRequestAutoAcceptOption( UINT32 ui32ServerIdx, PACKET_GC_H_CLAN_ACK* pPacketAckGCH )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK );
	SendPacket.WriteData(pPacketAckGCH,	sizeof(PACKET_GC_H_CLAN_ACK));
	g_pGateway->SendPacket( ui32ServerIdx, &SendPacket);
}

void	CTaskProcessor::ClanFindUserInfo( CLAN_USER_INFO* pstUserInfo, T_UID i64FindUID )
{
	//-------------------------------------------------------------------------------------------------
	RING_IN_DB	stRingInDB;
	stRingInDB._ui8Type			= RING_DB_CLAN_FIND_USER_INFO; 
	stRingInDB._ui32ServerIdx	= pstUserInfo->ui32ServerIdx;
	stRingInDB._i32SessionIdx	= pstUserInfo->i32SessionIdx;
	stRingInDB._i64UID			= pstUserInfo->i64UID;	
	stRingInDB.Write( &i64FindUID, sizeof(T_UID) );
	
	if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorkingUID( i64FindUID, &stRingInDB ) )
	{
		PACKET_GC_NEW_CLAN_USER_INFO_ACK	PacketAckGC = {0,};
		PacketAckGC.m_i32SessionIdx			= pstUserInfo->i32SessionIdx;
		PacketAckGC.m_i64UID				= pstUserInfo->i64UID;
		PacketAckGC.m_i64FindUID			= i64FindUID;

		SendClanFindUserInfo( stRingInDB._ui32ServerIdx, &PacketAckGC );
		g_pLog->WriteLog( L"ClanFindUserInfo(EVENT_ERROR_EVENT_BUFFER_FULL)" );
	}
}

void	CTaskProcessor::SendClanFindUserInfo( UINT32 ui32ServerIdx, PACKET_GC_NEW_CLAN_USER_INFO_ACK* pPacketAckGC )
{
	i3NetworkPacket SendPacket( PROTOCOL_GC_CLAN_USER_INFO_ACK );
	SendPacket.WriteData(pPacketAckGC,	sizeof(PACKET_GC_NEW_CLAN_USER_INFO_ACK));
	g_pGateway->SendPacket( ui32ServerIdx, &SendPacket);
}

// -----------------------------------------------------------
// Name : SetMatchResultLastCount
// Desc : 클랜전 매치 정보 저장 마지막 카운터 위치 저장
// -----------------------------------------------------------
void CTaskProcessor::SetMatchResultLastCount(MATCH_RESULT_CONTEXT_SERVER* pClanMatchResultContext)
{
	if( pClanMatchResultContext->_ui8LastIdx == MAX_MATCH_RESULT_COUNT )
	{
		pClanMatchResultContext->_ui8LastIdx = 0;
	}
	else
	{ 
		pClanMatchResultContext->_ui8LastIdx++; 
		if( pClanMatchResultContext->_totalCount < MAX_MATCH_RESULT_COUNT )
			pClanMatchResultContext->_totalCount++;
	}
}

bool CTaskProcessor::SendMatchingResult(INT32 i32ServerIdx, MATCHING_RESULT* result)
{
	if( !result ) return false;

	T_MatchingID	TMatchingID = MATCHING_ID_INVALID_IDX;
	CMatching* pMatching = g_pTaskProcessor->GetEmptyMatchBuffer(i32ServerIdx, TMatchingID);
	if( !pMatching ) return true;

	CTeamManager *pTeamManager = m_pTeamManager[i32ServerIdx];
	if( !pTeamManager )  return true;

	if( EV_FAILED(result->m_TResult) )
	{	// 실패처리.
		result->m_ui8TeamCount = MIN(result->m_ui8TeamCount, TEAM_COUNT);
		for( INT32 i = 0 ; i < result->m_ui8TeamCount; ++i )
		{
			i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_MATCHMAKING_ACK);
			if( result->m_TResult == EVENT_ERROR_CLAN_WAR_CANCEL_MATCHMAKING )
			{
				SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK);
				result->m_TResult = EVENT_ERROR_SUCCESS;
			}
			SendPacket.WriteData(&result->m_TResult,					sizeof(T_RESULT)); 
			pTeamManager->MakePacketTeamUIDList(result->m_TTeamID[i], &SendPacket );
			g_pGateway->SendPacket(i32ServerIdx, &SendPacket);
		}
		
		return true;
	}

	// 팀 해체시 실패처리.
	CMatchTeam* pMatchTeams[TEAM_COUNT];
	for( INT32 i = 0 ; i < TEAM_COUNT; ++i )
	{
		pMatchTeams[i] = pTeamManager->FindMatchTeam(result->m_TTeamID[i]);
	}

	// 매치팀 해체시 실패처리.
	if( !pMatchTeams[TEAM_RED] ) 
	{	
		if( pMatchTeams[TEAM_BLUE] )
		{
			InsertMatchmakingRequest( pTeamManager, i32ServerIdx, result->m_TTeamID[TEAM_BLUE]);
		}
		return true;
	}
	if( !pMatchTeams[TEAM_BLUE] ) 
	{	  
		InsertMatchmakingRequest( pTeamManager, i32ServerIdx, result->m_TTeamID[TEAM_RED]); 
		return true;
	}

	// 매칭 성공 
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
	{
		pMatching->SetMatchTeam(static_cast<TEAM_TYPE>(i), pMatchTeams[i]);
		pMatchTeams[i]->SetTeamState(TEAM_STATE_MATCHED);
		pMatchTeams[i]->SetMatchedTeam(static_cast<TEAM_TYPE>(i) == TEAM_RED ? result->m_TTeamID[TEAM_BLUE] : result->m_TTeamID[TEAM_RED]);
	}

	pMatching->SetMaxPlayer(pMatchTeams[TEAM_RED]->GetMaxMemberCount());
	pMatching->SetState(MATCH_MATCHED);
	pMatching->SetMatchingID(TMatchingID);

	return pMatching->SendCreateRoom(i32ServerIdx);
}


T_RESULT CTaskProcessor::CreateClanMatchTeam(CTeamManager* pTeamManager, CLAN_USER_INFO* pUser, UINT8 ui8MaxPlayer, INT32 i32KillDeathRate, T_TeamID& TTeamID)
{
	CLAN_DETAIL_INFO_SERVER* pClanDetail = FindClan2(pUser->ui32ClanDBIdx);
	if( !pClanDetail )
		return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	CLAN_MEMBER_DETAIL_INFO* pClanMember = static_cast<CLAN_MEMBER_DETAIL_INFO*>( pClanDetail->_pMember->FindItemEx(pUser->i64UID) );
	if( !pClanMember )
		return EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX;

	/*if( GET_FRIEND_MATCH(pClanMember->_state) != MATCH_TEAM_INVALID_IDX )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL;*/

	if( MAX_CLAN_MATCH_TEAM_COUNT <= pClanDetail->GetMatchTeamCount() )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_FULL_FAIL;

	CMatchTeam* pMatchTeam = pTeamManager->CreateMatchTeam(pClanDetail, ui8MaxPlayer, pClanMember, i32KillDeathRate, pUser->ui32ServerIdx);
	if( !pMatchTeam )
		return EVENT_ERROR_EVENT_BUFFER_FULL;

	TTeamID = pMatchTeam->GetTeamID(); 
	pClanMember->_state			= g_pConfig->MakeStateMatch( pClanMember->_state, TTeamID );
	pMatchTeam->SetClanTamIdx(  pClanDetail->RegisterMatchTeam( pMatchTeam ) );
	SendClanMemberStateChange(pClanDetail, pClanMember);

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CTaskProcessor::JoinClanMatchTeam(CTeamManager* pTeamManager, CLAN_USER_INFO* pReqUser, T_TeamID TTeamID,  INT32 i32KillDeathRate, INT8& i8JoinSlot)
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	return pMatchTeam->JoinMember( pReqUser->i64UID, i32KillDeathRate, i8JoinSlot );
}

INT32 CTaskProcessor::LeaveClanMatchTeam(CTeamManager* pTeamManager, CLAN_USER_INFO *pUserInfo , T_TeamID TTeamID, INT8& i8LeaveSlot )
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam ) return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	T_UID TUID = pUserInfo->i64UID;

	i8LeaveSlot = (INT8)pMatchTeam->GetSlotIdx( TUID ); 
	if( i8LeaveSlot < 0 )	return EVENT_ERROR_WRONG_SLOT_IDX;
	
	return  pMatchTeam->LeaveMember( pUserInfo->ui32ServerIdx, TUID, i8LeaveSlot, pUserInfo->ui32ClanDBIdx );
}

T_RESULT CTaskProcessor::ChangeOperationClanMatchTeam(CTeamManager* pTeamManager, T_UID TUID, T_TeamID TTeamID, TTCHAR* pOperation)
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	return pMatchTeam->ChangeOperation( TUID, pOperation);
}

T_RESULT CTaskProcessor::ChangeMaxPerCountClanMatchTeam(CTeamManager* pTeamManager, T_UID TUID, T_TeamID TTeamID, UINT8 ui8MaxPerCount )
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	return pMatchTeam->ChangeMaxPerCount( TUID, ui8MaxPerCount);
}


T_RESULT CTaskProcessor::InviteMercenary(CMercenaryManager* pMercenaryManager, CLAN_USER_INFO* pUserInfo, T_MerID TMerID, T_TeamID TTeamID)
{
	CLAN_DETAIL_INFO_SERVER* pInviterClan = FindClan2(pUserInfo->ui32ClanDBIdx);
	if( !pInviterClan )
		return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	CLAN_MEMBER_DETAIL_INFO* pInviter = static_cast<CLAN_MEMBER_DETAIL_INFO*>( pInviterClan->_pMember->FindItemEx(pUserInfo->i64UID) );
	if( !pInviter )
		return EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX;

	CMatchTeam* pMatchTeam = m_pTeamManager[pUserInfo->ui32ServerIdx]->FindMatchTeam(TTeamID);
	if( !pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	if( pMatchTeam->GetLeaderUID() != pUserInfo->i64UID )
		return EVENT_ERROR_CLAN_WAR_NOT_LEADER;

	if( pMatchTeam->GetMercenaryCount() + 1 > MAX_MERCENARY_IN_TEAM )
		return EVENT_ERROR_EXCEED_MERCENARY_LIMIT;

	return pMercenaryManager->InviteMercenary(pUserInfo->ui32ServerIdx, TMerID, pMatchTeam);
}

T_RESULT CTaskProcessor::InsertMatchmakingRequest(CTeamManager* pTeamManager, INT32 i32ServerIdx, T_TeamID TTeamID)
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	if( pMatchTeam->GetTeamState() != TEAM_STATE_WAIT )
		return EVENT_ERROR_CANT_REQUEST_MATCHMAKING;

	if( !pMatchTeam->IsFilledAllMember() )
	{
#if defined( I3_DEBUG ) && defined(CLAN_MATCH_MAKING)
		// 강제로 4vs4방으로 설정을 바꿉니다.
		pMatchTeam->SetMaxMember( 4 );
#else
		return EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_NOPER_FAIL;
#endif
	}

	MATCHING_REQUEST stMatchingRequest;
	pMatchTeam->MakeMatchingRequst(stMatchingRequest);
	stMatchingRequest.m_dt32MatchLimitTime = i3ThreadTimer::LocalTime();
	stMatchingRequest.m_dt32MatchLimitTime.AddTime( I3DATE_ADD_TYPE_SECOND, MATCHING_LIMIT_TIME );
	
	T_RESULT TResult = g_pMatchingManager->InsertMatchRequestBuffer(i32ServerIdx, stMatchingRequest);
	if( EV_SUCCESSED(TResult) )
	{
		pMatchTeam->SetTeamState(TEAM_STATE_MATCHING);
	}

	return TResult;
}

T_RESULT CTaskProcessor::CancelMatchmaking(CTeamManager* pTeamManager, T_TeamID TTeamID)
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	if( pMatchTeam->GetTeamState() != TEAM_STATE_MATCHING )
		return EVENT_ERROR_NOT_MATCHMAKING;

	pMatchTeam->SetTeamState(TEAM_STATE_CANCEL_MATCHING);
	return EVENT_ERROR_SUCCESS;
}

void CTaskProcessor::SendInviteAccept( T_RESULT TResult, CLAN_USER_INFO *pClanUser )
{
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK);
	SendPacket.WriteData(&pClanUser->i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&pClanUser->i64UID,		sizeof(T_UID)); 
	SendPacket.WriteData(&TResult,					sizeof(T_RESULT)); 

	g_pGateway->SendPacket(pClanUser->ui32ServerIdx, &SendPacket);

	return;
}

void CTaskProcessor::SendInviteDenial( T_RESULT TResult, CLAN_USER_INFO *pClanUser, T_MerID TMerID, T_UID TLeaderUID )
{
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK);
	SendPacket.WriteData(&pClanUser->i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&pClanUser->i64UID,		sizeof(T_UID)); 
	SendPacket.WriteData(&TResult,					sizeof(T_RESULT)); 
	SendPacket.WriteData(&TLeaderUID,				sizeof(T_UID)); 
	SendPacket.WriteData(&TMerID,					sizeof(T_MerID)); 

	g_pGateway->SendPacket(pClanUser->ui32ServerIdx, &SendPacket);

	return;
}

void CTaskProcessor::ClanWarLogout( CLAN_WAR_INFO* pWarInfo  )
{
	// 1. 매치팀에서 나갑니다.
	if( pWarInfo->m_TTeamID != MATCH_TEAM_INVALID_IDX ) 
	{
		CTeamManager*	pTeamManager = m_pTeamManager[pWarInfo->ui32ServerIdx];

		INT8	i8LeaveSlot;
		CLAN_USER_INFO stUserInfo;
		i3mem::Copy( &stUserInfo, pWarInfo, sizeof( CLAN_USER_INFO ) );

		T_RESULT TLeaveResult = LeaveClanMatchTeam(pTeamManager, &stUserInfo , pWarInfo->m_TTeamID, i8LeaveSlot); 

		SendLeaveMatchTeam( pTeamManager, pWarInfo, i8LeaveSlot, TLeaveResult); 
	}

	// 2. 용병 해제 시켜줍니다.
	if( MERCENARY_INVALID_IDX != pWarInfo->m_TMerID )
	{
		CMercenaryManager* pMercenaryManager	= m_pMercenaryMgr[pWarInfo->ui32ServerIdx];
		CMercenary *pMercenary = pMercenaryManager->FindMercenary( pWarInfo->m_TMerID );
		if( pMercenary ) 
		{
			T_RESULT TResult = EVENT_ERROR_SUCCESS;
			T_MerID TMerID = pMercenary->GetMerID();

			i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK);
			stPacket.WriteData(&pWarInfo->i32SessionIdx,	sizeof(INT32));
			stPacket.WriteData(&pWarInfo->i64UID,			sizeof(T_UID));

			if( !pMercenaryManager->RemoveMercenary( TMerID ) )
			{
				TResult = EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY;
				stPacket.WriteData(&TResult, sizeof(T_RESULT));
			}
			else
			{
				stPacket.WriteData(&TResult,	sizeof(T_RESULT));
				stPacket.WriteData(&TMerID,		sizeof(T_MerID));
			}
			g_pGateway->SendPacket(pWarInfo->ui32ServerIdx, &stPacket);
		}
	}

}
 
void CTaskProcessor::ClanWarChangeMemberInfo( CHANGE_INFO	eType , CLAN_WAR_INFO* pWarInfo, MATCH_MEMBER_INFO* pMemberInfo )
{
	pMemberInfo->m_TUID = pWarInfo->i64UID;

	// 1. 매치팀원 정보 변경
	if( MATCH_TEAM_INVALID_IDX != pWarInfo->m_TTeamID ) 
	{
		CMatchTeam* pMatchTeam = m_pTeamManager[pWarInfo->ui32ServerIdx]->FindMatchTeam(pWarInfo->m_TTeamID);
		if( pMatchTeam )
		{
			pMatchTeam->ChangeMemberInfo( eType, pWarInfo, pMemberInfo );
		}
	}

	// 2. 용병 정보 변경
	if( MERCENARY_INVALID_IDX != pWarInfo->m_TMerID )
	{
		if( MATCH_TEAM_INVALID_IDX == pWarInfo->m_TTeamID ) 
		{	// 매치팀원이면 1에서 이미 변경되었음.
			CMercenaryManager* pMercenaryManager	= m_pMercenaryMgr[pWarInfo->ui32ServerIdx];
			pMercenaryManager->ChangeMemberInfo( eType, pWarInfo, pMemberInfo );
		}
	}
}

void CTaskProcessor::SendLeaveMatchTeam(CTeamManager* pTeamManager, CLAN_WAR_INFO *pWarInfo , INT8 i8LeaveSlot, T_RESULT TResult )
{
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK);
	SendPacket.WriteData(&pWarInfo->i32SessionIdx,		sizeof(INT32));
	SendPacket.WriteData(&pWarInfo->i64UID,				sizeof(T_UID));
	SendPacket.WriteData(&TResult,						sizeof(T_RESULT));
	SendPacket.WriteData(&i8LeaveSlot,					sizeof(INT8));

	pTeamManager->MakePacketLeaveTeam(pWarInfo->m_TTeamID, &SendPacket);

	g_pGateway->SendPacket(pWarInfo->ui32ServerIdx, &SendPacket);

	return;
}


INT32 CTaskProcessor::GetMatchingPoint(CTeamManager* pTeamManager, T_TeamID TTeamID)
{
	CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam(TTeamID);
	if( !pMatchTeam ) return 0;

	return pMatchTeam->CalculateMatchingPoint();
}

void	CTaskProcessor::SendClanWarResult( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MATCH_MAKING_RESULT* pResult)
{
	i3mem::FillZero( m_pui8SendUserCount, sizeof( UINT8 )*g_pConfig->GetServerCount());

	INT32	i32MemberCount = MIN( pstClan->_pMember->GetCount(), MAX_MEMBER_COUNT );
	
	for( INT32 i = 0 ; i < i32MemberCount ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( i );
		if( FRIEND_STATE_OFFLINE == GET_FRIEND_STATE( (UINT32)pstMember->_state )  )	continue;
		if( -1 == pstMember->_session )													continue;

		UINT32 ui32Server = GET_FRIEND_SERVER( (UINT32)pstMember->_state );
		if( ui32Server >= g_pConfig->GetServerCount() )									continue;

		m_ppi32SendSession[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]	= pstMember->_session;
		m_ppi64SendDBIdx[ ui32Server ][ m_pui8SendUserCount[ ui32Server ] ]		= pstMember->_i64UID;

		m_pui8SendUserCount[ ui32Server ]++;
	}

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		if( m_pui8SendUserCount[ i ] == 0 ) continue;

		i3NetworkPacket SendPacket( PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK );
		SendPacket.WriteData( &m_pui8SendUserCount[ i ],	sizeof(UINT8) );
		SendPacket.WriteData( m_ppi32SendSession[ i ],		(UINT16)(sizeof(INT32) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( m_ppi64SendDBIdx[ i ],		(UINT16)(sizeof(T_UID) * m_pui8SendUserCount[ i ]) );
		SendPacket.WriteData( pResult,						sizeof(CLAN_MATCH_MAKING_RESULT) );
		
		g_pGateway->SendPacket( i, &SendPacket );
	}
}

void	CTaskProcessor::MakePacketClanWarResult(CLAN_DETAIL_INFO_SERVER* pstClan, i3NetworkPacket* pPakcet)
{
	CLAN_WAR_RESULT	ClawWarResult[MAX_MATCH_RESULT_COUNT];
	UINT8 ui8ClanWarCount = 0;

	for (INT32 i = 0; i < MAX_MATCH_RESULT_COUNT; ++i)
	{
		if (pstClan->m_ClanWarResult[i].m_i32MatchIdx > 0)
		{
			ClawWarResult[ui8ClanWarCount++] = pstClan->m_ClanWarResult[i];
		}
	}

	pPakcet->WriteData(&ui8ClanWarCount, sizeof(UINT8));
	pPakcet->WriteData(ClawWarResult, sizeof(CLAN_WAR_RESULT) * ui8ClanWarCount);
}