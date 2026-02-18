#include "pch.h"
#include "MatchingManager.h"

#include "DataBase.h"
#include "Gateway.h"
#include "TaskProcessor.h"
#include "../../CommonServerSource/TrueSkill/TrueSkill.h"

#define MATCHING_TIME_VALUE	 1000	// 1000		// 서버별로 클랜 매칭 시키는 시간 계산을 위한 값

I3_CLASS_INSTANCE(CMatchingManager, i3Thread);

CMatchingManager* g_pMatchingManager = NULL;

//////////////////////////////////////////////////////////////////////////
// CMatching
CMatching::CMatching()
: m_i32ChannelIndex(INVALID_IDX)
, m_i32RoomIndex(INVALID_IDX)
, m_ui8MaxPlayer(0)
, m_eMatchState(MATCH_NONE)
{
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
		m_pMatchTeams[i] = NULL;
}

CMatching::~CMatching()
{
}

bool CMatching::SetMatchTeam(TEAM_TYPE eTeamType, CMatchTeam* pMatchTeam)
{
	if( TEAM_NONE < eTeamType && eTeamType < TEAM_COUNT )
	{
		m_pMatchTeams[eTeamType] = pMatchTeam;
		return true;
	}
	return false;
}

T_RESULT CMatching::LeaveMatchRoom(T_TeamID TTeamID, CLAN_USER_INFO* pUserInfo, INT8& i8LeaveSlot, UINT8 ui8RoomState, bool bForceLeave  )
{
	TEAM_TYPE eTeamType = TEAM_NONE;
	for (INT32 i = 0; i < TEAM_COUNT; ++i)
	{
		if (TTeamID == m_pMatchTeams[i]->GetTeamID())
		{
			eTeamType = (TEAM_TYPE)i;
			break;
		}
	}

	if (TEAM_NONE == eTeamType) return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;


	return m_pMatchTeams[eTeamType]->LeaveMatchRoom(pUserInfo, GetChannaelIdx(), i8LeaveSlot, ui8RoomState, bForceLeave );
}

T_TeamID CMatching::GetTeamID(TEAM_TYPE eTeamType)
{
	if( eTeamType <= TEAM_NONE && TEAM_COUNT < eTeamType ) return MATCH_TEAM_INVALID_IDX;

	return m_pMatchTeams[eTeamType]->GetTeamID(); 
}

bool CMatching::MakePacketJoinRoom(i3NetworkPacket* pPacket )
{
	pPacket->WriteData(&m_i32ChannelIndex,	sizeof(INT32));
	pPacket->WriteData(&m_i32RoomIndex,		sizeof(INT32));
	
	// 방장( 레드팀 리더 ) UID를 별도로 추가합니다.( Join에서 제외시키기 위해 )
	T_UID THostUID = m_pMatchTeams[TEAM_RED]->GetLeaderUID();
	pPacket->WriteData(&THostUID,			sizeof(T_UID));
	
	// Game 서버 처리를 깔끔하게 하기 위해 이렇게 갑니다.
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
	{
		m_pMatchTeams[i]->MakePacketClanMatchingInfo(pPacket);
	}

	for( INT32 i = 0; i < TEAM_COUNT; ++i )
	{
		m_pMatchTeams[i]->MakePacketJoinRoom(pPacket);
	}

	return true;
}

