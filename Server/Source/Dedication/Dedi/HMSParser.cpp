#include "pch.h"
#include "HMSParser.h"
#include "DediRoom.h"
#include "DroppedWeaponMgr.h"
#include "WeaponTable.h"
#include "StageTable.h"

#define HACK_LOG_LEVEL  11
#define HACK_PACKET_DROP_LEVEL  12

// I3ASSERT 걸리면 UDPChecker를 수정해야 합니다.

// For StormTube 스톰튜브
NxVec3 g_MovingBombPos		= NxVec3(	0.243652f,	-2.200000f,	2.103516f	);		// 오브젝트 기준 로컬 좌표.			GetOnLoad == true 일 때 계산을 위함
NxVec3 g_MovedBombPos		= NxVec3(	-25.6100f,	2.1935f,	2.2021f		);		// 오브젝트 이동 후의 월드 좌표.	GetOnLoad == false 일 때 계산을 위함

INT32 HMSParser::GamePacketParsing( CDediRoom* pRoom, RECV_PACKET_INFO* pInfo )
{
	REAL32 r32PacketTime	= pInfo->m_pHeader->_GameTime;
	UINT32 ui32RespawnCount	= pInfo->m_pHeader->_RespawnCount;
	UINT32 ui32SlotIdx		= pInfo->m_pHeader->_Index;
	
	INT32	i32Size					= 0; // sub packet size
	
	if( pRoom->IsHackUser( ui32SlotIdx ) )	 return FALSE;
	
	// roll back data 
	DS_PACKET_ROLLBACK	 rollBack; 	rollBack.Reset();
	rollBack.m_ui32SlotIdx  = ui32SlotIdx;
	rollBack.m_pRoom		= pRoom;

	RECV_PACKET_SUB_INFO* pSubInfo;

	for( INT32 i = 0 ; i < pInfo->m_i32EventCount ; i++ )
	{
		pSubInfo	= &pInfo->m_SubInfo[ i ];

		switch( pSubInfo->m_pSubHead->_Type )
		{
		case P2P_SUB_HEAD_USER :
			
			I3ASSERT( (UINT32) pSubInfo->m_pSubHead->_Index == ui32SlotIdx );
			
			// info flag
			i32Size			= pSubInfo->m_pSubHead->_Code;

			// save rollback data
			rollBack.m_ui32InfoFlag = pSubInfo->m_pChara_Def->iInfoFlag;
			_SaveData( &rollBack );
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CHARASTATE )
			{
				if( !_ParseCharaState( pRoom, ui32SlotIdx, pSubInfo->m_pChara_State ) )
				{
					_RollBack( &rollBack );
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT )
			{
				if( FALSE == _ParseCharaTakingObject( pRoom, ui32SlotIdx, &pSubInfo->m_TakingObject ) )
				{
					_RollBack( &rollBack );
					return 0;
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CHARAPOS )
			{
				if( !_ParseCharaPosRotPacket( pRoom, ui32SlotIdx, pSubInfo->m_pChara_PosRot, r32PacketTime, ui32RespawnCount ) )
				{
					_RollBack( &rollBack );
					return 0;
				}

				if( g_pConfig->GetCheckLevel(HACK_TYPE_RESPAWN_POSITION) )
				{
					for( INT32 i32SlotIdx = 0; i32SlotIdx < SLOT_MAX_COUNT; i32SlotIdx++ )
					{
						if( GetBit( pRoom->m_ui32FlagChkRespawnPos, i32SlotIdx ))
						{
							if( pRoom->IsHackUser( i32SlotIdx ) ) continue;
							if( !(pRoom->GetUserAlive( i32SlotIdx )) ) continue;
							if( pRoom->GetRespawnCount(i32SlotIdx) != ui32RespawnCount ) continue;

							ClearBit( &pRoom->m_ui32FlagChkRespawnPos, i32SlotIdx );				

							if( !(pRoom->IsValidRespawnPos( i32SlotIdx, pRoom->m_aui32RespawnIdx[i32SlotIdx] )) ) // 내부에서 위반 카운트 검사
							{
								pRoom->EnqueueKickUser( i32SlotIdx, HACK_TYPE_RESPAWN_POSITION );
								return FALSE;
							}
						}
					}
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_ACTIONKEY ) 
			{
				//if( FALSE == _ParseCharaActionKey( pRoom, ui32SlotIdx, &pSubInfo->m_ActionKey ) )
				//{	// use object가 여러번 오는 경우 실패.
				//	return 0;
				//}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE ) 
			{
				//pSubInfo->m_pChara_SyncObject;
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_RADIO )
			{
				//pSubInfo->m_pChara_Radio;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_WEAPONPARAM )
			{
				//pSubInfo->m_pChara_WeaponParam;
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_LIFE )
			{
				I3ASSERT(0); // 여기 들어오면 안됩니다
				_RollBack( &rollBack );
				return 0;
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE )
			{
				//pSubInfo->m_pui8Chara_SuicideCount;
				//pSubInfo->m_pChara_Suicide;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_MISSION )
			{
				if( FALSE == _ParseMissionPacket( pRoom, ui32SlotIdx, &pSubInfo->m_Mission, r32PacketTime ) )
				{
					return 0; // 해킹 이외에도 false 리턴 많음.
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
			{
				if( FALSE == _ParseDropWeaponPacket( pRoom, ui32SlotIdx, &pSubInfo->m_DropWeapon ) )
				{
					return 0; // 이미 버려진 무기: false 리턴
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_GETWEAPON )
			{
				if( FALSE == _ParseGetWeaponPacket( pRoom, ui32SlotIdx, &pSubInfo->m_GetWeapon ) )
				{
					return 0;// 바닥에 없는 무기: false 리턴
				}
			}

			if (pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
			{
				if (FALSE == _ParseDirectPickUpPacket(pRoom, ui32SlotIdx, &pSubInfo->m_DirectPickUp))
				{
					return 0; // 한 번에 안되는 무기
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_FIRE )
			{
				if( FALSE == _ParseBulletPacket( pRoom, ui32SlotIdx, &pSubInfo->m_Fire, r32PacketTime ) )
				{
					_RollBack( &rollBack ); 
					return 0;
				}
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HIT )
			{
				I3ASSERT(0); // 여기 들어오면 안됩니다
				_RollBack( &rollBack ); 
				return 0;
			}
			
			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYCHARA )
			{
				UINT32 ui32Count					= *pSubInfo->m_pui8Chara_Hit_ByChara;
				N_PCINFO_HIT_DEDICATED2* pHitInfo	= pSubInfo->m_pChara_Hit_ByChara;

				if( !(pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_FIRE) )
				{
					BOOL bPacketCheck = FALSE;
					WeaponInfo * pWeapon = Table::GetExtWeaponInfo( pHitInfo->_tWeapon.GetWeaponID() );
					if( NULL == pWeapon )
					{
						DSERROR( "P2P_INFO_FLAG_HITBYCHARA - pWeapon is NULL %d\n", pHitInfo->_tWeapon.GetWeaponID() );
						_RollBack( &rollBack ); 
						return 0;
					}

					if( Table::IsThrowWeapon( &pHitInfo->_tWeapon ) )
					{
						bPacketCheck = FALSE;
					}
					else 
					{
						bPacketCheck = TRUE;
					}

					if( bPacketCheck )
					{
						I3TRACE( "Hit - Fire Check !!!!\n" );

						HACK_LOG hi;
						hi.AddInfo("Hit - Fire Check\t P2P_INFO_FLAG_HITBYCHARA\t ItemID:%d\n",pWeapon->m_TItemID);

						char* msg = hi.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_FIRE_SPEED );
						if( msg) DSERROR(msg);

						_RollBack( &rollBack ); 
						return 0;
					}
				}
				
				if( FALSE == _ParseHitPacketByChara( pRoom, ui32SlotIdx, pHitInfo, ui32Count ) )
				{
					DSMSG("P2P_INFO_FLAG_HITBYCHARA dropped\n");

					_RollBack( &rollBack ); 
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION ) 
			{
				UINT32 ui32Count					= *pSubInfo->m_pui8Chara_Hit_ByExplosion;
				N_PCINFO_HIT_BYEXPLOSION* pHitInfo	= pSubInfo->m_pChara_Hit_ByExplosion;
				
				BOOL bPacketCheck = FALSE;
				WeaponInfo * pWeapon = Table::GetExtWeaponInfo( pHitInfo->_stWeapon.GetWeaponID() );
				if( NULL == pWeapon )
				{
					I3TRACE( "P2P_INFO_FLAG_HITBYEXPLOSION - pWeapon is NULL %d\n", pHitInfo->_stWeapon.GetWeaponID() );
					DSERROR( "P2P_INFO_FLAG_HITBYEXPLOSION - pWeapon is NULL %d\n", pHitInfo->_stWeapon.GetWeaponID() );
					_RollBack( &rollBack ); 
					return 0;
				}
				
				if( FALSE == Table::IsThrowWeapon( &pHitInfo->_stWeapon ) )
				{
					bPacketCheck = TRUE;
				}

				if( bPacketCheck )
				{
					I3TRACE( "Hit - Fire Check2 !!!!\n" );

					HACK_LOG hi;
					hi.AddInfo("Hit - Fire Check\t P2P_INFO_FLAG_HITBYEXPLOSION\t ItemID:%d\n",pWeapon->m_TItemID);

					char* msg = hi.MakeLogString(pRoom,ui32SlotIdx,HACK_TYPE_FIRE_SPEED);
					if( msg) DSERROR(msg);

					_RollBack( &rollBack ); 
					return 0;
				}

				if( !_ParseHitPacketByExplosion( pRoom, ui32SlotIdx, pHitInfo, ui32Count ) )
				{
					DSMSG("P2P_INFO_FLAG_HITBYEXPLOSION dropped net_slot_idx=%d\n",  pHitInfo->_i16NetSlotIdx );
					
					_RollBack( &rollBack ); 
					return 0; // 파서에서 핵탐지
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_HITBYOBJECT ) // skip
			{
				UINT32 ui32Count				= *pSubInfo->m_pui8Chara_Hit_ByObject;
				N_PCINFO_HIT_BYOBJECT* pHitInfo = pSubInfo->m_pChara_Hit_ByObject;

				if( !_ParseHitPacketByObject( pRoom, ui32SlotIdx, pHitInfo, ui32Count ))
				{
					DSMSG("P2P_INFO_FLAG_HITBYOBJECT dropped\n");

					_RollBack( &rollBack );
					return 0;
				}
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_DEATH )
			{
				I3ASSERT(0); // 여기 들어오면 안됩니다
				_RollBack( &rollBack ); 
				return 0;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING )
			{
				I3ASSERT(0); // 여기 들어오면 안됩니다
				_RollBack( &rollBack ); 
				return 0;
			}

			if( pSubInfo->m_pChara_Def->iInfoFlag & P2P_INFO_FLAG_TOUCHDOWN )
			{
				if( !_ParseTouchDown( pRoom, ui32SlotIdx, &pSubInfo->m_TouchDown ) )
				{
					_RollBack( &rollBack ); 
					return 0;
				}
			}
			break;
		case P2P_SUB_HEAD_GRENADE :
		case P2P_SUB_HEAD_DROPEDWEAPON :
			break;
		default :
			return 0;
		} //switch(packet->Type)
	}
	return 1;
}

// sync 정보 전부 필요
BOOL	HMSParser::_ParseCharaState( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_CHARA_STATE* pState )
{
	CCharacter*	pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	if( NULL == pChara ) return FALSE;
	
	return TRUE;
}

BOOL	HMSParser::_ParseCharaTakingObject( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_TAKING_OBJECT* pObject )
{
	CCharacter* pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	if( NULL == pChara ) return FALSE;
	
	if( pObject->getOnLoadObject() )
	{
		switch( pRoom->GetStageUID() )
		{
		case STAGE_UID_HELISPOT:
		case STAGE_UID_OUTPOST:
		case STAGE_UID_STORMTUBE:		
		case STAGE_UID_UPTOWN:		
		case STAGE_UID_PORT_AKABA:
		case STAGE_UID_RUSHHOUR:
		case STAGE_UID_CARGOPORT:
			break;
		default:	return FALSE; // 위 맵들 외에는 움직이는 오브젝트가 없으므로 OnLoad 를 보낼 수 없다.
		}

		pChara->SetOnLoad( TRUE );
		pChara->SetOnLoadIndex( pObject->getOnLoadObjectIndex() );
		pRoom->m_r32OnLoadPacketTime[ui32SlotIdx] = pRoom->GetGameTime();
	}
	else
	{
		if( pChara->GetAlive() )
		{
			pChara->SetOnLoad( FALSE );
		}
	}
	
	return TRUE;
}


BOOL HMSParser::_ParseCharaPosRotPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pPosRot, REAL32 r32PacketTime, UINT32 ui32RespawnCount )
{
	CCharacter*	pChara = pRoom->GetGameCharacter(ui32SlotIdx);
	if( NULL == pChara )
	{
		DSERROR("_ParseCharaPosRotPacket [%d Slot Chara is Null]\n",ui32SlotIdx );
		return FALSE;
	}

	pRoom->SetPosPacketTime( ui32SlotIdx );
	

	// ---------------------------------------------------------------------
	// Ghost Check
	// ---------------------------------------------------------------------
	_CheckGhost( pRoom, pChara, r32PacketTime, ui32SlotIdx, pPosRot );
	pChara->SetPosRecvTime( r32PacketTime );	// 처음 받은 포지션인지?


	if( !pChara->GetAlive() || pRoom->GetRespawnCount(ui32SlotIdx) != ui32RespawnCount ) return TRUE;

	// ---------------------------------------------------------------------
	// set position
	// ---------------------------------------------------------------------

	N_POS	pos0;
	N_PCINFO_POSROTATION	pos1;
	pPosRot->CopyTo( &pos1 );

	pChara->GetPosRot()->_Pos.CopyTo( &pos0 );	// old position
	pChara->SetPosRot( &pos1 );					// new position (udp checker에서 float 검사됨)
	
	// -------------------------------------
	// Respawn Hack Control
	// -------------------------------------
	CRespawnState* pRespawnState = pRoom->GetRespawnState();
	if( pRespawnState->IsPosSet( ui32SlotIdx ) == FALSE )
	{
		I3TRACE( "RespawnState - PosSet\n" );
		pRespawnState->SetPos( ui32SlotIdx, &pos1._Pos, ui32RespawnCount );
		pos1._Pos.CopyTo( &pRoom->m_vPosOldSpeed[ ui32SlotIdx ] );
		pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]	= r32PacketTime;
		SetBit  ( &pRoom->m_ui32FlagChkRespawnPos,  ui32SlotIdx );
		return TRUE;
	}
	
	if( pChara->GetOnLoad() )
	{
		// 오브젝트에 탑승하였다 내렸을때 오브젝트 속도 때문에 스피드 오차가 생길 수 있음.
		pos1._Pos.CopyTo( &pRoom->m_vPosOldSpeed[ ui32SlotIdx ] );
		pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]	= r32PacketTime;
		pRoom->m_bCheckOnLoad[ui32SlotIdx] = TRUE;
		return TRUE;
	}	

	if (pRoom->m_bCheckOnLoad[ ui32SlotIdx ])
	{
		// 오브젝트에서 내려온 첫번째 좌표는 스피드 검사를 하지 않는다.
		pos1._Pos.CopyTo( &pRoom->m_vPosOldSpeed[ ui32SlotIdx ] );
		pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]	= r32PacketTime;
		pRoom->m_bCheckOnLoad[ ui32SlotIdx ] = FALSE;
		return TRUE;
	}

	//QA커맨드
	if(pRoom->IsQaCommandCheatUser(ui32SlotIdx))return TRUE;


	// ---------------------------------------------------------------------
	// GravityMove control
	// ---------------------------------------------------------------------
	pRoom->_CheckGravityMove(ui32SlotIdx,&pPosRot->_Pos);


	// ---------------------------------------------------------------------
	// speed hack control
	// ---------------------------------------------------------------------

	// 공룡의 경우 특수 기술 사용 때문에 검사가 어려움
	if( pRoom->IsDino( ui32SlotIdx ) ) return TRUE;

	INT8 i8LogLevel = g_pConfig->GetCheckLevel(HACK_TYPE_SPEED);
	// 스피드핵 로그버전의 경우 걸린유저는 다시 체크하지 않도록 한다
	if (pRoom->isHackUserForLog(ui32SlotIdx,HACK_TYPE_SPEED) && pRoom->isLogLevel(i8LogLevel)) return TRUE;

	if (i8LogLevel != HACK_LOG_LEVEL)
	{
		INT32 i32DetectRate = pRoom->getDetectRate(ui32SlotIdx)->IncreaseDetectRateCnt(HACK_TYPE_SPEED);
		if (i8LogLevel <= i32DetectRate )
			return TRUE;
	}

	if( pRoom->m_ar32TimeOldSpeed[ui32SlotIdx] == 0 )
	{
		pRoom->m_ar32TimeOldSpeed[ui32SlotIdx] = r32PacketTime;
		pos1._Pos.CopyTo( &pRoom->m_vPosOldSpeed[ ui32SlotIdx ] );
		return TRUE;
	}

	if(	0.5f < (r32PacketTime - pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]) ) // 0.5초 이내에 온 속도값은 오차가 큼 ( 클라이언트에는 가속도가 있기 때문 )
	{
		if (!pRoom->IsSpaceMode())
		{
			REAL32 r32Speed = pChara->CalcSpeed(&pRoom->m_vPosOldSpeed[ui32SlotIdx], &pos1._Pos, r32PacketTime - pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]);
			I3ASSERT(IsFiniteNumber(r32Speed));

			pRoom->m_ar32TimeOldSpeed[ui32SlotIdx] = r32PacketTime;
			pos1._Pos.CopyTo(&pRoom->m_vPosOldSpeed[ui32SlotIdx]);

			//터치 다운 성공시 3초동안 스피드 체크 하지않도록 수정
			if (pRoom->IsEscapeMissionMode())
			{
				if (pRoom->IsTouchDown(ui32SlotIdx))
					return TRUE;
			}

			if (IsFiniteNumber(r32Speed) && r32Speed > 0.0f)
			{
				EquipmentInfo stitem;
				REAL32 r32AddSpeed = 0;
				
				// 장비, 스킬로 인한 스피드 증가값 + 호칭으로 인한 증가값
				r32AddSpeed = pChara->GetAddSpeed() + pChara->GetAddSpeedRate(pRoom->GetPBVer(ui32SlotIdx)) + pRoom->GetDesigSpeed(ui32SlotIdx);

				REAL32 r32CharaSpeed = 0.0f;
	
				r32CharaSpeed = pRoom->m_aCharaInfo[ui32SlotIdx].m_fFrontSpeed * r32AddSpeed + 0.5f; // 0.5f 는 오차 범위	
				
				if (r32CharaSpeed == 0.0f)
				{
					DSERROR("_ParseCharaPosRotPacket GetFrontSpeed is 0.0f , CharaID:%d", pChara->GetParts(CHAR_EQUIPMENT_PARTS_CHARA));
				}
							
				//캐릭터의 포지션이 겹치는 경우 밀어내기로 인한 의도치 않은 스피드 상승 효과로
				//오차범위를 2.f 증가 시킨다.
				if (r32Speed > r32CharaSpeed + 2.f)//MAX_SPEED )
				{
					VEC3D ClientPos;
					VEC3D OldPos;

					UDP_UTIL::ConvertNet3DtoVec3D(&ClientPos, pos1.GetPos());
					UDP_UTIL::ConvertNet3DtoVec3D(&OldPos, &pRoom->m_vPosOldSpeed[ui32SlotIdx]);

					CHMS* pHMS = pRoom->getHMS();

					if( pHMS->GetHackCount( ui32SlotIdx, HACK_TYPE_SPEED ) > MAX_HACK_SPEED_COUNT )
					{
						HACK_LOG hl;
						hl.AddInfo(  "HACK_TYPE_SPEED!\t Speed:%.2f\t MaxSpeed:%.2f\n", r32Speed ,r32CharaSpeed );
						char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_SPEED );
						if( msg) DSHACK(msg);

						switch(i8LogLevel)
						{
						case HACK_LOG_LEVEL:
							pRoom->SetHackUserForLog(ui32SlotIdx,HACK_TYPE_SPEED);
							break;

						case HACK_PACKET_DROP_LEVEL:
							return FALSE; 
							break;
						default:
							pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_SPEED );
							break;
						}
						return FALSE; 
					}	

				} // hack
			}
		}
		else //저중력 모드 체크
		{
			REAL32 r32Speed = pChara->CalcSpeed3D(&pRoom->m_vPosOldSpeed[ui32SlotIdx], &pos1._Pos, r32PacketTime - pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]);
			I3ASSERT(IsFiniteNumber(r32Speed));

			REAL32 r32Time = r32PacketTime - pRoom->m_ar32TimeOldSpeed[ui32SlotIdx];
			pos1._Pos.CopyTo(&pRoom->m_vPosOldSpeed[ui32SlotIdx]);		
			UINT32	ui32NowJumpState = UDP_UTIL::GetJumpHoldState(pChara->GetState()->iState);

			if ( CHARA_SPACE_NONE != ui32NowJumpState )
			{
				pRoom->m_ar32TimeStore[ui32SlotIdx] += r32Time;
			}		
			
			if (IsFiniteNumber(r32Speed) && r32Speed > 0.0f)
			{
				EquipmentInfo stitem;
				REAL32 r32AddSpeed = 0;		
				// 장비, 스킬로 인한 스피드 증가값 + 호칭으로 인한 증가값
				r32AddSpeed = pChara->GetAddSpeed() + pChara->GetAddSpeedRate(pRoom->GetPBVer(ui32SlotIdx)) + pRoom->GetDesigSpeed(ui32SlotIdx);
								
				REAL32 r32CharaSpeed = pRoom->m_aCharaInfo[ui32SlotIdx].m_fFrontSpeed * r32AddSpeed + StageTable::GetInstance().m_fFrontMoveSpeed[pRoom->GetStageUID()] + pRoom->m_aCharaInfo[ui32SlotIdx].m_fSpaceFrontSpeed;
	
				if (CHARA_SPACE_FORCE == ui32NowJumpState || CHARA_SPACE_FORCE == pRoom->m_ui32OldState[ui32SlotIdx])							
					r32CharaSpeed += pChara->GetJumpholdSpeed() + StageTable::GetInstance().m_rGravity[pRoom->GetStageUID()] * pRoom->m_ar32TimeStore[ui32SlotIdx] * 0.5;
				else if(CHARA_SPACE_DOWN == ui32NowJumpState || CHARA_SPACE_DOWN == pRoom->m_ui32OldState[ui32SlotIdx])
					r32CharaSpeed += StageTable::GetInstance().m_rGravity[pRoom->GetStageUID()] * pRoom->m_ar32TimeStore[ui32SlotIdx];
				else if (CHARA_SPACE_JUMP == ui32NowJumpState || CHARA_SPACE_JUMP == pRoom->m_ui32OldState[ui32SlotIdx])
					r32CharaSpeed += StageTable::GetInstance().m_rGravity[pRoom->GetStageUID()] * pRoom->m_ar32TimeStore[ui32SlotIdx] * 0.5;
							
				if (r32CharaSpeed == 0.0f)
				{
					DSERROR("_ParseCharaPosRotPacket GetFrontSpeed is 0.0f , CharaID:%d", pChara->GetParts(CHAR_EQUIPMENT_PARTS_CHARA));
				}			
				
				if (r32Speed > r32CharaSpeed + 4.0f)//MAX_SPEED )
				{					
					//저중력 모드는 로그만 남긴다.
					{
						HACK_LOG hl;
						hl.AddInfo("HACK_TYPE_SPACE!\t Speed:%.2f\t MaxSpeed:%.2f\n, State :%d, oldstate : %d ", r32Speed, r32CharaSpeed, ui32NowJumpState, pRoom->m_ui32OldState[ui32SlotIdx]);
						char* msg = hl.MakeLogString(pRoom, ui32SlotIdx, HACK_TYPE_SPACE);
						if (msg) DSHACK(msg);
					}

				} // hack
				if (pRoom->m_ui32OldState[ui32SlotIdx]!= ui32NowJumpState || CHARA_SPACE_NONE == ui32NowJumpState)
					pRoom->m_ar32TimeStore[ui32SlotIdx] = 0.0f;

				pRoom->m_ui32OldState[ui32SlotIdx] = ui32NowJumpState;
			}
			
		}
		pos1._Pos.CopyTo( &pRoom->m_vPosOldSpeed[ ui32SlotIdx ] );
		pRoom->m_ar32TimeOldSpeed[ui32SlotIdx]	= r32PacketTime;

	} // check speed range	
	return TRUE;
}

BOOL HMSParser::_ParseCharaWeaponParam( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pParam, REAL32 r32PacketTime )
{
	// 인도네시아와 개발 버전만 검사(2012-07-09). 
	if( g_pConfig->m_ui8NationalCode != 1 && g_pConfig->m_ui8NationalCode != 4 ) return TRUE; 
	if( g_pConfig->GetCheckLevel(HACK_TYPE_WEAPON_PARAM) == 0 )					 return TRUE; 

	CCharacter* pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	I3ASSERT( pChara != NULL );
	
#if defined (USE_WATCH_PARAM)

	if( HACK_TYPE_NO != _CheckWatchParam( pRoom, ui32SlotIdx, pParam ) )
	{
		pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_WEAPON_PARAM ); //  kick.
		return FALSE;
	}
#else
	I3ASSERT( pParam->iClass < MAX_WEAPON_PARAM );
	I3ASSERT( pParam->iCount <= MAX_WEAPON_PARAM_ARGS );

	if( HACK_TYPE_NO != _CheckWeaponParam( pRoom, ui32SlotIdx, pParam ) )
	{
		pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_WEAPON_PARAM ); //  kick.
		return FALSE;
	}
