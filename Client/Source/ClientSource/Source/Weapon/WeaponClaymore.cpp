#include "pch.h"
#include "WeaponClaymore.h"
#include "MainWeapon_Claymore.h"

#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"
#include "BattleServerContext.h"
#include "../StageBattle/UserContext.h"

#include "WeaponFunction.h"

I3_CLASS_INSTANCE(WeaponClaymore);

WeaponClaymore::WeaponClaymore(void)
{
	
}

WeaponClaymore::~WeaponClaymore(void)
{
}



/*virtual*/ MainWeapon*	WeaponClaymore::CreateMainWeapon()
{
	MainWeapon_Claymore * p = new MainWeapon_Claymore(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_Claymore));
	return p;
}

/*virtual*/ void WeaponClaymore::_InitCollideeGroup(void)
{
	WF::SetCollideeGroup_NormalWeapon(this);
	i3Scene::ModifyDynamicState(getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0);
}

/*virtual*/ void WeaponClaymore::_PlayFireSound(WEAPON::FIRE_SOUND_TYPE type)
{

}

void WeaponClaymore::_SetWeaponLookDir(void)
{
	I3ASSERT( getOwner() != nullptr);

	VEC3D vAt;
	i3Vector::Copy( &vAt, i3Matrix::GetAt( getOwner()->GetMatrix() ) );
	i3Vector::Normalize( &vAt, &vAt );

	MainWeapon_Claymore * pMainWeapon = static_cast<MainWeapon_Claymore*>(GetMainWeapon());
	pMainWeapon->SetLookAt(&vAt);
}

void WeaponClaymore::_CheckChainExplosion(REAL32 rRange, REAL32 rRadian/* = I3_PI*/)
{
	I3ASSERT( getOwner() != nullptr);

	MainWeapon_Claymore * pMainWeapon = static_cast<MainWeapon_Claymore*>(GetMainWeapon());
	pMainWeapon->CheckChainExplosion( GetPos(), rRange, rRadian );
}

/*virtual*/ void WeaponClaymore::SetHitCollideeGroup(void)
{
	WF::SetCollideeGroup_HitWeapon(this);
}

/*virtual*/ void WeaponClaymore::OnUpdate(REAL32 rDeltaSeconds)
{
	WeaponGrenade::OnUpdate( rDeltaSeconds);
}

/*virtual*/ void WeaponClaymore::_OnCheckCollisionByExplosion(void)
{
	if( m_pOwner == nullptr)
		return;

	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( BattleServerContext::i()->IsUseDedicationMode())
		bHost = m_pOwner->isLocal();

	//------------------------------------------------------------------------------------------//
	//										Character Collision	
	UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
	UINT32 nSelfColGroup = CGRP_MASKGROUP_CHARA_RED;

	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
	{
		nColGroup		= CGRP_MASKGROUP_CHARA_RED;
		nSelfColGroup	= CGRP_MASKGROUP_CHARA_BLUE;
	}

	REAL32 rRange = GetExplosionRange();
	REAL32 rNearRange = GetExplosionNearRange();

	for( INT32 idx = 0; idx < g_pCharaManager->getCharaCount(); ++idx)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( idx);
		if( pChara == nullptr)
			continue;

		if( pChara != m_pOwner && pChara->getCharaTeam() == m_pOwner->getCharaTeam() )
			continue;

		if( pChara->isCharaStateMask(CHARA_STATE_DEATH) )
			continue;

		UINT32 nChkColGroup = (pChara == m_pOwner) ? nSelfColGroup : nColGroup;
		if( pChara->IsRex()) rRange *= 2.0f;

		VEC3D vLength;
		i3Vector::Sub( &vLength, i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()), GetPos() );
		REAL32 rLength = i3Vector::Length( &vLength );

		if( bHost )
		{
			if( rLength <= rNearRange )
			{
				_OnCheckCharaCollisionByExplosion(pChara, nChkColGroup, rNearRange );
			}
			else
			{
				if( IsValidExplosionArea(i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix() ), rRange, I3_PI4) )
				{
					_OnCheckCharaCollisionByExplosion(pChara, nChkColGroup, rRange );
				}
				else if( IsValidExplosionArea(i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()), rRange, I3_PI4) )
				{
					_OnCheckCharaCollisionByExplosion(pChara, nChkColGroup, rRange );
				}
			}
		}

		//수류탄이 터지는 근처에 Owner가 위치해있는경우 카메라 흔들림 효과
		if( (g_pCamera != nullptr) && pChara->is1PV() && !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
		{	
			REAL32 MaxLength = rRange * 2.0f;
			if( rLength < MaxLength)
			{
				REAL32 TrembAmpRate = 1.0f - rLength/MaxLength;
				g_pCamera->SetTremble( 15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
			}
		}
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		// 수류탄으로 죽은 캐릭터를 우선적으로 보내줍니다.
		UserContext::i()->SendDeathChara();
	}

	//	Object Collision
	_CheckExplosionObjectCollision( rRange, I3_PI4);
	_CheckExplosionObjectCollision( rNearRange, I3_PI);

	// Weapon Collision (Claymore, Chain Explosion)
	_CheckExplosionWeapon( rRange, I3_PI4 );
	_CheckExplosionWeapon( rNearRange, I3_PI);

	/*_CheckChainExplosion( rRange, I3_PI4);
	_CheckChainExplosion( rNearRange, I3_PI);*/
}

