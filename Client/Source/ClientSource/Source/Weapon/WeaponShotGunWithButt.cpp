#include "pch.h"
#include "WeaponShotGunWithButt.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"

#include "WeaponFunction.h"

I3_CLASS_INSTANCE(WeaponShotGunWithButt);

WeaponShotGunWithButt::WeaponShotGunWithButt() : WeaponShotGun(), m_aii_SwingButt(-1)
{

}

WeaponShotGunWithButt::~WeaponShotGunWithButt()
{

}

void WeaponShotGunWithButt::PostCreate( void)
{
	char buttSoundName[MAX_PATH] = {0,};
	sprintf_s(buttSoundName, MAX_PATH, "WM_Zombie_Slayer_ATTACK2.wav");
	i3GameResSound* res = (i3GameResSound*)g_pFramework->FindResourceByOnlyName(buttSoundName);
	if (res)
	{
		I3_SAFE_RELEASE(m_pFireSoundPlayInfo[WEAPON::FIRE_SOUND_NORMAL_2]);
		i3SoundPlayInfo* info = res->getSound();
		m_pFireSoundPlayInfo[WEAPON::FIRE_SOUND_NORMAL_2] = i3SoundPlayInfo::new_object();
		info->CopyTo(m_pFireSoundPlayInfo[WEAPON::FIRE_SOUND_NORMAL_2], I3_COPY_INSTANCE);
	}
}

void WeaponShotGunWithButt::OnBindResource()
{
	WeaponShotGun::OnBindResource();

	setAIIndex( WEAPON::AI_SECFIRE, (INT8) m_pAICtx->FindAIStateByName( "SwingButt")) ;

	if( getAIIndex( WEAPON::AI_SECFIRE) == -1)
		setAIIndex( WEAPON::AI_SECFIRE, getAIIndex( WEAPON::AI_FIRE));
}

void WeaponShotGunWithButt::_OnInitVairable()
{
	WeaponShotGun::_OnInitVairable();
}
//
bool WeaponShotGunWithButt::Fire( WEAPON::FIRE_ORDER type)
{

	if( (type == WEAPON::PRIMARY_FIRE) && (getLoadedBulletCount() <= 0))
		return false;

	m_FireOrder = type;

	if( m_FireOrder == WEAPON::SECONDARY_FIRE)
	{
		addWeaponStateMask( WSM_NEED_SEND_FIREACTION);
	}

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

	NET_Fire( nullptr, m_TargetPos, m_FireOrder );

	return true;
}

void WeaponShotGunWithButt::FireEnd( void)
{
	removeWeaponStateMask( WSM_FIRING | WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);

	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
	{
		if(getWeaponInfo()->GetPumpAction() && !isWeaponStateMask( WSM_LOADED_BULLET))
		{
			addWeaponStateMask( WSM_FIRE_DISABLE);
		}

		setAutoFireCount( 0);
	}
}


void WeaponShotGunWithButt::MeleeCollision(  VEC3D * pStartPos, VEC3D * pTargetPos)
{
	INT32 collGroup;

	if( m_pOwner == nullptr )		return;
	
	collGroup = CGRP_MASKGROUP_ATTACK;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)	collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else												collGroup = CGRP_MASKGROUP_CHARA_RED;

	// 네트워크 싱크를 맞추기 위해 강제로 보조 공격으로 세팅합니다. by 정순구 2009.04.23
	m_FireOrder = WEAPON::SECONDARY_FIRE;
	
	if( pStartPos == nullptr)
	{
		if( m_pOwner->is1PV())
		{
			// 1인칭에서는 현재는 Camera의 지점
			m_pOwner->getCameraContext()->getVirtualCamPosition( m_StartPos );
		}
		else
		{
			//	3인칭에서는 캐릭터 좌표(정확하게는 캐릭터의 IK Bone)의 위치를 시작지점으로 카메라 방향으로
			// 스타트 포지션
			i3Vector::Sub( m_StartPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()), i3Matrix::GetPos( m_pOwner->getBoneContext()->getIKMatrix()));
			i3Vector::Scale( m_StartPos, m_StartPos, 0.5f);
			i3Vector::Sub( m_StartPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()), m_StartPos);
		}
	}
	else
	{
		i3Vector::Copy( m_StartPos, pStartPos);
	}

	if( m_pOwner->isLocal() || m_pOwner->isAI() )
	{
		GetShotTargetPos( m_TargetPos, m_StartPos);
	}
	else
	{
		i3Vector::Copy( m_TargetPos, pTargetPos );
	}

	NET_FireSync( m_TargetPos, getFireOrder() );
	
	/*GlobalFunc::PB_TRACE("m_StartPos %f %f %f m_TargetPos %f %f %f GetPos %f %f %f", m_StartPos.x, m_StartPos.y, m_StartPos.z, m_TargetPos.x, m_TargetPos.y, m_TargetPos.z,
												m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z);*/
	//	collision line
	{	
		i3CollideeLine col;

		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );

		bool	bWarningOwner = false;

		_DoFireCollisionKnife( &col, collGroup, &bWarningOwner);
		
	}
}



