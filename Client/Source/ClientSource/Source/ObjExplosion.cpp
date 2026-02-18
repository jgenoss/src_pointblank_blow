#include "pch.h"
#include "ObjExplosion.h"

#include "StageBattle.h"
#include "StageObject/GameControledObject.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "GameCharaBoneContext.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "Weapon/WeaponItemID.h"

#include "i3Base/string/ext/utf16_to_mb.h"


obj_explosion::obj_explosion() : m_pObjectHitCheck(nullptr), m_pObjectAccDamage(nullptr), m_pObjectExploded(nullptr)
{
	
	
}

obj_explosion::~obj_explosion()
{
		
	
}

void	obj_explosion::ProcessCreateStage()
{
	I3MEM_SAFE_FREE( m_pObjectAccDamage);
	I3MEM_SAFE_FREE( m_pObjectExploded);
	I3MEM_SAFE_FREE_POINTER( m_pObjectHitCheck );
}

void	obj_explosion::Process_InitObject(INT32 nStageObjCount)
{

	I3MEM_SAFE_FREE_POINTER( m_pObjectHitCheck );
	m_pObjectHitCheck = (UINT8*) i3MemAlloc( sizeof(UINT8) * nStageObjCount );

	// TODO : Dedicated
	{
		m_pObjectAccDamage = (REAL32*) i3MemAlloc( sizeof(REAL32) * nStageObjCount);
		I3ASSERT( m_pObjectAccDamage != nullptr);
		i3mem::FillZero( m_pObjectAccDamage, sizeof(REAL32) * nStageObjCount );
		m_pObjectExploded = (UINT8*) i3MemAlloc( sizeof(UINT8) * nStageObjCount);
		I3ASSERT( m_pObjectExploded != nullptr);
		i3mem::FillZero( m_pObjectExploded, sizeof(UINT8) * nStageObjCount );
	}

}

// 오브젝트 폭발 여부 계산
void	obj_explosion::Process_ResetObjectExploded(CStageBattle * sb)
{
	I3ASSERT(sb != nullptr);

	if( sb->GetStageObjectList() != nullptr)
	{
		size_t nObjCount = sb->GetStageObjectList()->size();

		if(nObjCount > 0)
		{
			I3ASSERT(m_pObjectExploded != nullptr);
			i3mem::FillZero(m_pObjectExploded, sizeof(UINT8) * nObjCount);
		}
	}
}

void	obj_explosion::ProcessDestroyStage()
{
	I3MEM_SAFE_FREE( m_pObjectAccDamage);
	I3MEM_SAFE_FREE( m_pObjectExploded);

	I3MEM_SAFE_FREE_POINTER( m_pObjectHitCheck );
}

void	obj_explosion::ProcessUpdate(CStageBattle* sb)
{
	// TODO : Dedicated
	// Dedicated Mode에서는 오브젝트에 주는 Damage를 한 프레임당 누적시킨다.
	if( BattleServerContext::i()->IsUseDedicationMode() && 
		( m_pObjectAccDamage != nullptr)  &&
		( sb->GetStageObjectList() != nullptr) )
	{
		i3mem::FillZero( m_pObjectAccDamage, sizeof(REAL32) * sb->GetStageObjectList()->size());
	}
}



