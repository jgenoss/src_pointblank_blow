#include "pch.h"
#include "MainWeapon_DualCIC.h"


MainWeapon_DualCIC::MainWeapon_DualCIC(WeaponBase* p) : MainWeapon_Dual(p)
{
	m_nCurrentUseWeapon = WEAPON_CIC_CURRENTUSE_RIGHT;	
}
	
bool	MainWeapon_DualCIC::Fire_DualCIC( WEAPON::FIRE_ORDER type )
{
	m_pWeaponBase->SetFireOrder(type);

	if( type == WEAPON::PRIMARY_FIRE)
	{
        if( m_nCurrentUseWeapon == WEAPON_CIC_CURRENTUSE_LEFT ) return false;

        if( m_pWeaponBase->getLoadedBulletCount() <= 0)	return false;
		// 시점이 향하고 있는 목표 지점을 구한다.
		m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos() );
		m_pWeaponBase->NET_Fire( nullptr, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->getFireOrder() );

		if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
			( (CGameCamera_FPS*)g_pCamera )->SetNeckAnim(false);
	}
	else
	{
		if( m_pWeaponBase->getOwner()->isLocal() )
		{
			m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
			m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
		}

		m_pLeftHandWeapon->Fire(WEAPON::PRIMARY_FIRE);
		m_pWeaponBase->addWeaponStateMask( WSM_NEED_SEND_FIREACTION);
		m_nCurrentUseWeapon = WEAPON_CIC_CURRENTUSE_LEFT;
	}

	return true;
}

void	MainWeapon_DualCIC::LoadMagazine_DualCIC( )
{
	if(	 m_pWeaponBase->getOwner() != nullptr && ( m_pWeaponBase->getOwner()->isLocal() ||  m_pWeaponBase->getOwner()->isAI()))
	{
		if(  m_pWeaponBase->getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			m_pWeaponBase->addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	m_pWeaponBase->OnPlayLoadMagAnimation();
	m_pLeftHandWeapon->OnPlayLoadMagAnimation();
}

void	MainWeapon_DualCIC::NET_Fire_DualCIC( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	if( type == WEAPON::PRIMARY_FIRE)
		m_pWeaponBase->WeaponBase::NET_Fire( pStartPos, pTargetPos, type);					// 한정자를 떼면 안된다...(비가상처리)
	else
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, WEAPON::PRIMARY_FIRE);
}

void	MainWeapon_DualCIC::ReturnToReady()
{
	MainWeapon_Dual::ReturnToReady();
	
	m_nCurrentUseWeapon = WEAPON_CIC_CURRENTUSE_RIGHT;
}

void	MainWeapon_DualCIC::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	m_nCurrentUseWeapon = WEAPON_CIC_CURRENTUSE_RIGHT;
}

void	MainWeapon_DualCIC::OnUpdate_DualCIC( REAL32 rDeltaSeconds)
{
	//if( m_pLeftHandWeapon->getWeaponState() == WEAPON::STATE_READY)
	//	m_nCurrentUseWeapon = WEAPON_CIC_CURRENTUSE_RIGHT;
}
