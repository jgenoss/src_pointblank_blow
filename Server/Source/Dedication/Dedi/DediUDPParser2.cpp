#include "pch.h"
#include "DediUDPParser2.h"
#include "DediRoom.h"
#include "WeaponTable.h"
#include "CharacterTable.h"
#include "ModuleControl.h"
#include "DroppedWeaponMgr.h"
#include "RespawnState.h"
#include "HmsParser.h"

INT32 UDPParser2::GamePacketParsing( CDediRoom* pRoom, RECV_PACKET_INFO* pRecvInfo, UINT32* ui32FireActionFlag )
{
	UINT32 ui32SlotIdx		= pRecvInfo->m_pHeader->_Index;
	if( SLOT_MAX_COUNT <= ui32SlotIdx )
	{
		DSWARNING( "UDPParser: slot idx error \n");			// 혹... 시나 해서 추가한 방어 코드입니다. 이 부분이 이상하다면 이 전 패킷 처리부분을 의심해야 합니다. 2014-08-26 김현우
		return 0;
	}

	UINT32	ui32InfoFlagAcc	= 0;

	INT32	i32Size			= 0;	// sub packet size

	RECV_PACKET_SUB_INFO* pSubInfo;

	for( INT32 i = 0 ; i < pRecvInfo->m_i32EventCount ; i++ )
	{
		pSubInfo = &pRecvInfo->m_SubInfo[ i ];

		switch( pSubInfo->m_pSubHead->_Type )
		{
		case P2P_SUB_HEAD_USER :
			if( pSubInfo->m_pSubHead->_Index != ui32SlotIdx)
			{
				DSWARNING( "[UDPParser2::P2P_SUB_HEAD_USER] invalid chara info index..%d\n", pSubInfo->m_pSubHead->_Index);
				return 0; 
			}

			// get character's net data
			i32Size			= pSubInfo->m_pSubHead->_Code;
			ui32InfoFlagAcc	= 0; // 실제로 받은 정보만

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
			{
				CCharacter*	pChara = pRoom->GetGameCharacter(ui32SlotIdx);
				
				// Task에서 한번의 Update 동안 FireAction 값이 켜져있던 것이 있으면 뒤쪽 패킷들도 켜진채 처리되게 하여야 함.
				// 이 부분이 없으면 FireAction 이 켜진게 중간 패킷에 있을 경우 뒤에 패킷 때문에 덮어씌워져 Fire 애니메이션을 안하고 총이나 수류탄이 발사되는 문제가 있음.
				if( pChara->GetAlive() )
				{
					if( ((pSubInfo->m_pChara_State->iState >> 15) & 0x01) )
						SetBit( ui32FireActionFlag, ui32SlotIdx );

					if( GetBit( *ui32FireActionFlag, ui32SlotIdx ) )
						pSubInfo->m_pChara_State->iState |= (1 << 15);
				}
				
				pChara->SetState( pSubInfo->m_pChara_State );

				UINT8 ui8Upper = 0;
				UINT8 ui8WeaponSlot = 0;
				UINT8 ui8Extension = 0;
				UDP_UTIL::ConvertNetActiontoChara( &ui8Extension, &ui8WeaponSlot, NULL, NULL, NULL, &ui8Upper, NULL, NULL, NULL, pSubInfo->m_pChara_State->iState );

				if( ui8Upper == CHARA_UPPER_LOADMAGAZINE		|| 
					ui8Upper == CHARA_UPPER_SWAPWEAPON			||
					ui8Upper == CHARA_UPPER_EXTENSION_ACTIVATE	||
					ui8Upper == CHARA_UPPER_EXTENSION_DEACTIVATE )
				{
					CHMS* pHMS = pRoom->getHMS();
				}

				N_PCINFO_WEAPON weapon;
				UINT32 weaponID = pChara->GetWeapon(ui8WeaponSlot);

				if( pRoom->IsUsurpationMode() && weaponID == Weapon::GetItemID(WEAPON_CLASS_KNIFE, KNIFE_CHINESE_CLEAVER_SEIZED) )
				{	// 탈취 모드 식칼은 주무기를 버리고 줍는 구조가 아니라(무기슬롯변경X) 예외처리 합니다.
					pChara->SetExtensionState( (BOOL)ui8Extension );
					weapon.SetWeapon( Weapon::GetItemID(WEAPON_CLASS_KNIFE, KNIFE_CHINESE_CLEAVER_SEIZED), (UINT8)WEAPON_USAGE_MELEE);
					weapon.SetIsExtension( TRUE );

					memcpy_s( pRoom->getChara_Weapon( ui32SlotIdx), sizeof( N_PCINFO_WEAPON ), &weapon, sizeof( N_PCINFO_WEAPON ));

					pRoom->SetChara_bWeaponSync( ui32SlotIdx, TRUE );
					pChara->setWeaponSlot( ui8WeaponSlot );
				}


				// 현재 들고있는 무기가 서버가 알고있는 슬롯과 다르거나,
				// 익스텐션 상태에 변화가 있을 경우 해당 유저의 무기 정보를 브로드캐스팅 해준다.
				// ( BroadCastInfo 세팅 시 SYNCWEAPON 플래그로 보냄 )
				if( pChara->getWeaponSlot() != ui8WeaponSlot	||
					pChara->GetExtensionState() != (BOOL)ui8Extension )
				{
					if( ui8WeaponSlot > WEAPON_USAGE_UNKNOWN &&
						ui8WeaponSlot < WEAPON_USAGE_COUNT &&
						pChara->GetWeapon( ui8WeaponSlot ) != 0 )
					{
						if( WEAPON_USAGE_MELEE == ui8WeaponSlot && pChara->GetUseM7() )
						{
							// Throwing Knife Sync Check
							WeaponInfo * pBaseWeapon = Table::GetWeaponInfo(weaponID);
							INT32 i32ExtIdx = pBaseWeapon->FindExtStateIdx(WEAPON_EXT_DUAL);
							
							UINT32 extItemID = SET_ITEM_EXT_STATE(i32ExtIdx, weaponID);

							if(i32ExtIdx < 0)
								extItemID = Weapon::GetItemID(WEAPON_CLASS_KNIFE, KNIFE_M7);

							weapon.SetWeapon( extItemID, (UINT8)WEAPON_USAGE_MELEE);
							weapon.SetIsExtension( FALSE );
							ui8Extension = 0;
						}
						else
						{
							// 서버가 알고있는 유저의 슬롯 무기정보를 입력해준다 ( 유저에게 무기를 받지 않기에 무기 변조 검사가 필요 없음 )
							weapon.SetWeapon( pChara->GetWeapon( ui8WeaponSlot ), ui8WeaponSlot);
							weapon.SetIsExtension( ui8Extension );
						}
						
						pChara->SetExtensionState( (BOOL)ui8Extension );

						memcpy_s( pRoom->getChara_Weapon( ui32SlotIdx), sizeof( N_PCINFO_WEAPON ), &weapon, sizeof( N_PCINFO_WEAPON ));
						
						pRoom->SetChara_bWeaponSync( ui32SlotIdx, TRUE );
						pChara->setWeaponSlot( ui8WeaponSlot );
					}
					else if( WEAPON_USAGE_MISSION == ui8WeaponSlot )
					{
						// 무기 슬롯이 Mission 일때는 별도로 처리를 해줍니다.
						// ( 게임서버로 받는 장착 무기정보에는 미션 무기가 장착되어있지 않기 때문입니다. ) 
						if( TEAM_RED == pRoom->GetTeamType( ui32SlotIdx ) && ( pRoom->IsBombMissionMode() || pRoom->IsUsurpationMode() ) ) 
						{
							BOOL bExtension = FALSE;
							if( pRoom->IsBombMissionMode() )
							{
								weapon.SetWeapon( Weapon::GetItemID(WEAPON_CLASS_MISSION, MISSION_C4), (UINT8)WEAPON_USAGE_MISSION);
							}
							else if( pRoom->IsUsurpationMode() )
							{
								weapon.SetWeapon( Weapon::GetItemID(WEAPON_CLASS_MISSION, MISSION_CROWN_WATER_MELON), (UINT8)WEAPON_USAGE_MISSION);
								bExtension = TRUE;
							}

							weapon.SetIsExtension( bExtension );

							memcpy_s( pRoom->getChara_Weapon( ui32SlotIdx), sizeof( N_PCINFO_WEAPON ), &weapon, sizeof( N_PCINFO_WEAPON ));
							
							pRoom->SetChara_bWeaponSync( ui32SlotIdx, TRUE );
							pChara->setWeaponSlot( ui8WeaponSlot );
						}
					}
				}

				pRoom->SetChara_State( ui32SlotIdx, pSubInfo->m_pChara_State );
				ui32InfoFlagAcc |= P2P_INFO_FLAG_CHARASTATE;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
			{
				pRoom->SetChara_OnLoadObject( ui32SlotIdx, pSubInfo->m_pChara_OnLoadObject );
				ui32InfoFlagAcc |= P2P_INFO_FLAG_ONLOADOBJECT;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
			{	
				pRoom->SetChara_PosRot( ui32SlotIdx, pSubInfo->m_pChara_PosRot );

				pRoom->SetSendEnable( ui32SlotIdx ); // 이때부터 SEND 가능 (2012-03-22)
				
				ui32InfoFlagAcc |= P2P_INFO_FLAG_CHARAPOS;
			}

			if( pRoom->IsHackUser( ui32SlotIdx ) == FALSE )  
			{
				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ACTIONKEY )
				{
					_ProcessCharaActionKey( pRoom, ui32SlotIdx, pSubInfo->m_pChara_ActionKey ); // 참고: CGameUDPHandler::_ProcessRecvPacket_Chara_ActionKey

					pRoom->SetChara_ActionKey( ui32SlotIdx, pSubInfo->m_pChara_ActionKey );
					ui32InfoFlagAcc |= P2P_INFO_FLAG_ACTIONKEY;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
				{
					pRoom->SetChara_SyncObject( ui32SlotIdx, pSubInfo->m_pChara_SyncObject );
					ui32InfoFlagAcc |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_RADIO)
				{
					pRoom->SetChara_RadioChat( ui32SlotIdx, pSubInfo->m_pChara_Radio );
					ui32InfoFlagAcc |= P2P_INFO_FLAG_RADIO;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_WEAPONPARAM )
				{
					pRoom->SetChara_WeaponParam( ui32SlotIdx, pSubInfo->m_pChara_WeaponParam );
					ui32InfoFlagAcc |= P2P_INFO_FLAG_WEAPONPARAM;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_LIFE )
				{
					// 서버에서만 만들 수 있는 패킷이다 skip
					//if( i32ReadPos + (INT32)sizeof( N_PCINFO_LIFE ) > nPacketSize ) return 0;
					//i32ReadPos += (INT32)sizeof( N_PCINFO_LIFE );
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
				{
					UINT8 ui8Count = *pSubInfo->m_pui8Chara_SuicideCount;//(UINT8) pPacket[ i32ReadPos++ ];
					if ( ui8Count <=  MAX_SUICIDE_COUNT )
					{
						N_PCINFO_SUICIDEDMG*	pSuicideDamage		= pRoom->getChara_Suicide (ui32SlotIdx);
						INT32*					pSuicideDamageCount = pRoom->getChara_SuicideCount( ui32SlotIdx );

						memcpy_s( pSuicideDamage, sizeof( N_PCINFO_SUICIDEDMG ) * MAX_SUICIDE_COUNT,
									pSubInfo->m_pChara_Suicide, sizeof( N_PCINFO_SUICIDEDMG ) * ui8Count );

						*pSuicideDamageCount  = ui8Count;
							
						pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_SUICIDEDAMAGE ); 
							
						ui32InfoFlagAcc |= P2P_INFO_FLAG_SUICIDEDAMAGE;						
					}
					else
					{
						return 0;
					}
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_MISSION)
				{
					pRoom->SetChara_Mission( ui32SlotIdx, pSubInfo->m_pChara_Mission );
						
					pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_MISSION, pRecvInfo->m_pHeader->_GameTime ); 
						
					ui32InfoFlagAcc |= P2P_INFO_FLAG_MISSION;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
				{
					if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYCHARA ) I3TRACE( ">>>>>>>>>>>>>>>>>DropWeapon with Hit<<<<<<<<<<<<\n" );

					if ( _ParseDropWeaponPacket( pRoom, ui32SlotIdx, pSubInfo->m_pChara_DropW ) )
					{
						pRoom->SetChara_DropWeapon( ui32SlotIdx, pSubInfo->m_pChara_DropW );
						
						ui32InfoFlagAcc |= P2P_INFO_FLAG_DROPWEAPON;
					}
					else return 0;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
				{
					if( _ParseGetWeaponPacket( pRoom, ui32SlotIdx, pSubInfo->m_pChara_GetW ))
					{
						pRoom->SetChara_GetWeapon( ui32SlotIdx, pSubInfo->m_pChara_GetW );
						
						ui32InfoFlagAcc |= P2P_INFO_FLAG_GETWEAPON;
					}
					else return 0;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_FIRE)
				{
					pRoom->SetChara_Bullet( ui32SlotIdx, pSubInfo->m_pChara_Bullet );

					ui32InfoFlagAcc |= P2P_INFO_FLAG_FIRE;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYCHARA)
				{
					pRoom->SetChara_Hit_ByChara( ui32SlotIdx, *pSubInfo->m_pui8Chara_Hit_ByChara, pSubInfo->m_pChara_Hit_ByChara );
							
					pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_HITBYCHARA );
						
					ui32InfoFlagAcc |= P2P_INFO_FLAG_HITBYCHARA;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION)
				{
					pRoom->SetChara_Hit_ByExplosion( ui32SlotIdx, *pSubInfo->m_pui8Chara_Hit_ByExplosion, pSubInfo->m_pChara_Hit_ByExplosion );
					
					pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_HITBYEXPLOSION );
						
					ui32InfoFlagAcc |= P2P_INFO_FLAG_HITBYEXPLOSION;					
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYOBJECT)
				{
					pRoom->SetChara_Hit_ByObject( ui32SlotIdx, *pSubInfo->m_pui8Chara_Hit_ByObject, pSubInfo->m_pChara_Hit_ByObject );

					pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_HITBYOBJECT );
						
					ui32InfoFlagAcc |= P2P_INFO_FLAG_HITBYOBJECT;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_TOUCHDOWN )
				{
					pRoom->TouchDown_C( ui32SlotIdx );
				}

				pRoom->getChara_Check( ui32SlotIdx)->bCheckBroadcast  = TRUE;
				pRoom->getChara_Check( ui32SlotIdx)->nP2PInfoFlag    |= ui32InfoFlagAcc;
			}
			
			pRoom->getChara_Check( ui32SlotIdx)->bCheckBroadcast  = TRUE;
			pRoom->getChara_Check( ui32SlotIdx)->nP2PInfoFlag    |= ui32InfoFlagAcc;

			break;
			
		case P2P_SUB_HEAD_GRENADE :

			if( pRoom->IsHackUser (ui32SlotIdx ) )
			{
				pRoom->SetCheckGrenade( pSubInfo->m_pSubHead->_Index, FALSE );
				return 0;
			}

			pRoom->SetGrenade( pSubInfo->m_pSubHead->_Index, pSubInfo->m_pGrenade );

			if ( pSubInfo->m_pSubHead->_Code == ((INT16)sizeof(N_WEAPONINFO_THROW) + (INT16)sizeof( N_GAME_SUBHEAD)) ) 
			{
				pRoom->SetCheckGrenade( pSubInfo->m_pSubHead->_Index, TRUE );
			}
			else
			{
				DSWARNING( "[UDPParser2::P2P_SUB_HEAD_GRENADE] parsing fail\n");
				return 0;
			}
			break;

		case P2P_SUB_HEAD_DROPEDWEAPON :
			{
				if( pSubInfo->m_pSubHead->_Index < MAX_DROPEDWEAPON_COUNT )
				{
					if( pRoom->IsHackUser( ui32SlotIdx ) )
					{
						pRoom->SetCheckGrenade( pSubInfo->m_pSubHead->_Index, FALSE );
						return 0;
					}
					
					CDroppedWeaponMgr* pDWM = pRoom->GetDroppedWeaponMgr();
					if( NULL == pDWM || !pDWM->CheckMyWeapon( ui32SlotIdx, pSubInfo->m_pSubHead->_Index ) ) 
					{
						return 0;
					}

					pRoom->SetDroppedWeapon( pSubInfo->m_pSubHead->_Index, pSubInfo->m_pDroppedWeapon );

					if( pSubInfo->m_pSubHead->_Code == ( (UINT16)sizeof( N_DROPEDWEAPONINFO) + (UINT16)sizeof( N_GAME_SUBHEAD)) )
					{
						// 첫자리 NET_WEAPONDROPED_DROPED = 0x80 ( 128 ) , 뒷자리 NET_WEAPONDROPED_STATE 로 상태 구별
						// 129 = 땅바닥에 정착, 131 = 바운드 상태
						if( 129 == pSubInfo->m_pDroppedWeapon->State || 131 == pSubInfo->m_pDroppedWeapon->State )
						{
							pDWM->SetDropPOS( pSubInfo->m_pSubHead->_Index, pSubInfo->m_pDroppedWeapon->_Pos );
						}
						*pRoom->getCheckDroppedWeapon( pSubInfo->m_pSubHead->_Index ) = TRUE;
					}
					else
					{
						DSWARNING( "[UDPParser::P2P_SUB_HEAD_DROPEDWEAPON] parsing fail..%d, %d\n", pSubInfo->m_pSubHead->_Code, sizeof( N_DROPEDWEAPONINFO ));
						return 0;
					}
				}
				else
				{
					DSWARNING( "[UDPParser2::P2P_SUB_HEAD_DROPEDWEAPON] invalid index..\n");
					return 0;
				}
			}
			break;
		default:
			DSWARNING( "[_GamePacketParsing()] Invalid recv packet type..\n");
			return 0;
			break;
		}//switch(packet->Type)

		//if( i32ReadPos >= nPacketSize ) break;
	}

	return 1;
}

