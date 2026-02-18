#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
//#include "LogFile.h"
#include "DataBase.h"
#include "Receiver.h"
#include "MatchingManager.h"


I3_CLASS_INSTANCE(CTaskProcessor, i3Thread);

CTaskProcessor* g_pTaskProcessor = NULL;
INT32	i32CreateCnt = 0;

CTaskProcessor::CTaskProcessor()
{
	m_bIsRunning				= FALSE;
	m_pui8RecvBuffer			= NULL;
	m_pui8SendBuffer			= NULL;
	m_ui32DataSize				= 0;
	m_ui32ReadedLength			= 0;
	m_pPacketLocker				= NULL;

	m_pClanDetailInfo			= NULL;

	m_pui8SendUserCount			= NULL;
	m_ppi32SendSession			= NULL;
	m_ppi64SendDBIdx			= NULL;

	m_pBattleTeam				= NULL;

	//m_pUserNode					= NULL;

	m_ui32ClanOrderIdx			= 0;

	m_pListClan					= NULL;
	m_pListClanTemp				= NULL;
	m_pListMemberTemp			= NULL;
	m_pListRequestTemp			= NULL;

	m_i32SeasonRankingCount = 0;
	i3mem::FillZero( m_aSeasonRankingClan, sizeof( CLAN_BASIC_INFO ) * MAX_SEASON_RANKING_TEAM_COUNT );
}

CTaskProcessor::~CTaskProcessor()
{
	m_bIsRunning = FALSE;

	i3Thread::WaitForFinish();
	
	I3MEM_SAFE_FREE(m_pui8RecvBuffer);
	I3MEM_SAFE_FREE(m_pui8SendBuffer);
	
	I3_SAFE_RELEASE( m_pBattleTeam );
	INT32 i32Count;
	
	i32Count = m_pListMemberTemp->GetCount();
	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)m_pListMemberTemp->GetItem(i);
		I3_SAFE_DELETE( pstMember );
	}
	I3_SAFE_RELEASE( m_pListMemberTemp );

	i32Count = m_pListRequestTemp->GetCount();
	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)m_pListRequestTemp->GetItem(i);
		I3_SAFE_DELETE( pstRequest );
	}
	I3_SAFE_RELEASE( m_pListRequestTemp );
	I3_SAFE_RELEASE( m_pListClanTemp );
	I3_SAFE_RELEASE( m_pListClan );

	for( UINT32 i = 0 ; i < g_pConfig->m_ui32ClanMax ; i++ )
	{
		CLAN_DETAIL_INFO_SERVER* pstClan = &m_pClanDetailInfo[i];

		i32Count = pstClan->_pMember->GetCount();
		for( INT32 j = 0 ; j < i32Count ; j++ )
		{
			CLAN_MEMBER_DETAIL_INFO* pMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem(j);
			I3_SAFE_DELETE( pMember );
		}
		I3_SAFE_RELEASE( pstClan->_pMember );

		i32Count = pstClan->_pRequest->GetCount();
		for( INT32 j = 0 ; j < i32Count ; j++ )
		{
			CLAN_REQUEST_SERVER_INFO* pRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->GetItem(j);
			I3_SAFE_DELETE( pRequest );
		}
		I3_SAFE_RELEASE( pstClan->_pRequest );
	}

	I3MEM_SAFE_FREE( m_pClanDetailInfo );

	for( UINT32 i = 0 ; i < g_pConfig->GetServerCount() ; i++ )
	{
		I3MEM_SAFE_FREE( m_ppi32SendSession[ i ] );
		I3MEM_SAFE_FREE( m_ppi64SendDBIdx[ i ] );
	}
	I3MEM_SAFE_FREE( m_ppi32SendSession );
	I3MEM_SAFE_FREE( m_ppi64SendDBIdx );
	I3MEM_SAFE_FREE( m_pui8SendUserCount );

	for( INT32 i = 0; i < static_cast<INT32>(g_pConfig->GetServerCount()); ++i )
	{
		I3_SAFE_DELETE(m_pMercenaryMgr[i]);
		I3_SAFE_DELETE(m_pMatchManager[i]);
		I3_SAFE_DELETE(m_pTeamManager[i]);
	}
}

