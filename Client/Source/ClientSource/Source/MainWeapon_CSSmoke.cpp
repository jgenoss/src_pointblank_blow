#include "pch.h"
#include "MainWeapon_CSSmoke.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "Weapon/WeaponInfo/GrenadeInfo.h"
#include "BattleSlotContext.h"
#include "UI/UIHudObserver.h"

MainWeapon_CSSmoke::MainWeapon_CSSmoke(WeaponBase* p) : MainWeapon_Grenade(p)
{
	m_rEffectTime = 0.f;
	m_rRadiusSlope = 0.f;
}

void	MainWeapon_CSSmoke::OnInitVariable()
{
	MainWeapon_Grenade::OnInitVariable();
	
	m_rEffectTime = 0.f;
	m_rRadiusSlope		= 0.f;
}

void	MainWeapon_CSSmoke::Reset()
{
	MainWeapon_Grenade::Reset();
	
	m_rEffectTime = 0.f;
	m_rRadiusSlope		= 0.f;
}

void	MainWeapon_CSSmoke::Explosion()
{
	// 폭발 이팩트
	i3Object * pObj = nullptr;
	i3Transform * pTrans = nullptr;
	I3_PHYSIX_HIT_RESULT * pResult;
	i3CollideeLine line;
	VEC3D vDir;

	i3Vector::Set( &vDir, 0.f, -1.f, 0.f);
	line.SetStart( m_pWeaponBase->GetPos());
	line.SetDir( &vDir);
	pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
	if( pResult != nullptr)
	{
		UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();
			
		if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
		{
			i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
			I3ASSERT( i3::kind_of<i3Object*>(pReceiver));

			pObj = (i3Object*) pReceiver;
			pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );
		}
	}
	if( pTrans != nullptr)		g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), m_pWeaponBase->GetPos(), &I3_YAXIS, pTrans->GetCacheMatrix(), 0.f, pObj);
	else					g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), m_pWeaponBase->GetPos(), &I3_YAXIS);

	addBombState( WEAPON_BOMBSTATE_EXPLOSION);
	
}

void	MainWeapon_CSSmoke::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade::OnUpdate_Grenade(rDeltaSeconds);
	
	if( this->isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
		_ProcessHazeBySmoke();
}

void MainWeapon_CSSmoke::_ProcessHazeBySmoke( void)
{
	INT32		i;
	VEC3D		vDir;
	MATRIX		mat;

	if (m_pWeaponBase->getWeaponInfo()->IsHazeEffect() == false) return;

	if( BattleServerContext::i()->IsUseDedicationMode() == false)
	{
		// 일반 Client는 계산하지 않느다.
		if( BattleServerContext::i()->IsP2PHost() == false)
			return;
	}
	//else {
		//if( m_pWeaponBase->getOwner() == nullptr || m_pWeaponBase->getOwner()->isLocal() == false)	// 이부분 확인 필요 nullptr인경우가 있다
			//return;
	//}

	m_rEffectTime += g_pViewer->getDeltaTime();
	if(m_rEffectTime > m_pWeaponBase->getWeaponInfo()->GetEffectiveTime() )
	{
		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
			pInfo->Reset();
		}

		if(g_pFramework->GetHazePtr()) g_pFramework->GetHazePtr()->EndEffect();
		observer_hud::Context::i()->Notify(observer_hud::EVT_HAZEMODE, false);

		g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
		return;
	}

	I3ASSERT( m_pWeaponBase->getOwner() != nullptr);

	UINT8		nUserCheck[ SLOT_MAX_COUNT] = {0,};


	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[ 256];
	
	NxSphere nxSphere;
	nxSphere.center.x = getX( m_pWeaponBase->GetPos());
	nxSphere.center.y = getY( m_pWeaponBase->GetPos());
	nxSphere.center.z = getZ( m_pWeaponBase->GetPos());

	m_rRadiusSlope += g_pViewer->getDeltaTime() * 1.7f;
	if( m_rRadiusSlope > m_pWeaponBase->getWeaponInfo()->GetExplosion_Range())
		m_rRadiusSlope =  m_pWeaponBase->getWeaponInfo()->GetExplosion_Range();

	nxSphere.radius = m_rRadiusSlope;

	INT32 iActiveGrp = (1<<CGRP_TEAM_BLUE) | (1<<CGRP_TEAM_RED);
	
	INT32 nOverlapCount = pNxScene->overlapSphereShapes( nxSphere, NX_DYNAMIC_SHAPES, 256, pShapeList, nullptr, iActiveGrp);

	if (g_pFramework->GetHazePtr()) g_pFramework->GetHazePtr()->EndEffect();
	observer_hud::Context::i()->Notify(observer_hud::EVT_HAZEMODE, false);

	//I3TRACE( "overlapcount : %d\n", nOverlapCount);
	for( i = 0; i < nOverlapCount; i++ )
	{
		i3EventReceiver * pReceiver = (i3EventReceiver*) pShapeList[ i]->userData;
		
		if( !i3::same_of<i3PhysixShapeSet*>(pReceiver))
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pReceiver;
		if( !i3::kind_of<CGameCharaBase*>(pShapeSet->getEventReceiver()))
			continue;
		
		CGameCharaBase * pChara = (CGameCharaBase*) pShapeSet->getEventReceiver();

		if( pChara->getCharaNetIndex() == -1)				continue;
		if( nUserCheck[ pChara->getCharaNetIndex()] == 1)	continue;
		nUserCheck[ pChara->getCharaNetIndex()] = 1;
		if( pChara->isEnable() == false) continue;
		if (pChara->IsDomiMutantRex()) continue;

		if( pChara->getCharaTeam() == m_pWeaponBase->getOwner()->getCharaTeam() )
		{
			if( pChara != m_pWeaponBase->getOwner() )
				continue;
		}

		if( pChara->isInvincible() ) continue;

		
		//옵져버 모드이면 Haze가 걸렷다는 Hud마크를 켜줘야함
		if (pChara->isCharaStateMask(CHARA_STATE_OBSERVER) && !pChara->isCharaStateMask(CHARA_STATE_DEATH))
		{
			if (observer_hud::Context::i()->SameOfUserIdx(pChara->getCharaNetIndex()))
			{
				observer_hud::Context::i()->Notify(observer_hud::EVT_HAZEMODE, true);
			}
		}

		//나인 경우에만
		if (BattleSlotContext::i()->getMySlotIdx() != pChara->getCharaNetIndex()) continue;

		if (g_pFramework->GetHazePtr())
		{
			if (pChara->isCharaStateMask(CHARA_STATE_DEATH))
			{
				g_pFramework->GetHazePtr()->InitEffect();
			}
			else
			{
				g_pFramework->GetHazePtr()->StartEffect();
			}
		}
	}
}
