#ifndef __PHYSICS_PROTOCOL_H__
#define __PHYSICS_PROTOCOL_H__

// -----------------------------------------------------
// Dedication Server와 Physics Server 간의 UDP 패킷 정의
// -----------------------------------------------------

// death 시에만 체크하기 때문에 1개만 사용된다
struct PHYSICS_PACKET_HEADER
{
	T_UID			m_uid;				// user id
	INT16			m_i16GroupIdx;		// group index
	UINT8			m_ui8SlotIdx;		// user slot index
	UINT8			m_ui8StageUID;		// stage ord
	UINT8			m_ui8RoundNum;		// round number
	UINT8			m_ui8RespawnCount;	// respawn count

	void Reset()
	{
		m_uid				= 0ULL;
		m_i16GroupIdx		= -1;	
		m_ui8SlotIdx		= (UINT8)-1;	
		m_ui8StageUID		= 0;
		m_ui8RoundNum		= 0;
		m_ui8RespawnCount   = 0;
	}
	
	PHYSICS_PACKET_HEADER()
	{
		Reset();
	}
	
	void	CopyTo( PHYSICS_PACKET_HEADER * pTarget ) 
	{
		pTarget->m_uid				= m_uid;
		pTarget->m_i16GroupIdx		= m_i16GroupIdx;		
		pTarget->m_ui8SlotIdx		= m_ui8SlotIdx;		
		pTarget->m_ui8StageUID		= m_ui8StageUID;
		pTarget->m_ui8RoundNum		= m_ui8RoundNum;
		pTarget->m_ui8RespawnCount = m_ui8RespawnCount;
	};

	UINT16	CopyToPacket( char * pPacket )
	{
		CopyTo( (PHYSICS_PACKET_HEADER*) pPacket);
		return (UINT16) sizeof(PHYSICS_PACKET_HEADER);
	};
};

// PHYSICS_PACKET_REQ, PHYSICS_PACKET_ACK 삭제
// REQ시 HEADER + N_PCINFO_HIT_DEDICATED2.
// ACK시 HEADER + UINT32 결과값

#endif