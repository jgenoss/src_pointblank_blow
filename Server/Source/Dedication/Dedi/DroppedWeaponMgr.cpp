#include "pch.h"
#include "DroppedWeaponMgr.h"
#include "DediRoom.h"

void CDroppedWeapon::AddWeapon( UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon, REAL32 r32DestroyTime )
{
	m_ui8SlotIdx		= (UINT8)ui32SlotIdx;
	m_r32DestroyTime	= r32DestroyTime;
	m_TWeaponID			= pWeapon->getWeaponID();

	for( INT32 i = 0 ; i < WEAPON::EXTENSION_MAX ; i++ )
	{
		for( INT32 j = 0 ; j < WEAPON::HAND_TYPE_MAX ; j++ )
		{
			m_ui8LoadBullet[i][j]		= pWeapon->GetLoadBulletCount( (WEAPON::EXTENSION_TYPE)i, (WEAPON::HAND_TYPE)j );
			m_ui16StandbyBullet[i][j]	= pWeapon->GetStandbyBulletCount( (WEAPON::EXTENSION_TYPE)i, (WEAPON::HAND_TYPE)j );
		}
	}
}

BOOL CDroppedWeapon::GetWeapon( GAMEINFO_GETWEAPON* pWeapon, REAL32 r32GameTime, GAMEINFO_BULLET* pBullet )
{
	if( IsEmpty( r32GameTime ) )						return FALSE;				// 이미 사라진 총
	if( m_TWeaponID != pWeapon->getWeaponID() )			return FALSE;				// 내 총이 아니면

	//pBullet
	// udp relay가 fail 되지 않도록 리셋하지 않는다.(2012-08-27)
	Reset();
	
	return TRUE; 
}

BOOL CDroppedWeapon::UpdateWeapon( UINT32 ui32SlotIdx, GAMEINFO_DROPEDWEAPON_NETDATA* pData )
{
	if( ui32SlotIdx != m_ui8SlotIdx )					return FALSE;			// 본인 무기인지 체크

	pData->CopyTo( this );
	m_bUDPSend = TRUE;

	return TRUE;
}

BOOL CDroppedWeapon::IsGetWeapon( T_ItemID TItemID, REAL32 r32GameTime )
{
	if( TItemID != m_TWeaponID )						return FALSE;			// 잘못된 ID 가 왔다면
	if( NET_WEAPONDROPED_SLEEP != _ui8State )			return FALSE;			// 땅에 다 떨어지지 않았으면
	if( m_r32DestroyTime < r32GameTime	)				return FALSE;			// 사라 졌으면			
	
	return TRUE;
}

BOOL CDroppedWeapon::IsGetWeaponPos( VEC3D* pPos )
{
	if( 1.5f <= distance_squared( pPos, &_vPos ))		return FALSE;
	
	return TRUE;
}

// 라운드 시작시 호출
void CDroppedWeaponMgr::Reset( REAL32 r32GameTime )
{
	m_bWeaponDestroy		= TRUE;
	m_r32CheckTime			= r32GameTime;
	m_i32DroppedWeaponIdx	= 0;
	m_i32NextSendIdx		= 0;

	for( INT32 i = 0; i < MAX_DROPEDWEAPON_COUNT; i++ )
	{
		m_DroppedWeapons[i].Reset(); 
	}
}

void CDroppedWeaponMgr::SetOption( BOOL bDestroy )
{
	m_bWeaponDestroy = bDestroy;
}

INT32 CDroppedWeaponMgr::GetDropWeaponIdx( REAL32 r32GameTime )
{	
	INT32 i32CheckIdx = m_i32DroppedWeaponIdx + 1;
	INT32 i32OldIdx = 0;
	REAL32 r32OldDestroyTime = m_DroppedWeapons[ m_i32DroppedWeaponIdx ].GetDestroyTime();
	REAL32 r32CheckDestoyTime = 0.0f;

	for( INT32 i = 0; i < MAX_DROPEDWEAPON_COUNT-1; ++i )
	{
		if( i32CheckIdx >= MAX_DROPEDWEAPON_COUNT ) 
			i32CheckIdx = 0;

		if( m_DroppedWeapons[ i32CheckIdx ].IsEmpty( r32GameTime ) )	
			return m_i32DroppedWeaponIdx = i32CheckIdx;
		
		r32CheckDestoyTime = m_DroppedWeapons[ i32CheckIdx ].GetDestroyTime();
		if( r32OldDestroyTime > r32CheckDestoyTime )
		{
			i32OldIdx = i32CheckIdx;
			r32OldDestroyTime = r32CheckDestoyTime;
		}
		i32CheckIdx++;
	}
	// 빈자리가 없을 경우, 가장 오래된 무기를 없앰
	m_DroppedWeapons[i32OldIdx].SetEmpty();

	return m_i32DroppedWeaponIdx = i32OldIdx;
}

