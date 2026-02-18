#pragma once

// Dual에 대해서는 익스텐션 속성으로 분리하려고 했으나, 기존 Dual파생 클래스들의 WeaponBase 가상함수 결합도가 극히 심해서
// 일단은, MainWeapon의 파생 카테고리로 먼저 작업한다..

// WeaponDualCIC , WeaponDualHandGun , WeaponDualKnife , WeaponDualSMG 가 원 WeaponDual의 직접 파생클래스이고

// WeaponCrossFire는 WeaponDualHandGun의 파생.
// WeaponKnuckle은 WeaponDualKnife의 파생.
// WeaponDualUzi는 WeaponDualSMG의 파생.

//
// WeaponKrissSuperV의 경우, 무기자체는 싱글인데....내부에서 멤버 웨폰이 듀얼로 존재하는 케이스임....
// 따라서, 수정하려면, WeaponKrissSuperV도 함께 포함해서 생각하고, WeaponKrissSuperV의 구현 대부분 역시 제거해야한다.
//

#include "MainWeapon.h"

class MainWeapon_Dual : public MainWeapon 
{
public:
	WeaponBase *	getLeftWeapon( void)					{ return m_pLeftHandWeapon; }
	void			setLeftWeapon_Null()					{ m_pLeftHandWeapon = nullptr; }			// 임시로 작성됨..(나중에 다시 리팩토링할것)
protected:
	MainWeapon_Dual(WeaponBase* p);
	virtual void	ReturnToReady() override;
	virtual void	OnCreate( bool bCreateInWeapon) override;							// 파생클래스로만 생성될수 있게 조정..
private:

	virtual void	ReturnInstance() override;
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;


	virtual void	SetOwner( CGameCharaBase * pOwner) override;

	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;
	
	// 호출되지 않습니다. 2017.03.16. soon9
	/*virtual void	PlayAnim_UI_IdleA( void);
	virtual void	PlayAnim_UI_IdleB( void);*/

	virtual bool	IsWeaponDualDerivedClass() const override { return true; }

public:
	void	NET_LoadMagazineDualGun( CHARA_VIEW_TYPE eCAT);
	void	NET_ChangeDualGun( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale );

protected:
	WeaponBase*		m_pLeftHandWeapon;
};
