#ifndef _RESPAWNSTATE_H
#define _RESPAWNSTATE_H

class CRespawnState
{
public:
	void	SetPos			( UINT32 ui32SlotIdx, N_POS* pPos, UINT32 ui32RespawnIdx );			
	void	ResetPos		( UINT32 ui32SlotIdx );												
	BOOL	CheckRespawnPos	( UINT32 ui32SlotIdx, NxVec3* pRespawnPosInServer, UINT32 ui32RespawnIdx ); 
	BOOL	IsPosSet		( UINT32 ui32SlotIdx ) const	{
																I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT);
																if( ui32SlotIdx < SLOT_MAX_COUNT )
																	return GetBit( m_ui32FlagPos, ui32SlotIdx );
																return FALSE;
															}

	N_POS*	GetPos	( UINT32 ui32SlotIdx )				{
															I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT);
															if( ui32SlotIdx < SLOT_MAX_COUNT )
																return &m_aPos[ui32SlotIdx ];
															return NULL;
														 }

	CRespawnState();
	~CRespawnState();
	void Reset();


private:
	// position buffer
	UINT32  m_ui32FlagPos;
	N_POS	m_aPos			 [SLOT_MAX_COUNT];
	UINT32	m_aui32RespawnIdx[SLOT_MAX_COUNT]; 
};

#endif