void	obj_explosion::call( i3GameObj * pObject, VEC3D * pPos, REAL32 rRange, REAL32 rForce, REAL32 rDamage, REAL32 rSlop )
{

	INT32	nObjHitCount;
	UINT32	j, nResultCount = 0;
	i3CollideeLine	ColLine;
	VEC3D	vDir;
	i3Object * pGameObj;
	I3_PHYSIX_HIT_RESULT * pNextResult;
	//I3_PHYSIX_HIT_RESULT * pRoughResult;
	CHARA_DAMAGEINFO DamageInfo;

	REAL32	rLength = 100000.f;

	CStageBattle* sb =	g_pFramework->GetStageBattle();
	I3ASSERT( i3::same_of<CStageBattle*>(sb));

	StageObjectListPtr	pObjectList = sb->GetStageObjectList();
	if( pObjectList == nullptr )
	{
		I3TRACE( "Stage : _CheckObjCollision() - InstanceList nullptr.\n" );
		return;
	}

	CGameCharaBase * pShooter = nullptr;
	WeaponBase * pShooterWeapon = nullptr;

	i3GameObj * pEventSender = pObject;

	if( pEventSender != nullptr )
	{
		while( i3::kind_of<i3Object*>(pEventSender))
		{
			pEventSender = pEventSender->getEventSender();
			if( pEventSender == nullptr )
				break;
		}

		if( i3::kind_of<WeaponBase*>(pEventSender))
		{
			pShooterWeapon = (WeaponBase*) pEventSender;
			pShooter = pShooterWeapon->getOwner();
		}
	}

	nObjHitCount = 0;

	VEC3D	vPos;

	ColLine.SetStart( pPos );

	for(size_t i = 0; i < pObjectList->size(); i++ )
		m_pObjectHitCheck[ i ] = 0;

	// Object Hit Collision
	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( BattleServerContext::i()->IsUseDedicationMode())
	{
		if( pShooter != nullptr)
		{
			if( pShooter->isLocal())
				bHost = true;
		}
	}

	if( bHost )
	{
		for(size_t i = 0; i < pObjectList->size(); i++ )
		{
			pGameObj = (*pObjectList)[i];

			if( pGameObj == pObject)
				continue;

			if( pGameObj->getCurState() == 0 )
				continue;

			i3Vector::Copy( &vPos, pGameObj->GetPos() );
			i3Vector::Add( &vPos, &vPos, pGameObj->getShapeCenterPos() );
			i3Vector::Sub( &vDir, &vPos, pPos );

			REAL32 rlen = i3Vector::LengthSq( &vDir );

			// 범위를 넘는 경우와 자신의 거리는 0이므로 스킵..(자신을 스킵)
			if( rlen > rRange * rRange || rlen == 0.f )
				continue;

			// 레이케스팅에서 이미 체크되었다면 스킵
			if( m_pObjectHitCheck[ pGameObj->getNetIndex() ] == 1 )
				continue;

			rLength = rRange;

			ColLine.SetEnd( &vPos );

			// 월드 체크
			nResultCount = g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_ATTACK );

			if( nResultCount > 0 )
			{
				pNextResult = g_pFramework->getRaycastResult();
				for( j = 0; j < nResultCount; j++ )
				{
					if( pNextResult[ j ].m_pDestShapeSet == nullptr )
					{
						I3PRINTLOG(I3LOG_WARN,  GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_WARNING_WRONG_OBJECT") );/*이상한 오브젝트입니다. m_pDestShapeSet = nullptr..*/
						continue;
					}

					UINT16 shapeGroup = pNextResult[ j ].m_pDestShapeSet->getShapeGroup();

					if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
					{
						i3Object * pObj = (i3Object*) pNextResult[ j ].m_pDestShapeSet->getEventReceiver();
						// Damage , Physix set~~
						REAL32 rangeForce = SetObjForce( &pNextResult[j], pPos, rRange, rForce, rSlop );

						if( BattleServerContext::i()->IsP2PHost() ||
							(pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE) )
						{
							pObj->Damage( (REAL32) rDamage * rangeForce, pEventSender, pNextResult[ j].m_pDestShapeSet);
							//I3TRACE( "Obj Damage By %s\n", pEventSender->static_meta()->class_name());
						}

						if( BattleServerContext::i()->IsUseDedicationMode())
						{
							if( bHost)
							{
								bool bDamageTo = true;

								if( pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_ANIM)
								{
									if( !i3::kind_of<CGameControledObject*>(pObject)
										|| ((CGameControledObject*)pObject)->isManualUpdate() == false)
									{
										bDamageTo = false;
									}
								}

								if( bDamageTo)
								{
									DamageInfo._HitType = GHT_OBJECT;
									DamageInfo._nVictimIdx = pObj->getNetIndex();
									DamageInfo._ObjNetType = pObj->getNetworkSyncMode();
									DamageInfo._rDamage = (rDamage * rangeForce);
									GAMEEVENT::Write_Chara_Hit( pShooter, &DamageInfo);
								}
							}
						}

						if( pNextResult[ j ].m_pTerrain == nullptr )
						{
							if( rLength > pNextResult[ j ].m_T )
								rLength = pNextResult[ j ].m_T;
						}
						else
						{
							UINT16 nTerrainType = pNextResult[ j ].m_pTerrain->m_Terrain;

							if( nTerrainType < I3_TERRAIN_CONCRETE_THIN )
							{	// 관통이 안됩니다.
								if( rLength > pNextResult[ j ].m_T )
									rLength = pNextResult[ j ].m_T;
							}
						}

						m_pObjectHitCheck[ pObj->getNetIndex() ] = 1;	// 해당 오브젝트는 컬리젼 되었음을 체크
					}
				}
			}
		}
	}

	// Chara Hit Collision
	UINT32	nColGroup = CGRP_MASKGROUP_ATTACK;
	UINT32	nRoughHitGroup;
	UINT8	nCharaHitCheck[ SLOT_MAX_COUNT ][HIT_CHECK_COUNT] = {0,};	// 머리, 몸통을 따로 체크

	DamageInfo._DeathType		= CHARA_DEATH_O;

	if( pShooterWeapon != nullptr )
	{
		DamageInfo.SetWeaponItemID(pShooterWeapon);
	}
	else
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_THROWING_GRENADE, WEAPON::getItemIndex( WEAPON::THROWING_K400), true );
		DamageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);
	}

	DamageInfo._DamageType		= WEAPON::DAMAGE_HP_LOSS;
	DamageInfo._rDamage			= rDamage;
	DamageInfo._rExplosionRange	= rRange;
	DamageInfo._rExplosionSlope	= rSlop;
	DamageInfo._HitType			= GHT_CHARA;
	DamageInfo._ByObjectIndex	= (UINT16) ((i3Object*)pObject)->getNetIndex();

	for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;

		INT32 idx = pChara->getCharaNetIndex();
		if( idx == -1)	continue;

		if( !BattleServerContext::i()->IsP2PHost() &&	(idx != BattleSlotContext::i()->getMySlotIdx()) )
			continue;

		VEC3D vLength;
		i3Vector::Sub( &vLength, pPos, pChara->GetPos() );

		// 이 부분 수정해야 됩니다.
		// rLength		= i3Vector::Length( &vLength);
		rLength		= i3Vector::LengthSq( &vLength);

		//if( rLength <= rRange )
		if( rLength * rLength > rRange * rRange)
			continue;

		rLength = rRange;

		if( BattleServerContext::i()->IsUseDedicationMode() == false)
		{
			if( pChara->getCharaTeam() & CHARACTER_TEAM_RED)
			{
				nColGroup	= CGRP_MASKGROUP_CHARA_RED;
				nRoughHitGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;
			}
			else
			{
				nColGroup	= CGRP_MASKGROUP_CHARA_BLUE;
				nRoughHitGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
			}

			// 머리에 대한 컬리젼
			ColLine.SetEnd( i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix() ) );
			CalcHitByObject(nCharaHitCheck, DamageInfo, ColLine, rLength, pChara, pShooter);

			rLength = rRange;

			// 몸통에 대해서 테스트
			ColLine.SetEnd( i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix() ) );
			CalcHitByObject(nCharaHitCheck, DamageInfo, ColLine, rLength, pChara, pShooter);
		}

		// 터지는 근처에 Owner가 위치해있는경우 카메라 흔들림 효과
		if( pChara->is1PV() && (g_pCamera != nullptr))
		{
			bool bCamTremble = false;
			REAL32 TrembAmpRate = 1.f;
			if( i3::same_of<CGameObjectRidingHelicopter*>(pObject)
				|| i3::same_of<CGameObjectRespawnHelicopter*>(pObject))
			{
				if( rLength < 30.f)
				{
					bCamTremble = true;
					TrembAmpRate = 1.f - (rLength * 0.03333f);
				}

				if( pChara->IsOnObject())
					bCamTremble = true;
			}
			else
			{
				if( rLength < rRange * 2.f)
				{
					bCamTremble = true;
					TrembAmpRate = (rLength / rRange * 2.f);
				}
			}

			if( bCamTremble)
				g_pCamera->SetTremble( 5.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, TrembAmpRate);
		}
	}

}

