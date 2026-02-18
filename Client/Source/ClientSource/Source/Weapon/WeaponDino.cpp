#include "pch.h"
#include "WeaponDino.h"
#include "GameCharaDino.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"
#include "WeaponItemID.h"
#include "WeaponInfo/WeaponDinoInfo.h"

I3_CLASS_INSTANCE( WeaponDino);

WeaponDino::WeaponDino() : m_pExtWeapon(nullptr), m_rAttakActiveTime(0.0f)
{
}

WeaponDino::~WeaponDino()
{
	I3_SAFE_RELEASE( m_pExtWeapon );
}

void WeaponDino::OnUpdate( REAL32 rDeltaSeconds)
{
	ProcessSound( rDeltaSeconds);
}

/*virtual*/ void WeaponDino::Create(CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create(pOwner, Itemid, bCreateInWeapon);

	// 공룡의 Extension은 최대 1개라고 가정합니다. ( Sting : Breath, Tank : Dash Crash)
	T_ItemID extItemID = m_pWeaponInfo->GetSubExtensionItemID();
	if(extItemID > 0)
		m_pExtWeapon = g_pWeaponManager->_CreateDinoWeapon(extItemID, extItemID, pOwner);
}

REAL32 WeaponDino::GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type)
{
	if( getFireOrder() == WEAPON::SECONDARY_FIRE)
		return WeaponBase::GetMoveSpeed( type);

	if( type == WEAPON_MOVE_SPEED_FIRE)
		type = WEAPON_MOVE_SPEED_NORMAL;

	return WeaponBase::GetMoveSpeed( type);
}


REAL32 WeaponDino::GetDamage( bool bRandomDamage)
{
	return (REAL32)m_pWeaponInfo->GetDamageA( getAutoFireCount() );
}

REAL32	WeaponDino::GetRange()
{
	return m_pWeaponInfo->GetRange();
}

REAL32	WeaponDino::GetFireDelay()
{
	return m_pWeaponInfo->GetFireDelay();
}

void WeaponDino::_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	INT32 idx = ((CWeaponDinoInfo*)m_pWeaponInfo)->GetTerrainHitSoundIndex();

	if( idx > -1)
		g_pSndMng->ReserveSoundPlay( IT_DINO_TERRAIN_TYPE, type, idx, pPos, false);
}

bool WeaponDino::_DoFireCollisionDino(i3CollideeLine * pCol, INT32 idxGroup)
{
	bool bWarningOwner = false;

	I3_PHYSIX_HIT_RESULT * pLastHitResult;

	pLastHitResult = _DoFireCollisionKnife( pCol, idxGroup, &bWarningOwner );

	if(pLastHitResult == nullptr)
		return false;

	if ( m_pOwner->IsTank() )
	{
		ProcessAreaHitDino();
	}

	//DoFireHitEffect();	

	I3ASSERT_RETURN(pLastHitResult->m_pDestShapeSet, false);

	CGameCharaBase * pHitChara = nullptr;

	i3EventReceiver * pReceiver = pLastHitResult->m_pDestShapeSet->getEventReceiver();
	I3ASSERT( pReceiver != nullptr );

	if( i3::kind_of<CGameCharaBase*>(pReceiver)
		&& m_pOwner->isLocal())
	{
		pHitChara = (CGameCharaBase*) pReceiver;	

		if(pHitChara->isInvincible() == false) 
		{
			if( i3::kind_of<CGameCharaDino*>(m_pOwner) ) 
				((CGameCharaDino*)m_pOwner)->PlayDinoCrossHair();
		}
	}

	if( (m_pOwner->is1PV() || m_pOwner->isAI() ) )
	{
		// 이부분을 CharaCommand로 변경해야 합니다.
		m_pOwner->getAnimContext()->SetUpperTimeScale( 0.002f);
	}	

	return true;
}

void WeaponDino::_PlayFireEffectDino(void)
{
	if( !m_pWeaponInfo->hasFireEffect() )
		return;

	CGameCharaDino * pDino = i3::kind_cast<CGameCharaDino*>(m_pOwner);

	for(INT32 i = 0 ; i < MAX_FIRE_EFFECT ; ++i )
	{
		if(m_idxFireEffect[i] >= 0)
			_FireEffect( pDino->getFXMatrix(), m_idxFireEffect[i], true);
	}
}

