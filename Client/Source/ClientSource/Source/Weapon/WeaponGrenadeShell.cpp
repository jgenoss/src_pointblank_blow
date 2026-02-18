#include "pch.h"
#include "WeaponGrenadeShell.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaBoneContext.h"
#include "WeaponGlobal.h"
#include "WeaponFunction.h"
#include "MainWeapon_GrenadeShell.h"


I3_CLASS_INSTANCE( WeaponGrenadeShell);//, WeaponBase);

WeaponGrenadeShell::WeaponGrenadeShell(void)
{

}

WeaponGrenadeShell::~WeaponGrenadeShell(void)
{
}

MainWeapon*	WeaponGrenadeShell::CreateMainWeapon()
{
	MainWeapon_GrenadeShell * p = new MainWeapon_GrenadeShell(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_GrenadeShell));
	return p;
}


void WeaponGrenadeShell::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}


void WeaponGrenadeShell::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.
}

void WeaponGrenadeShell::SetGrenadeOwner( CGameCharaBase * pOwner, CWeaponInfo * pOwnerWeaponInfo)
{
	SetOwner( pOwner);	// Owner 설정.

	m_pWeaponInfo = pOwnerWeaponInfo;
	m_pAbsoluteWeaponInfo = pOwnerWeaponInfo;		//	처음 초기화때만 셋팅되는 메인 정보이다.
}



void WeaponGrenadeShell::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);

//	SetEnable( true);
//	_OnPlayWeaponIdleAnim();
//	removeWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
//	m_bCheckThrow = false;
}

void WeaponGrenadeShell::OnIdle( i3AIContext * pCtx, REAL32 tm)
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


 
void WeaponGrenadeShell::OnUpdate( REAL32 rDeltaSeconds)
{
	MainWeapon_GrenadeShell* mainWeapon = static_cast<MainWeapon_GrenadeShell*>(GetMainWeapon());
	
	mainWeapon->OnUpdate_GrenadeShell(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}

//void WeaponGrenadeShell::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//	// 시점이 향하고 있는 목표 지점을 구한다.
//	NET_Fire( nullptr, nullptr );
//}


void WeaponGrenadeShell::FireEnd(void)
{	
}

void WeaponGrenadeShell::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	setWeaponState( WEAPON::STATE_FIRE);

	WeaponBase::NET_Fire( nullptr, nullptr, type );
}

//void WeaponGrenadeShell::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos )
//{
//}

void WeaponGrenadeShell::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	WeaponBase::NET_Bound( pStartPos, pDir, fSpeed, nBoundType );

	MainWeapon_GrenadeShell* mainWeapon = static_cast<MainWeapon_GrenadeShell*>(GetMainWeapon());
	
	mainWeapon->NET_Bound_GrenadeShell(pStartPos, pDir, fSpeed, nBoundType);
}



void WeaponGrenadeShell::ReturnInstance()
{
	WeaponBase::ReturnInstance();
}