void	obj_explosion::call_for_dedi( CGameCharaBase * pShooter,  CHARA_DAMAGEINFO& DamageInfo, i3Object * pObject)
{

	INT32 i, k;
	UINT32 j;

	i3CollideeLine ColLine;
	VEC3D vObjPos;
	VEC3D vLength;
	
	UINT8	nCharaHitCheck[ SLOT_MAX_COUNT ][HIT_CHECK_COUNT] = {0,};	// 머리, 몸통을 따로 체크

	if( pShooter->isLocal() == false)
		return;

	DamageInfo._ByObjectIndex	= (UINT16) pObject->getNetIndex();

	for( i = pObject->getCurState(); i < pObject->getStateCount(); i++)
	{
		// 다음 State까지 검사
		// 폭발이 여러번일 수 있기때문에...
		I3_OBJECT_STATE * pState = pObject->getState( i);
		I3ASSERT( pState != nullptr);

		i3TimeEventGen * pTimeEvent = pState->m_pTimeEvent;
		I3ASSERT( pTimeEvent != nullptr);

		for( j = 0; j < pTimeEvent->getEventCount(); j++)
		{
			i3TimeEventInfo * pInfo = pTimeEvent->getEvent( j);
			I3ASSERT( pInfo != nullptr);

			if( pInfo->getAction() == nullptr )
				continue;

			if( !i3::same_of<i3ActionExplosion*>(pInfo->getAction()))
				continue;

			i3ActionExplosion * pAction = (i3ActionExplosion *) pInfo->getAction();

			// 폭발 위치
			if( pAction->getAttachEnable())
			{
				i3SceneObject * pSceneObj = pObject->getSceneObject();
				MATRIX * pMtx = pSceneObj->FindCacheMatrixByName( pAction->getAttachBoneName());

				if( pMtx != nullptr)
				{
					i3Vector::Copy( &vObjPos, i3Matrix::GetPos( pMtx));
				}
			}
			else
			{
				i3Vector::Copy( &vObjPos, pObject->GetPos() );
			}

			i3Vector::Add( &vObjPos, &vObjPos, pObject->getShapeCenterPos() );

			ColLine.SetStart( &vObjPos);

			for( k = 0; k < g_pCharaManager->getCharaCount(); k++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( k);
				if( pChara == nullptr )	continue;

				INT32 idx = pChara->getCharaNetIndex();
				if( idx == -1)	continue;

				i3Vector::Sub( &vLength, &vObjPos, pChara->GetPos() );

				REAL32 rLength		= i3Vector::LengthSq( &vLength);

				if( (rLength * rLength > pAction->getRange() * pAction->getRange()) && !pChara->IsOnObject())	// revision 60420
					continue;
				
				// revision 62269
				REAL32 range = pAction->getRange();

				// 머리에 대한 컬리젼
				ColLine.SetEnd( i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix() ) );
				CollisionCalcHitByObject(nCharaHitCheck, DamageInfo, CHARA_HIT_HEAD, ColLine, range, pChara, pShooter);

				// 몸통에 대해서 테스트
				ColLine.SetEnd( i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix() ) );
				CollisionCalcHitByObject(nCharaHitCheck, DamageInfo, CHARA_HIT_BODY1, ColLine, range, pChara, pShooter);
			}
		}
	}

}

