#include "pch.h"
#include "HMS.h"
#include "HMSParser.h"
#include "DediRoom.h"
#include "DroppedWeaponMgr.h"
#include "WeaponTable.h"

// 선행조건 RMB check
BOOL IsThrowKnifeOnRMB(UINT32 ui32WeaponID, WEAPON::ATTACK_TYPE eAttackType)
{
	UINT32 ui32Class = GET_ITEM_SUBTYPE(ui32WeaponID);

	//듀얼나이프 투척(ex - 쿠나이)인 경우 예외처리 합니다.
	if (ui32Class == WEAPON_CLASS_DUALKNIFE) return TRUE;

	if (ui32Class != WEAPON_CLASS_KNIFE) return FALSE;

	if (eAttackType != WEAPON::ATTACK_TYPE_R1) return FALSE;

	WeaponInfo * pWeaponInfo = Table::GetWeaponInfo(ui32WeaponID);
	if (!pWeaponInfo) return FALSE;

	UINT32 ui32ExtItemID = pWeaponInfo->FindWeaponID(WEAPON::EXT_THROW);
	if (0 == ui32ExtItemID) return FALSE;

	WeaponInfo * pExtInfo = Table::GetWeaponInfo(ui32ExtItemID);
	if (pExtInfo && pExtInfo->IsValid())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsMedicalKit( UINT32 ui32Class)
{
	return (ui32Class == WEAPON_CLASS_THROWING_HEAL ? TRUE : FALSE);
}

BOOL IsWPSmoke( UINT32 ui32Class)
{
	return (ui32Class == WEAPON_CLASS_THROWING_ATTACK ? TRUE : FALSE);
}

// -----------------------------------------------------
// 근접 무기 거리 검사
// -----------------------------------------------------
DS_HACK_TYPE HMSParser::_CheckWeaponRange( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pHitInfo )
{ 
	// Range 핵 체크가 켜져있는지 체크
	if( FALSE == g_pConfig->GetCheckLevel( HACK_TYPE_RANGE ) )	return HACK_TYPE_NO;
	
	// --------------------------------------------------------------------------------------------
	// 패킷내 거리 검사 (공격자 및 피격자 위치 검사는 death 발생시 magnet에서 처리:kick)
	// --------------------------------------------------------------------------------------------
	REAL32 r32DistanceSquare	= (REAL32)distance_squared( &pHitInfo->_stFirePos, &pHitInfo->_HitPos );	// position checked
	REAL32 r32WeaponRange		= Table::GetWeaponRange( &pHitInfo->_tWeapon );
	r32WeaponRange *= r32WeaponRange;
	
	if( r32DistanceSquare > r32WeaponRange ) 
	{
		HACK_LOG hl;
		hl.AddInfo( "Weapon Range Error : ID=%d, Range^2=%.3f, WeaponRange^2=%.3f", pHitInfo->_tWeapon.GetWeaponID(), r32DistanceSquare, r32WeaponRange );
		char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_RANGE );
		if( msg ) DSERROR(msg);
		
		return HACK_TYPE_RANGE; 
	}
	
	return HACK_TYPE_NO; 
}