BOOL CTaskProcessor::Create(UINT32 ui32DataSize,CPacketLocker* pPacketLocker,CGateway* pGateway)
{
	if( FALSE == _ClanCreate() ) return FALSE;

	I3ASSERT(NULL == m_pui8RecvBuffer);
	I3ASSERT(NULL == m_pui8SendBuffer);
	I3ASSERT(ui32DataSize);
	I3ASSERT(pPacketLocker);
	I3ASSERT(pGateway);

	m_pui8RecvBuffer = (UINT8*)i3MemAlloc(ui32DataSize);
	i3mem::FillZero( m_pui8RecvBuffer, ui32DataSize );

	m_pui8SendBuffer = (UINT8*)i3MemAlloc(ui32DataSize);
	i3mem::FillZero( m_pui8SendBuffer, ui32DataSize );

	m_ui32DataSize = ui32DataSize;
	m_pPacketLocker = pPacketLocker;
	m_pGateway = pGateway;

	m_ui32dMatchTeamSendTime = i3ThreadTimer::GetServerTime();

	for( INT32 i = 0; i < static_cast<INT32>(g_pConfig->GetServerCount()); ++i )
	{
		m_pMatchManager[i] = new CMatchManager;
		if( !m_pMatchManager[i] )
			return FALSE;

		m_pMatchManager[i]->Create();

		m_pTeamManager[i] = new CTeamManager;
		if( !m_pTeamManager[i] )
			return FALSE;

		m_pMercenaryMgr[i] = new CMercenaryManager;
		if( !m_pMercenaryMgr[i] )
			return FALSE;
	}
	
	if (FALSE == i3Thread::Create("TaskProcessor", 0, 4096, NULL, PRIORITY_HIGHEST))
	{
		I3MEM_SAFE_FREE(m_pui8RecvBuffer);
		I3MEM_SAFE_FREE(m_pui8SendBuffer);
		m_ui32DataSize = 0;
		m_pPacketLocker = NULL;

		return FALSE;
	}

	return TRUE;
}

