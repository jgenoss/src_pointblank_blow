#include "pch.h"
#include "DSObject.h"
#include "DediRoom.h"

CDSObject::CDSObject()
{
}

CDSObject::~CDSObject()
{
}

void CDSObject::Init()
{
	m_eUseType				= OBJECT_USE_TYPE_NONE;
	m_TObjectType			= 0;
	m_TObjectSyncType		= 0;
	m_i16ExplosionHP[0]		= 0;
	_EventSender			= 0xFF;
	_EnterCharaIdx			= 0xFF;
	_fRespawnTime			= 0.0f; 
	_nShellCount			= 0;
	m_bTimerEnable			= FALSE; 
	m_bItemStartTimerEnable = FALSE;
	m_ui32StartTime			= 0;
	m_bPathKeyChanged		= false;
	m_vec16OrgPos			= m_vCenterPos;
	m_i8TeamIdx				= -1;
	//_nMaxShellCount			= m_i32DSMaxShellCount;

	m_bAnimRepeat			= TRUE;

	m_fActionDistance		= 1.f;
}

void CDSObject::Reset()								
{
	_nHP					= m_i16MaxHP;
	_nShellCount			= 1;
	_EventSender			= 0xFF;
	_EnterCharaIdx			= 0xFF;
	_fRespawnTime			= 0.0f;
	m_ui8PosState			= 0;
	_nState					= 0;
	m_r32LastSendTime		= 0.f;
	m_r32StateChangeTime	= 0.f;
	_nPathKey				= 0;
	ResetPosion();
}

void CDSObject::CopyTo( CDSObject* pObject )
{
	pObject->m_r32StateChangeTime			= m_r32StateChangeTime;
	pObject->m_r32FirstAnimationSyncTime	= m_r32FirstAnimationSyncTime;
	pObject->m_eSendHeadType				= m_eSendHeadType;
	pObject->m_ui8PosState					= m_ui8PosState;
	pObject->m_vec16OrgPos					= m_vec16OrgPos;
	pObject->m_eUseType						= m_eUseType;
	pObject->m_i8TeamIdx					= m_i8TeamIdx;
	pObject->m_bUDPSend						= m_bUDPSend;
	pObject->m_bTimerEnable					= m_bTimerEnable;
	pObject->m_bItemStartTimerEnable		= m_bItemStartTimerEnable;
	pObject->m_ui32StartTime				= m_ui32StartTime;
	pObject->m_r32StartTime					= m_r32StartTime;
	pObject->m_bPathKeyChanged				= m_bPathKeyChanged;
	pObject->m_rKeyFrame					= m_rKeyFrame;
	pObject->m_r32LastSendTime				= m_r32LastSendTime;
	pObject->m_TObjectType					= m_TObjectType;
	pObject->m_TObjectSyncType				= m_TObjectSyncType;
	pObject->m_ui8PathKeyMax				= m_ui8PathKeyMax;
	pObject->m_eResetPosition				= m_eResetPosition;
	pObject->m_fActionDistance				= m_fActionDistance;
	i3mem::Copy( pObject->m_i16ExplosionHP, m_i16ExplosionHP, sizeof(INT16)*3 );
		
	GAMEINFO_OBJECT_NETDATA::CopyTo( pObject );
	DS_GAME_OBJECT::CopyTo( (DS_GAME_OBJECT*)&pObject->m_iSType );
}

void CDSObject::OnUpdate()
{
}

void CDSObject::ResetUse( N_POS* pvec32Pos )			
{
	_EnterCharaIdx	= 0xFF;
	_nShellCount	= m_i32MaxShellCount;

	OBJECT_RESET_POSITION		eRespawnPos = m_eResetPosition;
	if( NULL == pvec32Pos )		eRespawnPos = OBJECT_RESET_POSITION_RESPAWN;

	switch( eRespawnPos )
	{
	case OBJECT_RESET_POSITION_RESPAWN:			ResetPosion();						break;
	case OBJECT_RESET_POSITION_DIRECT:			SetPosition( pvec32Pos );			break;
	}

	m_bUDPSend = TRUE;
}

