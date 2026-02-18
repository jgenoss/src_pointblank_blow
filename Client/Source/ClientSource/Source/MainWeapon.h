#pragma once

#include "i3Base/smart_ptr/scoped_ptr.h"

enum WEAPON_KNIFE_ATTACK_TYPE;

class MainWeapon : public i3::class_common_pool<MainWeapon>
{
public:

	MainWeapon(WeaponBase* pWeaponBase) : m_pWeaponBase(pWeaponBase) {}
	virtual ~MainWeapon() {}

	virtual void		OnCreate( bool bCreateInWeapon) {}
	virtual void		OnInitVariable() {}
	virtual void		Reset() {}
	virtual void		ReturnInstance() {}

	virtual void		ReturnToReady() {}
	virtual void		OnBindResource() {}

	virtual void		OnPlayChangeAni( REAL32 rTimeScale);
//	virtual void		OnFire( i3AIContext * pCtx, REAL32 tm) {}
	virtual void		OnEndFire( i3AIContext * pCtx, REAL32 tm) {}

	virtual void		AttachNode( i3Node * pSceneNode, i3GameNode * pOwner ) {}
	virtual void		DetachNode() {} 

	virtual void		OnSetBlendAlpha( REAL32 val) {}
	virtual void		OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) {}

	virtual void		OnChangeFPSView( void) {}
	virtual void		OnChangeObserverView( void) {}


	virtual void		SetKnifeAttackType( WEAPON_KNIFE_ATTACK_TYPE type) {}		// 우선 여기에 둔다... 

	virtual void		FireReady() {}

	virtual void		SetOwner( CGameCharaBase * pOwner) {}
	virtual void		SetEnable( bool bTrue = true, REAL32 tm = 0.f) {} 
	
	virtual void		OnEndLoadMag( i3AIContext * pCtx, REAL32 tm) {}

	virtual void		LoadMagazine( void) {}

	virtual bool		IsWeaponDualDerivedClass() const { return false; }

protected:
	WeaponBase*			m_pWeaponBase;		// WeaponBase내부에 MainWeapon이 존재... (현재 상호참조임)
};

// 
typedef i3::scoped_ptr<MainWeapon>		MainWeaponPtr;
//

class MainWeapon_Melee : public MainWeapon
{
public:
	MainWeapon_Melee(WeaponBase* p) : MainWeapon(p) {}
};