BOOL	CTaskProcessor::_ClanCreate(void)
{
	g_pLog->WriteLog(L"[CTaskProcessor::_ClanCreat] Start");

	// 서버 카운터 0이면 실패
	UINT32 ui32ServerCount = g_pConfig->GetServerCount();

	// 메모리 할당
	MEM_ALLOC(		m_pui8SendUserCount,		UINT8,						ui32ServerCount );
	MEM_ALLOC(		m_ppi32SendSession,			INT32*,						ui32ServerCount );
	MEM_ALLOC_LOOP(	m_ppi32SendSession,			INT32,						ui32ServerCount, MAX_MEMBER_COUNT );
	MEM_ALLOC(		m_ppi64SendDBIdx,			T_UID*,						ui32ServerCount );
	MEM_ALLOC_LOOP(	m_ppi64SendDBIdx,			T_UID,						ui32ServerCount, MAX_MEMBER_COUNT );
	MEM_ALLOC(		m_pClanDetailInfo,			CLAN_DETAIL_INFO_SERVER,	g_pConfig->m_ui32ClanMax );

	// 각 남는 메모리를 담아놓을 List 생성
	m_pListClan			= i3BinListEx::NewObject();
	m_pListClanTemp		= i3List::NewObject();
	m_pListMemberTemp	= i3List::NewObject();
	m_pListRequestTemp	= i3List::NewObject();

	////////////// 초기화////////////////////////////////////////////////////////////////////////////////////////
	i3mem::FillZero( &m_ClanListContext,				sizeof(CLAN_CONTEXT) );
	for( UINT32 i = 0 ; i < g_pConfig->m_ui32ClanMax ; i++ )
	{
		m_pClanDetailInfo[i]._pMember	= i3BinListEx64::NewObject();
		m_pClanDetailInfo[i]._pRequest	= i3BinListEx64::NewObject();
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// Load DataBase
	INT32 i32ClanCount = g_pDataBase->LoadClanDetailData(m_pClanDetailInfo, m_pListClan);
	if( i32ClanCount < 0 )
	{
		g_pLog->WriteLog( L"[ERROR][CTaskProcessor::_ClanCreat] Failed to load clan data.");
		return FALSE;
	}

	g_pLog->WriteLog(L"[CTaskProcessor::_ClanCreat] load clan data Complete");

	// Load SeasonRanking
	m_i32SeasonRankingCount  = g_pDataBase->LoadSeasonRanking(g_pConfig->GetSeasonIndex(SEASON_TYPE_PRE), DB_SORT_TYPE_DESC, m_aSeasonRankingClan);
	if( m_i32SeasonRankingCount  < 0 )
	{
		g_pLog->WriteLog( L"[ERROR][CTaskProcessor::_ClanCreat] Failed to load Season Ranking");
	}

	g_pLog->WriteLog(L"[CTaskProcessor::_ClanCreat] load Season Ranking Complete");

	//m_RankingManager.Initialize(i32ClanCount);

	m_ClanListContext._totalCount = i32ClanCount;

	SetClanListContect();

	m_i32ClanTempCount = m_ClanListContext._totalCount;

	// 남는 메모리 저장해 놓는다. ( Clan Create 를 위해서 클랜장의 메모리를 미리 생성해서 넣어둡니다. )
	for( INT32 i = g_pConfig->m_ui32ClanMax-1; i >= (INT32)m_ClanListContext._totalCount ; i-- )
	{
		CLAN_MEMBER_DETAIL_INFO* pstMember = PopTempMember();//new CLAN_MEMBER_DETAIL_INFO;
		if( NULL == pstMember )
		{
			g_pLog->WriteLog( L"[ERROR] Load Temp Clan Error");
			return FALSE;
		}

		if( 0 > m_pClanDetailInfo[i]._pMember->Add( pstMember ) )
		{
			g_pLog->WriteLog( L"[ERROR] Load Temp Clan Error");
			return FALSE;
		}
		if( 0 > m_pListClanTemp->Add( &(m_pClanDetailInfo[i]) ) )
		{
			g_pLog->WriteLog( L"[ERROR] Load Temp Clan Error");
			return FALSE;
		}
	}

	// Clan Item List
	if( EV_FAILED( g_pDataBase->LoadClanItem() ) )
	{
		g_pLog->WriteLog( L"[ERROR] Load ClanItem Error");
		return FALSE;
	}	
	g_pLog->WriteLog(L"[SUCC] Load ClanItem Complete");

	// Clan Member List
	if( EV_FAILED( g_pDataBase->LoadMemberData() ))
	{
		g_pLog->WriteLog( L"[ERROR] Load ClanMem Error");
		return FALSE;
	}
	g_pLog->WriteLog(L"[SUCC] Load ClanMem Complete");

	// Clan Request List
	if( EV_FAILED( g_pDataBase->LoadRequestData() ))
	{
		g_pLog->WriteLog( L"[ERROR] Load ClanReq Error");
		return FALSE;
	}
	g_pLog->WriteLog(L"[SUCC] Load ClanReq Complete");

	// Clan War Result List
	if (EV_FAILED(g_pDataBase->LoadClanWarResultData()))
	{
		g_pLog->WriteLog(L"[ERROR] Load ClanWarResultData Error");
		return FALSE;
	}
	g_pLog->WriteLog(L"[SUCC] Load ClanWarResultData Complete");

	CLAN_DETAIL_INFO_SERVER* pstClan;
	for( UINT32 i = 0 ; i < m_ClanListContext._totalCount ; i++ )
	{
		pstClan = &m_pClanDetailInfo[i];

		// 클랜 랭크 정의
		pstClan->_ClanDetailInfo.m_personRank	= GetClanPersonRank( pstClan->_ClanDetailInfo.m_currentPerson );
		pstClan->_ClanDetailInfo.m_expRank		= GetClanExpRank( pstClan->_ClanDetailInfo.m_exp );
		
		// 맴버 Request 로드
		SetMemberListContect( pstClan );
		SetRequestListContect( pstClan );
	}

	m_pBattleTeam = CBattleTeam::NewObject();
	if( FALSE == m_pBattleTeam->Create() )
	{
		g_pLog->WriteLog( L"[ERROR] Load Battle Team Error");
		return FALSE;
	}
	
	return TRUE;			
}

UINT32 CTaskProcessor::OnRunning(void* pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"TASK", 0, m_ThreadID);

	m_bIsRunning = TRUE;
	BOOL bIsWorking;

	while(m_bIsRunning)
	{
		bIsWorking = FALSE;

		for( UINT32 i = 0; i < m_pPacketLocker->getCount(); i++ )
		{
			m_ui32ReadedLength = m_pPacketLocker->Pop(i, m_pui8RecvBuffer, m_ui32DataSize);
			if( m_ui32ReadedLength > 0 )
			{
				switch(CReceiver::GetReceiverType(i))
				{
				case GAME_SERVER		:		_DoTaskForGame(i);		break;
				case UNKOWN_SERVER		:		_DoTaskForRecursive();	break;
				default					:								break;
				}
				bIsWorking = TRUE;
			}			
		}

		for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )
		{
			NSM_CRingBuffer* pRingOutDB = g_pDataBase->GetRingOutDB( i );
			INT32 i32OutDBCount = pRingOutDB->GetBufferCount();
			if( 0 < i32OutDBCount )
			{
				RING_OUT_DB* pOutDB = (RING_OUT_DB*)pRingOutDB->Pop();
				_DoTaskForDB( pOutDB );
				pRingOutDB->PopIdx();

				bIsWorking = TRUE;
			}
		}

		for( INT32 i = 0; i < static_cast<INT32>(g_pConfig->GetServerCount()); ++i )
		{
			for( INT32 j = 0; j < g_pMatchingManager->GetMatchingCount(i); ++j  )
			{
				MATCHING_RESULT* pMatchingResult = g_pMatchingManager->GetMatchingResult(i);
				if( SendMatchingResult(i, pMatchingResult) )
					bIsWorking = TRUE;
				else
					g_pLog->WriteLog( L"[ERROR][CTaskProcessor::OnRunning] Failed to process matching result.");

				g_pMatchingManager->PopMatchingIndex(i);
			}
		}

		// 전체 클랜 매치팀 카운트 보내기.
		if( (m_ui32dMatchTeamSendTime + MATCH_TEAM_COUNT_UPDATE_TIME) < i3ThreadTimer::GetServerTime() )
		{
			SendAllMatchTeamCount(g_pConfig->GetServerCount());
			m_ui32dMatchTeamSendTime = i3ThreadTimer::GetServerTime();
		}

		if( FALSE == bIsWorking )
		{
			if( m_ui32TeamOrderTime + 1 < i3ThreadTimer::GetServerTime() )
			{
				m_ui32TeamOrderTime = i3ThreadTimer::GetServerTime();

				for( INT32 i = 0 ; i < 5 ; i++ )
				{
					if( m_pClanDetailInfo[ m_ui32ClanOrderIdx ]._bClanOrder )
					{				
						RING_IN_DB stRingInDB;
						stRingInDB._ui8Type			= RING_DB_MEMBER_ORDER; 
						stRingInDB._ui32ClanDBIdx	= m_pClanDetailInfo[ m_ui32ClanOrderIdx ]._ClanDetailInfo.m_ui32CID;
						g_pDataBase->InsertWorking( stRingInDB._ui32ClanDBIdx, &stRingInDB );
					}
					m_ui32ClanOrderIdx++;

					if( m_ui32ClanOrderIdx >= m_ClanListContext._totalCount )
						m_ui32ClanOrderIdx = 0;
				}
			}
		}

		if( !bIsWorking )
			::Sleep(1);
	}

	return 0;
}