BOOL CDroppedWeaponMgr::AddDropWeapon( UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon, REAL32 r32GameTime, INT32 i32DropIndex )
{	
	if( FALSE == m_DroppedWeapons[ i32DropIndex ].IsEmpty( r32GameTime ) )		return FALSE;

	REAL32 r32DestroyTime;
	if( m_bWeaponDestroy )	r32DestroyTime = r32GameTime+DROPPED_WEAPON_DISAPPEAR_TIME;
	else					r32DestroyTime = (3.402823466E+38f);									// 방이 폭파될 때까지 무기는 사라지지 않습니다.

	m_DroppedWeapons[ i32DropIndex ].AddWeapon( ui32SlotIdx, pWeapon, r32DestroyTime );

	return TRUE;
}

BOOL CDroppedWeaponMgr::UpdateDropWeapon( UINT32 ui32SlotIdx, UINT32 ui32Idx, GAMEINFO_DROPEDWEAPON_NETDATA* pData )
{
	if( MAX_DROPEDWEAPON_COUNT <= ui32Idx )	return FALSE;
	
	return m_DroppedWeapons[ ui32Idx ].UpdateWeapon( ui32SlotIdx, pData );
}

// 바닥에서 사라진 무기일 경우 브로드캐스팅 되지 않도록 필터링만 수행.
BOOL CDroppedWeaponMgr::GetDropWeapon( GAMEINFO_GETWEAPON* pWeapon, REAL32 r32GameTime, GAMEINFO_BULLET* pBullet ) 
{
	INT32 i32DroppedSlot = pWeapon->getIndex();
	
	if ( i32DroppedSlot >= MAX_DROPEDWEAPON_COUNT )		return FALSE;
	
	return m_DroppedWeapons[ i32DroppedSlot ].GetWeapon( pWeapon, r32GameTime, pBullet );
}

BOOL CDroppedWeaponMgr::IsGetWeapon( UINT32 ui32Idx, T_ItemID TItemID, REAL32 r32GameTime )
{
	if ( MAX_DROPEDWEAPON_COUNT <= ui32Idx )			return FALSE;
	return m_DroppedWeapons[ ui32Idx ].IsGetWeapon( TItemID, r32GameTime );
}

BOOL	CDroppedWeaponMgr::IsGetWeaponPos( UINT32 ui32Idx, VEC3D* pPos )
{
	if ( ui32Idx >= MAX_DROPEDWEAPON_COUNT ) return FALSE;
	return m_DroppedWeapons[ ui32Idx ].IsGetWeaponPos( pPos );
}

UINT32 CDroppedWeaponMgr::MakePacket( char* pBuffer, UINT32 ui32MaxSize )
{
	UINT32 ui32SendPos = 0;
	UINT32 ui32WriteSize;
	for( INT32 i = 0 ; i < MAX_DROPEDWEAPON_COUNT ; i++ )
	{
		m_i32NextSendIdx++;
		if( MAX_DROPEDWEAPON_COUNT <= m_i32NextSendIdx )	m_i32NextSendIdx = 0;

		if( FALSE == m_DroppedWeapons[i].GetUDPSend() )	continue;

		ui32WriteSize = m_DroppedWeapons[i].PackFull( &pBuffer[ ui32SendPos ], ui32MaxSize-ui32SendPos, P2P_SUB_HEAD_DROPEDWEAPON, i );
		if( 0 == ui32WriteSize )	return ui32SendPos;

		ui32SendPos += ui32WriteSize;

		m_DroppedWeapons[i].SetUDPSend( FALSE );
	}

	return ui32SendPos;
}