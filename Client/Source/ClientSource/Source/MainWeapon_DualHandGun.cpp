#include "pch.h"
#include "MainWeapon_DualHandGun.h"	


MainWeapon_DualHandGun::MainWeapon_DualHandGun(WeaponBase* p) : MainWeapon_Dual(p)
{
	m_nNextUseFireWeapon	= 0;
}
	
void	MainWeapon_DualHandGun::Reset()
{
	MainWeapon_Dual::Reset();

	if (m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->Reset();
		m_pLeftHandWeapon->setLoadedBulletCount( m_pWeaponBase->getLoadedBulletCount() / 2);
	}
}

bool	MainWeapon_DualHandGun::Fire_DualHandGun( WEAPON::FIRE_ORDER type )
{
		
	if( m_nNextUseFireWeapon == 0)
	{
		if( m_pWeaponBase->getLoadedBulletCount() <= 0)	return false;

		//	m_FireOrder = type;
		m_pWeaponBase->SetFireOrder(type);

		// 시점이 향하고 있는 목표 지점을 구한다.
		m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos() );
		m_pWeaponBase->NET_Fire( nullptr, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->getFireOrder() );
	}
	else
	{
		if( m_pWeaponBase->getOwner()->isLocal() )
		{
			m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
			m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
		}

		m_pLeftHandWeapon->Fire(WEAPON::PRIMARY_FIRE);
		m_nNextUseFireWeapon = 0;

		INT32 iBullet = m_pWeaponBase->getLoadedBulletCount();

		iBullet--;
		m_pWeaponBase->setLoadedBulletCount( iBullet);
	}

	return true;
}

void	MainWeapon_DualHandGun::LoadMagazine( void)
{
	MainWeapon_Dual::LoadMagazine();

	if( m_pLeftHandWeapon != nullptr)
	{
		m_pLeftHandWeapon->removeWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
		m_pLeftHandWeapon->LoadMagazine();
	}

	m_nNextUseFireWeapon = 0;
}

void	MainWeapon_DualHandGun::ReturnToReady( void)
{
	MainWeapon_Dual::ReturnToReady();

	m_nNextUseFireWeapon = 0;
}

void	MainWeapon_DualHandGun::NET_Fire_DualHandGun( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	if( m_nNextUseFireWeapon == 0)
	{
		m_pWeaponBase->WeaponBase::NET_Fire( pStartPos, pTargetPos, type);			// 한정자 떼지말것..
		m_nNextUseFireWeapon = 1;
	}
	else
	{
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, type);
		m_nNextUseFireWeapon = 0;
	}
}

void	MainWeapon_DualHandGun::OnPlayLoadMagAnimation_DualHandGun()
{
	MainWeapon_Dual::NET_LoadMagazineDualGun( m_pWeaponBase->getOwner()->getViewType());
}

void	MainWeapon_DualHandGun::OnLoadMag_DualHandGun(i3AIContext * pCtx, REAL32 tm, bool bDoneLoadMag)
{
	if( bDoneLoadMag )
	{
		m_pLeftHandWeapon->setLoadedBulletCount(  m_pWeaponBase->getLoadedBulletCount() / 2);
	}

}