BOOL CTaskProcessor::SendPacketMessage(UINT32 ui32idx,i3NetworkPacket* pPacket)
{
	return m_pGateway->SendPacket(ui32idx, pPacket);
}

BOOL CTaskProcessor::SendRecursivePacket(i3NetworkPacket* pPacket)
{	
	UINT32 ui32recursiveIdx = m_pPacketLocker->getCount() - 1;
	return m_pPacketLocker->Push(ui32recursiveIdx, (UINT8*)pPacket->GetPacketBuffer(), pPacket->GetPacketSize());
}

void	CTaskProcessor::PushTempClan( CLAN_DETAIL_INFO_SERVER* pstClan )
{
	if( NULL == pstClan )	return;

	pstClan->_bClanOrder = FALSE;
	INT32 i32FindIdx = m_pListClan->FindItem( &pstClan->_ClanDetailInfo.m_ui32CID );
	m_pListClan->Delete( i32FindIdx );

	if( i32FindIdx != m_pListClan->GetCount() )
	{
		INT32 i32EndIdx = m_pListClan->GetCount()-1;
		CLAN_DETAIL_INFO_SERVER* pstClanMove = (CLAN_DETAIL_INFO_SERVER*)m_pListClan->GetItem( i32EndIdx );
		CLAN_DETAIL_INFO_SERVER stClanTemp;
		memcpy( &stClanTemp,	pstClanMove,	sizeof(CLAN_DETAIL_INFO_SERVER) );
		memcpy( pstClanMove,	pstClan,		sizeof(CLAN_DETAIL_INFO_SERVER) );
		memcpy( pstClan,		&stClanTemp,	sizeof(CLAN_DETAIL_INFO_SERVER) );

		m_pListClan->SetItem( i32EndIdx, pstClan );

		m_pListClanTemp->Add( pstClanMove );
	}
	else
	{
		m_pListClanTemp->Add( pstClan );
	}
}