void CMatching::ApplyMatchResult(CLAN_RESULT* aResult, UINT32 ui32ServerIdx, UINT32 ui32StageID, UINT8 ui8TotalPlayer, CLAN_RESULT_USER* pPlayer)
{
	if (!m_pMatchTeams[TEAM_RED] || !m_pMatchTeams[TEAM_BLUE])
	{
		g_pLog->WriteLog(L"[ERROR][CMatching::ApplyMatchResult] Not found match team. / RedClanIdx : %d/ BlueClanIdx : %d",
			aResult[TEAM_RED]._ClanIdx, aResult[TEAM_BLUE]._ClanIdx);
		return;
	}

	CLAN_DETAIL_INFO_SERVER* pRedTeamClan = m_pMatchTeams[TEAM_RED]->GetOwnerClan();
	CLAN_DETAIL_INFO_SERVER* pBlueTeamClan = m_pMatchTeams[TEAM_BLUE]->GetOwnerClan();
	if (!pRedTeamClan || !pBlueTeamClan)
	{
		g_pLog->WriteLog(L"[ERROR][CMatching::ApplyMatchResult] Not found Owner clan. RedTeamID : %d / BlueTeamID : %d/ RedClanIdx : %d / BlueClanIdx : %d",
			m_pMatchTeams[TEAM_RED]->GetTeamID(), m_pMatchTeams[TEAM_BLUE]->GetTeamID(), aResult[TEAM_RED]._ClanIdx, aResult[TEAM_BLUE]._ClanIdx);
		return;
	}

	// 동일 클랜이면 클랜 포인트 및 시즌 전적을 계산하지 않습니다.
	if (m_pMatchTeams[TEAM_RED]->GetClanID() == m_pMatchTeams[TEAM_BLUE]->GetClanID())
	{
		return;
	}

	// 팀 체인지시 결과를 Swap해 줍니다.
	if (aResult[TEAM_RED]._ClanIdx != m_pMatchTeams[TEAM_RED]->GetClanID())
	{
		CLAN_RESULT stClanResult;

		i3mem::Copy(&stClanResult, &aResult[TEAM_RED], sizeof(CLAN_RESULT));
		i3mem::Copy(&aResult[TEAM_RED], &aResult[TEAM_BLUE], sizeof(CLAN_RESULT));
		i3mem::Copy(&aResult[TEAM_BLUE], &stClanResult, sizeof(CLAN_RESULT));
	}

	CalculateRating(aResult);

	CLAN_WAR_RESULT		stClanWarResult;
	stClanWarResult.m_ui32StageID = ui32StageID;

	T_UID	aClanWarPlayerUID[CLAN_TEAM_MEMBER_COUNT];	// 용병을 제외한 클랜 매치 메이킹 참여인원.  최근 1개.


	for (INT32 i = 0; i < TEAM_COUNT; ++i)
	{
		CLAN_RESULT* pClanResult = &aResult[i];
		if (!pClanResult) continue;

		// 매치메이킹 참여 인원(용병제외) UID.
		UINT8 ui8PlayerCount = 0;
		i3mem::FillZero(aClanWarPlayerUID, sizeof(T_UID)*CLAN_TEAM_MEMBER_COUNT);

		for (UINT8 k = 0; k < ui8TotalPlayer; ++k)
		{
			if (ui8PlayerCount >= CLAN_TEAM_MEMBER_COUNT) break;

			CLAN_RESULT_USER* pUser = &pPlayer[k];

			// 용병 제외.
			if (MERCENARY_INVALID_IDX != pUser->m_TMerID) continue;

			// 상대편 클랜원 제외.
			if (pClanResult->_ClanIdx != pUser->_ClanIdx) continue;

			aClanWarPlayerUID[ui8PlayerCount++] = pUser->_i64UID;
		}

		// 매치팀내 용병 랭크업여부를 전달. 
		m_pMatchTeams[i]->SendMerRankUp(ui32ServerIdx);

		if (1 == pClanResult->_Win)		stClanWarResult.m_Result = (UINT8)ROOM_ENDTYPE_WIN;
		else if (1 == pClanResult->_Lose)	stClanWarResult.m_Result = (UINT8)ROOM_ENDTYPE_LOSE;
		else								stClanWarResult.m_Result = (UINT8)ROOM_ENDTYPE_DRAW;

		stClanWarResult.m_MyClanScore = pClanResult->_RoundWinCount;

		switch (i)
		{
		case TEAM_RED:
		{
			stClanWarResult.m_OtherClanScore = aResult[TEAM_BLUE]._RoundWinCount;
			i3String::Copy(stClanWarResult.m_strClanName, pBlueTeamClan->_ClanDetailInfo.m_name, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
		}
		break;
		case TEAM_BLUE:
		{
			stClanWarResult.m_OtherClanScore = aResult[TEAM_RED]._RoundWinCount;
			i3String::Copy(stClanWarResult.m_strClanName, pRedTeamClan->_ClanDetailInfo.m_name, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
		}
		break;
		}

		m_pMatchTeams[i]->ApplyMatchResult(&stClanWarResult, aClanWarPlayerUID);
	}
}

void CMatching::CalculateRating(CLAN_RESULT* aResult)
{
	REAL64 r64C, r64V, r64W;

	switch (aResult[TEAM_RED]._Win)
	{
	case 0:
		CTrueSkill::GetValue(m_pMatchTeams[TEAM_BLUE]->GetClanRating(), m_pMatchTeams[TEAM_RED]->GetClanRating(), m_ui8MaxPlayer + 1, r64C, r64V, r64W);
		break;
	case 1:
		CTrueSkill::GetValue(m_pMatchTeams[TEAM_RED]->GetClanRating(), m_pMatchTeams[TEAM_BLUE]->GetClanRating(), m_ui8MaxPlayer + 1, r64C, r64V, r64W);
		break;
	default:
		r64C = 0.f;
		r64V = 0.f;
		r64W = 0.f;
		break;
	}

	ROOM_ENDTYPE eResult;
	for (INT32 i = 0; i < TEAM_COUNT; ++i)
	{
		aResult[i]._Win == 1 ? eResult = ROOM_ENDTYPE_WIN : eResult = ROOM_ENDTYPE_LOSE;

		m_pMatchTeams[i]->CalculateRatings(eResult, r64C, r64V, r64W);
	}
}

void CMatching::SimulateRating(TEAM_TYPE eWinTeam, double *pResultClanPoint)
{
	ROOM_ENDTYPE eResult[TEAM_COUNT];

	REAL64 r64C, r64V, r64W;

	CRating TeamRating[TEAM_COUNT];
	i3mem::Copy( &TeamRating[TEAM_RED], &m_pMatchTeams[TEAM_RED]->GetClanRating(), sizeof(CRating) );
	i3mem::Copy( &TeamRating[TEAM_BLUE], &m_pMatchTeams[TEAM_BLUE]->GetClanRating(), sizeof(CRating) );
	
	switch( eWinTeam ) 
	{
	case TEAM_RED :
		CTrueSkill::GetValue(TeamRating[TEAM_RED], TeamRating[TEAM_BLUE], m_ui8MaxPlayer + 1, r64C, r64V, r64W);
		eResult[TEAM_RED] = ROOM_ENDTYPE_WIN;
		eResult[TEAM_BLUE] = ROOM_ENDTYPE_LOSE;
		break;
	case TEAM_BLUE :
		CTrueSkill::GetValue(TeamRating[TEAM_BLUE], TeamRating[TEAM_RED], m_ui8MaxPlayer + 1, r64C, r64V, r64W);
		eResult[TEAM_RED] = ROOM_ENDTYPE_LOSE;
		eResult[TEAM_BLUE] = ROOM_ENDTYPE_WIN;
		break;
	default:
		return;
	}		

	for( INT32 i = 0; i < TEAM_COUNT; ++i )
	{
		CTrueSkill::CalculateRating( TeamRating[i], eResult[i], r64C, r64V, r64W );
		pResultClanPoint[i] = TeamRating[i].GetClanPoint();
	}
}

void CMatching::ResetMatching()
{
	m_eMatchState		= MATCH_NONE;
	m_i32ChannelIndex	= INVALID_IDX;
	m_i32RoomIndex		= INVALID_IDX;
	m_ui8MaxPlayer		= 0;
	m_TMatchingID		= MATCHING_ID_INVALID_IDX;
	
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
		m_pMatchTeams[i] = NULL;
}

bool CMatching::SendCreateRoom(INT32 i32ServerIdx )
{
	T_UID i64TeamHeadUID = m_pMatchTeams[TEAM_RED]->GetLeaderUID();
	if( 0 == i64TeamHeadUID )
	{
		g_pLog->WriteLog( L"[ERROR][CMatching::SendCreateRoom] Not found match team reader.");
		return false;
	}

	UINT8 ui8MaxMemberCnt = m_pMatchTeams[TEAM_RED]->GetMaxMemberCount();

	i3NetworkPacket stCreateRoomPacket(PROTOCOL_CLAN_WAR_CREATE_ROOM_REQ);
	stCreateRoomPacket.WriteData(&i64TeamHeadUID,	sizeof(T_UID));
	stCreateRoomPacket.WriteData(&ui8MaxMemberCnt,	sizeof(UINT8));
	stCreateRoomPacket.WriteData(&m_TMatchingID,	sizeof(T_MatchingID));
	
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
		m_pMatchTeams[i]->MakePacketCreateRoom(&stCreateRoomPacket);

	return g_pGateway->SendPacket(i32ServerIdx, &stCreateRoomPacket) == TRUE;
}


//////////////////////////////////////////////////////////////////////////
// CMatcingManager
CMatchingManager::CMatchingManager()
: m_bRunning(false)
, m_i32ServerCount(0)
{
}

CMatchingManager::~CMatchingManager()
{
	Destroy(); 
}


bool CMatchingManager::Create(INT32 i32ServerCount)
{
	m_i32ServerCount			= i32ServerCount;

	if(  i32ServerCount <= 1 )	m_ui32UpdateTime = MATCHING_TIME_VALUE;
	else						m_ui32UpdateTime = MATCHING_TIME_VALUE / i32ServerCount;

	m_ui32LastUpdateServerIdx	= 0;

	for( INT32 i = 0; i < m_i32ServerCount; ++i )
	{
		m_pRequestBuffer[i] = new NSM_CRingBuffer;
		if( !m_pRequestBuffer[i] )
			return false;

		m_pRequestBuffer[i]->OnCreate(sizeof(MATCHING_REQUEST), CLAN_MATCH_BUFFER_SIZE);

		m_pResultBuffer[i] = new NSM_CRingBuffer;
		if( !m_pResultBuffer[i] )
			return false;

		m_pResultBuffer[i]->OnCreate(sizeof(MATCHING_REQUEST), CLAN_MATCH_BUFFER_SIZE);
	}

	if( FALSE == i3Thread::Create("MatchingManager") )
		return FALSE;
	
	return TRUE;
}

void CMatchingManager::Destroy()
{
	m_bRunning = false;
	i3Thread::WaitForFinish();

	for( INT32 i = 0; i < m_i32ServerCount; ++i )
	{
		I3_SAFE_DELETE(m_pRequestBuffer[i]);
		I3_SAFE_DELETE(m_pResultBuffer[i]);
	}
}

UINT32 CMatchingManager::OnRunning(void* pData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"MATMGR", 0, m_ThreadID);

	m_bRunning = true;
	while( m_bRunning )
	{
		DWORD dwOldTime = GetTickCount();

		// 1. 제한 시간이 초과한 요청 제거. 매칭 취소 팀도 여기서 제거됨
		_RemoveMatchRequest(m_ui32LastUpdateServerIdx);

		// 2. Task에서 push된 요청을 m_MatchRequest로 가져온다.
		_InsertMatchRequest(m_ui32LastUpdateServerIdx);

		// 3. m_MatchRequest의 요청들을 비교해 매칭시킴.
		_Matchmaking(m_ui32LastUpdateServerIdx);

		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// TIMEOVER
			g_pLog->WriteLog( L"[CMatchingManager::OnRunning()] Matching Time Over : %d / ServerIdx : %d ", dwNewTime, m_ui32LastUpdateServerIdx );
		}

		++m_ui32LastUpdateServerIdx %= m_i32ServerCount;

		::WaitForSingleObject( m_Handle, m_ui32UpdateTime);
	}

	return 0;
}

