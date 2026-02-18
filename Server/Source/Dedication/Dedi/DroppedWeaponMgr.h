#pragma once

#define DROPPED_WEAPON_DISAPPEAR_TIME	15.0f

class CDroppedWeapon : public GAMEINFO_DROPEDWEAPON_NETDATA
{
	T_ItemID				m_TWeaponID;
	UINT8					m_ui8LoadBullet[ WEAPON::EXTENSION_MAX ][ WEAPON::HAND_TYPE_MAX ];
	UINT16					m_ui16StandbyBullet[ WEAPON::EXTENSION_MAX ][ WEAPON::HAND_TYPE_MAX ];

	UINT8					m_ui8SlotIdx;					// 무기를 버린 유저
	REAL32					m_r32DestroyTime;				// 이 시간 이후 사라집니다
	
	BOOL					m_bUDPSend;

public:
	CDroppedWeapon()
	{
		Reset();
	}
	~CDroppedWeapon()	{};

	void					Reset() 
	{
		GAMEINFO_DROPEDWEAPON_NETDATA::Reset();

		m_TWeaponID			= 0;
		m_ui8SlotIdx		= 0xFF;
		m_r32DestroyTime	= 0.0f; 
		m_bUDPSend			= FALSE;
		i3mem::FillZero( m_ui8LoadBullet,		sizeof(m_ui8LoadBullet) );
		i3mem::FillZero( m_ui16StandbyBullet,	sizeof(m_ui16StandbyBullet) );
	};

	BOOL					GetUDPSend()								{	return m_bUDPSend;	}
	void					SetUDPSend( BOOL bSend )					{	m_bUDPSend = bSend;	}

	BOOL					IsEmpty( REAL32 r32GameTime )				{	return m_r32DestroyTime < r32GameTime ? TRUE : FALSE;	}
	void					SetEmpty()									{	m_r32DestroyTime = 0.0f; }
	REAL32					GetDestroyTime()							{	return m_r32DestroyTime; }
	void					AddWeapon( UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon, REAL32 r32GameTime );
	BOOL					UpdateWeapon( UINT32 ui32SlotIdx, GAMEINFO_DROPEDWEAPON_NETDATA* pData );
	BOOL					GetWeapon( GAMEINFO_GETWEAPON* pWeapon, REAL32 r32GameTime, GAMEINFO_BULLET* pBullet );
	BOOL					IsGetWeapon( T_ItemID TItemID, REAL32 r32GameTime );
	BOOL					IsGetWeaponPos( VEC3D* pPos );
};

// dropped weapon을 관리하기 위한 클래스
class CDroppedWeaponMgr 
{
	BOOL					m_bWeaponDestroy;								// 시간이 지나 무기가 사라질지 아닐지에 대해 체크
	CDroppedWeapon			m_DroppedWeapons[ MAX_DROPEDWEAPON_COUNT ];
	INT32					m_i32DroppedWeaponIdx;
	REAL32					m_r32CheckTime;

	INT32					m_i32NextSendIdx;
	
public:

	CDroppedWeaponMgr() { }
	~CDroppedWeaponMgr() { }
	
	void					Reset( REAL32 r32GameTime );
	void					SetOption( BOOL bDestroy );

	INT32					GetDropWeaponIdx( REAL32 r32GameTime );
	BOOL					AddDropWeapon( UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon, REAL32 r32GameTime, INT32 i32DropIndex );		// 두번 드롭되지 않도록
	BOOL					UpdateDropWeapon( UINT32 ui32SlotIdx, UINT32 ui32Idx, GAMEINFO_DROPEDWEAPON_NETDATA* pData );					// 패킷을 업데이트 한다.
	BOOL					GetDropWeapon( GAMEINFO_GETWEAPON* pWeapon, REAL32 r32GameTime, GAMEINFO_BULLET* pBullet );						// 없는 무기 일경우 브로드캐스팅 되지 않도록

	BOOL					IsGetWeapon( UINT32 ui32Idx, T_ItemID TItemID, REAL32 r32GameTime );
	BOOL					IsGetWeaponPos( UINT32 ui32Idx, VEC3D* pPos );
	
	UINT32					MakePacket( char* pBuffer, UINT32 ui32MaxSize );
};