#endif

	return TRUE;
}


// 검토 필요 yjha 2013-01-04
// 1. NET_MISSION_STATE는 10진수 enum임. cancel 셋팅시 비트연산하고 있음 + crash 가능
// 2. life -> respawn recv flag 사용.
// 3. hack인 경우에만 버리도록 수정. 
BOOL  HMSParser::_ParseMissionPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_MISSION* pMission, REAL32 r32PacketTime )
{
	if( g_pConfig->GetCheckLevel( HACK_TYPE_INSTALL_C4) == 0 ) return TRUE;
	
	INT32				i32GroupIdx		= pRoom->GetGroupIndex();
	CCharacter*	pChara			= pRoom->GetGameCharacter( ui32SlotIdx );
	I3ASSERT( pChara != NULL );
	
	STAGE_UID		  eStageUID			= pRoom->GetStageUID(); 	
	REAL32			  r32DistSquared	= 0.0f;			// (bomb position - user's position) ^ 2
	N_POS*			  pPos				= NULL;			// c4 install position
	VEC3D			  vPos;								// temp
	BOOL			  bIsHack			= FALSE;		// return value
	
	// bomb state / area / Trigger arm or disarm start time
	NET_MISSION_STATE missionState = pMission->GetState();
	BOMB_AREA		  area		   = (BOMB_AREA)pMission->GetArea();
	REAL32			  r32StartTime = pMission->GetStartTime();

	switch ( missionState )
	{
	case NET_MISSION_TRIGGER_ARM_START: 
		{
			// 이미 설치가 된 경우 무시합니다.
			if( pRoom->m_bArmDone ) return FALSE;

			// 패킷 타임보다 클 수 없다.
			if( r32StartTime > r32PacketTime )
			{
				DSERROR("HMS:STAGE:%d,G:%d,S:%d,ARM Start Time^2=%f, PacketTime^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32StartTime, r32PacketTime );
				return FALSE;
			}

			// 설치 시간이 이렇게 빠를 수 없습니다.
			if( r32StartTime <= 10.0f )
			{
				DSERROR("HMS:STAGE:%d,G:%d,S:%d,ARM Start Time^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32StartTime );
				return FALSE;
			}

			I3TRACE("[%d] NET_MISSION_TRIGGER_ARM_START\n", ui32SlotIdx);

			// chara position
			pPos = &pChara->GetPosRot()->_Pos;
			I3ASSERT( IsOverflow( pPos ) == FALSE ); // position 값을 넣기 전에 검사해야 함
			UDP_UTIL::ConvertNet3DtoVec3D( &vPos, pPos );

			NxVec3 NvPos( vPos.x, vPos.y, vPos.z );

			// check position
			if( area == BOMB_AREA_A )
			{
				if( NvPos.y > StageTable::GetInstance().m_vBombPosition_A[eStageUID].y + 0.2f ||
					NvPos.y + 0.2f < StageTable::GetInstance().m_vBombPosition_A[eStageUID].y ) return FALSE;

				r32DistSquared = StageTable::GetInstance().m_vBombPosition_A[eStageUID].distanceSquared( NvPos );
				if( !IsFiniteNumber( r32DistSquared ) ) return FALSE;

				if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 1.0...
				{
					if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
					{
						if( r32StartTime + 1.5f < r32PacketTime )
						{
							pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
							return FALSE;
						}

						pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
						return TRUE;
					}
				}
				else
				{
					DSERROR("HMS:STAGE:%d,G:%d,S:%d,BOMB_A distance^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32DistSquared );
					return FALSE;
				}
			}
			else if( area == BOMB_AREA_B)
			{

				if( false == StageTable::GetInstance().m_abBombB[eStageUID] )
				{
					DSERROR("C4Error StageUID:%d Bomb Area_Error\n", eStageUID);
					return FALSE;
				}

				// 스톰튜브 예외처리
				if( STAGE_UID_STORMTUBE == eStageUID ) // 1286m (로컬좌표)
				{
					if (pChara->GetOnLoad())
					{
						//Local좌표의 경우 Local오브젝트에 맞춰진 좌표이며 실제 축이 다름
						//월드 좌표 변환 계산이전까지 로컬좌표로 체크
						if( NvPos.z > g_MovingBombPos.z + 0.2f ||
							NvPos.z + 0.2f < g_MovingBombPos.z	) return FALSE;

						r32DistSquared = g_MovingBombPos.distanceSquared( NvPos );
						if( !IsFiniteNumber( r32DistSquared ) ) return FALSE;

						I3TRACE("Len : %f \n",r32DistSquared );

						if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 0.9...
						{

							if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
							{
								if( r32StartTime + 1.5f < r32PacketTime )
								{
									pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
									return FALSE;
								}
								pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
								return TRUE;
							}
						}
					}
				}
				else
				{
					if( NvPos.y > StageTable::GetInstance().m_vBombPosition_B[eStageUID].y + 0.2f ||
						NvPos.y + 0.2f < StageTable::GetInstance().m_vBombPosition_B[eStageUID].y ) return FALSE;

					r32DistSquared = StageTable::GetInstance().m_vBombPosition_B[eStageUID].distanceSquared( NvPos );
					if( !IsFiniteNumber( r32DistSquared ) ) return FALSE;

					if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 0.9...
					{
						if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
						{
							if( r32StartTime + 1.5f < r32PacketTime )
							{
								pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
								return FALSE;
							}
							pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
							return TRUE;
						}
					}
					else
					{
						DSERROR("HMS:STAGE:%d,G:%d,S:%d,BOMB_B distance^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32DistSquared );
						return FALSE;
					}
				}
			}
			else
			{
				I3ASSERT(0);
				DSERROR("HMS:STAGE:%d,G:%d,S:%d,NOT BOMB_A or BOMB_B\n", eStageUID, i32GroupIdx, ui32SlotIdx );
				return FALSE;
			}
		}
		break;
	case NET_MISSION_TRIGGER_ARM_CANCEL:
		{
			I3TRACE("[%d] NET_MISSION_TRIGGER_ARM_CANCEL\n", ui32SlotIdx);
			return TRUE;
		}
		break;
	case NET_MISSION_TRIGGER_ARM_DONE: 
		{
			DSERROR("HMS:STAGE:%d,G:%d,S:%d, Done Packet\n", eStageUID, i32GroupIdx, ui32SlotIdx );
			bIsHack = TRUE;
		}
		break;
	case NET_MISSION_TRIGGER_ARM_FAIL:
		break;
	case NET_MISSION_TRIGGER_DISARM_START: 
		{
			// 아직 설치가 안된 경우 무시합니다.
			if( !pRoom->m_bArmDone ) return FALSE;

			// 패킷 타임보다 클 수 없다.
			if( r32StartTime > r32PacketTime )
			{
				DSERROR("HMS:STAGE:%d,G:%d,S:%d,ARM Start Time^2=%f, PacketTime^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32StartTime, r32PacketTime );
				return FALSE;
			}

			// 해제 시간이 이렇게 빠를 수 없습니다.
			if( r32StartTime <= 10.0f )
			{
				DSERROR("HMS:STAGE:%d,G:%d,S:%d,DISARM Start Time^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32StartTime );
				return FALSE;
			}

			I3TRACE("[%d] NET_MISSION_TRIGGER_DISARM_START\n", ui32SlotIdx);

			// chara position
			pPos = &pChara->GetPosRot()->_Pos;
			I3ASSERT( IsOverflow( pPos ) == FALSE ); // 포지션 값을 넣기 전에 검사해야 함
			UDP_UTIL::ConvertNet3DtoVec3D( &vPos, pPos );

			NxVec3 NvPos( vPos.x, vPos.y, vPos.z );

			// check position
			if( pRoom->m_MissionArea == area )
			{
				if( area == BOMB_AREA_A )
				{
					if( NvPos.y > StageTable::GetInstance().m_vBombPosition_A[eStageUID].y + 1.1f ||
						NvPos.y + 1.1f < StageTable::GetInstance().m_vBombPosition_A[eStageUID].y	) return FALSE;

					r32DistSquared = StageTable::GetInstance().m_vBombPosition_A[eStageUID].distanceSquared( NvPos );

					if( !IsFiniteNumber( r32DistSquared ) ) return FALSE;

					if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 1.0...
					{
						if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
						{
							if( r32StartTime + 1.5f < r32PacketTime )
							{
								pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
								return FALSE;
							}

							pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
							return TRUE;
						}
					}
					else
					{
						DSERROR("HMS:STAGE:%d,G:%d,S:%d,UNINSTALL BOMB_A distance^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32DistSquared );
						return FALSE;
					}
				}
				else if( area == BOMB_AREA_B )
				{
					// 스톰튜브 예외처리
					if( STAGE_UID_STORMTUBE == eStageUID ) // 1286m (로컬좌표)
					{

						if (pChara->GetOnLoad())
						{
							if( NvPos.z > g_MovingBombPos.z + 1.1f ||
								NvPos.z + 1.1f < g_MovingBombPos.z	) return FALSE;

							r32DistSquared = g_MovingBombPos.distanceSquared( NvPos );
							if( !IsFiniteNumber( r32DistSquared ) ) return FALSE;

							if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 0.9...
							{
								if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
								{

									if( r32StartTime + 1.5f < r32PacketTime )
									{
										pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
										return FALSE;
									}
									pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
									return TRUE;
								}
							}

						}
						else
						{
							if( NvPos.y > g_MovedBombPos.y + 1.1f ||
								NvPos.y + 1.1f < g_MovedBombPos.y	) return FALSE;

							r32DistSquared = g_MovedBombPos.distanceSquared( NvPos );
							if( !IsFiniteNumber( r32DistSquared ) )
								return FALSE;

							I3TRACE("Len : %f \n",r32DistSquared );

							if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 0.9...
							{

								if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
								{
									if( r32StartTime + 1.5f < r32PacketTime )
									{
										pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
										return FALSE;
									}
									pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
									return TRUE;
								}
							}
						}
					}
					else
					{
						if( NvPos.y > StageTable::GetInstance().m_vBombPosition_B[eStageUID].y + 1.1f ||
							NvPos.y + 1.1f < StageTable::GetInstance().m_vBombPosition_B[eStageUID].y	)	return FALSE;

						r32DistSquared = StageTable::GetInstance().m_vBombPosition_B[eStageUID].distanceSquared( NvPos );
						if( !IsFiniteNumber( r32DistSquared ) ) return FALSE;

						if(  r32DistSquared < BOME_DISTANCE_SQUARE ) // 0.9...
						{
							if( pRoom->m_r32TriggerTime[ui32SlotIdx] == 0.0f )
							{
								if( r32StartTime + 1.5f < r32PacketTime )
								{
									pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
									return FALSE;
								}

								pRoom->m_r32TriggerTime[ui32SlotIdx] = r32StartTime;
								return TRUE;
							}
						}
						else
						{
							DSERROR("HMS:STAGE:%d,G:%d,S:%d,UNINSTALL BOMB_B distance^2=%f\n", eStageUID, i32GroupIdx, ui32SlotIdx, r32DistSquared );
							return FALSE;
						}
					}
				}
			}
			else
			{
				DSERROR("HMS:STAGE:%d,G:%d,S:%d,NOT BOMB_A or BOMB_B\n", eStageUID, i32GroupIdx, ui32SlotIdx );
				return FALSE; // packet error
			}
		}
		break;
	case NET_MISSION_TRIGGER_DISARM_CANCEL: 
		{
			I3TRACE("[%d] NET_MISSION_TRIGGER_DISARM_CANCEL\n", ui32SlotIdx);
			return TRUE;
		}
		break;
	case NET_MISSION_TRIGGER_DISARM_DONE: 
		{
			DSERROR("HMS:STAGE:%d,G:%d,S:%d, Done Packet\n", eStageUID, i32GroupIdx, ui32SlotIdx );
			bIsHack = TRUE;
		}
		break;
	case NET_MISSION_TRIGGER_DISARM_FAIL: 
		break;
	default:
		I3ASSERT(0);
		return FALSE;
	}
	
	if( bIsHack )
	{
		pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_INSTALL_C4 );
		return FALSE; // 패킷이 처리되지 않도록
	}

	return TRUE;
}

BOOL HMSParser::_ParseGetWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_GETWEAPON* pWeapon )
{
	CCharacter* pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	I3ASSERT( pChara != NULL );
	
	// find weapon on the floor
	UINT32 ui32Idx	= pWeapon->getIndex();

	N_POS* pPos = NULL;
	if( FALSE == pChara->GetOnLoad() ) pPos = pRoom->getChara_PosRot(ui32SlotIdx)->GetPos();

	if ( NULL == pPos )
	{
		pWeapon->setSuccess(0);
		return FALSE;
	}

	VEC3D vecChar;
	UDP_UTIL::ConvertNet3DtoVec3D( &vecChar, pPos );

	if( pRoom->IsGetDropWeaponPos( ui32Idx, &vecChar ) )
	{			
		I3TRACE("_ParseGetWeaponPacket:%d: pickup weapon success (ItemID:%d)\n", ui32SlotIdx, pWeapon->getWeaponID() );

		if ( Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7_OUTPOST)	== pWeapon->getWeaponID() ||
				Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7_BOSS)	== pWeapon->getWeaponID() ||
				Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7)		== pWeapon->getWeaponID())
		{
			pChara->SetNaturalRPGGet(TRUE);
		}
	}
	else
	{
		pWeapon->setSuccess( 0 );
		I3TRACE("_ParseGetWeaponPacket:%d: Weapon Not Found on the floor: ItemID = %d, OR Not yet setting POS\n", ui32SlotIdx, pWeapon->getWeaponID() );
		return FALSE;
	}
	
	return TRUE;
}

BOOL HMSParser::_ParseDirectPickUpPacket(CDediRoom * pRoom, UINT32 ui32SlotIdx, GAMEINFO_DIRECTPICKUP * pWeapon)
{
	return TRUE;
}

BOOL HMSParser::_ParseDropWeaponPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pDropWeapon )
{
	return TRUE; // 드롭 실패한 경우에도 중계해야 함
}


BOOL	HMSParser::_ParseBulletPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire, REAL32 r32PacketTime )
{
	CCharacter*	pChara	= pRoom->GetGameCharacter( ui32SlotIdx );

	if( NULL == pChara )
	{
		//DSERROR( "ParseBulletPacket : pChara is NULL\n" );
		return FALSE;
	}
	if( !pChara->GetAlive() )
	{
		//DSERROR( "ParseBulletPacket : pChara is dead\n" );
		return FALSE;
	}
	if( pChara->IsUseM197() )	return TRUE;
	
	// QA커맨드 
	if (pRoom->IsQaCommandCheatUser(ui32SlotIdx))
		return TRUE;
	// ----------------------------------------------------------------------------------
	// save bullet and get previous value
	// ----------------------------------------------------------------------------------

	UINT32 ui32StateOnFire = pChara->GetState()->iState;
	pChara->SetStateOnFire( ui32StateOnFire );

	// dual 무기인지 확인
	UINT16				ui16Dec		= 1;

	UINT32	ui32Usage		= pFire->getWeaponUsage();
	UINT32	ui32WeaponID	= pRoom->GetServerWeaponID( ui32SlotIdx, ui32Usage );
	if( 0 == ui32WeaponID )
	{
		//DSERROR( "ParseBulletPacket : WeaponID is 0\n" );
		return FALSE;
	}

	WeaponInfo* pBaseWeapon = Table::GetWeaponInfo ( ui32WeaponID );
	
	if( pBaseWeapon )
	{
		if( pFire->getWeaponExtension() )
		{
			WeaponInfo* pExtWeapon = Table::GetWeaponInfo( pBaseWeapon->FindWeaponID(WEAPON::EXT_DUAL) );
			if( pExtWeapon && pExtWeapon->IsValid() )
			{
				ui16Dec = pExtWeapon->m_ui8FireDecBulletCount;
				if( ui16Dec == 0 ) ui16Dec = 1; // 셋팅 안되어 있는 경우
			}
		}
		else
		{
			ui16Dec = pBaseWeapon->m_ui8FireDecBulletCount;
			if( ui16Dec == 0 ) ui16Dec = 1;
		}
	}
	else
	{
		DSERROR( "ParseBulletPacket : not valid weapon %d\n", ui32WeaponID );
		return FALSE;
	}
	
	// ----------------------------------------------------------------------------------
	// 회수 제한 없는 공격
	// ----------------------------------------------------------------------------------
	UINT32	ui32Class	= pFire->getWeaponClass();
	UINT32	ui32Number	= pFire->getWeaponNumber();
	
	if( ui32Class >= WEAPON_CLASS_DINO && ui32Class <= WEAPON_CLASS_ELITE_LEFT_SCRATCH ) 
	{
		return TRUE;  // dino
	}

	if( ui32Class == WEAPON_CLASS_BOMBTRIGGER) 
		return TRUE; // c5 : 폭발 회수로 count.

	// ----------------------------------------------------------------------------------
	// right mouse button 
	// ----------------------------------------------------------------------------------
	
	UINT8 ui8SecFire = 0;
	UDP_UTIL::ConvertNetActiontoChara( NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ui8SecFire, NULL, ui32StateOnFire );

	if( ui8SecFire && ui32Class != WEAPON_CLASS_KNIFE )
	{
		//I3TRACE("***************** \n");
		return TRUE; // to count ballistic knife, mini axe. but exclude carbin knife.
	}

	// ----------------------------------------------------------------------------------
	// check bullet count
	// ----------------------------------------------------------------------------------
	//무기의 익스텐션타입 얻기(소음기,착검 예외처리)
	INT32 i32ExtType = 0;
	i32ExtType = i32ExtType | Table::GetWeaponExtType(pBaseWeapon);
	
	if (FALSE == pChara->IsDecreaseBullet(pFire->getID(), pFire->getWeapon(), ui16Dec, i32ExtType))
	{
		DSERROR("[%d] ParseBulletPacket : RemainBullets is 0 - type 1, WeaponID : %d\n", ui32SlotIdx, pFire->getWeaponID());
		return FALSE;
	}

	switch (ui32Usage)
	{
	case WEAPON_SLOT_MELEE:	// 근접무기 투척(fire bullet count) 
		{
			WeaponInfo * pFireWeapon = Table::GetWeaponInfo( pFire->getWeaponID() );
			if( pFireWeapon == NULL ) break;

			WeaponInfo * pExtInfo = Table::GetWeaponInfo( pFireWeapon->FindWeaponID(WEAPON::EXT_THROW) );
			if( pExtInfo == NULL ) break;

			BOOL bCheck = FALSE;
			if( pExtInfo->IsValid() && pExtInfo->m_ui16MaxBullet > 0 )
				bCheck = TRUE;

			if( ui8SecFire && bCheck )
			{
				pChara->DecreaseBullet(pFire->getWeapon(), ui16Dec);
			}
		}
		break;
	}

	// ----------------------------------------------------------------------------------
	// 발사 속도 검사 : primary weapon은 respawn 받았을 때만 셋팅됨
	// ----------------------------------------------------------------------------------

	if( g_pConfig->GetCheckLevel( HACK_TYPE_FIRE_SPEED ) )
	{
		if( HACK_TYPE_NO != _CheckFireSpeed( pRoom, ui32SlotIdx, pFire, r32PacketTime ) )
		{
			DSERROR( "ParseBulletPacket : CheckFireSpeed\n" );
			return FALSE; // kick 하지 않음.
		}
	} // level check


	if ( g_pConfig->GetCheckLevel( HACK_TYPE_WEAPON ) )
	{
		if( FALSE == pRoom->IsEscapeMissionMode())
		{
			if( HACK_TYPE_NO != _CheckWeaponSync( pRoom, ui32SlotIdx, pFire->getWeapon() ) )
			{
				// 내부에서 로그 남김
				return FALSE;
			}
		}
	}
	if ( g_pConfig->GetCheckLevel( HACK_TYPE_RPG_CHEAT ) )
	{
		if( FALSE == pRoom->IsEscapeMissionMode())
		{
			if ( _CheckNaturalRPGGet( pRoom, ui32SlotIdx, ui32Class, ui32Number ) )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

// TRUE: hacking
BOOL   HMSParser::_CheckRecallHack( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_POS_R* pDeathPos0, N_POS* pDeathPos1, N_PCINFO_HIT_WEAPON* pHitWeapon , BOOL bAttacker )
{
	if (pRoom->IsSpaceMode()) return FALSE;  //스페이스 모드 리콜핵 체크 무시

	VEC3D   vPosTemp; 
	N_POS_R vPosHitChara;
	REAL32  r32DistSqr = 0.0f;

	I3ASSERT( pHitWeapon != NULL );
	I3ASSERT( IsOverflow( pDeathPos0 ) == FALSE );	// hit packet (attacker)
	I3ASSERT( IsOverflow( pDeathPos1 ) == FALSE );	// pos packet (hit chara)

	if( g_pConfig->GetCheckLevel(HACK_TYPE_RECALL) == 0 ) return FALSE;

	// 서버 포지션이 0인 경우는 해당유저의 포지션 정보가 업데이트되지 않는 경우입니다.
	REAL16	r16DefPos = 0;
	if( r16DefPos == pDeathPos1->x && r16DefPos == pDeathPos1->y && r16DefPos == pDeathPos1->z ) return FALSE;
	
	if( UDP_UTIL::ConvertNet3DtoVec3D( &vPosTemp, pDeathPos1 ) == false ) return FALSE;
	
	vPosHitChara.x = vPosTemp.x;
	vPosHitChara.y = vPosTemp.y;
	vPosHitChara.z = vPosTemp.z;
	
	r32DistSqr = (REAL32)::distance_squared( pDeathPos0, &vPosHitChara );

	BOOL bHackUser = FALSE;

	if (bAttacker)
	{
		if( r32DistSqr > ATTACKER_DISTANCE_SQUARE ) // 2.2이상 차이가 나는 경우
			bHackUser =TRUE;
	}
	else
	{
		if( r32DistSqr > RECALL_DISTANCE_SQURE ) // 5m이상 차이가 나는 경우
			bHackUser =TRUE;
	}

	if( bHackUser ) 
	{
		if( pRoom->getHMS()->GetHackCount( ui32SlotIdx, HACK_TYPE_RECALL) > MAX_HACK_RECALL_COUNT )
		{
			HACK_LOG hl;
			hl.AddInfo( "bAttacker:%d, distance_squared=%f, death pos(hit=%.2f,%.2f,%.2f svr=%.2f,%.2f,%.2f), weapon:%d:%d", 
				bAttacker,
				r32DistSqr,
				pDeathPos0->x, pDeathPos0->y, pDeathPos0->z, 
				vPosHitChara.x, vPosHitChara.y, vPosHitChara.z,
				pHitWeapon->GetWeaponClass(), pHitWeapon->GetWeaponNumber() );

			char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_RECALL);
			if( msg) DSERROR(msg);
			return TRUE;
		}

		return FALSE;

	}

	return FALSE;
}

// 검사비율 적용 : 파서의 마지막 단계이기 때문에 보관하지 않습니다.
// 데미지 검사, 레인지 검사는 동일한 히트를 사용합니다. 
// 월샷은 케릭터가 죽었을 때만 체크합니다.
// yjha 로그 보강 필요 (2012-01-04)
BOOL HMSParser::_ParseHitPacketByChara( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pArrHit, UINT32 ui32Count )
{
	CCharacter* pAttacker = pRoom->GetGameCharacter( ui32SlotIdx );
	
	// 죽어있는 유저는 무시합니다.
	if( FALSE == pAttacker->GetAlive() )				return FALSE;
	
	// M197 을 사용하면 해킹 검사 제외 왜?? 김현우
	if( pAttacker->IsUseM197() )						return TRUE;

	//QA 커맨드
	if (pRoom->IsQaCommandCheatUser( ui32SlotIdx ))		return TRUE;

	for( UINT32 i = 0; i < ui32Count; i++ )
	{
		N_PCINFO_HIT_DEDICATED2* pHitInfo = pArrHit + i; // checked
		N_PCINFO_HIT_DEFAULT*	 pDefault = &pHitInfo->_tDefault;

		switch( pDefault->GetHitType() )
		{
		case GHT_CHARA:
			{
				// Hit 당한 유저가 현재 무적시간인지 확인
				if( pRoom->IsInvincibleTime( pDefault->GetIndex() ) )
				{	// 이 Idx 에 대한 예외처리가 들어가야 합니다. 김현우
					return FALSE;
				}
				
				if ( g_pConfig->GetCheckLevel( HACK_TYPE_WEAPON ) )
				{
					if( FALSE == pRoom->IsEscapeMissionMode())
					{
						UINT32 ui32Class = pHitInfo->_tWeapon.GetWeaponClass();
						UINT32 ui32Number = pHitInfo->_tWeapon.GetWeaponNumber();
						//UINT32 ui32Usage = pHitInfo->_tWeapon.GetWeaponUsage();

						T_ItemID TWeaponID = pHitInfo->_tWeapon.GetWeaponID();
						N_PCINFO_WEAPON UsedWeapon;
						UsedWeapon.Reset();
						UsedWeapon.SetWeaponID( TWeaponID );
						UsedWeapon.SetIsExtension( pHitInfo->_tWeapon.IsExtension() );
						UsedWeapon.SetAttackType( pHitInfo->_tWeapon.GetAttackType() );

						if( HACK_TYPE_NO != _CheckWeaponSync( pRoom, ui32SlotIdx, &UsedWeapon ) )
						{
							// 내부에서 로그 남김
							return FALSE;
						}
						if ( HACK_TYPE_NO != _CheckNaturalRPGGet( pRoom, ui32SlotIdx, ui32Class, ui32Number ) )
						{
							return FALSE; // 내부에서 로그 남김
						}
					}
				}

				// 무기의 거리가 올바른지 체크 합니다.
				if( HACK_TYPE_NO != _CheckWeaponRange( pRoom, ui32SlotIdx, pHitInfo ) )				return FALSE;

				if( g_pConfig->m_bPhysicsActive )
				{	
					BOOL bThrowKnife = IsThrowKnifeOnRMB(pHitInfo->_tWeapon.GetWeaponID(), (WEAPON::ATTACK_TYPE)pHitInfo->_tWeapon.GetAttackType());
					//UINT32 ui32ExtState = GET_ITEM_EXT_STATE(pHitInfo->_tWeapon.IsExtension());
					// 근접무기 투척일 경우 체크 불가 합니다.
					if (!bThrowKnife &&
						(g_pConfig->m_aui8HackCheckLevel[HACK_TYPE_WALLSHOT] > pRoom->getDetectRate(ui32SlotIdx)->IncreaseDetectRateCnt(HACK_TYPE_WALLSHOT) ) ) 
					{
					//티라노의 밟기공격의 경우 범위공격인 관계로 레이캐스트 정확한 결과를 
					//예측할 수 없습니다.
					//하드코딩 추후 수정하여야 합니다.
					if(!(pHitInfo->_tWeapon.GetWeaponClass() == WEAPON_CLASS_DINO &&
						 pHitInfo->_tWeapon.GetWeaponNumber() ==DINO_TREX_CRASH))
						pRoom->_EnqueueRaycastRequest(ui32SlotIdx,pHitInfo);
					}
				}

				// Check Head Shot
				CHARA_HIT_PART hitPart = pRoom->NetIdxToHitPart( (CHARA_HIT_PART)pDefault->GetHitPart() );
				UINT32			   ui32TargetIdx = pDefault->GetIndex();
				if( hitPart == CHARA_HIT_HEAD_OLD ||  hitPart == CHARA_HIT_HEAD ) // head_shot
				{
					// 헬멧 프로텍션 상태(데미지를 줄 수 없다)
					if( (pDefault->GetHelmet() & NET_HELMET_FLAG_PROTECT) &&
						pRoom->IsChaosMode() != TRUE) // NET_HELMET_FLAG_DROP과 oring 될 수 있음
					{
						I3TRACE("NET_HELMET_FLAG_PROTECT\n" );
						continue;
					}
				
					if( !pRoom->IsDino( ui32TargetIdx ) ) // 공룡은 헤드샷 데미지 다름.
					{
						if( pRoom->_CheckHeadShotHack( ui32SlotIdx, pDefault ) )
						{

							HACK_LOG hl;
							hl.AddInfo("HACK_TYPE_HEADSHOT \n");
							char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_HEADSHOT );
							if( msg ) DSHACK( msg );

							pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_HEADSHOT );
							return FALSE;
						}
					} // check dino
				} // part

				if( pRoom->GetUserOnLoad( ui32SlotIdx ) ) continue;

				if( pRoom->GetUserAlive( ui32TargetIdx ) && !(pRoom->GetUserOnLoad( ui32TargetIdx )) )
				{
					CCharacter* pTarget = pRoom->GetGameCharacter( ui32TargetIdx );
				
					if (pRoom->IsTRex(ui32TargetIdx) || pRoom->IsTRex(ui32SlotIdx))
						return TRUE;
				
					if( _CheckRecallHack( pRoom, ui32SlotIdx, &pHitInfo->_stFirePos, &pAttacker->GetPosRot()->_Pos, &pHitInfo->_tWeapon, TRUE ) ||
						_CheckRecallHack( pRoom, ui32SlotIdx, &pHitInfo->_HitPos,	&pTarget->GetPosRot()->_Pos, &pHitInfo->_tWeapon, FALSE ) )
					{
						HACK_LOG hl;
						hl.AddInfo("HACK_TYPE_RECALL \n");
						char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_RECALL );
						if( msg ) DSHACK( msg );

						pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_RECALL );
						return FALSE;
					}
				}
			}
			break;

		} // target chara

	}

	return TRUE;
}