CLAN_DETAIL_INFO_SERVER*	CTaskProcessor::PopTempClan()
{
	if( 0 == m_pListClanTemp->GetCount() ) return NULL;

	INT32 i32EndIdx = m_pListClanTemp->GetCount()-1;
	CLAN_DETAIL_INFO_SERVER* pstClan = (CLAN_DETAIL_INFO_SERVER*)m_pListClanTemp->GetItem( i32EndIdx );
	if( NULL == pstClan ) return NULL;
	m_pListClanTemp->Delete( i32EndIdx );

	pstClan->Reset();

	return pstClan;
}

void	CTaskProcessor::PushTempMember( CLAN_MEMBER_DETAIL_INFO* pstMember )
{
	if( NULL == pstMember ) return;
	m_pListMemberTemp->Add( pstMember );
}

CLAN_MEMBER_DETAIL_INFO*	CTaskProcessor::PopTempMember()
{
	CLAN_MEMBER_DETAIL_INFO* pstMember = NULL;
	if( 0 < m_pListMemberTemp->GetCount() )
	{
		pstMember = (CLAN_MEMBER_DETAIL_INFO*)m_pListMemberTemp->GetItem( 0 );
		if( NULL == pstMember ) return NULL;

		m_pListMemberTemp->Delete( 0 );
		pstMember->Reset();
	}
	else
	{
		pstMember = new CLAN_MEMBER_DETAIL_INFO;
	}
	return pstMember;
}

void	CTaskProcessor::PushTempRequest( CLAN_REQUEST_SERVER_INFO* pstReauest )
{
	if( NULL == pstReauest ) return;
	m_pListRequestTemp->Add( pstReauest );
}