BOOL UDPParser2::_ProcessCharaActionKey( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_ACTIONKEY * pActionKey )
{
	if( pActionKey->iUseObjectIdx >= OBJ_TOTAL_COUNT )
		return FALSE;

	// 오브젝트 사용
	if( ( pActionKey->iKeyState & NET_ACTIONKEY_USEOBJECT) )
	{
		DS_GAME_OBJECT* pObjectInfo = pRoom->GetGameObject( pActionKey->iUseObjectIdx );
		if( NULL == pObjectInfo )			return TRUE;
		
		if( pObjectInfo->GetSType() & OBJECT_S_TYPE_HELICOPTER )
		{
			DS_ANIMATION_OBJECT* pObj = pRoom->GetAnimationObject( pActionKey->iUseObjectIdx );
			if( NULL == pObj )				return TRUE;
			
			if( pObj->SetUse( (UINT8)ui32SlotIdx ) == FALSE )
			{
				pActionKey->iKeyState &= ~NET_ACTIONKEY_USEOBJECT;

			}
			else
			{	// 성공시
				// outpost 예외처리
				if( pObj->m_pObject->GetSType() == OBJECT_S_TYPE_RIDING_HELICOPTER )
				{
					// 패스키를 계산
					pObj->m_pObject->m_ui8PathKey = pRoom->CalcPathKey( pObj->m_pObject );
					pObj->m_bPathKeyChanged = true;
					I3TRACE("TRACE_OUTPOST: NET_ACTIONKEY_USEOBJECT: ON pathkey changed:%d)\n", pObj->m_pObject->m_ui8PathKey );
				}
			}
		}
		else if( pObjectInfo->GetSType() == OBJECT_S_TYPE_W_BOX )
		{
			DS_CONTROLLED_OBJECT* pObj = pRoom->GetControlledObject( pActionKey->iUseObjectIdx ); 
			if( NULL == pObj )				return TRUE;
				
			if ( pObj->SetUse (ui32SlotIdx ) == FALSE)
			{
				pActionKey->iKeyState &= ~NET_ACTIONKEY_USEOBJECT;
			}
			else
			{
				if( pObj->m_ui8ShellCount > 0 )
				{
					--pObj->m_ui8ShellCount;

					// 
					if( pObjectInfo->m_eWeaponType == WT_CROWNWATERMELON )
					{	// 탈취물이면 상태 UPDATE.
						return pRoom->SetUsurpationObject( (USURPATION_OBJECT_STATE)ui32SlotIdx, 5 );
					}
					//I3TRACE("##### USEOBJECT #### : %d\n", GetTickCount());
				}
			}
		}
	}
	else if( ( pActionKey->iKeyState & NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT) )
	{
		DS_ANIMATION_OBJECT* pObjectInfo = pRoom->GetAnimationObject( pActionKey->iUseObjectIdx );
		if( NULL == pObjectInfo )			return TRUE;
		
		if( pObjectInfo->m_pObject->GetSType() & OBJECT_S_TYPE_HELICOPTER )
		{
			pObjectInfo->ResetUse(); // 항상 sucess
		}
	}
	else if( ( pActionKey->iKeyState & NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT) )
	{
		DS_ANIMATION_OBJECT* pObjectInfo = pRoom->GetAnimationObject( pActionKey->iUseObjectIdx );

		if( NULL == pObjectInfo )			return TRUE;
		
		if( pObjectInfo->m_pObject->GetSType() & OBJECT_S_TYPE_HELICOPTER )
		{
			if( pObjectInfo->SetUse( ui32SlotIdx ) == FALSE) // 사용중일 때는 false
			{
				pActionKey->iKeyState &= ~NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT;
			}
		}
	} 
 
	return TRUE;
}

