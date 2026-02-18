#include "pch.h"
#include "WeaponC5.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaMoveContext.h"
#include "WeaponFunction.h"
#include "MainWeapon_C5.h"
#include "WeaponItemID.h"

I3_CLASS_INSTANCE( WeaponC5);//, WeaponBase);

MainWeapon*	WeaponC5::CreateMainWeapon()
{
	MainWeapon_C5 * p = new MainWeapon_C5(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_C5));
	return p;
}

WeaponC5::WeaponC5()
{

}

WeaponC5::~WeaponC5()
{
}

void WeaponC5::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NormalWeapon(this);
	i3Scene::ModifyDynamicState( getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0);
}

void WeaponC5::_OnHitCharaByExplosion( CGameCharaBase * pChara, CHARA_HIT_PART part, VEC3D * pDir, VEC3D * pStart )
{
	VEC3D	vDir;
	I3_PHYSIX_HIT_RESULT	tempHitResult;
	I3_PHYSIX_HIT_RESULT *	pWallHitResultTemp;

	I3ASSERT( pChara != nullptr );
	I3ASSERT( pChara->getSceneObject()->GetBody() != nullptr);
	I3ASSERT( pChara->getSceneObject()->GetBody()->getFrameLOD() != nullptr);

	i3Skeleton * pSkel = pChara->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();

	I3ASSERT( pSkel != nullptr);

	CHARA_DAMAGEINFO DamageInfo;

	CGameCharaBase * pKiller = m_pOwner;
	I3ASSERT( pKiller != nullptr);

	tempHitResult.m_T			= i3Vector::Normalize( &tempHitResult.m_Normal, pDir);
	if( (part == CHARA_HIT_HEAD))
	{
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix()) );
		if( pChara->IsRex())
		{	
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( TREX_BONE_HEAD) );
		}
		else if( pChara->IsRaptorBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_HEAD) );
		}
		else
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_HEAD) );
		}
	}
	else if( part == CHARA_HIT_ROOT)
	{
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()) );

		if( pChara->IsRex())
		{	
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( TREX_BONE_PELVIS) );
		}
		else if( pChara->IsRaptorBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_ROOT) );
		}
		else
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_ROOT) );
		}
	}
	else
		tempHitResult.m_pDestShapeSet = nullptr;

	I3ASSERT( tempHitResult.m_pDestShapeSet != nullptr);

	i3Vector::Scale( &tempHitResult.m_Normal, &tempHitResult.m_Normal, -1.f);

	m_pWeaponInfo->GetDamageInfo(&DamageInfo);

	DamageInfo.SetWeaponItemID(this);
	DamageInfo._FireOrder		= getFireOrder();
	DamageInfo._rDamage			= getAP();
	DamageInfo._IsOwner			= pKiller->isLocal();
	DamageInfo._nKillerIdx		= pKiller->getCharaNetIndex();
	DamageInfo._pHitResult		= &tempHitResult;
	DamageInfo._HitType			= GHT_CHARA;
	DamageInfo.SetHitPart( part );
	DamageInfo._rDistance		= tempHitResult.m_T;
	DamageInfo._nNetidx			= this->getNetworkIdx();

	if( g_pEnvSet->m_bEnableBlood )
	{
		VEC3D	vAdd;
		i3CollideeLine BloodLine;

		if( (part == CHARA_HIT_HEAD))
		{
			BloodLine.SetStart( i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix()) );
		}
		else if( part == CHARA_HIT_ROOT)
		{
			BloodLine.SetStart( i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()) );
		}

		i3Vector::Add( &vAdd, BloodLine.GetStart(), pDir);
		BloodLine.SetEnd( &vAdd );

		pWallHitResultTemp = g_pFramework->raycastClosest( &BloodLine, CGRP_MASKGROUP_ATTACK );
		if( pWallHitResultTemp != nullptr )
		{
			if( pWallHitResultTemp->m_T < BLOODEFFECT_HIT_LENGTH )
				DamageInfo._pWallHitResult = pWallHitResultTemp;
		}
	}

	//	폭발물에 죽음
	if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_GRENADESHELL) &&
		(m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::GSHELL_POISON)))
	{
		DamageInfo._DeathType	= CHARA_DEATH_POISON;
	}
	else
		DamageInfo._DeathType	= CHARA_DEATH_B;

	DamageInfo._pVecPos		= GetPos();

	i3Vector::Scale( &vDir, &tempHitResult.m_Normal, -1.f);
	DamageInfo._pVecDir		= &vDir;

	i3Vector::Copy( &DamageInfo._vStartPos, GetPos() );
	i3Vector::Copy( &DamageInfo._vHitPos, &tempHitResult.m_Intersect);
	DamageInfo._nVictimIdx		= pChara->getCharaNetIndex();

	I3ASSERT( DamageInfo.isValid() );

	// 캐릭터의 HP 및 이펙트 처리
	pChara->OnHit( &DamageInfo);
}
 
