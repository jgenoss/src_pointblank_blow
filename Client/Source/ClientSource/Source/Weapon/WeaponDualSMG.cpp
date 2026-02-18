#include "pch.h"
#include "WeaponDualSMG.h"
#include "GameCharaBase.h"


#include "MainWeapon_DualSMG.h"

I3_CLASS_INSTANCE( WeaponDualSMG);//, WeaponDual);

MainWeapon*		WeaponDualSMG::CreateMainWeapon()
{
	MainWeapon_DualSMG * p = new MainWeapon_DualSMG(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualSMG));
	return p;
}

WeaponDualSMG::WeaponDualSMG()
{
}

WeaponDualSMG::~WeaponDualSMG()
{
}

/*virtual*/void WeaponDualSMG::Reset( void)
{
	WeaponBase::Reset();
}

/*virtual*/ void WeaponDualSMG::OnBindResource(void)
{
	WeaponBase::OnBindResource();
}

/*virtual*/bool WeaponDualSMG::Fire( WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualSMG* mainWeapon = static_cast<MainWeapon_DualSMG*>(GetMainWeapon());
	return mainWeapon->Fire_DualSMG(type);
}

/*virtual*/void WeaponDualSMG::LoadMagazine( void)
{
	WeaponBase::LoadMagazine();
}


/*virtual*/void	WeaponDualSMG::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();
}


/*virtual*/void WeaponDualSMG::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualSMG* mainWeapon = static_cast<MainWeapon_DualSMG*>(GetMainWeapon());
	mainWeapon->NET_Fire_DualSMG(pStartPos, pTargetPos, type);	
}

/*virtual*/void	WeaponDualSMG::OnPlayLoadMagAnimation( void)
{
//	WeaponDual::NET_LoadMagazineDualGun( m_pOwner->getViewType());
	MainWeapon_Dual* mainWeapon = static_cast<MainWeapon_Dual*>(GetMainWeapon());
	mainWeapon->NET_LoadMagazineDualGun( m_pOwner->getViewType() );
}

void WeaponDualSMG::OnPlayChangeAniImpl(REAL32 rTimeScale)
{
	if (m_pOwner != nullptr && g_pFramework->IsBattleStage())
	{
		if ((getAIIndex(WEAPON::AI_CHANGEDUAL1PV_RIGHT) != -1) && (getAIIndex(WEAPON::AI_CHANGEDUAL1PV_LEFT) != -1))
		{
			MainWeapon_Dual* mainWeapon = static_cast<MainWeapon_Dual*>(GetMainWeapon());
			mainWeapon->NET_ChangeDualGun(m_pOwner->getViewType(), rTimeScale);
		}
		else
		{
			// _BindAnimationIndex에 다른 교체 애니메이션 등록 후 이용해주세요..
			WeaponBase::OnPlayChangeAniImpl_Default(rTimeScale);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

/*virtual*/ bool WeaponDualSMG::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRv = WeaponBase::OnLoadMag( pCtx, tm);

	MainWeapon_DualSMG* mainWeapon = static_cast<MainWeapon_DualSMG*>(GetMainWeapon());
	mainWeapon->OnLoadMag_DualSMG(pCtx, tm, bRv);

	return bRv;
}

INT32 WeaponDualSMG::_OnProcessFireBulletCount(void)
{
	INT32 iBullet = getLoadedBulletCount();

	iBullet -= m_pWeaponInfo->GetFireDecBulletCount() * 2;

	if (iBullet <= 0)
		iBullet = 0;

	setLoadedBulletCount(iBullet);

	return iBullet;

}