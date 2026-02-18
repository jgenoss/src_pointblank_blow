// 리스폰 해킹을 방어하기 위한 클래스
// 그룹에 1개씩 할당된다
class CRespawnMgr 
{
public:
	// 팀 인덱스는 0부터 시작합니다
	INT32	GetRespawnCount( INT32 i32Team )	{
					switch( i32Team )
					{
						case 0: return m_ui32RedCount;
						case 1: return m_ui32BlueCount;
						case 2: return m_ui32RedNpcCount;
						case 3: return m_ui32BlueNpcCount;
						default: I3ASSERT(0);
					};
					return 0;
			}
	const NxVec3*	GetRespawnPosition( INT32 i32Team, INT32  i32Idx) {
					switch( i32Team )
					{
						case 0: 
								if( m_ui32RedCount )
								{
									i32Idx = i32Idx % m_ui32RedCount;
									return &m_avRed[i32Idx];
								}
								return NULL;
						case 1: 
								if( m_ui32BlueCount )
								{
									i32Idx = i32Idx % m_ui32BlueCount;
									return &m_avBlue[i32Idx];
								}
								return NULL;
						case 2: 
								if( m_ui32RedNpcCount )
								{
									i32Idx = i32Idx % m_ui32RedNpcCount;
									return &m_avRedNpc[i32Idx];
								}
								return NULL;
						case 3: 
								if( m_ui32BlueNpcCount )
								{
									i32Idx = i32Idx % m_ui32BlueNpcCount;
									return &m_avBlueNpc[i32Idx];
								}
								return NULL;
						default: I3ASSERT(0); return NULL;
					};
			}

	BOOL	AddRespawnPosition( UINT8 ui8TeamType, NxVec3& pos) ;
	void	Reset();
	
	CRespawnMgr();
private:
	NxVec3	m_avRed		[ DS_MAX_RESPAWN_PER_TEAM ];
	NxVec3	m_avBlue	[ DS_MAX_RESPAWN_PER_TEAM ];
	NxVec3	m_avRedNpc  [ DS_MAX_RESPAWN_PER_TEAM ];
	NxVec3	m_avBlueNpc [ DS_MAX_RESPAWN_PER_TEAM ];

	UINT32	m_ui32RedCount;
	UINT32	m_ui32BlueCount;
	UINT32	m_ui32RedNpcCount;
	UINT32	m_ui32BlueNpcCount;
};