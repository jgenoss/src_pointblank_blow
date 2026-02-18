#include "pch.h"
#include "DediRoom.h"

PACKET_CHECK CDediRoom::ParseCharaActionKey( UINT32 ui32SlotIdx, GAMEINFO_ACTIONKEY* pActionKey )
{
	// Client -> Server 일 경우 0 번 idx 에만 값이 담겨 옵니다.
	if( ( pActionKey->getActionKey(0) & NET_ACTIONKEY_USEOBJECT )				||  
	    ( pActionKey->getActionKey(0) & NET_ACTIONKEY_USE_MOVE_CHARA_OBJECT )	||			
 		( pActionKey->getActionKey(0) & NET_ACTIONKEY_USEITEMBOX )	||	
		( pActionKey->getActionKey(0) & NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT) ||
		( pActionKey->getActionKey(0) & NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT ) 
	  )
	{
		if( IsConvoyMode() &&  !g_pConfig->GetCheckLevel(HACK_TYPE_USURPATION) ) 
		{
			return PACKET_CHECK_SUCCESS;
		}

		if( pActionKey->getUseActionObjectIndex(0) >= m_i32GameObjectCount )
		{
			return PACKET_CHECK_FAIL;
		}
	}
	else
	{
		return PACKET_CHECK_DELETE;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	// 여기서 부터 해킹 체크
	CCharacter* pChara = &m_pGameCharacters[ ui32SlotIdx ];
	
	N_PCINFO_WEAPON weapon; 
	weapon.Reset();

	CDSObject* pObject;
	for( INT32 i = 0 ; i < pActionKey->getActionCount() ; i++ )
	{
		if(  ( pActionKey->getActionKey(i) & NET_ACTIONKEY_USEOBJECT) ||  
			 ( pActionKey->getActionKey(i) & NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT ))
		{
			pObject = GetGameObject( pActionKey->getUseActionObjectIndex(i) );
			if( NULL == pObject )																return PACKET_CHECK_FAIL;
			if( FALSE == pObject->CheckActionDistance( pChara->GetPosRot()->GetPos() ) )
			{
				DSERROR( "[ERR] UDPChecker::ParseCharaActionKey Distance ObjectIdx = %d\n", pActionKey->getUseActionObjectIndex(i) );
				return PACKET_CHECK_FAIL;
			}

			if( pObject->GetSType() & OBJECT_S_TYPE_HELICOPTER )
			{
				if( pObject->GetCanUse( (UINT8)ui32SlotIdx ) )
				{				
					pChara->SetUseM197(TRUE);
				}
			}
			// WeaponChange 검사: 웨폰 박스를 든 경우 예외처리를 해줘야 합니다
			else if ( pObject->GetSType() == OBJECT_S_TYPE_W_BOX )  
			{
				if( 0 == pObject->getShellCount() )
				{
					return PACKET_CHECK_FAIL;
				}

				T_ItemID TItemID;
				WEAPON_SLOT_TYPE eSlot;
				if( FALSE == ChangeAvailableWeapon( (EWEAPON_TYPE)pObject->m_eWeaponType, TItemID, eSlot ) )
				{
					return PACKET_CHECK_FAIL;
				}

				WeaponInfo* pWeaponInfo = Table::GetWeaponInfo( TItemID );
				if( NULL == pWeaponInfo )	return PACKET_CHECK_FAIL;

				// CGameWeaponManager::Create( i3Stage * pStage) 참고
				switch( (EWEAPON_TYPE)pObject->m_eWeaponType )
				{
				case WT_CROWNWATERMELON:  
					{
						// 탈취물 획득 가능한지 팀체크.
						if( TEAM_RED != GetTeamType(ui32SlotIdx) ) return PACKET_CHECK_FAIL;
					}
					break;
				}
			}
		}
		else if( ( pActionKey->getActionKey(i) & NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT) )
		{
			pObject = GetGameObject( pActionKey->getUseActionObjectIndex(i) );
			if( NULL == pObject )																return PACKET_CHECK_FAIL;
			if( FALSE == pObject->CheckActionDistance( pChara->GetPosRot()->GetPos() ) )		return PACKET_CHECK_FAIL;

			if( pObject->GetSType() & OBJECT_S_TYPE_HELICOPTER )
			{
				pChara->SetUseM197(FALSE);
			}
		}
		else if ((pActionKey->getActionKey(i) & NET_ACTIONKEY_USE_MOVE_CHARA_OBJECT))
		{
			pObject = GetGameObject(pActionKey->getUseActionObjectIndex(i));
			if (NULL == pObject)																return PACKET_CHECK_FAIL;				
			if (FALSE == pObject->CheckActionDistanceSpace(pChara->GetPosRot()->GetPos()))		return PACKET_CHECK_FAIL;
			// SPACE MODE CHECK 점프 속도를 더한 포지션 값으로 체크문을 넣어주세용. 추후 개발이 되면. 참조는 위에껄로!  ( TO.종혁님 )

			VEC3D JumpHold;
			JumpHold.x = 0;
			JumpHold.y = pObject->m_r32JumpHoldHeight;
			JumpHold.z = pObject->m_r32JumpHoldPower;

			REAL32 r32Speed= i3Vector::Length(&JumpHold);		
			pChara->SetJumpholdSpeed(r32Speed);  //밟은 점프대의 속도 설정			

		}
		// 핵 체크 위치.
		else if ((pActionKey->getActionKey(i) & NET_ACTIONKEY_USEITEMBOX))
		{
			// 기본적인 핵 체크
			pObject = GetGameObject(pActionKey->getUseActionObjectIndex(i));
			if (NULL == pObject)																return PACKET_CHECK_FAIL;
			// 우선 거리로 체크 하는 핵 체크는 끕니다.
			//if (FALSE == pObject->CheckActionDistance(pChara->GetPosRot()->GetPos()))			return PACKET_CHECK_FAIL;
		}
	}

	return PACKET_CHECK_SUCCESS;
}

PACKET_CHECK CDediRoom::ParseGrenadePacket( UINT32 ui32SlotIdx, UINT16 ui16GrenadeIdx, GAMEINFO_THROW_WEAPON_NETDATA* pThorwWeapon )
{
	// Check Index
	if( ui16GrenadeIdx < ui32SlotIdx * MAX_THROWWEAPON_SLOP )							return PACKET_CHECK_FAIL;
	if( ui16GrenadeIdx >= (ui32SlotIdx + 1) * MAX_THROWWEAPON_SLOP )					return PACKET_CHECK_FAIL;
	// position
	if( IsOverflow( pThorwWeapon->GetPos() ) )											return PACKET_CHECK_FAIL;
	if( IsOverflow( pThorwWeapon->GetDir() ) )											return PACKET_CHECK_FAIL;
	
	/////////////////////////////////////////////////////////////////////////////
	// 여기서 부터 해킹 체크
	if( FALSE == g_pConfig->GetCheckLevel( HACK_TYPE_GRENADE ) )						return PACKET_CHECK_SUCCESS;

	// 무기가 정상인지 체크
	WeaponInfo* pInfo = Table::GetWeaponInfo( pThorwWeapon->getWeaponID() );	
	if( NULL == pInfo )
	{
		// 해킹일 경우
		DSERROR( "[ERR] UDPChecker::_ParseGrenadePacket WeaponID = %d\n", pThorwWeapon->getWeaponID() );
		return PACKET_CHECK_FAIL;
	}
	// 던지는 계열 무기인지 체크 ( 던지는 계열의 아이템은 총이 아닌 총알로 계산되기 때문에 Ext OFF, L1 탄으로 계산 됩니다. )
	if( FALSE == Table::IsThrowWeapon( pThorwWeapon->GetWeapon()->getWeaponID(), FALSE, WEAPON::ATTACK_TYPE_L1 ) )
	{
		DSERROR( "[ERR] UDPChecker::_ParseGrenadePacket Not Throw Weapon WeaponID = %d\n", pThorwWeapon->getWeaponID() );
		return PACKET_CHECK_FAIL;
	}
	
	N_PCINFO_HIT_RAYCHECK RayCast;
	RayCast.m_bCheckRay = FALSE;
	// 던질 수 있는 상태인지 체크
	if( FALSE == IsThrowWeapon( ui32SlotIdx, ui16GrenadeIdx, pThorwWeapon, &RayCast ) )
	{	// 너무 많이 남깁니다. 추후 재확인후 주석 제거.
		//DSERROR( "[ERR] UDPChecker::_ParseGrenadePacket Not Throw State WeaponID = %d\n", pThorwWeapon->getWeaponID() );
		return PACKET_CHECK_FAIL;
	}

	//투척단검인 경우 Raycast로 월샷 체크를 하지 않습니다.(쿠나이 월샷 오탐 수정후 제거 필요)
	UINT32 ui32Class = GET_ITEM_SUBTYPE(pThorwWeapon->GetWeapon()->getWeaponID());
	if (WEAPON_CLASS_KNIFE == ui32Class) return PACKET_CHECK_SUCCESS;

	if( g_pConfig->m_aui8HackCheckLevel[HACK_TYPE_WALLSHOT] > getDetectRate(ui32SlotIdx)->IncreaseDetectRateCnt(HACK_TYPE_WALLSHOT) ) 
	{
		if( g_pConfig->m_bPhysicsActive && RayCast.m_bCheckRay )
		{
			_EnqueueRaycastRequest( ui32SlotIdx, &RayCast );
		}
	}

	return PACKET_CHECK_SUCCESS;
}