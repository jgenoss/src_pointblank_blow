#include "pch.h"
#include "MainWeapon_DualSMG.h"

#include "GameCharaWeaponContext.h"

MainWeapon_DualSMG::MainWeapon_DualSMG(WeaponBase* p) : MainWeapon_Dual(p)
{
	
}

void	MainWeapon_DualSMG::Reset()
{
	MainWeapon_Dual::Reset();

	if( m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->Reset();
		m_pLeftHandWeapon->setLoadedBulletCount( m_pWeaponBase->getLoadedBulletCount() / 2);
	}
}

bool	MainWeapon_DualSMG::Fire_DualSMG( WEAPON::FIRE_ORDER type )
{
	// righthand
	if( m_pWeaponBase->getLoadedBulletCount() <= 0)	
		return false;

	m_pWeaponBase->SetFireOrder(type);

	// 시점이 향하고 있는 목표 지점을 구한다.
	m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos() );
	m_pWeaponBase->NET_Fire( nullptr, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->getFireOrder() );

	// lefthand
	{
		if( m_pWeaponBase->getOwner()->isLocal() )
		{
			m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
			m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
		}

		if( m_pLeftHandWeapon)
			m_pLeftHandWeapon->Fire(WEAPON::PRIMARY_FIRE);

	}

	return true;
}

void	MainWeapon_DualSMG::LoadMagazine( void)
{
	MainWeapon_Dual::LoadMagazine();

	if( m_pLeftHandWeapon)
		m_pLeftHandWeapon->removeWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
}

void	MainWeapon_DualSMG::ReturnToReady()
{
	MainWeapon_Dual::ReturnToReady();

//	if( m_pOwner == nullptr)
//		return;
	if( !i3::same_of<CGameCharaBase*>(m_pWeaponBase->getOwner() ))
		return;

	if( m_pWeaponBase->getOwner()->GetCurrentCharaWeapon() == m_pWeaponBase->getPairWeapon())
	{
		WeaponBase * pWeapon = m_pWeaponBase->getPairWeapon();

		// Kriss S.V. 일 경우
		if(  i3::same_of<WeaponKrissSuperV*>(pWeapon))
		{
			if( ((WeaponKrissSuperV*) pWeapon)->isUsedDualWeapon() )
			{
				m_pWeaponBase->getOwner()->Cmd_Extension( false);
			}
		}
	}	

}

void	MainWeapon_DualSMG::NET_Fire_DualSMG( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	m_pWeaponBase->WeaponBase::NET_Fire( pStartPos, pTargetPos, type);				// 한정자 떼면 안됨..

	if( m_pLeftHandWeapon)
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, type);
}

void	MainWeapon_DualSMG::OnLoadMag_DualSMG(i3AIContext * pCtx, REAL32 tm, bool bDoneLoadMag )
{
	if (bDoneLoadMag)
	{
		if (m_pLeftHandWeapon != nullptr)
		{
			m_pLeftHandWeapon->setLoadedBulletCount(m_pWeaponBase->getLoadedBulletCount() / 2);
			m_pLeftHandWeapon->_OnPlayWeaponIdleAnim();
		}
		m_pWeaponBase->_OnPlayWeaponIdleAnim();
	}
}