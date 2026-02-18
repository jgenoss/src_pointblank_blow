#include "pch.h"
#include "GameEvent.h"

#include "GlobalVariables.h"
#include "./Network/GameNetworkManager.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaMoveContext.h"
#include "GameUDPHandler.h"
#include "StageBattle.h"
#include "Weapon/WeaponItemID.h"
#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"

inline
CGameUDPHandler *	GetUDPHandler( void)
{
	return CGameUDPHandler::i(); 
}

void GAMEEVENT::Write_Chara_Fire( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon)
{
	GetUDPHandler()->WriteGameEvent_Fire( pChara, pTargetPos, pWeapon);
}

#ifdef DOMI_STING_POISON_GRENADE
void GAMEEVENT::Write_Chara_Fire_DomiDinoGrenade( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon)
{
	GetUDPHandler()->WriteGameEvent_Fire_DomiDinoGrenade( pChara, pTargetPos, pWeapon);
}
#endif

void GAMEEVENT::Write_Chara_Hit( CGameCharaBase * pChara, const CHARA_DAMAGEINFO * pDamageInfo, bool bByObj)
{
	GetUDPHandler()->WriteGameEvent_Hit( pChara, pDamageInfo, bByObj);
}

void GAMEEVENT::Write_Chara_DropWeapon( CGameCharaBase * pChara, WEAPON_SLOT_TYPE iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet)
{
	GetUDPHandler()->WriteGameEvent_DropWeapon( pChara, iWeaponSlot, iLoadedBullet, iDualLoadedBullet, iTotalBullet);
}

void GAMEEVENT::Write_Chara_PickupWeapon( CGameCharaBase * pChara, WeaponBase * pWeapon)
{
	GetUDPHandler()->WriteGameEvent_PickupWeapon( pChara, pWeapon);
}

void GAMEEVENT::Write_Chara_DirectPickupWeapon(CGameCharaBase * pChara, T_ItemID GetItemID)
{
	GetUDPHandler()->WriteGameEvent_DirectPickupWeapon(pChara, GetItemID);
}

void GAMEEVENT::Write_Chara_Mission( CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea, T_ItemID ItemId,INT32 ObjIdx,REAL32 fTime =0.0f)
{
	GetUDPHandler()->WriteGameEvent_Mission( pChara, nState, nArea, ItemId,ObjIdx, fTime); 
}

void GAMEEVENT::Write_Chara_SuicideDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type,
		CHARA_HIT_PART part, UINT16 nDamage /*= 100*/, VEC3D * pDir /*= nullptr*/,
		WEAPON_CLASS_TYPE WeaponClass /*= WEAPON_CLASS_UNKNOWN*/, INT32 WeaponNum /*= -1*/,
		UINT16 iByObjectIdx /*= 0xFFFF*/, INT32 KillerIdx /*= 0*/, WEAPON_SLOT_TYPE WeaponUsage /*= WEAPON_SLOT_PRIMARY*/)
{
	GetUDPHandler()->WriteGameEvent_SuicideDamage( pChara, type, part, nDamage, pDir, WeaponClass, WeaponNum, iByObjectIdx, KillerIdx, WeaponUsage);
}

void GAMEEVENT::Write_Chara_RadioChat( CGameCharaBase * pChara, UINT8 nRadio)
{
	GetUDPHandler()->WriteGameEvent_RadioChat( pChara, nRadio);
}

void GAMEEVENT::Write_Chara_UseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey)
{
	GetUDPHandler()->WriteGameEvent_UseObject( pChara, pObj, nActionKey);
}

void GAMEEVENT::Write_Chara_SyncUseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey)
{
	GetUDPHandler()->WriteGameEvent_UseObjectSync( pChara, pObj, nSyncActionKey);
}

static void s_CheckAndChangeDeathInfoWeapon( GAMETOCONTEXT_INFO_CHARA_DEATH * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	// KEYWORD : WEAPON_CIC
	// 더미 무기는 일반 무기로 취급하도록 해야 합니다.
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pInfo->_ui32WeaponID);
	
	if( pWeaponInfo != nullptr && pWeaponInfo->GetOriginalWeaponItemID() > 0)
	{
		pInfo->_ui32WeaponID = pWeaponInfo->GetOriginalWeaponItemID();
	}
	
	// PEF로 변경
}

