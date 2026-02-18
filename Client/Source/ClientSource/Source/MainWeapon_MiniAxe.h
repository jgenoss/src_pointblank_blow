#pragma once

#include "MainWeapon_Knife.h"

//
//

class MainWeapon_MiniAxe : public MainWeapon_Knife
{
	friend class MainWeapon_FryingPan;

public:
	MainWeapon_MiniAxe(WeaponBase* p);
	WeaponBase*			GetAltWeapon() const { return m_pWeaponKnife; }			// K-7이라고 한다.. (도끼던지고 난 다음)..Alt는 Alternative의 준말이다..

	void			OnFire_MiniAxe( i3AIContext * pCtx, REAL32 tm);
	void			OnSecondaryFire_MiniAxe( i3AIContext * pCtx, REAL32 tm);

	bool			Fire_MiniAxe( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void			FireEnd_MiniAxe(void);
	void			FireCollision_MiniAxe( VEC3D * pStartPos, VEC3D * pTargetPos);
	void			NET_Fire_MiniAxe( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE );

	void			ReturnToReady_MiniAxe( void);

	bool			SetEnableMiniAxe( bool bUse);
	void			SetEnableBaseWeapon(bool bEnable);

	void			NET_FirstThrow_MiniAxe( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx);
	void			NET_Throw_MiniAxe( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx );

private:

	virtual void		OnCreate(bool bCreateInWeapon) override;
	virtual void		Reset() override;
	virtual void		ReturnInstance() override;
	virtual void		OnBindResource() override;

	virtual void		OnSetBlendAlpha( REAL32 val) override;
	virtual void		OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;
	virtual void		OnChangeFPSView( void) override;
	virtual void		OnChangeObserverView( void) override;

	virtual void		AttachNode( i3Node * pSceneNode, i3GameNode * pOwner ) override;
	virtual void		DetachNode() override;

	virtual void		OnPlayChangeAni( REAL32 rTimeScale) override;
	virtual void		SetOwner( CGameCharaBase * pOwner) override;

	virtual void		SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;


	void				_Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx);

private:
	WeaponBase *		m_pWeaponKnife;   /*WeaponKnife*/

	bool				m_bCheckThrow;	// 던져졌는지 체크

	INT32				m_iAttackSoundPlayCount;


	INT8			m_aiiProfileB_Rebel_Male;
	INT8			m_aiiProfileB_Rebel_Female;

	INT8			m_aiiProfileB_Rebel_Male2;
	INT8			m_aiiProfileB_Rebel_Female2;

	T_ItemID			m_ThrowItemID;

};

