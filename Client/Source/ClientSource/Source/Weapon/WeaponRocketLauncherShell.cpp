#include "pch.h"
#include "WeaponRocketLauncherShell.h"
#include "GlobalVariables.h"
#include "StageBattle.h"

#include "WeaponGlobal.h"


#include "WeaponFunction.h"

#include "MainWeapon_RLShell.h"





I3_CLASS_INSTANCE( WeaponRocketLauncherShell);//, WeaponBase);

MainWeapon*	WeaponRocketLauncherShell::CreateMainWeapon() 
{
	MainWeapon_RLShell * p = new MainWeapon_RLShell(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_RLShell));
	return p;
}

WeaponRocketLauncherShell::WeaponRocketLauncherShell(void)
{

}

WeaponRocketLauncherShell::~WeaponRocketLauncherShell(void)
{
}

void WeaponRocketLauncherShell::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}


void WeaponRocketLauncherShell::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.
}



void WeaponRocketLauncherShell::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);


}

void WeaponRocketLauncherShell::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_LOADBULLET_DISABLE);

	if( getLoadedBulletCount() <= 0)
	{
		setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);
		if( getOwner() != nullptr && !getOwner()->isNetwork() )
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		SetEnable( false);
	}
	else
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE);
		if( getOwner() != nullptr && !getOwner()->isNetwork() )
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		setWeaponState( WEAPON::STATE_READY);
	}
}


void WeaponRocketLauncherShell::OnUpdate( REAL32 rDeltaSeconds)
{
	MainWeapon_RLShell* mainWeapon = static_cast<MainWeapon_RLShell*>(GetMainWeapon());

	mainWeapon->OnUpdate_RLShell(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}

//void WeaponRocketLauncherShell::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//	// 시점이 향하고 있는 목표 지점을 구한다.
//	NET_Fire( nullptr, nullptr );
//}

void WeaponRocketLauncherShell::FireEnd(void)
{	
}

void WeaponRocketLauncherShell::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	setWeaponState(  WEAPON::STATE_FIRE);
	WeaponBase::NET_Fire( nullptr, nullptr, type );
}

void WeaponRocketLauncherShell::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	WeaponBase::NET_Bound( pStartPos, pDir, fSpeed, nBoundType );

	MainWeapon_RLShell* mainWeapon = static_cast<MainWeapon_RLShell*>(GetMainWeapon());
	mainWeapon->NET_Bound_RLShell(pStartPos, pDir, fSpeed, nBoundType);
}