// slow weapon speed
DS_HACK_TYPE HMSParser::_CheckFireSpeed( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire, REAL32 r32FireTime )
{
	CCharacter* pChara = pRoom->GetGameCharacter( ui32SlotIdx );
	if( ! pChara ) return HACK_TYPE_FIRE_SPEED;

	UINT32 ui32SlotItem = pChara->GetSlotItem();
	//UINT32 ui32Beret	= 0;
	
	REAL32		r32TimeOld		= pChara->GetLastFireTime();
	T_ItemID	TItemIDOld		= pChara->GetLastFireItemID();
	WEAPON::ATTACK_TYPE eAttackTypeOld = pChara->GetLastFireAttackType();
	pChara->SetLastFireInfo( r32FireTime, pFire->getWeaponID(), pFire->getWeaponAttackType() );

	if( r32TimeOld < I3_EPS ) // 1st shot
		return HACK_TYPE_NO;

	REAL32 r32ElapsedTime = r32FireTime - r32TimeOld;
// 	if( r32ElapsedTime > 1.0f ) // 검사할 필요 없음
// 		return HACK_TYPE_NO; 

	WeaponInfo* pWeaponInfo = WeaponTable::GetInstance().GetWeaponInfo( pFire->getWeaponID() );
	if( ! pWeaponInfo  || ! pWeaponInfo->IsValid() )
	{
		HACK_LOG hl;
		hl.AddInfo("HACK_TYPE_WEAPON_NOT_FOUND\tweapon:%d\n", pFire->getWeaponID() );
		char* msg = hl.MakeLogString( pRoom,
			ui32SlotIdx,
			HACK_TYPE_WEAPON_NOT_FOUND);

		if( msg) DSERROR(msg);

		return HACK_TYPE_WEAPON_NOT_FOUND; // kick 하지 않음(2012-11-26)
	}

	// 2016-09-27 채종현 : Decrease를 0.7에서 0.5로 낮춥니다. ( V1, V3 ) 공격 - 듀얼 - 공격 Delay 상관 없이 가능하게 하기 위함
	REAL32 r32DelayDecrease = 0.5f;

	//UINT32 ui32Type		= GET_ITEM_TYPE( pFire->getWeaponID() );
	UINT32 ui32Class	= GET_ITEM_SUBTYPE( pFire->getWeaponID() );
	UINT32 ui32Number	= GET_ITEM_NUMBER( pFire->getWeaponID() );
	
	if( ( ui32SlotItem & SLOT_ITEM_QUICK_CHANGE_WEAPON ) )
	{
		r32DelayDecrease = 0.3f;
	}

	switch( ui32Class )
	{
	//case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_DUALHANDGUN:
		break;
	default:
		{
			REAL32 rFireDelay;

			if( TItemIDOld == pFire->getWeaponID() )
			{
				if( eAttackTypeOld == pFire->getWeaponAttackType() )
				{
					// FireDelay 로 체크
					rFireDelay =  pWeaponInfo->m_r32FireDelay * r32DelayDecrease;
				}
				else
				{
					// AttackTime 으로 체크
					rFireDelay = pWeaponInfo->m_r32AttackDelay[ pFire->getWeaponAttackType() ] * r32DelayDecrease;
				}
			}
			else
			{
				// FireDelay 로 체크
				rFireDelay =  pWeaponInfo->m_r32FireDelay * r32DelayDecrease;
			}
			
			I3TRACE( "AttackType : %d, ElTime: %f, FireTime:%f\n", pFire->getWeaponAttackType(), r32ElapsedTime, rFireDelay );

			if( r32ElapsedTime < rFireDelay )
			{
				{//hack log	
					HACK_LOG hl;
					hl.AddInfo("HACK_TYPE_FIRE_SPEED\tweapon:%d,elapsed time:%f,delay:%f", 
						pFire->getWeaponID(), r32ElapsedTime, rFireDelay );

					char* msg = hl.MakeLogString( pRoom,ui32SlotIdx,HACK_TYPE_FIRE_SPEED);
					if( msg) DSERROR(msg);

				} // log

				return HACK_TYPE_FIRE_SPEED;
			}
		} // case
		break;
	} // switch ( class )

	return HACK_TYPE_NO;
}