// revision 62135 /*주의*/ 오브젝트 폭파 컬리전 체크 수정.

void obj_explosion::CollisionCalcHitByObject( UINT8 nCharaHitCheck[][HIT_CHECK_COUNT],
											CHARA_DAMAGEINFO& DamageInfo, CHARA_HIT_PART hitPart, i3CollideeLine& ColLine, REAL32& rLength,
											CGameCharaBase* pChara, CGameCharaBase* pShooter)
{
	I3_PHYSIX_HIT_RESULT * pNextResult, * pRoughResult;
	INT32 nRoughResultCount = 0;
	INT32 j = 0;
	INT32 k = 0;
	INT32 idx = pChara->getCharaNetIndex();
	INT32 nColGroup = 0;
	INT32 nRoughHitGroup = 0;
	INT32 nResultCount = 0;

	if( pChara->getCharaTeam() & CHARACTER_TEAM_RED)
	{
		nColGroup	= CGRP_MASKGROUP_CHARA_RED;
		nRoughHitGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;
	}
	else
	{
		nColGroup	= CGRP_MASKGROUP_CHARA_BLUE;
		nRoughHitGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
	}

	HIT_CHECK hitCheck = HIT_CHECK_COUNT;

	if( hitPart == CHARA_HIT_HEAD ) {
		hitCheck = HIT_CHECK_HEAD;
	}
	else if(hitPart == CHARA_HIT_BODY1) {
		hitCheck = HIT_CHECK_BODY;
	}
	else {
		I3ASSERT_0;
	}

	if( nCharaHitCheck[ idx ][hitCheck] == 0 )
	{
		//World에 대해서 한번 테스트
		pNextResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ONLYWORLDHIT );
		if( pNextResult != nullptr )
		{
			if( (pNextResult->m_pDestShapeSet->getShapeGroup() == CGRP_WORLD_ATTACK) &&
				(pNextResult->m_T < rLength) )	// 월드에 대한 컬리젼 거리를 알아둔다
			{
				rLength = pNextResult->m_T;
			}
		}

		// RoughHit check
		nRoughResultCount = g_pFramework->raycast( &ColLine, nRoughHitGroup );
		if( nRoughResultCount > 0 )
		{
			pRoughResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nRoughResultCount );

			for( j = 0; j < nRoughResultCount; j++ )
			{
				i3EventReceiver * pRoughReceiver = pRoughResult[ j ].m_pDestShapeSet->getEventReceiver();
				if( !i3::kind_of<CGameCharaBase*>(pRoughReceiver))
					continue;

				CGameCharaBase * pRoughChara = (CGameCharaBase*) pRoughReceiver;
				if( pRoughChara->getCharaNetIndex() == -1 )	continue;
				if( rLength < pRoughResult[ j ].m_T ) continue;

				if( nCharaHitCheck[ pRoughChara->getCharaNetIndex() ][HIT_CHECK_HEAD] &&
					nCharaHitCheck[ pRoughChara->getCharaNetIndex() ][HIT_CHECK_BODY] )
					continue;

				nResultCount = g_pFramework->raycast( &ColLine, nColGroup );
				if( nResultCount > 0 )
				{
					pNextResult = g_pFramework->getRaycastResult();

					for( k = 0; k < nResultCount; k++ )
					{
						i3EventReceiver * pReceiver = pNextResult[ k ].m_pDestShapeSet->getEventReceiver();
						if( !i3::kind_of<CGameCharaBase*>(pReceiver))
							continue;

						CGameCharaBase * pHitChara = (CGameCharaBase*) pReceiver;

						if( pHitChara->getCharaNetIndex() == -1 )	
							continue;

						if( rLength < pNextResult[ k ].m_T )
							continue;

						// 캐릭터 히트 계산
						if( !nCharaHitCheck[ pHitChara->getCharaNetIndex() ][hitCheck] )
							nCharaHitCheck[ pHitChara->getCharaNetIndex() ][hitCheck] = 1;
						else
							continue;

						//DamageInfo에 오브젝트 폭발 범위 내에 있다고 세팅해줌.
						DamageInfo._HitByObjExplosion[pHitChara->getCharaNetIndex()] = 1;
						//I3ASSERT( DamageInfo.isValid());
					}
				}
			}
			g_pFramework->resetRaycastResult();
		}
	}
}


