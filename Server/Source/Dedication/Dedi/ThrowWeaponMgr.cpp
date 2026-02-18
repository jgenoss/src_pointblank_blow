#include "pch.h"
#include "ThrowWeaponMgr.h"

CThrowWeapon::CThrowWeapon()
{
}

CThrowWeapon::~CThrowWeapon()
{
}

BOOL CThrowWeapon::IsThrow( GAMEINFO_THROW_WEAPON_NETDATA* pThrow, BOOL& bFirstPacket, N_PCINFO_HIT_RAYCHECK* pRay )
{
	if( (0 == GetState() || 
		NET_GRENADE_STATE_EXPLOSION == GetState())&&
		NET_GRENADE_STATE_EXPLOSION != pThrow->GetState() )
	{
		bFirstPacket	= TRUE;
	}
	else
	{
		bFirstPacket	= FALSE;
	}

	// 첫 패킷이 아니며, 바로 앞에 던져진 위치와만 비교합니다.
	// 만약 중간에 패킷이 빠졌을 경우는 오탐이 발생 할 수 있으므로 따로 체크 하지 않습니다.
	if( (FALSE == bFirstPacket) && 
		(getFlyCount() + 1 == pThrow->getFlyCount()) )
	{
		pRay->m_bCheckRay				= TRUE;
		pRay->_tWeapon.ui32WeaponID		= pThrow->GetWeapon()->getWeaponID();
		pRay->_tWeapon.State			= pThrow->GetState();
		i3mem::Copy( &pRay->_HitPos,	pThrow->GetPos(),	sizeof(VEC3D) );
		i3mem::Copy( &pRay->_stFirePos,	GetPos(),			sizeof(VEC3D) );		
	}
#ifdef _DEBUG
	if (GetState() & NET_GRENADE_STATE_TAKEOBJECT)
	{
		DSMSG("[GRENADE] AttachPos : %.2f, %.2f, %.2f\n", pThrow->GetAttachPos()->x, pThrow->GetAttachPos()->y, pThrow->GetAttachPos()->z);
			//		pRay->m_bCheckRay, getFlyCount(), pThrow->getFlyCount(), pThrow->GetState(), GetPos()->x, GetPos()->y, GetPos()->z, pThrow->GetPos()->x, pThrow->GetPos()->y, pThrow->GetPos()->z);
	}

	//DSMSG("[GRENADE] bCheckRay = %d, pre-flyCount = %d, flyCount = %d\n GrenadeState = %#x\t firPos(%.2f, %.2f, %.2f), hitPos(%.2f, %.2f, %.2f) \n",
//		pRay->m_bCheckRay, getFlyCount(), pThrow->getFlyCount(), pThrow->GetState(), GetPos()->x, GetPos()->y, GetPos()->z, pThrow->GetPos()->x, pThrow->GetPos()->y, pThrow->GetPos()->z);
#endif
	return TRUE;
}

BOOL CThrowWeapon::AddThrow( UINT32 ui32SlotIdx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow, BOOL& bFirstPacket )
{
	// 이전 상태가 초기화이거나 폭발 상태일 때 폭발이 아닌 값이 들어오면 맨 처음 들어온 패킷으로 간주합니다.
	if( (0 == GetState() || 
		NET_GRENADE_STATE_EXPLOSION == GetState())&&
		NET_GRENADE_STATE_EXPLOSION != pThrow->GetState() )
	{
		bFirstPacket	= TRUE;
	}
	else
	{
		bFirstPacket	= FALSE;
	}

	pThrow->CopyTo( this );
	m_ui32SlotIdx	= ui32SlotIdx;
	m_bUDPSend		= TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// ThrowWeaponMgr
////////////////////////////////////////////////////////////////////////
CThrowWeaponMgr::CThrowWeaponMgr()
{
	m_ui32Count = 0;
}

CThrowWeaponMgr::~CThrowWeaponMgr()
{
}

void CThrowWeaponMgr::Reset()
{
	m_ui32Count				= 0;

	m_i32NextSendIdx		= 0;

	for( INT32 i = 0 ; i < MAX_THROWWEAPON_COUNT ; i++ )
	{
		m_ThrowWeapon[i].Reset();
	}
}

UINT32 CThrowWeaponMgr::MakePacket( char* pBuffer, UINT32 ui32MaxSize )
{
	UINT32 ui32SendPos = 0;
	UINT32 ui32WriteSize;
	for( INT32 i = 0 ; i < MAX_THROWWEAPON_COUNT ; i++ )
	{
		m_i32NextSendIdx++;
		if( MAX_THROWWEAPON_COUNT <= m_i32NextSendIdx )	m_i32NextSendIdx = 0;

		if( FALSE == m_ThrowWeapon[i].GetUDPSend() )	continue;

		ui32WriteSize = m_ThrowWeapon[i].PackFull( &pBuffer[ ui32SendPos ], ui32MaxSize-ui32SendPos, P2P_SUB_HEAD_GRENADE, i );
		if( 0 == ui32WriteSize )	return ui32SendPos;

		ui32SendPos += ui32WriteSize;

		m_ThrowWeapon[i].SetUDPSend( FALSE );
	}

	return ui32SendPos;
}