void GAMEEVENT::Write_Chara_Death( GAMETOCONTEXT_INFO_CHARA_DEATH * pDeathInfo)
{
	UINT32 nTemp = 0;

	I3ASSERT( BattleServerContext::i()->IsP2PHost() == true);

	if( !BattleServerContext::i()->IsP2PActivate() )
	{
		NETTRACE( "Context::WriteCharaDeath() - Invalid function call for CN.\n" );
		return;
	}

	I3ASSERT( pDeathInfo->_iKillidx < SLOT_MAX_COUNT);
	if( pDeathInfo->_iKillidx == -1 )	// 유저가 나간경우입니다. 네트워크 처리 하지 않습니다.
	{
		I3PRINTLOG(I3LOG_NOTICE,"pDeathInfo->_iKillidx == -1");
		return;
	}

	I3_BOUNDCHK( pDeathInfo->_iDeathidx, SLOT_MAX_COUNT );
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pDeathInfo->_iDeathidx );
	if( pChara == nullptr )
		return;

	//2번 죽는것을 막는다.
	if( UserContext::i()->Death[pDeathInfo->_iDeathidx])
		return;

	UserContext::i()->Death[pDeathInfo->_iDeathidx] = true;
	ISNETWORKRETURN(false)

	if( BattleSlotContext::i()->getSlotInfo( pDeathInfo->_iDeathidx )->_State != SLOT_STATE_BATTLE )
	{
		// AI모드의 AI의 경우 전투상태가 아니라도 패킷처리 한다.
		if( (false ==  (  MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) ||MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )) 
			|| !pChara->isAI())
		{
			I3PRINTLOG(I3LOG_WARN,  "CGameContext::WriteCharaDeath() - 해당 캐릭터는 전투상태가 아닙니다." );
			return;
		}
	}
	
	{
		//0x00		0		0
		//	message	State	Death
		nTemp = 0;
		nTemp = SET_DEATHINFO_IDX_TYPE( nTemp, pDeathInfo->_iDeathidx );
		nTemp = SET_DEATHINFO_DEATH_TYPE( nTemp, pDeathInfo->_nDeathType );

		// Defence Object를 공격중인지 확인	
	
		if( (pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx) &&
			CGameMissionManager::i()->isAttackingMissionObj( pDeathInfo->_iDeathidx) )
		{
			nTemp = SET_DEATHINFO_ENEMY_TYPE( nTemp, CHARA_DEATH_ENEMY_STATE_DEFENCE );
		}
		else if ( (pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx) && pDeathInfo->IsSameClass( WEAPON_CLASS_DINO ) 
			&&g_pCharaManager->IsAssist_Kill(pDeathInfo->_iKillidx,pDeathInfo->_iDeathidx))
		{	
			nTemp = SET_DEATHINFO_MY_TYPE( nTemp, CHARA_DEATH_MY_STATE_ASSIST );
		}
		else if (pDeathInfo->IsSameClass( WEAPON_CLASS_GRENADESHELL ) &&
			pDeathInfo->IsSameNumber( WEAPON::getItemIndex(WEAPON::GSHELL_POISON) ) &&
			g_pCharaManager->IsAssist_Kill(pDeathInfo->_iKillidx,pDeathInfo->_iDeathidx))
		{
			nTemp = SET_DEATHINFO_MY_TYPE( nTemp, CHARA_DEATH_MY_STATE_ASSIST );
		}

		if ( (pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx) &&
			g_pCharaManager->IsEscapeNearKill(pDeathInfo->_iDeathidx))
		{
			nTemp = SET_DEATHINFO_ENEMY_TYPE( nTemp, CHARA_DEATH_ENEMY_STATE_ESCAPE );
		}

		CWeaponInfo * pwinfo = nullptr;
		UINT8 ui8Weapon = 0;

		if( pChara->IsDino())
		{
			pwinfo = ((CGameCharaDino*)pChara)->GetDinoWeapon()->getWeaponInfo();
		}
		else
		{
			I3ASSERT( pChara->getWeaponContext()->getStaticCurrentWeapon() != nullptr);
			pwinfo = pChara->getWeaponContext()->getStaticCurrentWeapon()->getWeaponInfo();
		}
		
		if( pwinfo != nullptr)
		{
			ui8Weapon = (UINT8)pwinfo->GetTypeClass();
		}

		// 무기 정보를 변경해서 보내는 경우 확인..
		s_CheckAndChangeDeathInfoWeapon( pDeathInfo);

		// TCP로 MS에 알려줍니다.
		// 좌표 계산
		// Object 탑승시 월드 좌표로 변환해줄 필요가 있다. #8551
		VEC3D vDeathPos;
		I3ASSERT( pChara->getMoveContext() != nullptr);
		if( pChara->getMoveContext()->getRideObject() != nullptr)
		{
			i3Object * pObject = pChara->getMoveContext()->getRideObject();
			i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObject->getSceneObject() );
			if( pTrans != nullptr)
			{
				i3Vector::TransformCoord( &vDeathPos, pChara->GetPos(), pTrans->GetCacheMatrix() );
			}
			else
			{
				i3Vector::Copy( &vDeathPos, pChara->GetPos());
			}
		}
		else
		{	// 일반 지형
			i3Vector::Copy( &vDeathPos, pChara->GetPos());
		}

		UserContext::i()->QueDeathChara( pDeathInfo->_iKillidx, ui8Weapon, nTemp, pDeathInfo->_nDeathType, pDeathInfo->_ui32WeaponID, &vDeathPos, pDeathInfo->_iByObjectIdx );
	}

	// UDP용
	{
		GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( pDeathInfo->_iDeathidx );

		pCharaInfo->_tNetData._tHP.setHP( 0);
		pCharaInfo->_tNetData._tSufferData.setShooterIndex( (UINT8) pDeathInfo->_iKillidx);
		pCharaInfo->_tNetData._tSufferData.setDeathType( (UINT8) pDeathInfo->_nDeathType);
#if defined( _SHUFFLE_HIT_PART)
		pCharaInfo->_tNetData._tSufferData.setHitPart( HitPartContext::i()->EncryptHitPart( pDeathInfo->_nHitPart));
#else
		pCharaInfo->_tNetData._tSufferData.setHitPart( (UINT8) pDeathInfo->_nHitPart);
#endif

		pCharaInfo->_tNetData._tSufferData.setHitDirection( pDeathInfo->_pHitDir);

		pCharaInfo->_tNetData._iInfoFlag |= (P2P_INFO_FLAG_CS_DEATH | P2P_INFO_FLAG_CS_LIFE);
		pCharaInfo->_bSendFlag			= true;
	}
}