/*virtual*/ void WeaponC5::SetHitCollideeGroup(void)
{
	WF::SetCollideeGroup_HitWeapon(this);
}

void WeaponC5::OnUpdate( REAL32 rDeltaSeconds)
{
	MainWeapon_C5* mainWeapon = static_cast<MainWeapon_C5*>(GetMainWeapon());
	mainWeapon->OnUpdate_C5(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}

//void WeaponC5::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//	// 시점이 향하고 있는 목표 지점을 구한다.
//	NET_Fire( nullptr, nullptr );
//}

void WeaponC5::FireEnd(void)
{	
}

void WeaponC5::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	setWeaponState( WEAPON::STATE_FIRE);

	WeaponBase::NET_Fire( nullptr, nullptr, type );
}

void WeaponC5::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx )
{
	WeaponBase::NET_Throw( pStartPos, pTargetPos, nNetSlotIdx );

	VEC3D start, dir;
	
	if( pStartPos == nullptr )
	{	
		m_pOwner->getCameraContext()->getVirtualCamPosition( &start);					// 현재는 Camera의 지점
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	if( m_pAICtx != nullptr)
		m_pAICtx->SetExclusiveEnable( false, false);

	//다른캐릭터나 NoneServer일경우만 처리했으나
	//내 캐릭터도 바로 처리한다.
	if( getOwner() != nullptr )
	{
		// 수류탄이 던져집니다.
		i3Vector::Sub( &dir, pTargetPos, &start);
		i3Vector::Normalize( &dir, &dir);

		NET_FirstThrow( &start, &dir, nNetSlotIdx );
	}
}

/*virtual*/ REAL32 WeaponC5::GetThrowSpeed( void)
{
	I3ASSERT( m_pOwner != nullptr);
	return (m_pOwner->getMoveContext()->getSpeed() * 1.5f) + WeaponBase::GetThrowSpeed();
}

void WeaponC5::NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx )
{
	I3ASSERT( m_pOwner != nullptr);

	MainWeapon_C5* mainWeapon = static_cast<MainWeapon_C5*>(GetMainWeapon());
	mainWeapon->Throw( pStartPos, pDir, m_pOwner->GetThrowSpeed(this->getWeaponInfo()->GetTypeUsage()) );


#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGrenade())		//	Use Cheat
#endif
	{
		setLoadedBulletCount( i3Math::softAdd( getLoadedBulletCount(), -m_pWeaponInfo->GetFireDecBulletCount()) );
	}

	WeaponBase::NET_FirstThrow( pStartPos, pDir, nNetSlotIdx );
}

void WeaponC5::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	WeaponBase::NET_Bound( pStartPos, pDir, fSpeed, nBoundType );

	MainWeapon_C5* mainWeapon = static_cast<MainWeapon_C5*>(GetMainWeapon());

	mainWeapon->NET_Bound_C5(pStartPos, pDir, fSpeed, nBoundType);
}

void WeaponC5::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_BOUND_THROW, GetPos(), false);
}

void WeaponC5::ReturnInstance()
{
	WeaponBase::ReturnInstance();
}


// UI weapon animation
void WeaponC5::PlayAnim_UI_IdleB()
{
	SetVisible();
	_OnPlayWeaponIdleAnim();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			Animation Callback
void WeaponC5::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);
	
//	_OnPlayWeaponIdleAnim();
//	removeWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);

}

void WeaponC5::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	setWeaponState( WEAPON::STATE_READY);
}

void WeaponC5::OnFire( i3AIContext * pCtx, REAL32 tm)
{	
}


