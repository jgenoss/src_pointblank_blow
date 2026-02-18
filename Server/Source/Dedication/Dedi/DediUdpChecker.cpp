#include "pch.h"
#include "DediUdpChecker.h"
#include "DediRoom.h"
#include "DroppedWeaponMgr.h"
#include "WeaponTable.h"

#define READ_DATA( data, type, size )	\
{\
	if( (i32TotalReadPos + ((INT32)size)) > i32TotalSize )		return 0;\
	data = (type*)&pPacket[ i32TotalReadPos ];\
	i32TotalReadPos += size;\
}

// 훈련전이 아닌 모드에서 size와 index 검사를 한다
// 일부 sub packet이 잘못되는 경우 패킷의 일부분만 처리되는 이슈 때문에 처리 전에 패킷을 검사
// 즉, 버려지는 패킷은 모두 여기서 버려야 한다 (return 0)
INT32 UDPChecker::GamePacketParsing( CDediRoom* pRoom, const char* pPacket, INT32 i32TotalSize, RECV_PACKET_INFO* pRecvInfo )
{
	if( pRoom->IsGMPause() )
	{	// GM Pasue 일 경우 어떠한 패킷도 적용하지 않습니다.
		return 0;
	}

	INT32	i32TotalReadPos			= 0;
	INT32	i32ReadSize;
	INT32	i32SubReadSize			= 0; // sub packet size

	UINT32	ui32SlotIdx				= pRecvInfo->m_pHeader->_Index;
	UINT32	ui32RespawnCount		= pRecvInfo->m_pHeader->_RespawnCount;
	UINT32	ui32TouchDownCount		= pRecvInfo->m_pHeader->_TouchDownCount;	
	INT32	i32DiffRespawnCount		= (INT32)(pRoom->GetRespawnCount(ui32SlotIdx) - ui32RespawnCount ); 
	INT32	i32DiffTouchDownCount	= (INT32)(pRoom->GetTouchDownCount(ui32SlotIdx) - ui32TouchDownCount ); 
	
	if( SLOT_MAX_COUNT <= ui32SlotIdx )
	{
		DSWARNING( "UDPChecker: slot idx error \n");
		return 0;
	}
	
	pRecvInfo->m_i32EventCount	= 0;

	RECV_PACKET_SUB_INFO* pSubInfo;
	while ( i32TotalReadPos <  i32TotalSize )
	{
		pSubInfo = &pRecvInfo->m_SubInfo[ pRecvInfo->m_i32EventCount ];
		pRecvInfo->m_i32EventCount++;

		READ_DATA( pSubInfo->m_pSubHead, N_GAME_SUBHEAD, sizeof(N_GAME_SUBHEAD) );
	
		switch( pSubInfo->m_pSubHead->_Type )
		{
		case P2P_SUB_HEAD_USER :

			// index
			if( (UINT32)pSubInfo->m_pSubHead->_Index != ui32SlotIdx )
			{
				DSWARNING( "[UDPChecker::P2P_SUB_HEAD_USER] invalid chara info index..%d\n", pSubInfo->m_pSubHead->_Index );
				return 0; 
			}
			
			// info flag
			i32SubReadSize		+= pSubInfo->m_pSubHead->_Code;

			READ_DATA( pSubInfo->m_pChara_Def, N_PCINFO_DEF2, sizeof(N_PCINFO_DEF2) );
		
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CHARASTATE )
			{
				READ_DATA( pSubInfo->m_pChara_State, N_PCINFO_CHARA_STATE, sizeof(N_PCINFO_CHARA_STATE) );
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
			{
				i32ReadSize = pSubInfo->m_TakingObject.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				if( !_ParseCharaTakingObject( pRoom, ui32SlotIdx, &pSubInfo->m_TakingObject ) )
				{
					DSWARNING( "UDPChecker:_ParseCharaTakingObject() fail\n");
					return 0;
				}				
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
			{
				READ_DATA( pSubInfo->m_pChara_PosRot, N_PCINFO_POSROTATION, sizeof(N_PCINFO_POSROTATION) );

				if( !_ParseCharaPosRotPacket( pRoom, ui32SlotIdx, pSubInfo->m_pChara_PosRot ) )
				{
					DSWARNING( "UDPChecker:_ParseCharaPosRotPacket() fail\n");
					return 0;
				}
				
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ACTIONKEY ) 
			{
				i32ReadSize = pSubInfo->m_ActionKey.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;
				
				switch( pRoom->ParseCharaActionKey( ui32SlotIdx, &pSubInfo->m_ActionKey ) )
				{
				case PACKET_CHECK_FAIL:
					DSWARNING( "UDPChecker:_ParseCharaActionKey() fail\n");
					return 0;
				case PACKET_CHECK_DELETE:
					pSubInfo->m_pChara_Def->iInfoFlag &= ~P2P_INFO_FLAG_ACTIONKEY;
					break;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
			{
				i32ReadSize = pSubInfo->m_SyncObject.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;
				
				if( !_ParseCharaSyncObj( pRoom, ui32SlotIdx, &pSubInfo->m_SyncObject ) )
				{
					DSWARNING( "UDPChecker:_ParseCharaSyncObj() fail\n");
					return 0;
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_RADIO ) 
			{
				i32ReadSize = pSubInfo->m_RadioChat.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				if( !_ParseCharaRadioChat( pRoom, ui32SlotIdx, &pSubInfo->m_RadioChat ) )
				{
					DSWARNING( "UDPChecker:_ParseCharaRadioChat() fail\n");
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_WEAPONPARAM )
			{
				READ_DATA( pSubInfo->m_pChara_WeaponParam, N_PCINFO_WEAPON_PARAM, sizeof(N_PCINFO_WEAPON_PARAM) );
				
				//if( !_ParseCharaWeaponParam( pRoom, ui32SlotIdx, pSubInfo->m_pCastPacket_Chara_WeaponParam ) )
				//{
				//	DSWARNING( "UDPChecker:_ParseCharaWeaponParam() fail\n");
				//	return 0;
				//}
			}
		
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
			{
				DSWARNING( "UDPChecker: packet info flag Error at P2P_INFO_FLAG_CS_LIFE\n");
				return 0;
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
			{
				i32ReadSize = pSubInfo->m_SuicideDamage.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				if( FALSE == _ParseCharaSuicideDmg( pRoom, ui32SlotIdx, &pSubInfo->m_SuicideDamage ) )
				{
					DSWARNING( "UDPChecker:_ParseCharaSuicideDmg() fail\n");
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_MISSION)
			{
				i32ReadSize = pSubInfo->m_Mission.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				if( !_ParseMissionPacket( pRoom, ui32SlotIdx, &pSubInfo->m_Mission ) )
				{
					DSWARNING( "UDPChecker:_ParseMissionPacket() fail\n");
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
			{
				i32ReadSize = pSubInfo->m_DropWeapon.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				if( ! _ParseDropWeaponPacket( pRoom, ui32SlotIdx, &pSubInfo->m_DropWeapon ) ) 
				{
					DSWARNING( "UDPChecker:_ParseDropWeaponPacket() fail\n");
					return 0;
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
			{
				i32ReadSize = pSubInfo->m_GetWeapon.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				switch( _ParseGetWeaponPacket( pRoom, ui32SlotIdx, &pSubInfo->m_GetWeapon ) )
				{
				case PACKET_CHECK_FAIL:
					DSWARNING( "UDPChecker:_ParseGetWeaponPacket() fail\n");
					return 0;
				case PACKET_CHECK_DELETE:
					pSubInfo->m_pChara_Def->iInfoFlag &= ~P2P_INFO_FLAG_GETWEAPON;
					break;
				}
			}

			if (pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
			{
				i32ReadSize = pSubInfo->m_DirectPickUp.Unpack(&pPacket[i32TotalReadPos]);
				if (i32TotalReadPos + i32ReadSize > i32TotalSize)			return 0;
				i32TotalReadPos += i32ReadSize;

				if (!_ParseDirectPickUpPacket(pRoom, ui32SlotIdx, &pSubInfo->m_DirectPickUp))
				{
					DSWARNING("UDPChecker:_ParseDirectPickUpPacket() fail\n");
					return 0;
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_FIRE)
			{
				i32ReadSize = pSubInfo->m_Fire.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;

				if( FALSE == _ParseFirePacket( pRoom, ui32SlotIdx, &pSubInfo->m_Fire ) )
				{
					DSWARNING( "UDPChecker:_ParseBulletPacket() fail\n");
					return 0;
				}
			}
			
			// HIT가 들어오면 안된다
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HIT )
			{
				DSWARNING( "UDPChecker: packet info flag Error at P2P_INFO_FLAG_HIT\n");
				return 0;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYCHARA )
			{
				READ_DATA( pSubInfo->m_pui8Chara_Hit_ByChara, UINT8, sizeof(UINT8) );

				READ_DATA( pSubInfo->m_pChara_Hit_ByChara, N_PCINFO_HIT_DEDICATED2, (sizeof(N_PCINFO_HIT_DEDICATED2)*(*pSubInfo->m_pui8Chara_Hit_ByChara)) );

				if( !_ParseHitPacketByChara( pRoom, ui32SlotIdx, pSubInfo->m_pChara_Hit_ByChara, *pSubInfo->m_pui8Chara_Hit_ByChara ) ) 
				{
					DSWARNING( "UDPChecker:_ParseHitPacketByChara() fail\n");
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION ) 
			{
				READ_DATA( pSubInfo->m_pui8Chara_Hit_ByExplosion, UINT8, sizeof(UINT8) );

				READ_DATA( pSubInfo->m_pChara_Hit_ByExplosion, N_PCINFO_HIT_BYEXPLOSION, (sizeof(N_PCINFO_HIT_BYEXPLOSION)*(*pSubInfo->m_pui8Chara_Hit_ByExplosion)) );

				if( ! _ParseHitPacketByExplosion( pRoom, ui32SlotIdx, pSubInfo->m_pChara_Hit_ByExplosion, *pSubInfo->m_pui8Chara_Hit_ByExplosion )) 
				{
					DSWARNING( "UDPChecker:_ParseHitPacketByExplosion() fail\n");
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYOBJECT ) 
			{
				READ_DATA( pSubInfo->m_pui8Chara_Hit_ByObject, UINT8, sizeof(UINT8) );

				READ_DATA( pSubInfo->m_pChara_Hit_ByObject, N_PCINFO_HIT_BYOBJECT, (sizeof(N_PCINFO_HIT_BYOBJECT)*(*pSubInfo->m_pui8Chara_Hit_ByObject)) );

				if( ! _ParseHitPacketByObject( pRoom, ui32SlotIdx, pSubInfo->m_pChara_Hit_ByObject, *pSubInfo->m_pui8Chara_Hit_ByObject )) 
				{
					DSWARNING( "UDPChecker:_ParseHitPacketByObject() fail\n");
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_DEATH )
			{
				DSWARNING( "UDPChecker: packet info flag Error at P2P_INFO_FLAG_CS_DEATH\n");
				return 0;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING )
			{
				DSWARNING( "UDPChecker: packet info flag Error at P2P_INFO_FLAG_CS_SUFFERING\n");
				return 0;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_TOUCHDOWN )
			{
				i32ReadSize = pSubInfo->m_TouchDown.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;
			}
			break;

		case P2P_SUB_HEAD_GRENADE :
			{
				i32SubReadSize		+= pSubInfo->m_pSubHead->_Code;

				if( (i32DiffRespawnCount < 0 || i32DiffRespawnCount > 1) ||
					(i32DiffTouchDownCount < 0 || i32DiffTouchDownCount > 1) ) 
				{
#ifdef _DEBUG
					DSWARNING( "[UDPChecker::P2P_SUB_HEAD_GRENADE] invalid respawn count...\n");
#endif 
					return 0;
				}
				
				i32ReadSize = pSubInfo->m_ThorwWeapon.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;


				switch( pRoom->ParseGrenadePacket( ui32SlotIdx, pSubInfo->m_pSubHead->_Index, &pSubInfo->m_ThorwWeapon ) )
				{
				case PACKET_CHECK_FAIL:
					DSWARNING( "UDPChecker:_ParseGrenadePacket() fail\n");
					return 0;
				}	
			}
			break;

		case P2P_SUB_HEAD_DROPEDWEAPON :
			{

				i32SubReadSize		+= pSubInfo->m_pSubHead->_Code;

				if( (i32DiffRespawnCount < 0 || i32DiffRespawnCount > 1) ||
					(i32DiffTouchDownCount < 0 || i32DiffTouchDownCount > 1) ) 
				{
#ifdef _DEBUG
					DSWARNING( "[UDPChecker::P2P_SUB_HEAD_DROPEDWEAPON] invalid respawn count..\n");
#endif 
					return 0;
				}

				if( pSubInfo->m_pSubHead->_Index >= MAX_DROPEDWEAPON_COUNT )
				{
					DSWARNING( "[UDPChecker::P2P_SUB_HEAD_DROPEDWEAPON] invalid index..%d\n", pSubInfo->m_pSubHead->_Index );
					return 0;
				}
				
				i32ReadSize = pSubInfo->m_DroppedWeapon.Unpack( &pPacket[ i32TotalReadPos ] );
				if( i32TotalReadPos + i32ReadSize > i32TotalSize )			return 0;
				i32TotalReadPos += i32ReadSize;
								
				if( ! _ParseDroppedWeaponPacket( pRoom, ui32SlotIdx, &pSubInfo->m_DroppedWeapon ) )
				{
					DSWARNING( "UDPChecker:_ParseDroppedWeaponPacket() fail\n");
					return 0;
				}
			}
			break; 

		default :
			DSWARNING( "[UDPChecker::_GamePacketParsing()] Invalid recv packet type..\n");
			return 0;
			break;
		} //switch(packet->Type)
	}

	if( i32SubReadSize != i32TotalReadPos )
	{
		DSWARNING( "[UDPChecker::P2P_SUB_HEAD_USER] parsing fail..head->_Code:%d, nReadPos:%d\n", i32SubReadSize, i32TotalReadPos );
		return 0;
	}

	return i32TotalReadPos;
}

BOOL	UDPChecker::_ParseCharaTakingObject( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_TAKING_OBJECT* pObject )
{
	if( FALSE == pObject->getOnLoadObject() )								return TRUE;
	if( pObject->getOnLoadObjectIndex() < pRoom->m_i32GameObjectCount )		return TRUE;

	return FALSE;
}

BOOL UDPChecker::_ParseCharaPosRotPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pPosRot )
{
	BOOL bValid = TRUE;
	
	if( IsOverflow( &pPosRot->_Pos ) )		 bValid = FALSE;
	if( !IsFiniteNumber( pPosRot->iTheta ) ) bValid = FALSE;
	if( !IsFiniteNumber( pPosRot->iRho ) )   bValid = FALSE;
		
	return bValid;
}

BOOL UDPChecker::_ParseCharaSyncObj( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_SYNC_OBJECT* pObject )
{
	return ( pObject->getIndex() < pRoom->m_i32GameObjectCount);
}

BOOL UDPChecker::_ParseCharaRadioChat( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_RADIO_CHAT* pRadioChat )
{
	return ( pRadioChat->getRadio() < (RADIOCHAT_TYPE_COUNT * 10));
}


BOOL UDPChecker::_ParseCharaWeaponInfo ( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON* pWeapon )
{
	if( NULL == Table::GetWeaponInfo( pWeapon->GetWeaponID() ) )	return FALSE;
	return TRUE;
}

BOOL UDPChecker::_ParseCharaWeaponParam( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pParam )
{
	if( NULL == Table::GetWeaponInfo( pParam->_tWeapon.GetWeaponID() ) )	return FALSE;

	return ( ( pParam->iClass < MAX_WEAPON_PARAM )			&& 
			 ( pParam->iCount <= MAX_WEAPON_PARAM_ARGS)	);
}


BOOL UDPChecker::_ParseCharaSuicideDmg( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_SUICIDEDAMAGE* pSuicideDmg )
{
	if( pSuicideDmg->getCount() > MAX_SUICIDE_COUNT )						return FALSE;

	for( INT32 i = 0; i < pSuicideDmg->getCount() ; i++ )
	{
		if( pSuicideDmg->getDeathType(i)	>= CHARA_DEATH_MAXCOUNT )		return FALSE;
		if( pSuicideDmg->getHitPart(i)		>= CHARA_HIT_MAXCOUNT )			return FALSE;
		if( pSuicideDmg->GetWeaponClass(i)	>= WEAPON_CLASS_COUNT )			return FALSE;
		if( pSuicideDmg->GetWeaponUsage(i)	>= WEAPON_SLOT_COUNT )			return FALSE;
		if( IsOverflow( (VEC3D*)pSuicideDmg->getHitDirection(i) ) )			return FALSE;
	}
		
	return TRUE;
}


BOOL  UDPChecker::_ParseMissionPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_MISSION* pMission )
{
	if( 0 >= pRoom->getChara_Life( ui32SlotIdx )->iLife )				return FALSE; // 죽은 유저의 패킷은 무시
	if( FALSE == pRoom->GetGameCharacter( ui32SlotIdx )->GetAlive() )	return FALSE; // 리스폰하지 않은 유저의 패킷은 무시

	NET_MISSION_STATE	state	= pMission->GetState();
	BOMB_AREA			area	= (BOMB_AREA)pMission->GetArea();
	//REAL32		r32Time = pMission->r32StartTime;
	
	return ( (state < NET_MISSION_STATE_MAXCOUNT )	&&
			 (area  < MAX_BOMB_AREA_COUNT) );
}

PACKET_CHECK UDPChecker::_ParseGetWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_GETWEAPON* pWeapon )
{
	if( 0 == pWeapon->getWeaponClass() )					return PACKET_CHECK_FAIL;
	if( WEAPON_CLASS_COUNT <= pWeapon->getWeaponClass() )	return PACKET_CHECK_FAIL;

	// Weapon Idx 가 정상인지
	UINT32 ui32Idx	= pWeapon->getIndex();
	if( ui32Idx >= MAX_DROPEDWEAPON_COUNT )					return PACKET_CHECK_FAIL;

	CCharacter* pChara	= pRoom->GetGameCharacter( ui32SlotIdx );
	INT32 i32Usage = pWeapon->getWeaponUsage();

	if( i32Usage == WEAPON_SLOT_UNKNOWN )
	{
		return PACKET_CHECK_DELETE;
	}

	if( pChara->IsFullSlot( (UINT8)i32Usage ) )
	{	// 더 이상 가질 수 없다.
		return PACKET_CHECK_DELETE;
	}
	if( FALSE == pRoom->IsGetDropWeapon( ui32Idx, pWeapon->getWeaponID(), pRoom->GetGameTime() ) )
	{	// 주울수 없는 상태이다.
		return PACKET_CHECK_DELETE;
	}

	return PACKET_CHECK_SUCCESS;
}

BOOL UDPChecker::_ParseDirectPickUpPacket(CDediRoom * pRoom, UINT32 ui32SlotIdx, GAMEINFO_DIRECTPICKUP * pWeapon)
{
	// -----------------------------------------------------------------------
	// weapon 검사
	// -----------------------------------------------------------------------
	WeaponInfo* pInfo = Table::GetWeaponInfo(pWeapon->getDropWeapon()->getWeaponID());
	if (NULL == pInfo)
	{
		// 해킹일 경우
		DSWARNING("ERR. UDPChecker::_ParseDirectPickUpPacket WeaponID = %d\n", pWeapon->getDropWeapon()->getWeaponID());
		return FALSE;
	}

	INT32 i32DropIdx = pRoom->GetDropWeaponIdx(pRoom->GetGameTime());
	pWeapon->getDropWeapon()->setIndex(i32DropIdx);

	return TRUE;
}

BOOL UDPChecker::_ParseDropWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon )
{
	// -----------------------------------------------------------------------
	// weapon 검사
	// -----------------------------------------------------------------------
	T_ItemID TItemID = pWeapon->getWeaponID();
	WeaponInfo* pInfo = Table::GetWeaponInfo( TItemID );	
	if( NULL == pInfo )
	{
		// 해킹일 경우
		DSERROR( "ERR. UDPChecker::_ParseDropWeaponPacket WeaponID = %d\n", TItemID );
		return FALSE;
	}

	UINT32 ui32Usage = Weapon::GetUsageFromItemID( TItemID );
	UINT8 ui8MultiWeaponIdx = pWeapon->GetMultiWeaponSlot();
	// 유효한 무기인지 검사
	CCharacter* pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	if( TItemID != pChara->GetOwnWeapon(ui32Usage, ui8MultiWeaponIdx ) )
	{
		DSERROR("UDPChecker::_ParseDropWeaponPacket [%d] NOT OWN WEAPON! ITEMID : %d, MultiWeaponIdx : %d\n", ui32SlotIdx, TItemID, ui8MultiWeaponIdx );
		pWeapon->setSuccess(0);
		return FALSE;
	}

	INT32 i32DropIdx = pRoom->GetDropWeaponIdx( pRoom->GetGameTime() );
	pWeapon->setIndex( i32DropIdx );

	return TRUE;
}

BOOL	UDPChecker::_ParseFirePacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire )
{
	const GAMEINFO_CHARA_WEAPON* pWeapon = pFire->getWeapon();
	WeaponInfo* pInfo = Table::GetWeaponInfo( pWeapon->getWeaponID() );	
	if( NULL == pInfo )
	{
		// 해킹일 경우
		DSERROR( "ERR. UDPChecker::_ParseFirePacket WeaponID = %d\n", pWeapon->getWeaponID() );
		return FALSE;
	}	
	if( WEAPON::ATTACK_TYPE_MAX < pWeapon->GetAttackType() )
	{
		// 해킹일 경우
		DSERROR( "ERR. UDPChecker::_ParseFirePacket WeaponID = %d, AttackType = %d\n", pWeapon->getWeaponID(), pWeapon->GetAttackType() );
		return FALSE;
	}
	
	return ( IsOverflow( pFire->getTargetPosition() ) == FALSE );
}

BOOL UDPChecker::_ParseHitPacketByChara( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pArrHit, UINT32 ui32Count )
{
	if( ui32Count == 0 || ui32Count > MAX_FIREHIT_COUNT )
		return FALSE;
		
	for( UINT32 i = 0; i < ui32Count; ++i )
	{
		N_PCINFO_HIT_DEDICATED2* pHitInfo = &pArrHit[i];

		if( IsOverflow(&pHitInfo->_stFirePos) )																				return FALSE;
		if( IsOverflow(&pHitInfo->_HitPos) )																				return FALSE;
		if( pHitInfo->_tWeapon.GetWeaponClass() >= WEAPON_CLASS_COUNT )														return FALSE;
		if( pHitInfo->_tWeapon.GetWeaponClass() == 0 )																		return FALSE;
		if( pHitInfo->_tWeapon.GetWeaponUsage() >= WEAPON_SLOT_COUNT )														return FALSE;
		if( pHitInfo->_tDefault.GetHitType() == GHT_CHARA   && pHitInfo->_tDefault.GetIndex()	>= SLOT_MAX_COUNT		)	return FALSE;
		if( pHitInfo->_tDefault.GetHitType() == GHT_CHARA   && pHitInfo->_tDefault.GetHitPart() >= CHARA_HIT_MAXCOUNT	)	return FALSE;
		if( pHitInfo->_tDefault.GetHitType() == GHT_OBJECT  && pHitInfo->_tDefault.GetIndex()   >= DS_MAX_OBJECT		)	return FALSE;
	}

	return TRUE;
}

BOOL	UDPChecker::_ParseHitPacketByExplosion( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_BYEXPLOSION* pArrHit, UINT32 ui32Count )
{
	if( ui32Count == 0 || ui32Count > MAX_FIREHIT_COUNT )
		return FALSE;
	
	for( UINT32 i = 0; i < ui32Count; ++i )
	{
		N_PCINFO_HIT_BYEXPLOSION* pExplosionInfo = &pArrHit[i];

		if( IsOverflow(&pExplosionInfo->_stExplosionPos) )																				return FALSE;
		if( MAX_THROWWEAPON_COUNT <= pExplosionInfo->GetNetSlotIdx() )																	return FALSE;
		if( pExplosionInfo->_stWeapon.GetWeaponClass() == 0 || WEAPON_CLASS_COUNT <= pExplosionInfo->_stWeapon.GetWeaponClass() )		return FALSE;
		if( WEAPON_SLOT_COUNT <= pExplosionInfo->_stWeapon.GetWeaponUsage() )															return FALSE;
		if( IsOverflow( &pExplosionInfo->_Direction ) )																					return FALSE;
		if( pExplosionInfo->_tDefault.GetHitType() == GHT_CHARA   && pExplosionInfo->_tDefault.GetIndex()	>= SLOT_MAX_COUNT		)	return FALSE;
		if( pExplosionInfo->_tDefault.GetHitType() == GHT_CHARA   && pExplosionInfo->_tDefault.GetHitPart() >= CHARA_HIT_MAXCOUNT	)	return FALSE;
		if( pExplosionInfo->_tDefault.GetHitType() == GHT_OBJECT  && pExplosionInfo->_tDefault.GetIndex()   >= DS_MAX_OBJECT		)	return FALSE;
	}

	return TRUE;
}

BOOL	UDPChecker::_ParseHitPacketByObject( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_BYOBJECT* pArrHit, UINT32 ui32Count )
{
	if( ui32Count == 0 || ui32Count > MAX_FIREHIT_COUNT )
		return FALSE;
	
	for( UINT32 i = 0; i < ui32Count; ++i )
	{
		N_PCINFO_HIT_BYOBJECT* pHitInfo = &pArrHit[i];

		if( pRoom->m_i32GameObjectCount <= pHitInfo->GetByObjectIndex() )													return FALSE;
		if( IsOverflow( &pHitInfo->_Direction ) )																			return FALSE;
		if( pHitInfo->_tDefault.GetHitType() == GHT_CHARA   && pHitInfo->_tDefault.GetIndex()	>= SLOT_MAX_COUNT		)	return FALSE;
		if( pHitInfo->_tDefault.GetHitType() == GHT_CHARA   && pHitInfo->_tDefault.GetHitPart() >= CHARA_HIT_MAXCOUNT	)	return FALSE;
		if( pHitInfo->_tDefault.GetHitType() == GHT_OBJECT  && pHitInfo->_tDefault.GetIndex()   >= DS_MAX_OBJECT		)	return FALSE;
	}

	return TRUE;
}

BOOL UDPChecker::_ParseDroppedWeaponPacket	( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DROPEDWEAPON_NETDATA* pDropInfo )
{
	if( IsOverflow( pDropInfo->getPosition() ) ) return FALSE;
	if( IsOverflow( pDropInfo->getRotation() ) ) return FALSE;
	return TRUE;
}