DS_HACK_TYPE HMSParser::_CheckWeaponSync( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON* pWeapon )
{
	UINT8 ui8Usage		= pWeapon->GetWeaponUsage();
	UINT32 ui32Class	= pWeapon->GetWeaponClass();
	UINT32 ui32Number	= pWeapon->GetWeaponNumber();

	// RPG 인지 체크
	if( FALSE == _CheckRPG( pRoom, pWeapon->GetWeaponID() ) )
	{
		goto hack_user;
	}

	pRoom->CheckUseWeapon( ui32SlotIdx, pWeapon->GetWeaponID() );

	UINT32 ui32ServerWeaponID = 0;
	N_PCINFO_WEAPON *pServerWeapon = pRoom->getChara_Weapon(ui32SlotIdx);
	if (pServerWeapon)
	{
		if(ui8Usage != pServerWeapon->GetWeaponUsage() )
		{
			if ( ui32Class !=WEAPON_CLASS_GRENADESHELL
				&& ui32Class != WEAPON_CLASS_BOMBTRIGGER
				&& ui32Class != WEAPON_CLASS_THROWING_ATTACK
				&& ui32Class != WEAPON_CLASS_THROWING_HEAL
				&& FALSE == ::Table::IsThrowWeapon( pWeapon ) )
			{
				ui32ServerWeaponID = pServerWeapon->GetWeaponID();
				goto hack_user;
			}
		}
	}
	return HACK_TYPE_NO;

hack_user:

	HACK_LOG hl;
	hl.AddInfo("HACK_TYPE_WEAPON Chara1\t ClientWaeponID:%d ServerWeaponID:%d \n", pWeapon->GetWeaponID(), ui32ServerWeaponID);
	char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_WEAPON );
	if( msg ) DSHACK( msg );

	return HACK_TYPE_NO;

	// 로그 분석후에 주석 해제 예정. 2016.03.08 어깨깡패
	//pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_WEAPON );
	//return HACK_TYPE_WEAPON;

}

DS_HACK_TYPE HMSParser::_CheckWeaponSync( CDediRoom* pRoom, UINT32 ui32SlotIdx, const GAMEINFO_CHARA_WEAPON* pWeapon )
{
	I3ASSERT( pRoom != NULL );
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
	I3ASSERT( pWeapon != NULL );

	UINT8 ui8Usage		= pWeapon->getWeaponUsage();
	UINT32 ui32Class	= pWeapon->getWeaponClass();
	UINT32 ui32Number	= pWeapon->getWeaponNumber();

	UINT32 ui32ServerWeaponID = 0;

	// RPG 인지 체크
	if( FALSE == _CheckRPG( pRoom, pWeapon->getWeaponID() ) )
	{
		goto hack_user;
	}

	N_PCINFO_WEAPON *pServerWeapon = pRoom->getChara_Weapon(ui32SlotIdx);
	if (pServerWeapon)
	{
		if(ui8Usage != pServerWeapon->GetWeaponUsage() )
		{
			if ( ui32Class !=WEAPON_CLASS_GRENADESHELL
				&& ui32Class != WEAPON_CLASS_BOMBTRIGGER
				&& ui32Class != WEAPON_CLASS_THROWING_ATTACK
				&& ui32Class != WEAPON_CLASS_THROWING_HEAL
				&& FALSE == Table::IsThrowWeapon( pWeapon ) )
			{
				ui32ServerWeaponID = pServerWeapon->GetWeaponID();
				goto hack_user;
			}
		}
	}
	return HACK_TYPE_NO;

hack_user:

	HACK_LOG hl;
	hl.AddInfo("HACK_TYPE_WEAPON Chara2\t ClientWaeponID:%d ServerWeaponID:%d \n", pWeapon->getWeaponID(), ui32ServerWeaponID );
	char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_WEAPON );
	if( msg ) DSHACK( msg );

	return HACK_TYPE_NO;

	// 로그 분석후에 주석 해제 예정. 2016.03.08 어깨깡패
	//pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_WEAPON );
	//return HACK_TYPE_WEAPON;

}