/*virtual*/ bool WeaponClaymore::IsValidExplosionArea(VEC3D * pTargetPos, REAL32 rRange, REAL32 rRadian, bool bChkWorld)
{
	VEC3D vLength;
	i3Vector::Sub( &vLength, pTargetPos, GetPos() );
	REAL32 rLength = i3Vector::Length( &vLength );

	if( rLength <= rRange )
	{
		VEC3D vDir;
		i3Vector::Normalize(&vDir, &vLength);

		MainWeapon_Claymore * pMainWeapon = static_cast<MainWeapon_Claymore*>(GetMainWeapon());

		REAL32 rDot = i3Vector::Dot(pMainWeapon->GetLookAt(), &vDir);
		rDot = i3Math::acos(rDot);

		if( rDot <= rRadian )
		{
			if( bChkWorld == false )
				return true;

			if( IsThereWallBetween( *GetPos(), *pTargetPos, rRange) )
				return true;
		}
	}

	return false;
}

/*virtual*/ void WeaponClaymore::OnFire(i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenade::OnFire(pCtx, tm);
}

/*virtual*/ void WeaponClaymore::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenade::OnEndFire(pCtx, tm);
}

/*virtual*/ void WeaponClaymore::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx )
{
	WeaponBase::NET_Throw( pStartPos, pTargetPos, nNetSlotIdx );

	VEC3D start, dir;

	if( pStartPos == nullptr )
	{	
		i3Vector::Copy( &start, m_pOwner->GetPos() );
		i3Vector::SetY( &start, getY( m_pOwner->GetPos()) + m_pOwner->getCameraContext()->getIKHeightForCam());
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	i3Vector::Copy(pTargetPos, &start);
	i3Vector::SetY(pTargetPos, 0.0f);

	// 수류탄이 던져집니다.
	NET_FireSync( pTargetPos );

	//다른캐릭터나 NoneServer일경우만 처리했으나
	//내 캐릭터도 바로 처리한다.
	if( getOwner() != nullptr )
	{
		// Claymore Look 방향 설정
		_SetWeaponLookDir();

		// 수류탄이 던져집니다.
		i3Vector::Sub( &dir, pTargetPos, &start);
		i3Vector::Normalize( &dir, &dir);
		//2014-10-08 크레이모아는 무조건 지상으로 던져져야 합니다. 양수 값이 나올 경우 변환해 줍니다. ENG_김대영
		if( getY(&dir) > 0.0f ) i3Vector::SetY(&dir, ( getY(&dir) * -1.0f ) );

		NET_FirstThrow( &start, &dir, nNetSlotIdx );
	}
}

void WeaponClaymore::SetWeaponLookDir(VEC3D * pLookDir)
{
	MainWeapon_Claymore * pMainWeapon = static_cast<MainWeapon_Claymore*>(GetMainWeapon());
	pMainWeapon->SetLookAt(pLookDir);
}