void CMatchingManager::_RemoveMatchRequest(UINT32 ui32ServerIdx)
{
	DATE32 dt32CurTime = i3ThreadTimer::LocalTime();

	for( INT32 LoopPerMax = 0; LoopPerMax < BUFFER_CNT_PER_MEMBER; ++LoopPerMax )	// 팀 인원수 종류 만큼( 4vs4 ~ 8vs8 )
	{
		i3BinList* pMatching = &m_MatchRequest[ui32ServerIdx][LoopPerMax];
		INT32 i32Count = pMatching->GetCount();
 
		for( INT32 LoopReq = 0; LoopReq < i32Count; ++LoopReq )	// 요청 갯수만큼
		{
			MATCHING_REQUEST* pReq = static_cast<MATCHING_REQUEST*>(pMatching->GetItem(LoopReq));

			if( !pReq ) continue;

			CMatchTeam* pMatchTeam = FindMatchTeam( ui32ServerIdx, pReq->m_TTeamID );

			if( !pMatchTeam ) continue;

			T_RESULT result = EVENT_ERROR_FAIL;
			// 1. 요청 시간 초과
			if( pReq->m_dt32MatchLimitTime < dt32CurTime ) 
			{
				result = EVENT_ERROR_CLAN_WAR_TIME_OUT;
			}
			else if( pMatchTeam->GetTeamState() == TEAM_STATE_CANCEL_MATCHING ) // 매치 취소
			{
				result = EVENT_ERROR_CLAN_WAR_CANCEL_MATCHMAKING;
			}
			else
			{
				continue;
			}

			MATCHING_RESULT stMatchingResult;
			stMatchingResult.m_ui8TeamCount = 1;
			stMatchingResult.m_TTeamID[0] = pReq->m_TTeamID;
			stMatchingResult.m_TResult = result;

			_ChangeTeamState( ui32ServerIdx, pReq->m_TTeamID, TEAM_STATE_WAIT );

			// 매칭 실패 전달
			m_pResultBuffer[ui32ServerIdx]->Push(&stMatchingResult);

			// 매칭 리스트에서 제거.
			pMatching->Remove(pReq);

		}

	}
}

