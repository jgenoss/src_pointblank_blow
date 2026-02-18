#include "pch.h"
#include "MainWeapon_WPSmoke.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "Weapon/WeaponInfo/GrenadeInfo.h"

MainWeapon_WPSmoke::MainWeapon_WPSmoke(WeaponBase* p) : MainWeapon_Grenade(p)
{
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_bUserFirstOverlap[ i] = false;
		m_rUserOverlapTime[ i] = 0.f;
	}
	m_rDamageCheckTime = 0.f;
	m_rFlushTime = 0.f;
	m_rRadiusSlope = 0.f;
}

void	MainWeapon_WPSmoke::OnInitVariable() 
{
	MainWeapon_Grenade::OnInitVariable();
	
	m_rFlushTime		= 0.f;
	m_rDamageCheckTime	= 0.f;
	m_rRadiusSlope		= 0.f;
	i3mem::FillZero( m_rUserOverlapTime, sizeof( m_rUserOverlapTime));
	i3mem::FillZero( m_bUserFirstOverlap, sizeof( m_bUserFirstOverlap));
	
}

void	MainWeapon_WPSmoke::Reset() 
{
	MainWeapon_Grenade::Reset();
	
	m_rFlushTime		= 0.f;
	m_rDamageCheckTime	= 0.f;
	m_rRadiusSlope		= 0.f;
	i3mem::FillZero( m_rUserOverlapTime, sizeof( m_rUserOverlapTime));
	i3mem::FillZero( m_bUserFirstOverlap, sizeof( m_bUserFirstOverlap));

}

void	MainWeapon_WPSmoke::Explosion()
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

void	MainWeapon_WPSmoke::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade::OnUpdate_Grenade(rDeltaSeconds);
	
	if( this->isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
		_ProcessDamageBySmoke();
	
}


void MainWeapon_WPSmoke::_ProcessDamageBySmoke( void)
{
	INT32		i;
	VEC3D		vDir;
	MATRIX		mat;

	// AI모드일때
	if( BattleServerContext::i()->IsUseDedicationMode() == false)
	{
		//내가 던진게 아니라면
		if( BattleServerContext::i()->IsP2PHost() == false)
			return;
	}
	else {
		if( m_pWeaponBase->getOwner() == nullptr || m_pWeaponBase->getOwner()->isLocal() == false)	// 이부분 확인 필요 nullptr인경우가 있다
			return;
	}

	m_rFlushTime += g_pViewer->getDeltaTime();
	if( m_rFlushTime > m_pWeaponBase->getWeaponInfo()->GetEffectiveTime() )
	{
		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
			pInfo->Reset();
		}

		g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
		return;
	}

	m_rDamageCheckTime += g_pViewer->getDeltaTime();
	if( m_rDamageCheckTime < 0.1f)	return;

	I3ASSERT( m_pWeaponBase->getOwner() != nullptr);

	UINT8		nUserCheck[ SLOT_MAX_COUNT] = {0,};
	
	REAL32	rDamage = m_pWeaponBase->getAP();

	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[ 256];
	
	NxSphere nxSphere;
	nxSphere.center.x = getX( m_pWeaponBase->GetPos());
	nxSphere.center.y = getY( m_pWeaponBase->GetPos());
	nxSphere.center.z = getZ( m_pWeaponBase->GetPos());

	m_rRadiusSlope += m_rDamageCheckTime * 1.7f;
	if( m_rRadiusSlope > m_pWeaponBase->getWeaponInfo()->GetExplosion_Range())
		m_rRadiusSlope =  m_pWeaponBase->getWeaponInfo()->GetExplosion_Range();

	nxSphere.radius = m_rRadiusSlope;

	INT32 iActiveGrp = (1<<CGRP_TEAM_BLUE) | (1<<CGRP_TEAM_RED);
	
	INT32 nOverlapCount = pNxScene->overlapSphereShapes( nxSphere, NX_DYNAMIC_SHAPES, 256, pShapeList, nullptr, iActiveGrp);
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

		m_rUserOverlapTime[ pChara->getCharaNetIndex()] += m_rDamageCheckTime;
		if( m_rUserOverlapTime[ pChara->getCharaNetIndex()] < ((CGrenadeInfo*)m_pWeaponBase->getWeaponInfo())->GetDamageTerm()) continue;
		m_rUserOverlapTime[ pChara->getCharaNetIndex()] = 0.f;

		I3_PHYSIX_HIT_RESULT	tempHitResult;

		const PHYSIX_SHAPESET_INFO * pShapeSetInfo = pChara->getPhysixShapeSetInfo( CHARA_HIT_BODY3);

		CHARA_DAMAGEINFO DamageInfo;
		
		pShapeSetInfo->_pShapeSet->getShapeGlobalPose( &mat);
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( &mat));
		i3Vector::Sub( &vDir, i3Matrix::GetPos( &mat), m_pWeaponBase->GetPos());
		tempHitResult.m_T			= i3Vector::Normalize( &tempHitResult.m_Normal, &vDir);
		tempHitResult.m_pDestShapeSet = pShapeSetInfo->_pShapeSet;
		i3Vector::Scale( &tempHitResult.m_Normal, &tempHitResult.m_Normal, -1.f);

		m_pWeaponBase->getWeaponInfo()->GetDamageInfo( &DamageInfo);
		DamageInfo.SetWeaponItemID(m_pWeaponBase);

		DamageInfo._FireOrder		= m_pWeaponBase->getFireOrder();
		DamageInfo._IsOwner			= m_pWeaponBase->getOwner()->isLocal();
		DamageInfo._nKillerIdx		= m_pWeaponBase->getOwner()->getCharaNetIndex();
		DamageInfo._pHitResult		= &tempHitResult;
		DamageInfo._HitType			= GHT_CHARA;
		DamageInfo.SetHitPart( CHARA_HIT_BODY3 );
		DamageInfo._pWeapon			= m_pWeaponBase;
		DamageInfo._pWeaponInfo		= m_pWeaponBase->getWeaponInfo();
		DamageInfo._DeathType		= CHARA_DEATH_POISON;
		DamageInfo._pVecPos			= m_pWeaponBase->GetPos();
		DamageInfo._pVecDir			= &vDir;
		DamageInfo._rDistance		= tempHitResult.m_T;
		DamageInfo._rDamage			= rDamage;
		DamageInfo._EnableEffect	= !m_bUserFirstOverlap[ pChara->getCharaNetIndex()];
		DamageInfo._nNetidx			= m_pWeaponBase->getNetworkIdx();
		DamageInfo._nVictimIdx		= pChara->getCharaNetIndex();

		I3ASSERT( DamageInfo.isValid() );
		i3Vector::Copy( &DamageInfo._vStartPos, m_pWeaponBase->GetPos());

		pChara->OnHit( &DamageInfo, m_pWeaponBase->getOwner());

		m_bUserFirstOverlap[ pChara->getCharaNetIndex()] = true;
	}

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( nUserCheck[ i] == 0)
		{
			m_rUserOverlapTime[ i] -= m_rDamageCheckTime;
			if( m_rUserOverlapTime[ i] < 0.f)
			{
				m_rUserOverlapTime[ i] = 0.f;
				m_bUserFirstOverlap[ i] = false;
			}
		}
	}

	m_rDamageCheckTime = 0.f;
}