void WeaponShotGunWithButt::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun::OnIdle( pCtx, tm);

	removeWeaponStateMask( WSM_READY_TO_LOADMAG);
	m_FireOrder = WEAPON::PRIMARY_FIRE;
}



void WeaponShotGunWithButt::OnSecondaryFire(i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner == nullptr)
	{
		ReturnToReady();
		return;
	}

	setWeaponState( WEAPON::STATE_FIRE);

	// 개머리판 공격중에는 장전 안됨.
	if( getFireOrder() == WEAPON::SECONDARY_FIRE)
	{
		addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);		
	}

	REAL32 fAttackDelay = m_pOwner->getAnimContext()->getUpperActualDuration();

	INT32 autoFireCount = getAutoFireCount();

	//	공격 시작부터 타격 시간이 되면 조준점까지 한번 컬리전 한다.
	if( autoFireCount == 0 && pCtx->getLocalTime() > m_pWeaponInfo->GetExtAttackTime() )
	{
		if( m_pOwner->isLocal())
		{
			MeleeCollision( nullptr, nullptr );
			//_OnPlayFireSound();
			autoFireCount = 0;
		}

		autoFireCount++;
	}


	setAutoFireCount( autoFireCount);

	// 공격 지연시간이 충분히 지났다면...Ready 상태로
	if( pCtx->getLocalTime() + tm > fAttackDelay )
	{
		// 개머리판 공격중에는 장전 안됨.
		if( getFireOrder() == WEAPON::SECONDARY_FIRE)
		{
			removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

		ReturnToReady();

		m_FireOrder = WEAPON::PRIMARY_FIRE;
	}
}

void WeaponShotGunWithButt::_OnPlayWeaponSecondFireAnim()
{
	if (m_pOwner)
	{
		INT8 ai_idx = getAIIndex( WEAPON::AI_SECFIRE);

		i3AIState * pAIState = m_pAICtx->getAIState( ai_idx );
		I3ASSERT( pAIState != nullptr);

		REAL32 rDelay = 1.f;
		REAL32 rTimeScale = pAIState->getDuration() / rDelay;

		WF::PlayWeaponAnim(this, ai_idx, rTimeScale);
	}
}

void WeaponShotGunWithButt::_OnPlayTerrainHitSound(WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	if( fireOrder == WEAPON::PRIMARY_FIRE)
		WeaponBase::_OnPlayTerrainHitSound( fireOrder, type, pPos);
	else
	{	// melee attack hit sound
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_M1887_HIT, pPos, false);
	}
}

void WeaponShotGunWithButt::_OnAddTerrainEffect(WEAPON_TERRAIN_EFFECT_INFO* pInfo)
{
	I3ASSERT( pInfo->_pPos != nullptr);
	I3ASSERT( pInfo->_pNormal != nullptr);
	I3ASSERT( pInfo->_pDir != nullptr);

	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
	{
		WeaponBase::_OnAddTerrainEffect( pInfo);
	}
	else
	{
		g_pEffectManager->AddBlowEffect( pInfo->_terrainType, WEAPON_CLASS_KNIFE,
			static_cast<BULLET_EFFECT_TYPE>(getWeaponInfo()->GetLoadBulletType()),
			pInfo->_pPos, pInfo->_pNormal);
	}
}

REAL32 WeaponShotGunWithButt::GetMoveSpeed(WEAPON_MOVE_SPEED_TYPE type)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetMoveSpeed( type);

	if( type == WEAPON_MOVE_SPEED_FIRE)
		type = WEAPON_MOVE_SPEED_NORMAL;

	return WeaponBase::GetMoveSpeed( type);
}

REAL32 WeaponShotGunWithButt::GetRange()
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetRange();

	return  m_pWeaponInfo->GetExtRange();
}

REAL32 WeaponShotGunWithButt::GetDPD_Rate()	
{	// M1887 의 개머리판 공격에 DPD_Rate 값이 적용되어 CalcDamage 양이 극도로 적어지는 
	// 버그를 방지하기 위해 Ext(개머리판 공격) DPD Rate를 적용한다.
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDPD_Rate();

	return  m_pWeaponInfo->GetExt_DPD_Rate();
}

REAL32 WeaponShotGunWithButt::GetDamage(bool bRandomDamage)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDamage( bRandomDamage);

	return m_pWeaponInfo->GetExtDamage();
}
