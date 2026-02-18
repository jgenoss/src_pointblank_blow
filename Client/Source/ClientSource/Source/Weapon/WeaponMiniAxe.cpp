#include "pch.h"
#include "WeaponMiniAxe.h"
#include "GameCharaActionContext.h"

#include "MainWeapon_MiniAxe.h"




I3_CLASS_INSTANCE( WeaponMiniAxe);//, WeaponKnife);

WeaponMiniAxe::WeaponMiniAxe()
{
}

WeaponMiniAxe::~WeaponMiniAxe()
{
}

MainWeapon*	WeaponMiniAxe::CreateMainWeapon()
{
	MainWeapon_MiniAxe * p = new MainWeapon_MiniAxe(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_MiniAxe));
	return p;
}


/*virtual*/ void WeaponMiniAxe::OnBindResource(void)
{
	WeaponKnife::OnBindResource();
}

void WeaponMiniAxe::Reset( void)
{
	WeaponBase::Reset();
}

void WeaponMiniAxe::ReturnInstance( void)
{
	WeaponKnife::ReturnInstance();
}

void WeaponMiniAxe::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponKnife::OnUpdate( rDeltaSeconds);
}

void WeaponMiniAxe::OnSetBlendAlpha( REAL32 val)
{
	WeaponKnife::OnSetBlendAlpha( val);
}

void WeaponMiniAxe::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponKnife::OnSetMaterialColor( pfDiffuse, pfSpecular);
}

void WeaponMiniAxe::OnChangeFPSView( void)
{
	WeaponKnife::OnChangeFPSView();
}

void WeaponMiniAxe::OnChangeObserverView( void)
{
	WeaponKnife::OnChangeObserverView();
}

void WeaponMiniAxe::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponKnife::AttachNode( pSceneNode, pOwner);
}



void WeaponMiniAxe::DetachNode( void )
{
	WeaponKnife::DetachNode();
}

void WeaponMiniAxe::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->OnFire_MiniAxe(pCtx, tm);
}


/*virtual*/ void WeaponMiniAxe::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->OnSecondaryFire_MiniAxe(pCtx, tm);
}


bool WeaponMiniAxe::Fire( WEAPON::FIRE_ORDER type)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	return mainWeapon->Fire_MiniAxe(type);
}

void WeaponMiniAxe::FireEnd(void)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->FireEnd_MiniAxe();
}

void WeaponMiniAxe::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->FireCollision_MiniAxe(pStartPos, pTargetPos);
}

void WeaponMiniAxe::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->NET_Fire_MiniAxe(pStartPos, pTargetPos, type);	
}

/*virtual*/ bool WeaponMiniAxe::OnCheckExtensionPossible( bool bVal)
{
	if( isExtensionActivate() != bVal)	return false;
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	return mainWeapon->SetEnableMiniAxe( bVal);
}

/*
void WeaponMiniAxe::OnPlayChangeAniImpl( REAL32 rTimeScale //= 1.f)
{

}
*/

// 이 ReturnToReady 함수는 없애기 어려움 (기본클래스 함수 호출 유무가 분기됨)

/*virtual*/ void WeaponMiniAxe::ReturnToReady( void)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->ReturnToReady_MiniAxe();
}

void WeaponMiniAxe::SetOwner( CGameCharaBase * pOwner)
{
	WeaponBase::SetOwner( pOwner);
}

void WeaponMiniAxe::NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx)
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->NET_FirstThrow_MiniAxe(pStartPos, pDir, nNetSlotIdx);
}

void WeaponMiniAxe::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx )
{
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	mainWeapon->NET_Throw_MiniAxe(pStartPos, pTargetPos, nThrowNetIdx);
}

void WeaponMiniAxe::PlayAnim_UI_IdleB( void)
{
	if( getWeaponInfo()->GetRealNumber() == GetItemIndex( "KNIFE_MINIAXE" ) )
	{

	}
	else
	{
		WeaponBase::PlayAnim_UI_IdleB();
	}
}

void WeaponMiniAxe::_PlaySwapSound( void )
{		
	if( isExtensionActivate() == false)
	{
		WeaponKnife::_PlaySwapSound();
	}
	else
	{
		MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
		WeaponBase* pAltWeapon = mainWeapon->GetAltWeapon();
		pAltWeapon->PlaySwapSound();
	}
}

WeaponBase *	WeaponMiniAxe::getUsedWeapon( void)				
{ 
	if ( isExtensionActivate() == false) 
		return this;
	
	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	return	mainWeapon->GetAltWeapon(); 
}

CWeaponInfo *	WeaponMiniAxe::getRealWeaponInfo( void)			
{ 
	if ( isExtensionActivate() == false) 
		return m_pWeaponInfo;

	MainWeapon_MiniAxe* mainWeapon = static_cast<MainWeapon_MiniAxe*>(GetMainWeapon());
	return mainWeapon->GetAltWeapon()->getWeaponInfo(); 
}