void CMatchingManager::_InsertMatchRequest(UINT32 ui32ServerIdx)
{
	NSM_CRingBuffer* pBuffer = m_pRequestBuffer[ui32ServerIdx];

	INT32 i32BufferCount = pBuffer->GetBufferCount();

	for( INT32 i = 0; i < i32BufferCount; ++i )
	{
		MATCHING_REQUEST* pRequestTeam = static_cast<MATCHING_REQUEST*>( pBuffer->Pop() );
		if( pRequestTeam ) 
		{
			INT32 i32BufferIdx = pRequestTeam->m_ui8MaxMember - MIN_CLAN_MATCH_MEMBER;
			if( m_MatchRequest[ui32ServerIdx][i32BufferIdx].FindItem(pRequestTeam) < 0 )
			{
				m_MatchRequest[ui32ServerIdx][i32BufferIdx].Add(pRequestTeam);
			}			
		}

		pBuffer->PopIdx();			
	}
}


void CMatchingManager::_Matchmaking(UINT32 ui32ServerIdx)
{
	for( INT32 LoopPerMax = 0; LoopPerMax < BUFFER_CNT_PER_MEMBER; ++LoopPerMax )	// 팀 인원수 종류 만큼( 4vs4 ~ 8vs8 )
	{
		i3BinList* pMatching = &m_MatchRequest[ui32ServerIdx][LoopPerMax];
		INT32 i32Count = pMatching->GetCount();
		if( i32Count < 2 ) continue;

		MATCHING_REQUEST* pRed		= NULL;
		MATCHING_REQUEST* pBlue		= NULL; 
		MATCHING_REQUEST* pMatchedRed	= NULL;
		MATCHING_REQUEST* pMatchedBlue	= NULL; 
		MATCHING_REQUEST* pSameClanRed	= NULL;
		MATCHING_REQUEST* pSameClanBlue	= NULL; 
		BOOL	bMatchedTeam	= FALSE;
		BOOL	bSameClan		= FALSE;

		UINT32 ui32MinDiffPoint = 0xFFFFFFFF;	// 레드팀, 블루팀간 매칭 포인트 격차. 가장 적은 값을 찾아 냅니다.

		for( INT32 c = 0; c < i32Count - 1; ++c )	// 요청 갯수만큼
		{
			MATCHING_REQUEST* pTempRed	= NULL;
			MATCHING_REQUEST* pTempBlue = NULL; 
			pTempRed = static_cast<MATCHING_REQUEST*>( pMatching->GetItem(c) );
			if( !pTempRed ) continue;	// 나올수 있습니다. MatchingBuffer.Remove( pMatchingTeams[g] );에 의해


			for( INT32 d = c + 1; d < i32Count; ++d )
			{
				pTempBlue = static_cast<MATCHING_REQUEST*>( pMatching->GetItem(d) );
				if( !pTempBlue ) continue;	// 나올수 있습니다. MatchingBuffer.Remove( pMatchingTeams[g] );에 의해

//#ifndef CLAN_MATCH_MAKING
				// 동일 클랜에서 생성된 매치팀이면 continue.
				if( pTempRed->m_ui32ClanID == pTempBlue->m_ui32ClanID ) 
				{	// 적절한 매칭팀을 검색하지 못할 경우를 대비해 백업해 둡니다.
					pSameClanRed	= pTempRed;
					pSameClanBlue	= pTempBlue;
					bSameClan		= TRUE;
					continue;
				}
//#endif

				// 직전에 매칭된 팀이면 continue.
				if( pTempRed->m_TMatchedTeamID == pTempBlue->m_TTeamID || pTempBlue->m_TMatchedTeamID == pTempRed->m_TTeamID ) 
				{	// 적절한 매칭팀을 검색하지 못할 경우를 대비해 백업해 둡니다.
					pMatchedRed		= pTempRed;
					pMatchedBlue	= pTempBlue;
					bMatchedTeam	= TRUE;
					continue;
				} 

				UINT32 ui32DiffPoint = abs( pTempRed->m_i32MatchingPoint - pTempBlue->m_i32MatchingPoint );
				if( ui32MinDiffPoint <= ui32DiffPoint  ) continue;
				ui32MinDiffPoint = ui32DiffPoint;

				pRed = pTempRed;
				pBlue = pTempBlue;  
			}
		}

		// 버퍼별( 인원수 종류별 )로 최적의 매칭 2팀만 선별 합니다.

		if( ui32MinDiffPoint == 0xFFFFFFFF ) 
		{	// 매칭 실패시 백업된 매칭 확인.
			if( bMatchedTeam )
			{
				pRed	= pMatchedRed;
				pBlue	= pMatchedBlue;
			}
			else if( bSameClan )
			{
				pRed	= pSameClanRed;
				pBlue	= pSameClanBlue;
			}
			else continue;
		}


		MATCHING_RESULT stMatchingResult;
		stMatchingResult.m_TTeamID[TEAM_RED]		= pRed->m_TTeamID;
		stMatchingResult.m_TTeamID[TEAM_BLUE]		= pBlue->m_TTeamID;

		if( RINGBUFFER_SUCCESS == m_pResultBuffer[ui32ServerIdx]->Push(&stMatchingResult) )
		{	// 매칭된 두팀은 매칭 리스트에서 제거.
			pMatching->Remove( pRed );
			pMatching->Remove( pBlue ); 
		}
	}

	return;
}