void obj_explosion::CalcHitByObject( UINT8 nCharaHitCheck[][HIT_CHECK_COUNT],
								CHARA_DAMAGEINFO& DamageInfo, i3CollideeLine& ColLine,
								REAL32& rLength, CGameCharaBase* pChara,
								CGameCharaBase* pShooter)
{
	I3_PHYSIX_HIT_RESULT * pNextResult, * pRoughResult;
	INT32 nRoughResultCount = 0;
	INT32 j = 0;
	INT32 k = 0;
	INT32 idx = pChara->getCharaNetIndex();
	INT32 nColGroup = 0;
	INT32 nRoughHitGroup = 0;
	INT32 nResultCount = 0;

	if( pChara->getCharaTeam() & CHARACTER_TEAM_RED)
	{
		nColGroup	= CGRP_MASKGROUP_CHARA_RED;
		nRoughHitGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;
	}
	else
	{
		nColGroup	= CGRP_MASKGROUP_CHARA_BLUE;
		nRoughHitGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
	}

	HIT_CHECK hitCheck = HIT_CHECK_COUNT;

	if(DamageInfo.GetHitPart() == CHARA_HIT_HEAD) {
		hitCheck = HIT_CHECK_HEAD;
	}
	else if(DamageInfo.GetHitPart() == CHARA_HIT_BODY1) {
		hitCheck = HIT_CHECK_BODY;
	}
	else {
		I3ASSERT_0;
	}

	if( nCharaHitCheck[ idx ][hitCheck] == 0 )
	{
		//World에 대해서 한번 테스트
		pNextResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ONLYWORLDHIT );
		if( pNextResult != nullptr )
		{
			if( (pNextResult->m_pDestShapeSet->getShapeGroup() == CGRP_WORLD_ATTACK) &&
				(pNextResult->m_T < rLength) )	// 월드에 대한 컬리젼 거리를 알아둔다
			{
				rLength = pNextResult->m_T;
			}
		}

		// RoughHit check
		nRoughResultCount = g_pFramework->raycast( &ColLine, nRoughHitGroup );
		if( nRoughResultCount > 0 )
		{
			pRoughResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nRoughResultCount );

			for( j = 0; j < nRoughResultCount; j++ )
			{
				i3EventReceiver * pRoughReceiver = pRoughResult[ j ].m_pDestShapeSet->getEventReceiver();
				if( !i3::kind_of<CGameCharaBase*>(pRoughReceiver))
					continue;

				CGameCharaBase * pRoughChara = (CGameCharaBase*) pRoughReceiver;
				if( pRoughChara->getCharaNetIndex() == -1 )	continue;
				if( rLength < pRoughResult[ j ].m_T ) continue;

				if( nCharaHitCheck[ pRoughChara->getCharaNetIndex() ][HIT_CHECK_HEAD] &&
					nCharaHitCheck[ pRoughChara->getCharaNetIndex() ][HIT_CHECK_BODY] )
					continue;

				nResultCount = g_pFramework->raycast( &ColLine, nColGroup );
				if( nResultCount > 0 )
				{
					pNextResult = g_pFramework->getRaycastResult();

					for( k = 0; k < nResultCount; k++ )
					{
						i3EventReceiver * pReceiver = pNextResult[ k ].m_pDestShapeSet->getEventReceiver();
						if( !i3::kind_of<CGameCharaBase*>(pReceiver))
							continue;

						CGameCharaBase * pHitChara = (CGameCharaBase*) pReceiver;
						if( pHitChara->getCharaNetIndex() == -1 )	continue;
						
						if( rLength < pNextResult[ k ].m_T )
							continue;

						// 캐릭터 히트 계산
						if( !nCharaHitCheck[ pHitChara->getCharaNetIndex() ][hitCheck] )
							nCharaHitCheck[ pHitChara->getCharaNetIndex() ][hitCheck] = 1;
						else
							continue;

						{
							if( pShooter != nullptr )	DamageInfo._nKillerIdx	= pShooter->getCharaNetIndex();
							else					DamageInfo._nKillerIdx	= pHitChara->getCharaNetIndex();
							DamageInfo._pVecDir		= ColLine.GetDir();
							DamageInfo._pVecPos		= ColLine.GetEnd();
							DamageInfo._rDistance	= pNextResult[ k ] .m_T;
							DamageInfo._pHitResult	= &pNextResult[ k ];
							DamageInfo._IsNPC = false;
							DamageInfo._nVictimIdx	= pHitChara->getCharaNetIndex();

							i3Vector::Copy( &DamageInfo._vStartPos, ColLine.GetStart());
							i3Vector::Copy( &DamageInfo._vHitPos, &pNextResult[k].m_Intersect);

							I3ASSERT( DamageInfo.isValid() );

							pHitChara->OnHit( &DamageInfo );
						}
					}
				}
			}
			g_pFramework->resetRaycastResult();
		}
	}
}