DS_HACK_TYPE HMSParser::_CheckWeaponParam( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pWeaponParam )
{
	I3ASSERT( pWeaponParam != NULL );
	I3ASSERT( pWeaponParam->iClass < MAX_WEAPON_PARAM );
	I3ASSERT( pWeaponParam->iCount <= MAX_WEAPON_PARAM_ARGS );

	N_PCINFO_HIT_WEAPON* pWeapon = &pWeaponParam->_tWeapon;
	
	// -------------------------------------------------------------------
	// find weapon info
	// -------------------------------------------------------------------

	WeaponInfo* pWeaponInfo = Table::GetWeaponInfo( pWeapon->GetWeaponID() );
	if( pWeaponInfo == NULL || pWeaponInfo->IsValid() == false ) goto hack_log;

	BOOL bDoCompare = FALSE;
	
	// 참고 UDP_UTIL::BuildCharaInfo_WeaponCheckData
	REAL32 ServerArgs[ MAX_WEAPON_PARAM_ARGS ];
	
	switch( pWeaponParam->iClass )
	{
	case WEAPON_PARAM_RECOIL:
		
		ServerArgs[0] = pWeaponInfo->m_r32RecoilHorzAngle;
		ServerArgs[1] = pWeaponInfo->m_r32RecoilHorzMax;
		ServerArgs[2] = pWeaponInfo->m_r32RecoilVertAngle;
		ServerArgs[3] = pWeaponInfo->m_r32RecoilVertMax ;
		ServerArgs[4] = pWeaponInfo->m_r32Deviation;

		bDoCompare = TRUE;
		
		break;
	case WEAPON_PARAM_BULLET_COUNT:
		
		ServerArgs[0] = (REAL32) pWeaponInfo->m_ui8FireBulletCount;

		bDoCompare = TRUE;

		break;

	case WEAPON_PARAM_DAMAGE:

		ServerArgs[0] = pWeaponInfo->m_r32Damage[ WEAPON::ATTACK_TYPE_L1 ];

		bDoCompare = TRUE;

		break;
	} // switch

	if( bDoCompare && CompareWeaponData( (WeaponParamClass)pWeaponParam->iClass, pWeaponParam->Args, ServerArgs, pWeaponParam->iCount ) )
	{
		return HACK_TYPE_NO;	
	}

hack_log:
	
	HACK_LOG hl;
	hl.AddInfo( "weapon:%d:%d, param_class=%d", pWeaponParam->_tWeapon.GetWeaponClass(), pWeaponParam->_tWeapon.GetWeaponNumber(), pWeaponParam->iClass );
	char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_WEAPON_PARAM );
	if( msg) DSERROR(msg);

	return HACK_TYPE_WEAPON_PARAM;
}

DS_HACK_TYPE HMSParser::_CheckNaturalRPGGet(  CDediRoom* pRoom, UINT32 ui32SlotIdx, UINT32 ui32Class, UINT32 ui32Number )
{
	if( ! g_pConfig->GetCheckLevel(HACK_TYPE_RPG_CHEAT) ) return HACK_TYPE_NO;
	if (pRoom->IsSpaceMode()) return HACK_TYPE_NO;

	if (ui32Class == WEAPON_CLASS_GRENADESHELL )
	{
		if (ui32Number == GRENADESHELL_RPG7 ||
			ui32Number == GRENADESHELL_RPG7_FOR_DINO ||
			ui32Number == GRENADESHELL_RPG7_FOR_MODE )
			;
		else
			return HACK_TYPE_NO;
	}

	if ( ui32Class != WEAPON_CLASS_ROCKET_LAUNCHER )
	{
		return HACK_TYPE_NO;
	}

	if( GET_ROOM_INFO_WEAPON_RPG7( pRoom->GetAvailableWeapon()) )
	{
		return HACK_TYPE_NO;
	}

	if ( !pRoom->GetGameCharacter(ui32SlotIdx)->GetNaturalRPGGet() )
	{
		//hack log
		char  szExt[MAX_PATH];
		sprintf_s(szExt, sizeof(szExt),"HACK_TYPE_RPG_CHEAT\t Weapon:%d:%d\n", 
			ui32Class,
			ui32Number);

		HACK_LOG hl;
		hl.AddInfo( "HACK_TYPE_RPG_CHEAT\t Weapon:%d:%d", ui32Class, ui32Number );
		char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_RPG_CHEAT );
		if( msg) DSHACK(msg);

		pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_RPG_CHEAT );
		return HACK_TYPE_RPG_CHEAT;
	}

	return HACK_TYPE_NO;
}