void	CMatchingManager::_ChangeTeamState(UINT32 ui32ServerIdx, T_TeamID TTeamID, TEAM_STATE eStaate)
{
	CMatchTeam* pMatchTeam = FindMatchTeam( ui32ServerIdx, TTeamID );
	if( !pMatchTeam ) return;
		
	pMatchTeam->SetTeamState( eStaate );
	return;
}

CMatchTeam*	CMatchingManager::FindMatchTeam(UINT32 ui32ServerIdx, T_TeamID TTeamID)
{
	if( !g_pTaskProcessor ) return NULL;
	CTeamManager* pTeamManager	= g_pTaskProcessor->GetTeamManager(ui32ServerIdx);

	if( !pTeamManager ) return NULL;

	return pTeamManager->FindMatchTeam(TTeamID);
}

T_RESULT CMatchingManager::InsertMatchRequestBuffer(INT32 i32ServerIdx, MATCHING_REQUEST& request)
{
	INT32 i32BufferIdx = request.m_ui8MaxMember - MIN_CLAN_MATCH_MEMBER;

	if( i32BufferIdx >= BUFFER_CNT_PER_MEMBER ) 
		return EVENT_ERROR_CLAN_WAR_PER_COUNT;

	if( 0 <= m_MatchRequest[i32ServerIdx][i32BufferIdx].FindItem(&request) )
		return EVENT_ERROR_CLAN_MATCH_ALREADY_REQUESTED;

	if( m_pRequestBuffer[i32ServerIdx]->Push(&request) != RINGBUFFER_SUCCESS )
		return EVENT_ERROR_EVENT_BUFFER_FULL;

	return EVENT_ERROR_SUCCESS;
}

INT32 CMatchingManager::GetMatchingCount(INT32 i32ServerIdx)
{
	return m_pResultBuffer[i32ServerIdx]->GetBufferCount();
}

MATCHING_RESULT*	CMatchingManager::GetMatchingResult(INT32 i32ServerIdx)
{
	return static_cast<MATCHING_RESULT*>( m_pResultBuffer[i32ServerIdx]->Pop() );
}

void CMatchingManager::PopMatchingIndex(INT32 I32ServerIdx)
{	
	m_pResultBuffer[I32ServerIdx]->PopIdx();
}