CLAN_REQUEST_SERVER_INFO*	CTaskProcessor::PopTempRequest()
{
	CLAN_REQUEST_SERVER_INFO* pstReauest = NULL;
	if (0 < m_pListRequestTemp->GetCount())
	{
		pstReauest = (CLAN_REQUEST_SERVER_INFO*)m_pListRequestTemp->GetItem(0);
		if (NULL == pstReauest) return NULL;

		m_pListRequestTemp->Delete(0);
		pstReauest->Reset();
	}
	else
	{
		pstReauest = new CLAN_REQUEST_SERVER_INFO;
		i32CreateCnt++;
	}
	return pstReauest;
}

void CTaskProcessor::_DoTaskForGame(UINT32 ui32idxBuffer)
{
	i3NetworkPacket packet;
	packet.CopyToBuffer((char*)m_pui8RecvBuffer, m_ui32ReadedLength);

	if( _DoTaskForGameGlobal( ui32idxBuffer, &packet ) )	return; 
	
	switch( packet.GetProtocolID() & 0xFF00 )
	{
		case PROTOCOL_CS:		_DoTaskForGameUser( ui32idxBuffer, &packet );	break; 
		case PROTOCOL_CS_MATCH:	_DoTaskForGameBattle( ui32idxBuffer, &packet ); break;
		case PROTOCOL_CHEAT:
		case PROTOCOL_CLAN_WAR:	_DoTaskForClanWar(ui32idxBuffer, &packet);	break;
		default:
			{	// Error Packet
				g_pLog->WriteLog( L"[Error] CTaskProcessor::_DoTaskForGame() - Unkown protocol id(%d)", packet.GetProtocolID());	
			}
			break;
	}
}