// 검사비율 적용 : 파서의 마지막 단계이기 때문에 보관하지 않습니다.
// 마그넷 검사를 수행합니다.
// 이미 해킹유저인지 판단하여, 해킹 유저라면 무시합니다
// FALSE : 패킷 버림.
BOOL	HMSParser::_ParseHitPacketByExplosion( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_BYEXPLOSION* pArrHit, UINT32 ui32Count )
{
	// QA 커맨드 유저
	if( pRoom->IsQaCommandCheatUser( ui32SlotIdx ) )				return TRUE;
	// 수류탄 해킹 체크 유저
	if( FALSE == g_pConfig->GetCheckLevel( HACK_TYPE_GRENADE ) )	return TRUE;

	UINT32		ui32Class;
	UINT32		ui32Number;
	UINT32		ui32Usage;
	UINT32		ui32HitWeaponID;

	for( UINT32 i = 0; i < ui32Count; i++, pArrHit++ )
	{
		// hit weapon
		ui32HitWeaponID		= Table::GetExtWeaponID( pArrHit->_stWeapon.GetWeaponID() );
		ui32Class			= GET_ITEM_SUBTYPE(ui32HitWeaponID);
		ui32Number			= GET_ITEM_NUMBER(ui32HitWeaponID);
		ui32Usage			= GET_ITEM_TYPE(ui32HitWeaponID);

		// 소유중인 무기인지 체크
		if ( g_pConfig->GetCheckLevel( HACK_TYPE_WEAPON ) )
		{
			if( HACK_TYPE_NO != _CheckUseWeapon( pRoom, ui32SlotIdx, pArrHit->_stWeapon.GetWeaponID() ) )
			{
				return FALSE;
			}
		}	

		// --------------------------------------------------------
		// fly 없는 수류탄 검사
		// --------------------------------------------------------
		if( FALSE == Table::IsThrowWeapon( ui32HitWeaponID, pArrHit->_stWeapon.IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pArrHit->_stWeapon.GetAttackType()) ) )
		{
			DSERROR("ERR. weapon Class error. must be throwing1, throwing2, grenade shell or dino (%d:%d)\n", ui32Class, ui32Number );
			return FALSE;
		}	

		// --------------------------------------------------------
		// 거리 검사
		// --------------------------------------------------------

		if( ui32Class == WEAPON_CLASS_THROWING_GRENADE || IsWPSmoke( ui32Class ) )
		{
			// Hit Type 확인
			if( pArrHit->_tDefault.GetHitType() != GHT_CHARA ) continue; 
			if( pArrHit->_tDefault.GetIndex() == ui32SlotIdx ) continue; // 자살

			// Net slot index 확인
			UINT16 ui16NetSlotIdx = pArrHit->GetNetSlotIdx();
			if( ( ui16NetSlotIdx < ui32SlotIdx * MAX_THROWWEAPON_SLOP ) ||	( ui16NetSlotIdx >= (ui32SlotIdx + 1) * MAX_THROWWEAPON_SLOP ) )
			{
				I3ASSERT( ui16NetSlotIdx < MAX_THROWWEAPON_COUNT );
				I3ASSERT( ui16NetSlotIdx >= ui32SlotIdx * MAX_THROWWEAPON_SLOP && ui16NetSlotIdx < (ui32SlotIdx + 1) * MAX_THROWWEAPON_SLOP );
				DSERROR("HMSParser::_ParseHitPacketByExplosion NetSlotIdx error idx=%d, slot=%d, HitItmeID : %d, ExtItemID : %d\n", ui16NetSlotIdx, ui32SlotIdx, pArrHit->_stWeapon.GetWeaponID(), ui32HitWeaponID );
				return FALSE;
			}

			DS_THROWN_GRENADE* pThrownWeaponInfo = pRoom->GetThrownWeaponInfo( ui16NetSlotIdx );
			if( ! pThrownWeaponInfo ) 
			{
				I3ASSERT( pThrownWeaponInfo != NULL );
				DSERROR("HMSParser::_ParseHitPacketByExplosion NetSlotIdx error idx=%d, slot=%d\n", ui16NetSlotIdx, ui32SlotIdx );
				return FALSE;
			}

			// Hit Chara 확인
			// Hit 당한 유저가 현재 무적시간인지 확인	이 Idx 에 대한 예외처리가 들어가야 합니다. 김현우
			if( pRoom->IsInvincibleTime( pArrHit->_tDefault.GetIndex() ) )
			{
				return FALSE;
			}
			CCharacter* pHitChara = pRoom->GetGameCharacter( pArrHit->_tDefault.GetIndex() );
			if( pHitChara == NULL )			return FALSE;
			if( pHitChara->GetOnLoad() )	continue;
			if( pHitChara->GetHP() <= 0 )	continue;

			// weapon 확인
			if( pThrownWeaponInfo->m_weapon.GetWeaponClass() == ui32Class && 
				pThrownWeaponInfo->m_weapon.GetWeaponNumber() == ui32Number &&
				pThrownWeaponInfo->m_ui8State & NET_GRENADE_STATE_SETTLED )
			{
				N_POS* pHitCharaPos= &pHitChara->GetPosRot()->_Pos;
				N_POS* pGrenadePos = &pThrownWeaponInfo->m_pos;

				double dDistanceSquare = distance_squared2D( pHitCharaPos, pGrenadePos );

				if( dDistanceSquare > GRENADE_RANGE_SQUARE ) 
				{

					{//hack log	
						HACK_LOG hl;
						hl.AddInfo( "EXPLOSION RANGE (NetSlotIdx=%d, distance:%.3f, weapon:%d:%d)", ui16NetSlotIdx, dDistanceSquare, ui32Class, ui32Number );

						char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_GRENADE );
						if( msg ) DSERROR(msg);
					}

					return FALSE;

				}
			}
			// fly check 삭제. 직전 isFly()로만 검사 (2012-10-05 yjha)
		} // weapon class 
	} // for 

	return TRUE;
}

