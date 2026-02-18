#include "pch.h"
#include "DediUDPParser.h"
#include "DediRoom.h"
#include "WeaponTable.h"
#include "CharacterTable.h"
#include "ModuleControl.h"
#include "DroppedWeaponMgr.h"
#include "RespawnState.h"
#include "HmsParser.h"

INT32 UDPParser::GamePacketParsing( CDediRoom* pRoom, RECV_PACKET_INFO* pRecvInfo, UINT32* ui32FireActionFlag )
{
	UINT32 ui32SlotIdx		= pRecvInfo->m_pHeader->_Index;
	if( SLOT_MAX_COUNT <= ui32SlotIdx )
	{
		DSWARNING( "UDPParser: slot idx error \n");			// 혹... 시나 해서 추가한 방어 코드입니다. 이 부분이 이상하다면 이 전 패킷 처리부분을 의심해야 합니다. 2014-08-26 김현우
		return 0;
	}

#ifdef __USE_PACKET_OPTIMIZE__
	pRoom->RecvUpdateIdx( ui32SlotIdx, pRecvInfo->m_pHeader->m_ui8PacketUpdateIdx );
#endif

	CCharacterCastPacket* pCastPacket = pRoom->GetCharCastPacket( ui32SlotIdx );

	UINT32	ui32InfoFlagAcc	= 0;

	INT32	i32Size			= 0;	// sub packet size
	
	RECV_PACKET_SUB_INFO* pSubInfo;

	for( INT32 i = 0 ; i < pRecvInfo->m_i32EventCount ; i++ )
	{
		pSubInfo = &pRecvInfo->m_SubInfo[ i ];

		switch( pSubInfo->m_pSubHead->_Type )
		{
		case P2P_SUB_HEAD_USER :

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
				UINT8 ui8Extension = 0;
				UINT8 ui8WeaponSlot = 0;
				UINT8 ui8MultiWeaponSlot = 0;

				UDP_UTIL::ConvertNetActiontoChara( &ui8Extension, &ui8WeaponSlot, &ui8MultiWeaponSlot, NULL, NULL, NULL, &ui8Upper, NULL, NULL, NULL, NULL, pSubInfo->m_pChara_State->iState );
				
				UINT8 ui8SyncWeaponInfo = MAKE_SYNC_WEAPON(ui8Extension, ui8WeaponSlot, ui8MultiWeaponSlot);

				N_PCINFO_WEAPON weapon;
				weapon.Reset();
				UINT32 weaponID = pChara->GetEquipWeaponID(ui8WeaponSlot);

				if( pRoom->IsConvoyMode() && weaponID == Weapon::GetItemID(WEAPON_CLASS_KNIFE, KNIFE_CHINESE_CLEAVER_SEIZED) )
				{	// 탈취 모드 식칼은 주무기를 버리고 줍는 구조가 아니라(무기슬롯변경X) 예외처리 합니다.
					pChara->SetSyncExtension( (BOOL)ui8Extension );
					weapon.SetWeaponID( Weapon::GetItemID(WEAPON_CLASS_KNIFE, KNIFE_CHINESE_CLEAVER_SEIZED) );
					weapon.SetIsExtension( TRUE );

					pRoom->SetChara_Weapon( ui32SlotIdx, &weapon );
					pChara->SetSyncWeaponSlot( ui8WeaponSlot );
					pRoom->SetChara_bWeaponSync( ui32SlotIdx, TRUE );	
				}


				// 현재 들고있는 무기가 서버가 알고있는 슬롯과 다르거나,
				// 익스텐션 상태에 변화가 있을 경우 해당 유저의 무기 정보를 브로드캐스팅 해준다.
				// ( BroadCastInfo 세팅 시 SYNCWEAPON 플래그로 보냄 )
				if( pChara->GetSyncWeaponInfo() != ui8SyncWeaponInfo )
				{
					if( pChara->GetOwnWeapon( ui8WeaponSlot, ui8MultiWeaponSlot ) != 0 )
					{
						// 무기 스왑 : 클라이언트에서 받은 슬롯 정보에 있는 서버의 무기정보 브로드 캐스팅
						pChara->ChangeMultiWeapon(ui8WeaponSlot, ui8MultiWeaponSlot);
						pChara->SetSyncMultiWeaponSlot(ui8MultiWeaponSlot);
						pChara->SetSyncExtension( (BOOL)ui8Extension );
						pChara->SetSyncWeaponSlot( ui8WeaponSlot );

						// 서버가 알고있는 유저의 무기정보를 입력해준다 ( 유저에게 무기를 받지 않기에 무기 변조 검사가 필요 없음 )
						weapon.SetWeaponID( pChara->GetEquipWeaponID( ui8WeaponSlot ));
						weapon.SetIsExtension( ui8Extension );	
						weapon.SetIsSwap(true);

						pRoom->SetChara_Weapon( ui32SlotIdx, &weapon );
						pRoom->SetChara_bWeaponSync( ui32SlotIdx, TRUE );
					}
					else if( WEAPON_SLOT_MISSION == ui8WeaponSlot )
					{
						// 무기 슬롯이 Mission 일때는 별도로 처리를 해줍니다.
						// ( 게임서버로 받는 장착 무기정보에는 미션 무기가 장착되어있지 않기 때문입니다. ) 
						if( TEAM_RED == pRoom->GetTeamType( ui32SlotIdx ) && ( pRoom->IsBombMissionMode() || pRoom->IsConvoyMode() ) ) 
						{
							BOOL bExtension = FALSE;
							if( pRoom->IsBombMissionMode() )
							{
								weapon.SetWeaponID( Weapon::GetItemID(WEAPON_CLASS_MISSION, MISSION_C4) );
							}
							else if( pRoom->IsConvoyMode() )
							{
								weapon.SetWeaponID( Weapon::GetItemID(WEAPON_CLASS_MISSION, MISSION_CROWN_WATER_MELON) );
								bExtension = TRUE;
							}

							weapon.SetIsExtension( bExtension );
							weapon.SetIsSwap(true);

							pRoom->SetChara_Weapon( ui32SlotIdx, &weapon );
							pChara->SetSyncWeaponSlot( ui8WeaponSlot );
							pRoom->SetChara_bWeaponSync( ui32SlotIdx, TRUE );		
						}
					}
				}

				pRoom->SetChara_State( ui32SlotIdx, pSubInfo->m_pChara_State );
				ui32InfoFlagAcc |= P2P_INFO_FLAG_CHARASTATE;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
			{
				pSubInfo->m_TakingObject.CopyTo( &pCastPacket->m_TakingObject );
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
					_ProcessCharaActionKey( pRoom, ui32SlotIdx, &pSubInfo->m_ActionKey ); // 참고: CGameUDPHandler::_ProcessRecvPacket_Chara_ActionKey

					pRoom->GetCharCastPacket( ui32SlotIdx )->m_ActionKey.Inset( &pSubInfo->m_ActionKey );

					ui32InfoFlagAcc |= P2P_INFO_FLAG_ACTIONKEY;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
				{
					pSubInfo->m_SyncObject.CopyTo( &pCastPacket->m_SyncObject );
					ui32InfoFlagAcc |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_RADIO)
				{
					pSubInfo->m_RadioChat.CopyTo( &pCastPacket->m_RadioChat );
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
					pSubInfo->m_SuicideDamage.CopyTo( &pCastPacket->m_SuicideDamage );
					pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_SUICIDEDAMAGE ); 
							
					ui32InfoFlagAcc |= P2P_INFO_FLAG_SUICIDEDAMAGE;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_MISSION)
				{
					pSubInfo->m_Mission.CopyTo( &pCastPacket->m_Mission );
						
					pRoom->UpdateReferenceTable( ui32SlotIdx, P2P_INFO_FLAG_MISSION, pRecvInfo->m_pHeader->_GameTime ); 
						
					ui32InfoFlagAcc |= P2P_INFO_FLAG_MISSION;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
				{
					pSubInfo->m_DropWeapon.CopyTo( &pCastPacket->m_DropWeapon );

					if( _ParseDropWeaponPacket( pRoom, ui32SlotIdx, &pCastPacket->m_DropWeapon ) )
					{
						ui32InfoFlagAcc |= P2P_INFO_FLAG_DROPWEAPON;
					}
					else return 0;
					
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
				{
					pSubInfo->m_GetWeapon.CopyTo( &pCastPacket->m_GetWeapon );

					if( _ParseGetWeaponPacket( pRoom, ui32SlotIdx, &pCastPacket->m_GetWeapon ))
					{
						ui32InfoFlagAcc |= P2P_INFO_FLAG_GETWEAPON;
					}
					else return 0;
				}

				if (pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
				{
					pSubInfo->m_DirectPickUp.CopyTo(&pCastPacket->m_DirectPickUp);

					if (_ParseDirectPickUpPacket(pRoom, ui32SlotIdx, &pCastPacket->m_DirectPickUp))
					{
						ui32InfoFlagAcc |= P2P_INFO_FLAG_DIRECTPICKUP;
					}
					else return 0;
				}

				if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_FIRE)
				{
					pSubInfo->m_Fire.CopyTo( &pCastPacket->m_Fire );

					if( _PacketFirePacket( pRoom, ui32SlotIdx, &pCastPacket->m_Fire ) )
					{
						ui32InfoFlagAcc |= P2P_INFO_FLAG_FIRE;
					}
					
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
			{
				if( pRoom->IsHackUser (ui32SlotIdx ) )
				{
					//pRoom->SetCheckGrenade( pSubInfo->m_pSubHead->_Index, FALSE );
					return 0;
				}

				if( FALSE == pRoom->AddThrowWeapon( ui32SlotIdx, pSubInfo->m_pSubHead->_Index, &pSubInfo->m_ThorwWeapon ) )
				{
					return 0;
				}

 				//pRoom->SetGrenade( pSubInfo->m_pSubHead->_Index, pSubInfo->m_pGrenade );
				//pRoom->SetCheckGrenade( pSubInfo->m_pSubHead->_Index, TRUE );
			}
			break;

		case P2P_SUB_HEAD_DROPEDWEAPON :
			{

				if( pRoom->IsHackUser( ui32SlotIdx ) )
				{
				//	pRoom->SetCheckGrenade( pSubInfo->m_pSubHead->_Index, FALSE );
					return 0;
				}
					
				if( FALSE == pRoom->UpdateDropWeapon( ui32SlotIdx, pSubInfo->m_pSubHead->_Index, &pSubInfo->m_DroppedWeapon ) )
				{
					return FALSE;
				}
			}
			break;
		default:
			DSWARNING( "[_GamePacketParsing()] Invalid recv packet type..\n");
			return 0;
			break;
		}
	}

	return 1;
}

BOOL UDPParser::_ProcessCharaActionKey( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_ACTIONKEY* pActionKey )
{
	CDSObject* pObjectInfo;

	for( INT32 i = 0 ; i < pActionKey->getActionCount() ; i++ )
	{
		pObjectInfo = pRoom->GetGameObject( pActionKey->getUseActionObjectIndex(i) );
		if( NULL == pObjectInfo )			return TRUE;

		// 오브젝트 사용
		if( pActionKey->getActionKey(i) & NET_ACTIONKEY_USEOBJECT )
		{
			if( FALSE == pObjectInfo->SetUse( (UINT8)ui32SlotIdx ) )
			{
				pActionKey->removeActionKey( i, NET_ACTIONKEY_USEOBJECT );
			}
			else
			{	// 성공시
				// outpost 예외처리
				// 패스키를 계산
				switch( pObjectInfo->m_eUseType )
				{
				case OBJECT_USE_TYPE_RIDING:
					{
						pObjectInfo->setPathKey( pRoom->CalcPathKey( pObjectInfo, FALSE ) );
						pObjectInfo->m_bPathKeyChanged		= TRUE;
					}
					break;
				case OBJECT_USE_TYPE_HAVE:
					{
						CCharacter*	pChara = pRoom->GetGameCharacter( ui32SlotIdx );
						if( FALSE == pChara->AddObject( pObjectInfo ) )
						{
							pActionKey->removeActionKey( i, NET_ACTIONKEY_USEOBJECT );
							pObjectInfo->ResetUse();
							return FALSE;
						}
					}
					break;
				case OBJECT_USE_TYPE_RESPAWN:
					{
						pObjectInfo->m_bTimerEnable		= TRUE;
						pObjectInfo->SetRespawnTimer( 0.0f );
						pObjectInfo->m_ui32StartTime	= (UINT32)GetTickCount();

						T_ItemID TItemID;
						WEAPON_SLOT_TYPE eSlot;
						ChangeAvailableWeapon( (EWEAPON_TYPE)pObjectInfo->m_eWeaponType, TItemID, eSlot );

						CCharacter* pChara = &pRoom->m_pGameCharacters[ ui32SlotIdx ];

						switch( (EWEAPON_TYPE)pObjectInfo->m_eWeaponType )
						{
						case WT_RPG7_OUTPOST:		// WT_RPG7_OUTPOST number:1
						case WT_RPG7_BOSS:			// WT_RPG7_BOSS: number:3
							// set bullet count
							{								
								pChara->SetNaturalRPGGet(TRUE);
							}
							//break; // break 없습니다.
						case WT_K201:				//WT_K201: K201_GL: 11
						case WT_CHINESE_CLEAVER:
							{
								pChara->SetEquipWeapon( TItemID, eSlot );
								pChara->SetBullets( eSlot );
							}
							break;
						}
					}
					break;
				}

				if( pObjectInfo->getShellCount() > 0 )
				{
					pObjectInfo->SubShellCount( 1 );
				}
			}
		}
		else if( ( pActionKey->getActionKey(i) & NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT) )
		{
			if( pObjectInfo->GetSType() & OBJECT_S_TYPE_HELICOPTER )
			{
				pObjectInfo->ResetUse(); // 항상 sucess
			}
		}
		else if( ( pActionKey->getActionKey(i) & NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT) )
		{
			if( pObjectInfo->GetSType() & OBJECT_S_TYPE_HELICOPTER )
			{
				if( FALSE == pObjectInfo->SetUse( ui32SlotIdx ) ) // 사용중일 때는 false
				{
					pActionKey->removeActionKey( i, NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT );
				}
			}
		}
		else if ((pActionKey->getActionKey(i) & NET_ACTIONKEY_USE_MOVE_CHARA_OBJECT))
		{
			//아직까지 예외처리 할 필요성이 없다.
			//추후 필요하면 넣어주세요.
		}
		// 실제 처리 위치
		else if ((pActionKey->getActionKey(i) & NET_ACTIONKEY_USEITEMBOX))
		{
			// SetUse가 실패 할 경우
			if (FALSE == pObjectInfo->SetUse((UINT8)ui32SlotIdx))
			{
				pActionKey->removeActionKey(i, NET_ACTIONKEY_USEITEMBOX);
			}
			// 나머지는 전부 성공 할 경우
			else
			{
				INT32 UseItemProbability = static_cast<INT32>(i3Math::Rand()) % 100;
				CCharacter* pChara = &pRoom->m_pGameCharacters[ui32SlotIdx];
				
				// 실제 아이템 사용 처리
				INT32 UseItemIdx = pObjectInfo->ProcessUseItem(UseItemProbability, pRoom);
		
				// 클라이언트로 전달 할 Item Idx
				if( UseItemIdx != -1 )
				{
					UINT8 ui8MoveKey = UseItemIdx;
					UINT8 ui8Usage = GET_ITEM_TYPE(pObjectInfo->m_ui32UseItemID[UseItemIdx]) - 1;
					if( ui8Usage < WEAPON_SLOT_COUNT )
					{
						UINT8 i8MultiSlotIdx = pChara->GetEquipMultiSlotIdx(ui8Usage);
						// MoveKey에 세팅합니다. ( MultiSlotIdx 4bit / ItemBoxIdx 4bit )
						ui8MoveKey = pChara->GetEquipMultiSlotIdx(ui8Usage) << 4 | UseItemIdx;
					}
					pActionKey->setMoveKey(i, ui8MoveKey);
				}			
			}
		}
	}
 
	return TRUE;
}

BOOL	UDPParser::_ParseGetWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_GETWEAPON * pWeapon )
{
	if( FALSE == pRoom->GetDropWeapon( pWeapon, pRoom->GetGameTime(), pWeapon->GetBullet() ) )	
		return FALSE;

	CCharacter * pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	INT8 i8MultiWeaponIdx = pChara->PickUpWeapon( pWeapon );

	if( -1 == i8MultiWeaponIdx )		
		return FALSE;
	pWeapon->SetMultiWeaponSlot( i8MultiWeaponIdx );
	return TRUE;
}

BOOL UDPParser::_ParseDirectPickUpPacket(CDediRoom * pRoom, UINT32 ui32SlotIdx, GAMEINFO_DIRECTPICKUP * pWeapon)
{
	if (pRoom->AddDropWeapon(ui32SlotIdx, pWeapon->getDropWeapon(), pRoom->GetGameTime(), pWeapon->getDropWeapon()->getIndex()))
	{
		// 무기가 Drop List에 붙었다. 그럼 이제 무기를 바꾼다.
		CCharacter * pChara = pRoom->GetGameCharacter(ui32SlotIdx);

		WeaponInfo* pWeaponInfo = Table::GetWeaponInfo(pWeapon->getWeaponID());

		if (pWeaponInfo != NULL)
		{
			UINT8 ui8Usage = pWeapon->getWeaponUsage();
			pChara->DropWeapon( ui8Usage );	// 장착 무기 드랍

			INT8 i8MultiWeaponIdx = pChara->PickUpWeapon( pWeapon );
			if( -1 == i8MultiWeaponIdx )		
				return FALSE;

			pWeapon->getDropWeapon()->SetMultiWeaponSlot( i8MultiWeaponIdx );
			// 드랍/픽업을 함께해서 Sync갱신이 안 되는 이슈 때문에 ( 슬롯 정보는 동일해서 )
			// 싱크용 멀티슬롯을 15로 변경합니다. if( pChara->GetSyncWeaponInfo() != ui8SyncWeaponInfo ) 참고
			pChara->SetSyncMultiWeaponSlot(0x0F);	
			// RPG일 경우에는 따로 체크 필요.
			if (Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7) == pWeapon->getWeaponID())
			{
				pChara->SetNaturalRPGGet(TRUE);
			}
		}
		else
			return FALSE;

		return TRUE;
	}
	else
	{
		// 사실 나올 수 없는 상황임.
		DSERROR("UDPParser::_ParseDirectPickUpPacket Add Fail");
	}

	return FALSE;
}

BOOL	UDPParser::_PacketFirePacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire )
{
	UINT32 ui32Dec = 1;

	WeaponInfo* pBaseWeapon = Table::GetWeaponInfo( pFire->getWeaponID() );
	if( pBaseWeapon )
	{
		if( pFire->getWeaponExtension() )
		{
			WeaponInfo* pExtWeapon = Table::GetWeaponInfo( pBaseWeapon->FindWeaponID(WEAPON::EXT_DUAL) );
			if( pExtWeapon && pExtWeapon->IsValid() )
			{
				ui32Dec = pExtWeapon->m_ui8FireDecBulletCount;
			}
		}
		else
		{
			ui32Dec = pBaseWeapon->m_ui8FireDecBulletCount;
		}
		if (ui32Dec == 0) ui32Dec = 1; // 셋팅 안되어 있는 경우
	}
	else
	{
		DSERROR( "ParseBulletPacket : not valid weapon %d\n", pFire->getWeaponID() );
		return FALSE;
	}
	//무기의 익스텐션 정보를 얻는다(소음기,착검 예외처리)
	ui32Dec = ui32Dec | Table::GetWeaponExtType(pBaseWeapon);

	// C5 예외 처리입니다. 주륵.. 이런코드 정말 싫었는데.. 조만간 Weapon 갈아엎을꺼니까.. 20150921 김현우
	if( ( WEAPON::SECONDARY_FIRE != pFire->getWeapon()->getFireOrder() ) )
	{
		CCharacter*	pChara = pRoom->GetGameCharacter(ui32SlotIdx);
		if (FALSE == pChara->DecreaseBullet(pFire->getID(), pFire->getWeapon(), ui32Dec))
		{
			DSERROR("ParseBulletPacket : RemainBullets is 0 - type 4, WeaponID : %d\n", pFire->getWeaponID());
			return FALSE;
		}
	}
	return TRUE;
}

BOOL	UDPParser::_ParseDropWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon)
{
	if( pRoom->AddDropWeapon( ui32SlotIdx, pWeapon, pRoom->GetGameTime(), pWeapon->getIndex() ) )
	{
		CCharacter* pChara = pRoom->GetGameCharacter( ui32SlotIdx );
		INT32 i32Usage = pWeapon->getWeaponUsage();
		if( i32Usage == WEAPON_SLOT_UNKNOWN )
		{
			return FALSE;
		}
		pChara->DropWeapon( i32Usage, pWeapon->GetMultiWeaponSlot() );
		return TRUE;
	}
	else
	{
		// 사실 나올 수 없는 상황임.
		DSERROR( "UDPParser::_ParseDropWeaponPacket Add Fail" );
	}

	return FALSE;
}