void GAMEEVENT::Write_Chara_Suffering( CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart, bool bFlag)
{
	GetUDPHandler()->WriteGameEvent_SufferingDamage( pChara, type, nShooter, nHitPart, bFlag);
}

void GAMEEVENT::Write_ThrowingWeapon( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo)
{
	GetUDPHandler()->WriteGameEvent_ThrowWeaponData( idx, pThrowWeaponInfo);
}

#ifdef DOMI_STING_POISON_GRENADE
void GAMEEVENT::Write_DomiDinoThrowWeapon( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo)
{
	GetUDPHandler()->WriteGameEvent_DomiDinoThrowWeaponData( idx, pThrowWeaponInfo);
}
#endif

//#ifdef DOMI_UDP_OBJECT
void GAMEEVENT::Write_DomiSkillObjectInstall(INT32 customerIdx, INT8 objectIdx)
{
	GetUDPHandler()->WriteGameEvent_DomiSkillObject_Install(customerIdx, objectIdx);
}
void GAMEEVENT::Write_DomiSkillObjectRepair(INT32 customerIdx, INT8 objectIdx)
{
	GetUDPHandler()->WriteGameEvent_DomiSkillObject_Repair(customerIdx, objectIdx);
}
void GAMEEVENT::Write_DomiSkillObjectUpgrade(INT32 customerIdx, INT8 objectIdx, INT8 lv)
{
	GetUDPHandler()->WriteGameEvent_DomiSkillObject_Upgrade(customerIdx, objectIdx, lv);
}
void GAMEEVENT::Write_DomiSkillObjectSupplyUse(INT32 customerIdx, INT8 objectIdx)
{
	GetUDPHandler()->WriteGameEvent_DomiSkillObject_SupplyUse(customerIdx, objectIdx);
}
void GAMEEVENT::Write_DomiDropItemCatched(INT32 userIdx, INT8 objectIdx)
{
	GetUDPHandler()->WriteGameEvent_DomiDropItem_Catched(userIdx, objectIdx);
}
//#endif