BOOL CTaskProcessor::_DoTaskForGameGlobal( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket )
{ 
	switch( pPacket->GetProtocolID() )
	{
		case PROTOCOL_BASE_HEART_BIT_REQ:								// Heart Bit
			{
				i3NetworkPacket	SendPacket(PROTOCOL_BASE_HEART_BIT_ACK);
				SendPacketMessage(ui32idxBuffer, &SendPacket);
			}
			break;
		//case PROTOCOL_OPER_STATE_REQ:									//서버 상태 알려주기 
		//	{
		//		i3NetworkPacket SendPacket(PROTOCOL_OPER_STATE_ACK);
		//		UINT32 nConnectedUser = 0;
		//		UINT32 nConnectedServer = g_pGateway->GetConnectedGameServerCount();

		//		SendPacket.WriteData(&nConnectedUser, sizeof(UINT32));
		//		SendPacket.WriteData(&nConnectedServer, sizeof(UINT32));

		//		SendPacket.WriteData(&g_pConfig->m_iCpuQuery, sizeof(INT32));
		//		SendPacket.WriteData(&g_pConfig->m_iHandleQuery, sizeof(INT32));

		//		SendPacketMessage(idxBuffer, &SendPacket);
		//	}
		//	break;
		case PROTOCOL_CS_MATCH_CHANNEL_REQ:
			{
				UINT8 ui8ChnnelCount;
				UINT8 aui8Channel[ 255 ];

				pPacket->ReadData(&ui8ChnnelCount,		sizeof(UINT8) );
				pPacket->ReadData(&aui8Channel,			sizeof(UINT8) * ui8ChnnelCount);

				//CreateBattleInfo( idxBuffer, cChnnelCount, cChannel );
			}
			break;
		case PROTOCOL_CS_CLAN_CONTEXT_REQ:								// 클랜목록 정보 요청
			{	/* No req data */
				i3NetworkPacket SendPacket(PROTOCOL_CS_CLAN_CONTEXT_ACK);

				UINT8 ui8SectionSize = CLAN_SECTION_SIZE;
				SendPacket.WriteData( &m_ClanListContext._totalCount,	sizeof(UINT32) );
				SendPacket.WriteData( &ui8SectionSize,					sizeof(UINT8) );
				SendPacket.WriteData( &m_ClanListContext._sectionCount, sizeof(UINT16) );
				SendPacket.WriteData( &m_ClanListContext._version,		sizeof(UINT32) );
				SendPacketMessage( ui32idxBuffer, &SendPacket );
			}
			break;
		case PROTOCOL_CS_CLAN_LIST_STREAM_REQ:							// 클랜목록 연속받기 요청
			{	/* No req data */
				StreamingClanList(ui32idxBuffer, 0);
			}
			break;
		
		case PROTOCOL_CS_MATCH_BATTLE_RESULT:
			{
				UINT8 ui8ClanCount;
				UINT8 ui8UserCount;
				pPacket->ReadData( &ui8ClanCount,				sizeof(UINT8) );
				pPacket->ReadData( &ui8UserCount,				sizeof(UINT8) );

				if( TEAM_COUNT < ui8ClanCount )
					return FALSE;

				if( SLOT_MAX_COUNT < ui8UserCount )
					return FALSE;

				CLAN_RESULT			aClanResult[ TEAM_COUNT ]			= {0,};
				CLAN_DETAIL_RESULT	ClanDetailResult					= {0,};
				CLAN_RESULT_USER	aClanResultUser[ SLOT_MAX_COUNT ]	= {0,};

				if( ui8ClanCount > 0 ) 
				{
					pPacket->ReadData( aClanResult,				sizeof(CLAN_RESULT) * ui8ClanCount );
					pPacket->ReadData(&ClanDetailResult,		sizeof(CLAN_DETAIL_RESULT) );
				}
				if( ui8UserCount > 0 )
				{
					pPacket->ReadData( aClanResultUser,			sizeof(CLAN_RESULT_USER) * ui8UserCount );
				}

				BOOL bDBSaveClan = TRUE;	// 클랜 전적 DB 저장 여부.
				if( 1 != ClanDetailResult.m_ui8IsMatchMaking )
				{
					UINT32 ui32ClanIdx[TEAM_COUNT] = {0,};

					if( 0 < ui8ClanCount ) 
					{
						ui32ClanIdx[0] = aClanResult[0]._ClanIdx;
						ui32ClanIdx[1] = aClanResult[1]._ClanIdx;
					}
					 
					for( INT32 i = 0 ; i < ui8UserCount ; i++ )
					{
						CLAN_RESULT_USER* pUser = &aClanResultUser[i];

						if( ui32ClanIdx[pUser->_ui8TeamIdx] != pUser->_ClanIdx )
						{	// 팀원중 클랜원이 아닌 유저가 있으면.
							bDBSaveClan = FALSE;
						}
					}
				}

				// 1. 클랜 누적 전적 반영
				if( bDBSaveClan )
				{
					BattleResult( ui8ClanCount, aClanResult, ui8UserCount, aClanResultUser, m_pTeamManager[ui32idxBuffer], ClanDetailResult.m_ui8IsMatchMaking );
				}

				// 2. 개인 전적 반영
				BattleResultUser( ui8UserCount, aClanResultUser, m_pMercenaryMgr[ui32idxBuffer] );


				// 3. 시즌 중에 매치 메이킹이면 클랜 시즌 전적 반영
				if( 1 == ClanDetailResult.m_ui8IsMatchMaking && g_pConfig->IsOngoingSeason( ) )
				{	
					CMatchManager* pMatch =  m_pMatchManager[ui32idxBuffer];
					CMatching* pMatching = pMatch->GetMatching( ClanDetailResult.m_TMatchingID );
					if( pMatching )
					{
						pMatching->ApplyMatchResult(aClanResult, ui32idxBuffer, ClanDetailResult.m_ui32StageID, ui8UserCount, aClanResultUser);
					}
				}
			}
			break;	
		default:
			{
			}
			return FALSE;
	}

	return TRUE;
}

