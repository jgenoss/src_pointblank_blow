#ifndef _GRENADE_STATE_H
#define _GRENADE_STATE_H

#include "Logfile.h"

struct DS_THROWN_GRENADE
{
	N_PCINFO_HIT_WEAPON	m_weapon;		// k400, k413, c5 
	N_POS				m_pos;			// 정지 위치
	UINT8				m_ui8State;		// 상태
	UINT8				m_ui8RespawnCount;	

	void Reset()
	{
		m_weapon.Reset();
		m_pos.Reset();
		m_ui8State = 0;
		m_ui8RespawnCount = 0;
	}

	DS_THROWN_GRENADE()
	{
		Reset();
	}

	void InitThrownInfo( UINT32 ui32RespawnCount, UINT16 ui16NetIdx, GAMEINFO_THROW_WEAPON_NETDATA* pThrowInfo )
	{
		//I3ASSERT( pThrowInfo != NULL );
		////I3ASSERT( pThrowInfo->State & NET_GRENADE_STATE_FLY );

		//m_ui8State	 = pThrowInfo->GetState();
		//pThrowInfo->GetWeapon()->CopyTo( &m_weapon );
		//pThrowInfo->GetPos().CopyTo( &m_pos );
		//I3ASSERT( ui32RespawnCount < 0xff );
		//m_ui8RespawnCount = (UINT8)(ui32RespawnCount & 0xff);
	}

	void UpdateThrownPos( UINT16 ui16NetIdx,  GAMEINFO_THROW_WEAPON_NETDATA* pThrowInfo )
	{
		//I3ASSERT( pThrowInfo != NULL );

		//m_ui8State	 = pThrowInfo->GetState();

		//// 상태검사 제거
		//pThrowInfo->_Pos.CopyTo( &m_pos );
	}
};

#endif 