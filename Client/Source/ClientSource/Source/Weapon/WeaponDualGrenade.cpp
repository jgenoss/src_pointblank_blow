#include "pch.h"
#include "WeaponDualGrenade.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaActionContext.h"

#include "MainWeapon_DualGrenade.h"

I3_CLASS_INSTANCE( WeaponDualGrenade);//, WeaponGrenade);


WeaponDualGrenade::WeaponDualGrenade()
: WeaponGrenade()
{
	
}


WeaponDualGrenade::~WeaponDualGrenade()
{

}

MainWeapon*	WeaponDualGrenade::CreateMainWeapon()
{
	MainWeapon_DualGrenade * p = new MainWeapon_DualGrenade(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualGrenade));
	return p;
}


/*virtual*/ void WeaponDualGrenade::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponGrenade::Create( pOwner, Itemid, bCreateInWeapon); 
}

/*virtual*/ void	WeaponDualGrenade::OnBindResource(void)
{
	//WeaponGrenade::OnBindResource(); 
	i3Chara::OnBindResource();	// 그냥 쓰레기 스럽다..

	m_pAICtx = this->FindAI( "Default");

	if( m_pAICtx == nullptr)
		return;
	
	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());
	mainWeapon->OnBindResource_DualGrenade();
}

/*virtual*/ void WeaponDualGrenade::SetOwner( CGameCharaBase * pOwner)
{
	WeaponBase::SetOwner( pOwner);

	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());
	mainWeapon->SetOwner_LeftGrenade(pOwner);
}

/*virtual*/ void WeaponDualGrenade::ReturnInstance()
{
	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());
	mainWeapon->ReturnInstance_LeftGrenade();

	WeaponGrenade::ReturnInstance();
}

/*virtual*/ void WeaponDualGrenade::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponGrenade::OnUpdate( rDeltaSeconds);	

}
 
/*virtual*/ void WeaponDualGrenade::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenade::OnFire( pCtx, tm);	
}

/*virtual*/ void WeaponDualGrenade::Reset(void)
{
	WeaponGrenade::Reset();
	
}

/*virtual*/ void WeaponDualGrenade::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();

}

bool WeaponDualGrenade::getThrowFirstGrenade()
{
	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());

	return mainWeapon->getThrowFirstGrenade();
}


/*virtual*/ bool WeaponDualGrenade::Fire( WEAPON::FIRE_ORDER type)
{
	bool re = WeaponGrenade::Fire( type);

	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());

	mainWeapon->PlayFireAnim();

	return re;
}

/*virtual*/ void WeaponDualGrenade::OnEndFire( i3AIContext * pCtx, REAL32 tm )
{
	WeaponGrenade::OnEndFire( pCtx, tm);
}

/*virtual*/ void WeaponDualGrenade::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponBase::AttachNode( pSceneNode, pOwner);
}

/*virtual*/ void WeaponDualGrenade::DetachNode( void )
{
	WeaponBase::DetachNode();
}

/*virtual*/ void WeaponDualGrenade::OnSetBlendAlpha( REAL32 val)
{
	WeaponBase::OnSetBlendAlpha( val);
}

/*virtual*/ void WeaponDualGrenade::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase::OnSetMaterialColor( pfDiffuse, pfSpecular);
}

/*virtual*/ void WeaponDualGrenade::_OnPlayWeaponIdleAnim(void)
{
	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());

	mainWeapon->PlayIdleAnim();
}

/*virtual*/ void WeaponDualGrenade::_OnPlayWeaponFireAnim( void )
{
	MainWeapon_DualGrenade* mainWeapon = static_cast<MainWeapon_DualGrenade*>(GetMainWeapon());

	mainWeapon->PlayFireAnim();
}


