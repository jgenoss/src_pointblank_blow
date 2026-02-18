#ifndef __THROW_WEAPON_MGR_H__
#define __THROW_WEAPON_MGR_H__

class CThrowWeapon : public GAMEINFO_THROW_WEAPON_NETDATA
{
	UINT32					m_ui32SlotIdx;
	BOOL					m_bUDPSend;

public:
	CThrowWeapon();
	~CThrowWeapon();

	void					Reset()
	{
		GAMEINFO_THROW_WEAPON_NETDATA::Reset();
		m_bUDPSend			= FALSE;	
	}

	BOOL					GetUDPSend()								{	return m_bUDPSend;	}
	void					SetUDPSend( BOOL bSend )					{	m_bUDPSend = bSend;	}

	BOOL					IsThrow( GAMEINFO_THROW_WEAPON_NETDATA* pThrow, BOOL& bFirstPacket, N_PCINFO_HIT_RAYCHECK* pRay );
	BOOL					AddThrow( UINT32 ui32SlotIdx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow, BOOL& bFirstPacket );

};

class CThrowWeaponMgr
{
	UINT32					m_ui32Count;
	CThrowWeapon			m_ThrowWeapon[ MAX_THROWWEAPON_COUNT ];

	INT32					m_i32NextSendIdx;

public:
	CThrowWeaponMgr();
	~CThrowWeaponMgr();

	void					Reset();
	BOOL					IsThrow( UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow, BOOL& bFirstPacket, N_PCINFO_HIT_RAYCHECK* pRay )
	{
		return m_ThrowWeapon[ ui16Idx ].IsThrow( pThrow, bFirstPacket, pRay );
	}
	BOOL					AddThrow( UINT32 ui32SlotIdx, UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow, BOOL& bFirstPacket )
	{
		return m_ThrowWeapon[ ui16Idx ].AddThrow( ui32SlotIdx, pThrow, bFirstPacket );
	}
	BOOL					DeleteThrow( UINT16 ui16Idx )					{	return TRUE;									}

	UINT32					MakePacket( char* pBuffer, UINT32 ui32MaxSize );
};

#endif