BOOL HMSParser::_CheckRPG( CDediRoom* pRoom, T_ItemID TItemID )
{ 
	// RPG 인지 체크
	if( GET_ITEM_SUBTYPE( TItemID )	 != WEAPON_CLASS_ROCKET_LAUNCHER )		return TRUE;

	// RPG 를 따로 체크할게 아니고 첨부터 RPG 를 들고 시작해야 할듯 수정요망
	if( GET_ROOM_INFO_WEAPON_RPG7( pRoom->GetAvailableWeapon() ) )
	{
		DSHACK("GET_ROOM_INFO_WEAPON_RPG7\n");

		return TRUE;
	}

	// 방에서 사용할수 있는무기인지 체크가 아닌 이 무기를 유저가 가지고있는지로 체크해야할듯? 수정요망
	if( pRoom->IsAvailableWeapon( TItemID ) )	
	{
		return TRUE;
	}
	return FALSE;
}

void HMSParser::_CheckGhost(  CDediRoom* pRoom,CCharacter * pChara, REAL32 r32PacketTime, UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pPosRot)
{
	if (pChara->GetPosRecvTime() < I3_EPS)
		return;

	if (r32PacketTime - pChara->GetPosRecvTime() > 6.f)
	{
		REAL32 rLenSqrt = static_cast<REAL32>(distance_squared( pChara->GetPosRot()->GetPos(),pPosRot->GetPos()));
		INT8 i8LogLevel = g_pConfig->GetCheckLevel(HACK_TYPE_GHOST);

		I3TRACE("_CheckGhost : %2f  \n",rLenSqrt );

		if (rLenSqrt > 25.f)
		{
			{//hack log	
				HACK_LOG hl;
				hl.AddInfo( "HACK_TYPE_GHOST \t CurrnetTime:%2f\t PrevTime:%2f\n", r32PacketTime, pChara->GetPosRecvTime() );
				char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_GHOST );
				if( msg) DSHACK(msg);
			}

			if(i8LogLevel>0)
				pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_GHOST ); //  kick.
		}
	}
}

DS_HACK_TYPE HMSParser::_CheckUseWeapon( CDediRoom* pRoom, UINT32 ui32SlotIdx, T_ItemID TItemID )
{
	// 공룡 무기 잠시 예외 처리
	if ( GET_ITEM_SUBTYPE(TItemID) == WEAPON_CLASS_DINO )	return HACK_TYPE_NO;
	
	if( FALSE == pRoom->CheckUseWeapon( ui32SlotIdx, TItemID ) )
	{		
		UINT32 ui32Usage = Weapon::GetUsageFromItemID(TItemID);
		UINT32 ui32ServerWeaponID = pRoom->GetServerWeaponID(ui32SlotIdx, ui32Usage);
		DSERROR("HMSParser::_CheckUseWeapon [%d] NOT EQUIP WEAPON! ITEMID : %d, MY_WEAPON : %d\n", ui32SlotIdx, TItemID, ui32ServerWeaponID );

		//hack log
		HACK_LOG hl;
		hl.AddInfo("HACK_TYPE_WEAPON Explosion\t WeaponID:%d ServerWaeponID:%d", TItemID, ui32ServerWeaponID);
		char* msg = hl.MakeLogString( pRoom, ui32SlotIdx, HACK_TYPE_WEAPON);
		if( msg) DSHACK(msg);

		// 메디컬 킷 or 유탄 무기는 통과
		if (GET_ITEM_TYPE(TItemID) != ITEM_TYPE_THROWING1)	return HACK_TYPE_NO;

		pRoom->EnqueueKickUser( ui32SlotIdx, HACK_TYPE_WEAPON);
		return HACK_TYPE_WEAPON;
	}

	return HACK_TYPE_NO;
}