REAL32	obj_explosion::SetObjForce( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pStartPos, REAL32 rRange, REAL32 rForce, REAL32 rSlop )
{
	VEC3D	vDir;
	REAL32	rLength = 0.f;

	i3PhysixShapeSet * pShapeSet = pResult->m_pDestShapeSet;
	i3Object * pObj = (i3Object*) pShapeSet->getEventReceiver();
	if( pObj == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN, GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_WARNING_NOT_EXIST_OBJECT"));/*오브젝트가 없어요~~ 2. CStageBattle::SetObjForce()..\n*/
		return 0.f;
	}

	if( pObj->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC )
	{
		if( BattleServerContext::i()->IsP2PHost() ||
			(pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE) )
		{
			i3Vector::Sub( &vDir, &pResult->m_Intersect, pStartPos );

			rLength = i3Vector::Normalize( &vDir, &vDir );
			rLength = rRange - rLength;
			if( rLength < I3_EPS ) rLength = I3_EPS;

			{
				VEC3D vForce, vTorque, vR;
				MATRIX mat;
				i3Vector::Scale( &vForce, &vDir, rLength * rForce * rSlop );

				pShapeSet->getShapeGlobalPose( &mat );
				i3Vector::Sub( &vR, &pResult->m_Intersect, i3Matrix::GetPos( &mat ) );
				i3Vector::Cross( &vTorque, &vR, &vForce );
				pShapeSet->addForce( &vForce);

				i3Vector::Scale( &vTorque, &vTorque, g_pPhysixInfoDataBase->getInfo()->m_rObjectAngSlop );
				pShapeSet->addTorque( &vTorque);
			}

			//I3TRACE( "ADD FORCE : %f (%f, %f, %f)\n", rLength * rForce * rSlop, rLength, rForce, rSlop);
			//PHYSIX_SetLinearForce( pObj, &vDir, &pResult->m_Intersect, rForce * rSlop, pResult->m_pDestShapeSet );
		}
	}

	return rLength;
	
}