BOOL CDSObject::SetUse( UINT32 ui32SlotIdx )
{
	if( 0xFF != _EnterCharaIdx ) return FALSE;
	_EnterCharaIdx		= ui32SlotIdx;
	m_ui8PosState		= OBJECT_POS_STATE_ON;
	m_bUDPSend			= TRUE;
	return TRUE;
}

void CDSObject::SetRespawnTimer( REAL32 r32Time )
{
	if( r32Time > m_r32RespawnTime )	_fRespawnTime = m_r32RespawnTime;
	else								_fRespawnTime = r32Time;
}

REAL32 CDSObject::GetDistanceSpace( N_POS* pPosition )
{
	VEC3D v3dExplosion;
	v3dExplosion.x = _vPos.x;
	v3dExplosion.y = _vPos.y;
	v3dExplosion.z = _vPos.z;

	VEC3D v3dTargetPos;
	v3dTargetPos.x = pPosition->x;
	v3dTargetPos.y = pPosition->y;
	v3dTargetPos.z = pPosition->z;

	VEC3D vDir;
	i3Vector::Sub(&vDir, &v3dTargetPos, &v3dExplosion);

	VEC3D JumpHold;
	JumpHold.x = 0;
	JumpHold.y = m_r32JumpHoldHeight;
	JumpHold.z = m_r32JumpHoldPower;
	
	VEC3D JumpPos;
	REAL32 jumpholdspeed = i3Vector::Length(&JumpHold);
	i3Vector::Scale(&JumpPos, &v3dExplosion, jumpholdspeed);	
	REAL32 jumpholdDistance = i3Vector::Length(&JumpPos) * 0.016;		
	return jumpholdDistance - i3Vector::Length( &vDir );
	
}
REAL32 CDSObject::GetDistance(N_POS* pPosition)
{
	VEC3D v3dExplosion;
	v3dExplosion.x = _vPos.x;
	v3dExplosion.y = _vPos.y;
	v3dExplosion.z = _vPos.z;

	VEC3D v3dTargetPos;
	v3dTargetPos.x = pPosition->x;
	v3dTargetPos.y = pPosition->y;
	v3dTargetPos.z = pPosition->z;
	
	VEC3D vDir;
	i3Vector::Sub(&vDir, &v3dTargetPos, &v3dExplosion);

	return i3Vector::Length(&vDir);
}