BOOL	UDPParser2::_ParseGetWeaponPacket ( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_GETWEAPON * pWeapon)
{
	if( pWeapon->m_ui32WeaponID == Weapon::GetItemID(WEAPON_CLASS_MISSION, MISSION_CROWN_WATER_MELON ) ) 
	{	// 탈취물이라면 탈취물 상태 UPDATE
		pRoom->SetUsurpationObject( (USURPATION_OBJECT_STATE)ui32SlotIdx, 6 );
		CCharacter * pChara = pRoom->GetGameCharacter( ui32SlotIdx );
		pChara->SetWeapon( pWeapon->m_ui32WeaponID, WEAPON_USAGE_MISSION );
		return FALSE;	// 아래 부분 처리 필요 없음.
	}

	UINT8 ui8Usage	= WEAPON_USAGE_PRIMARY;
	if( pWeapon->State & 0x20 ) 
		ui8Usage = WEAPON_USAGE_SECONDARY;

	CCharacter * pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	pChara->SetWeapon( pWeapon->m_ui32WeaponID, ui8Usage );

	// set bullet count
	UINT16 ui16TotalBulletCount = pWeapon->m_ui16MaxBullet + pWeapon->m_ui8LoadedBullet + pWeapon->m_ui8DualLoadedBullet;
	WeaponInfo * pInfo = Table::GetWeaponInfo(pWeapon->m_ui32WeaponID);

	if ( !pInfo || !pInfo->IsValid() )
	{
		DSERROR("ERR. SetRemainBullets: weapon id = %d invalid\n", pWeapon->m_ui32WeaponID);
		return FALSE;
	}

	pChara->SetBullets( pInfo, ui16TotalBulletCount );

	N_PCINFO_GETWEAPON wg;
	pWeapon->CopyTo(&wg);
 	wg.SetWeaponID(pWeapon->m_ui32WeaponID);

	return pRoom->GetDroppedWeaponMgr()->GetWeapon( ui32SlotIdx, &wg );
}