void GAMEEVENT::Write_Object( INT32 idx, INT32 EventSender, UINT16 nHP, VEC3D * pPos /*= nullptr*/, QUATERNION * pRot /*= nullptr*/ )
{
	GetUDPHandler()->WriteGameEvent_ObjectData( idx, EventSender, nHP, pPos, pRot);
}

void GAMEEVENT::Write_DroppedWeapon( INT32 idx, VEC3D * pPos, MATRIX * pMtxRot)
{
	GetUDPHandler()->WriteGameEvent_DropedWeaponData( idx, pPos, pMtxRot);
}

void GAMEEVENT::Write_Object_Animate( INT32 idx, INT32 EventSender, UINT16 nHP, REAL32 rKeyFrame /*= 0.0f*/)
{
	GetUDPHandler()->WriteGameEvent_AnimObjectData( idx, EventSender, nHP, rKeyFrame);
}

void GAMEEVENT::Write_Object_AnimateForOutpost( INT32 idx, INT32 EventSender, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, bool bSend)
{
	GetUDPHandler()->WriteGameEvent_AnimObjectDataForOutpost( idx, EventSender, nPathKey, nHP, rKeyFrame, bSend);
}

void GAMEEVENT::BindNetGrenadeInfo( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos)
{
	GetUDPHandler()->BindNetGrenadeInfo( idx, pWeapon, pInitPos);
}

void GAMEEVENT::BindNetC5Info( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos)
{
	GetUDPHandler()->BindNetC5Info( idx, pWeapon, pInitPos);
}

void GAMEEVENT::Write_Chara_TouchDown( CGameCharaBase * pChara, UINT16 ui16ObjectIdx )
{
	GetUDPHandler()->WriteGameEvent_TouchDown( pChara, ui16ObjectIdx );
}

INT64 GAMEEVENT::GetSecretKey( UINT64 ui64MacAddress )
{
	UINT32 ui32Key1 = static_cast<UINT32>((ui64MacAddress >> 32)&0xFFFFFFFF);
	UINT32 ui32Key2 = static_cast<UINT32>(ui64MacAddress&0xFFFFFFFF);

	UINT8	ui8Encrypt[8];
	ui8Encrypt[0] = (UINT8)((((ui32Key1>>24)	& 0xF0))	| ((ui32Key2>>24)	& 0xF0)>>4);
	ui8Encrypt[1] = (UINT8)((((ui32Key1>>8)		& 0x0F)<<4)	| ((ui32Key2>>16)	& 0x0F));
	ui8Encrypt[2] = (UINT8)((((ui32Key1>>0)		& 0xF0))	| ((ui32Key2>>8)	& 0x0F));
	ui8Encrypt[3] = (UINT8)((((ui32Key1>>16)	& 0x0F)<<4)	| ((ui32Key2>>0)	& 0x0F));
	ui8Encrypt[4] = (UINT8)((((ui32Key1>>8)		& 0xF0))	| ((ui32Key2>>24)	& 0x0F));
	ui8Encrypt[5] = (UINT8)((((ui32Key1>>24)	& 0x0F)<<4)	| ((ui32Key2>>16)	& 0xF0)>>4);
	ui8Encrypt[6] = (UINT8)((((ui32Key1>>0)		& 0x0F)<<4)	| ((ui32Key2>>0)	& 0xF0)>>4);
	ui8Encrypt[7] = (UINT8)((((ui32Key1>>16)	& 0xF0))	| ((ui32Key2>>8)	& 0xF0)>>4);

	BitRotateEncript( ui8Encrypt, sizeof( UINT64 ), 3 );

	INT64	i64Key;
	memcpy( &i64Key, ui8Encrypt, sizeof(UINT64) );
	return i64Key;
}