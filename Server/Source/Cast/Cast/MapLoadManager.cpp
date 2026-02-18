#include "pch.h"
#include "MapLoadManager.h"

CMapLoadManager::CMapLoadManager(void)
{	
	m_ui32BattleServerCount	= 0;
	m_ui8StageCount			= 0;
	m_ui8OpenStageCount		= 0;
	m_pServerInfo			= NULL;
}

CMapLoadManager::~CMapLoadManager(void)
{
	Destroy();
}

BOOL CMapLoadManager::Create(UINT32 ui32BattleServerCount, UINT8 ui8StageTotalCount, MAP_STAGE_DATA* pStages)
{
	m_ui32BattleServerCount = ui32BattleServerCount;
		
	i3mem::FillZero( m_aStages, sizeof(MAP_STAGE_DATA) * STAGE_UID_MAX );
	
	I3MEM_SAFE_FREE(m_pServerInfo);
	m_pServerInfo = (ServerLoadInfo*)i3MemAlloc(sizeof(ServerLoadInfo) * m_ui32BattleServerCount);
	i3mem::FillZero( m_pServerInfo, (sizeof(ServerLoadInfo) * m_ui32BattleServerCount));

	ui8StageTotalCount = MIN(ui8StageTotalCount, STAGE_UID_MAX);
	
	ui8StageTotalCount = MIN(ui8StageTotalCount, STAGE_UID_MAX);

	i3mem::Copy( m_aStages, pStages, sizeof(MAP_STAGE_DATA) * ui8StageTotalCount );

	m_ui8StageCount = ui8StageTotalCount;

	return TRUE;
}

BOOL CMapLoadManager::Destroy(void)
{
	I3MEM_SAFE_FREE(m_pServerInfo);
	
	return TRUE;
}

BOOL CMapLoadManager::MakeMapLoadList()
{	
	m_ui8OpenStageCount = 0;

	for( UINT8 i = 0; i < m_ui8StageCount; ++i )
	{
		if (TRUE == m_aStages[i].m_bIsOpen)
			m_ui8OpenStageCount++;
	}
		
	if( m_ui8OpenStageCount > DEDI_RESOURCE_MAX )	//로드 해야할 맵이 64개를 넘는경우 각 데디서버에 분산해서 로드합니다.
	{
		//그룹에 Dedi서버가 충분하지 않은경우 실패처리 합니다.
		if( m_ui32BattleServerCount * DEDI_RESOURCE_MAX < m_ui8OpenStageCount )
		{				
			g_pLog->WriteLog( L"[CMapLoadManager::MakeMapLoadList] Servercount : %d, Mapcount : %d", m_ui32BattleServerCount, m_ui8OpenStageCount );
			g_pLog->WriteLog( L"[CMapLoadManager::MakeMapLoadList] One Dedication Server can load 64 maps.");
			g_pLog->WriteLog( L"[CMapLoadManager::MakeMapLoadList] Dedication Server is not Enough. MapLoading Fail.");
			//return FALSE;
		}

		UINT32 ui32StageUID = 1;
		ServerLoadInfo* pServerInfo = NULL;

		//Group별 데디서버수에 맞게 분배합니다.
		while(!IsDivideFinish())
		{
			pServerInfo = GetAvailableServer( ui32StageUID );

			if( NULL != pServerInfo )
			{
				SetMapLoad( ui32StageUID, pServerInfo->m_aMapLoad );
				pServerInfo->m_ui16MapCount++;
			}

			++ui32StageUID;

			if( ui32StageUID >= STAGE_UID_MAX ) ui32StageUID = 1;
		};
		
	}
	else
	{
		bool	aMapLoad[STAGE_UID_MAX];
		i3mem::FillZero( aMapLoad, sizeof(bool) * STAGE_UID_MAX );

		for( UINT32 i = 0 ; i < STAGE_UID_MAX ; ++i )
		{
			SetMapLoad( i, aMapLoad );
		}

		for( UINT32 i = 0 ; i < m_ui32BattleServerCount ; ++i )
		{
			i3mem::Copy( m_pServerInfo[i].m_aMapLoad, aMapLoad, sizeof(bool) * STAGE_UID_MAX );
		}

	}

	return TRUE;
}

ServerLoadInfo* CMapLoadManager::GetAvailableServer( UINT32 ui32StageUID )
{	
	ServerLoadInfo* pResult = NULL;

	//로드되지 않는 맵은 생략합니다.
	if( FALSE == m_aStages[ui32StageUID].m_bIsOpen ) return NULL;

	for( UINT32 i = 0 ; i < m_ui32BattleServerCount ; ++i )
	{		
		
		//최대 리소스만큼 이미 할당된경우 대상에서 제외한다.
		if ( m_pServerInfo[i].m_ui16MapCount >= DEDI_RESOURCE_MAX ) continue;

		//해당서버에 이미 할당된 맵인지 확인한다.
		if ( m_pServerInfo[i].m_aMapLoad[ui32StageUID] ) continue;

		//할당된 서버가 없는 경우 우선 지정한다.
		if ( NULL == pResult )
		{
			pResult = &m_pServerInfo[i];
		}//현재까지 검색한 서버들중에 로드되는 맵수가 가장 작은지 검사한다.
		else if( pResult->m_ui16MapCount < m_pServerInfo[i].m_ui16MapCount )
		{
			pResult = &m_pServerInfo[i];
		}	
	}

	return pResult;
}