BOOL	HMSParser::_ParseHitPacketByObject( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_BYOBJECT* pArrHit, UINT32 ui32Count )
{
	// ----------------------------------------------------------------------------------
	// Hit 당한 유저가 현재 무적시간인지 확인
	// ----------------------------------------------------------------------------------
	if( GHT_CHARA == pArrHit->_tDefault.GetHitType() &&
		pRoom->IsInvincibleTime( pArrHit->_tDefault.GetIndex() ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	HMSParser::_ParseTouchDown( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_TOUCHDOWN* pTouchDown )
{
	//QA 커맨드
	if (pRoom->IsQaCommandCheatUser(ui32SlotIdx)) return TRUE;

	// 탈출 해킹 체크가 꺼져있으면 성공.
	if (!g_pConfig->GetCheckLevel(HACK_TYPE_ESCAPE)) return TRUE;

	STAGE_MODE eStageType = pRoom->GetStageType();
	switch ( eStageType )
	{
	case STAGE_MODE_ESCAPE:	// 탈출모드
		{
			// 인간팀, 터치다운 오브젝트 거리 체크.
			if( !pRoom->IsDino( ui32SlotIdx ) && !pRoom->CheckTouchDown( ui32SlotIdx, pTouchDown->GetTouchDownIdx() ) )
				return TRUE;
		}
		break;
	case STAGE_MODE_CONVOY:	// 탈취모드
		{	// 레드팀, 터치다운 오브젝트 거리 체크.
			if( !g_pConfig->GetCheckLevel(HACK_TYPE_USURPATION) ) return TRUE;

			if ( TEAM_RED == pRoom->GetTeamType( ui32SlotIdx ) && !pRoom->CheckTouchDown( ui32SlotIdx, pTouchDown->GetTouchDownIdx() )  )
				return TRUE;
		}
		break;
	default :
		break;
	}
	 
	HACK_LOG hl;
	hl.AddInfo( "HACK_TYPE_ESCAPE \n");
	char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_ESCAPE );
	if( msg) DSHACK(msg);

	pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_ESCAPE ); //  kick.
	return FALSE;
}