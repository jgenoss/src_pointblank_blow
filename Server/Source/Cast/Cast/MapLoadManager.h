#pragma once

#define	DEDI_RESOURCE_MAX	64

struct ServerLoadInfo
{	
	UINT16		m_ui16MapCount;				//로딩한 맵 수
	bool		m_aMapLoad[STAGE_UID_MAX];	//StageUID별 맵로드여부
};

class CMapLoadManager
{

private:	
	UINT32				m_ui32BattleServerCount;

	UINT8				m_ui8StageCount;			//전체 맵 리소스 Count
	UINT8				m_ui8OpenStageCount;		//로드할 맵 리소스 Count
	MAP_STAGE_DATA		m_aStages[STAGE_UID_MAX];	//맵별 사용률, 현재 사용횟수 저장	
	ServerLoadInfo*		m_pServerInfo;				//각 데디서버별 MapLoad정보
	
public:
	CMapLoadManager(void);
	~CMapLoadManager(void);

	BOOL	Create( UINT32 ui32BattleServerCount, UINT8	ui8StageTotalCount, MAP_STAGE_DATA* pStages );
	BOOL	Destroy(void);
	
	inline BOOL IsMapOpen( INT32 i32ServerIdx, STAGE_UID eStageUID )
	{
		if ( m_ui32BattleServerCount <= static_cast<UINT32>(i32ServerIdx) ) return false;
		if ( STAGE_UID_MAX <= eStageUID ) return false;

		return m_pServerInfo[i32ServerIdx].m_aMapLoad[eStageUID];
	}
	
	void CopyToPacket( INT32 i32Serveridx, MAP_STAGE_DATA* pOut, UINT32 ui32BufLen )
	{
		I3ASSERT( i32Serveridx < static_cast<INT32>(m_ui32BattleServerCount) );
		I3ASSERT( pOut != NULL );
		I3ASSERT( ui32BufLen >= sizeof(BOOL) * STAGE_UID_MAX );
		if( ui32BufLen < sizeof(BOOL) * STAGE_UID_MAX )
		{
			i3mem::FillZero( pOut, ui32BufLen );
			return;
		}
		
		for (INT32 i = 0; i < STAGE_UID_MAX; ++i)
		{
			pOut[i].m_bIsOpen = m_pServerInfo[i32Serveridx].m_aMapLoad[i];
			i3String::Copy(pOut[i].m_strCodeName, m_aStages[i].m_strCodeName, STAGE_CODE_NAME_COUNT);
		}
		
	}

	//해당 Dedi서버에 지정된 맵을 로드하도록 Set합니다.
	inline void SetMapLoad( UINT32 ui32StageUID, bool* aMapLoad )
	{
		//로드되지 않는 맵은 생략합니다.
		if( FALSE == m_aStages[ui32StageUID].m_bIsOpen ) return;

		aMapLoad[ui32StageUID] = true;
	}

	BOOL IsDivideFinish()
	{	

		for( UINT32 i = 0 ; i < m_ui32BattleServerCount ; ++i )
		{
			if ( m_pServerInfo[i].m_ui16MapCount < DEDI_RESOURCE_MAX ) return FALSE;
		}

		return TRUE;
	}

	ServerLoadInfo* GetAvailableServer( UINT32 ui32StageUID );
	BOOL MakeMapLoadList();

};