CHARA_DEATH_TYPE WeaponDino::_GetDeathType(WEAPON::DAMAGE_TYPE damageType)
{
	switch(damageType)
	{
	case WEAPON::DAMAGE_BUFF:		return CHARA_DEATH_BUFF;
	case WEAPON::DAMAGE_TRAMPLE:	return CHARA_DEATH_TRAMPLED;
	case WEAPON::DAMAGE_HOWL:		return CHARA_DEATH_HOWL;
	}

	return CHARA_DEATH_B;
}

/*virtual*/ void WeaponDino::FireCollisionDino(VEC3D * pStartPos, VEC3D * pTargetPos)
{
	INT32 collGroup;

	collGroup = CGRP_MASKGROUP_ATTACK;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)	collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else												collGroup = CGRP_MASKGROUP_CHARA_RED;


	if(pStartPos == nullptr)
		((CGameCharaDino*)m_pOwner)->SetDinoFireStartPos(m_StartPos);
	else
		i3Vector::Copy(m_StartPos, pStartPos);

	if( !m_pOwner->isNetwork())
	{
		if( m_pOwner->IsSting() || m_pOwner->IsCCSting() || m_pOwner->IsStingMars() )	
			WeaponBase::GetShotTargetPos( m_TargetPos, m_StartPos);
		else
			GetShotTargetPos( m_TargetPos, m_StartPos);
	}
	else
	{
		i3Vector::Copy(m_TargetPos, pTargetPos);
	}

	NET_FireSync( m_TargetPos, ((CGameCharaDino*)m_pOwner)->GetFireOrder() );

	{
		i3CollideeLine col;

		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );

		if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_RANGE )
			FireCollision(m_StartPos, m_TargetPos);
		else
			_DoFireCollisionDino(&col, collGroup);
	}
}

/*virtual*/ void WeaponDino::GetShotTargetPos(VEC3D * pDest, VEC3D * pStart /* = nullptr */ )
{
	VEC3D at;
	MATRIX mtx;
	VEC3D pos;
	REAL32	rRange = 400.f;

	m_pOwner->getCameraContext()->getVirtualCamPosition( &pos, &mtx);
	i3Vector::Scale( &at, i3Matrix::GetAt( &mtx), - rRange);
	i3Vector::Add( pDest, &at, &pos);
}

void WeaponDino::ProcessAreaHitDino(void)
{
	CHARA_DAMAGEINFO DamageInfo;
	
	DamageInfo.SetWeaponItemID(this);

	DamageInfo._DeathType			= _GetDeathType(m_pWeaponInfo->GetTypeDamage());
	DamageInfo._DamageType			= m_pWeaponInfo->GetTypeDamage();
	DamageInfo._rExplosionRange		= m_pWeaponInfo->GetExplosion_Range();
	DamageInfo._rExplosionSlope		= m_pWeaponInfo->GetExplosion_EffectiveSlope();
	DamageInfo._rEffectiveTime		= m_pWeaponInfo->GetEffectiveTime();
	DamageInfo._IsOwner				= m_pOwner->isLocal();			
	DamageInfo._rDamage				= GetDamage(false);
	DamageInfo._pWallHitResult		= nullptr;
	DamageInfo._WallShotReduceRate	= 0;
	DamageInfo._HitType				= GHT_CHARA;
	DamageInfo.SetHitPart(GetHitRagdollPart() );
	DamageInfo._pWeapon				= this;
	DamageInfo._AttackType			= m_pWeaponInfo->GetAttackType(getFireOrder(), getAutoFireCount());

	ProcessDinoHit(&DamageInfo);
}

REAL32	WeaponDino::GetDinoAttackTime(INT32 i32FireCount)
{
	REAL32 rDamage, rOffset = 0.0f;
	
	rDamage = m_pWeaponInfo->GetAttackTimeA(i32FireCount);
	rOffset = m_pWeaponInfo->GetAttackFreezeTime();

	return rDamage + rOffset;
}

/*virtual*/ bool WeaponDino::DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale)
{
	INT32 autoFireCount = getAutoFireCount();

	if( autoFireCount >= m_pWeaponInfo->GetAutoFireCount() )
		return false;

	bool isAttack = false;

	if( rAnimTime > GetDinoAttackTime(autoFireCount) )
	{
		if( getOwner()->isLocal() || getOwner()->isAI() )
		{
			FireCollisionDino(nullptr, nullptr);
			//_PlayFireEffectDino();
		}

		_PlayFireEffectDino(); // 네크워크 공룡의 공격일 경우 컬리전 판정은 없고 이펙트만 출력한다.

		autoFireCount++;
		isAttack = true;
		m_pOwner->getAnimContext()->SetUpperTimeScale(rTimeScale);
	}

	setAutoFireCount(autoFireCount);

	return isAttack;
}