BOOL CDSObject::CheckActionDistance( N_POS* pPosition )
{
	if (m_fActionDistance < GetDistance(pPosition))
	{
		DSHACK("CDSObject::[CheckActionDistance] Long Distance : %f\n", GetDistance(pPosition));
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDSObject::CheckActionDistanceSpace(N_POS* pPosition)
{
	if (GetDistanceSpace(pPosition) < 0)	return FALSE;

	return TRUE;
}

BOOL CDSObject::MakePacket( char* pBuffer, INT32 i32MaxSize, UINT32& ui32WriteSize, SEND_PACKET_TYPE ePacketType )
{
	ui32WriteSize		= 0;
	_ObjType			= 0;

	switch( ePacketType )
	{
	case SEND_PACKET_TYPE_LIVE:		_ObjType = m_TObjectType;			break;
	case SEND_PACKET_TYPE_SYNC:		_ObjType = m_TObjectSyncType;		break;
	}
	if( 0 == _ObjType )		return TRUE;

	if( i32MaxSize < sizeof( N_GAME_SUBHEAD ) )							return FALSE;

	ui32WriteSize = PackPacket( pBuffer + sizeof(N_GAME_SUBHEAD), i32MaxSize-sizeof(N_GAME_SUBHEAD) );
	if( 0 == ui32WriteSize )	return FALSE;

	ui32WriteSize += UDP_UTIL::BuildSubHead( pBuffer, P2P_SUB_HEAD_OBJECT, m_i16NetIdx, ui32WriteSize + sizeof(N_GAME_SUBHEAD) );

	return TRUE;
}

INT32 CDSObject::ProcessUseItem(INT32 UseItemProbability, CDediRoom* pRoom)
{
	// 여기서 확률을 가지고 세팅합니다.
	INT32 probability = 0;
	for (INT32 i = 0; i < m_ui8UseItemCount; i++)
	{
		// 확률을 더한다.
		probability += m_ui32UseItemProbability[i];
		
		// 확률 계산
		if (UseItemProbability < probability)
		{
			if (FALSE == _UseItem(pRoom, _GetUseItemType(i), i))
			{
				DSMSG("[ProcessUseItem] Use Item Fail!!! \n");
				return -1;
			}

			// State를 OFF로 바꿉니다.
			_nState = OBJECT_USE_ITEM_STATE_OFF;
			m_ui32StartTime = (UINT32)GetTickCount();
			m_bTimerEnable = TRUE;
			m_bUDPSend = TRUE;

			return i;
		}
	}
		
	return -1;
}

BOOL CDSObject::_UseItem(CDediRoom* pRoom, OBJECT_USE_ITEM_TYPE eType, INT32 UseItemidx)
{
	switch (eType)
	{
		case OBJECT_USE_ITEM_WEAPON:
		{
			CCharacter* pChara = pRoom->GetGameCharacter(_EnterCharaIdx);
			if (pChara != NULL)
			{
				WEAPON_SLOT_TYPE eSlot = (WEAPON_SLOT_TYPE)(GET_ITEM_TYPE(m_ui32UseItemID[UseItemidx]) - 1);
				if( !pChara->IsFullSlot(eSlot) ) // 슬롯이 비어 있을 경우에는 바로 무기를 장착합니다.
				{
					pChara->PickUpWeapon( m_ui32UseItemID[UseItemidx] );
					// RPG일 경우에는 따로 체크 필요.
					if( Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7) == m_ui32UseItemID[UseItemidx] )
					{
						pChara->SetNaturalRPGGet(TRUE);
					}
				}
					
			}
		}break;
		// HP 회복
		case OBJECT_USE_ITEM_HPUP:
		{
			if (pRoom != NULL)
			{
				// 캐릭터에 50을 투약합니다.
				CCharacter* pChara = pRoom->GetGameCharacter(_EnterCharaIdx);
				if (pChara != NULL)
				{
					pChara->AddHP(50);
					pRoom->SendLife(_EnterCharaIdx, P2P_INFO_FLAG_HITBYCHARA);
				}
			}
		}break;
	}

	return TRUE;
}

OBJECT_USE_ITEM_TYPE CDSObject::_GetUseItemType(INT32 UseItemidx)
{
	// 실제 사용하는 아이템의 type을 우선 체크
	ITEM_TYPE itemtype = (ITEM_TYPE)GET_ITEM_TYPE(m_ui32UseItemID[UseItemidx]);

	if (ITEM_TYPE_WRAP_COUNT == itemtype)
	{
		switch (m_ui32UseItemID[UseItemidx])
		{
			// 아이템 아이디를 박는 부분
			// HP
			case CASHITEM_ITEMID_HP_RECOVERY:
			{
				return OBJECT_USE_ITEM_HPUP;
			}break;
			// Gravity
			case CASHITEM_ITEMID_GRAVITY_RESOTRE:
			{
				return OBJECT_USE_ITEM_RECOVERY_GRAVITY;
			}break;
			// Unbeatable
			case CASHITEM_ITEMID_CHARA_UNBEATABLE:
			{
				return OBJECT_USE_ITEM_UNBEATABLE;
			}break;
		}
	}
	// 무기 인지 체크
	else if (ITEM_TYPE_UNKNOWN < itemtype && itemtype < ITEM_TYPE_CHARA)
	{
		return OBJECT_USE_ITEM_WEAPON;
	}

	DSMSG("[_GetUseItemType] Not Item Type Index %d\n", UseItemidx);
	return OBJECT_USE_ITEM_NONE;
}