BOOL	UDPParser2::_ParseDropWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_DROPWEAPON* pWeapon)
{
	UINT8 ui8Usage	= WEAPON_USAGE_PRIMARY;
	if( pWeapon->State & 0x20 ) ui8Usage = WEAPON_USAGE_SECONDARY;

	N_PCINFO_DROPWEAPON w;
	pWeapon->CopyTo(&w);
	w.SetWeaponID(pWeapon->m_ui32WeaponID);

	// yjha
	INT32 i32DropIndex;

	if( pRoom->GetDroppedWeaponMgr()->DropWeapon( ui32SlotIdx, &w, i32DropIndex ) )
	{
		CCharacter * pChara = pRoom->GetGameCharacter( ui32SlotIdx );
		pWeapon->State &= 0xE0;						// 뒤의 5비트를 끈다
		pWeapon->State |= (i32DropIndex & 0x1f);	// 5비트를 변경한다.
		pChara->SetWeapon( 0, ui8Usage );

		// set bullet count
		WeaponInfo * pInfo = Table::GetWeaponInfo(pWeapon->m_ui32WeaponID);

		I3ASSERT(pInfo != NULL);
		if ( !pInfo || !pInfo->IsValid() )
		{
			DSERROR("ERR. SetRemainBullets: weapon id = %d invalid\n", pWeapon->m_ui32WeaponID);
			return FALSE;
		}

		pChara->SetBullets( pInfo, 0 );
		return TRUE;
	}

	return FALSE;
}