void CTaskProcessor::_DoTaskForRecursive(void)
{
	i3NetworkPacket packet;
	packet.CopyToBuffer((char*)m_pui8RecvBuffer, m_ui32ReadedLength);

	switch(packet.GetProtocolID())
	{
	case PROTOCOL_CS_CLAN_LIST_STREAM_REQ:
		{
			UINT32 ui32ServerIdx;
			UINT32 ui32StartIdx;

			packet.ReadData(&ui32ServerIdx,		sizeof(UINT32));
			packet.ReadData(&ui32StartIdx,		sizeof(UINT32));

			StreamingClanList(ui32ServerIdx, ui32StartIdx);
		}
		break;
	case PROTOCOL_CS_MEMBER_LIST_REQ:
		{
			CLAN_USER_INFO	UserInfo;
			UINT8			ui8SectionIdx;

			packet.ReadData(&UserInfo.ui32ServerIdx,	sizeof(UINT32));
			packet.ReadData(&UserInfo.i32SessionIdx,	sizeof(INT32));
			packet.ReadData(&UserInfo.i64UID,			sizeof(T_UID));
			packet.ReadData(&UserInfo.ui32ClanDBIdx,	sizeof(UINT32));
			packet.ReadData(&ui8SectionIdx,				sizeof(UINT8));

			SendMemberList( &UserInfo, ui8SectionIdx );
		}
		break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ:
		{
			CLAN_USER_INFO	UserInfo;		
			UINT8			wSectionIdx;

			packet.ReadData(&UserInfo.ui32ServerIdx,	sizeof(UINT32));
			packet.ReadData(&UserInfo.i32SessionIdx,	sizeof(INT32));
			packet.ReadData(&UserInfo.i64UID,			sizeof(T_UID));
			packet.ReadData(&UserInfo.ui32ClanDBIdx,	sizeof(UINT32));
			packet.ReadData(&wSectionIdx,				sizeof(UINT8));

			SendMemberList( &UserInfo, wSectionIdx );
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ:
		{
			CLAN_USER_INFO	UserInfo;
			UINT8			ui8Channel;
			UINT16			ui16Section;

			packet.ReadData(&UserInfo.ui32ServerIdx,	sizeof(UINT32));
			packet.ReadData(&UserInfo.i32SessionIdx,	sizeof(INT32));
			packet.ReadData(&UserInfo.i64UID,			sizeof(T_UID));
			packet.ReadData(&UserInfo.ui32ClanDBIdx,	sizeof(UINT32));
			packet.ReadData(&ui8Channel,				sizeof(UINT8));
			packet.ReadData(&ui16Section,				sizeof(UINT16));

			BattleMatchAllTeamList( &UserInfo, ui8Channel, ui16Section );
		}
		break;
	default:
		/* Do nothing */
		break;
	}
}

void CTaskProcessor::SendAllMatchTeamCount( INT32 i32GameServerCount  )
{
	INT32 i32MatchingTeamCount = 0;
	INT32 i32MatchedTeamCount = 0;

	for( INT32 i = 0; i < i32GameServerCount ; ++i )
	{
		CTeamManager* pTeamManager = m_pTeamManager[i];

		INT32 pi32TeamCount[TEAM_STATE_COUNT] = {0,};

		pTeamManager->GetMatchTeamCountPerState( pi32TeamCount );

		i32MatchingTeamCount	+= pi32TeamCount[TEAM_STATE_MATCHING];
		i32MatchedTeamCount		+= pi32TeamCount[TEAM_STATE_MATCHED];
	}

	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK);
	SendPacket.WriteData( &i32MatchedTeamCount,		sizeof(INT32) );
	SendPacket.WriteData( &i32MatchingTeamCount,	sizeof(INT32) );

	for( INT32 i = 0; i < i32GameServerCount ; ++i )
	{
		 g_pGateway->SendPacket(i, &SendPacket);
	}
}

 