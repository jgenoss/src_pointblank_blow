#if !defined( _WEAPON_KRISSSUPERV_H__)
#define _WEAPON_KRISSSUPERV_H__

#include "WeaponBase.h"

class WeaponKrissSuperV :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponKrissSuperV, WeaponBase);

public:
	bool			m_bUseDualWeapon;
	WeaponBase *	m_pDualWeapon;

public:
	bool			isUsedDualWeapon( void)				{ return m_bUseDualWeapon; }
	WeaponBase *	getDualWeapon( void)				{ return m_pDualWeapon; }

	virtual INT32	getLoadedBulletDualCount( void) override { if( m_pDualWeapon != nullptr) return m_pDualWeapon->getLoadedBulletCount(); return 0;}
	virtual void	setLoadedBulletDualCount( INT32 nCount ) override { if( m_pDualWeapon != nullptr) m_pDualWeapon->setLoadedBulletCount( nCount); }

	virtual WeaponBase * getUsedWeapon( void) override { if( m_bUseDualWeapon) return m_pDualWeapon; return this;}

protected:
	bool			_SwitchingWeapon( WeaponBase * pPrevWeapon, WeaponBase * pNextWeapon, bool bPlaySound = false );			

public:
	WeaponKrissSuperV(void);
	virtual ~WeaponKrissSuperV(void);

	virtual void	PostCreate( void) override;
	virtual void	Reset( void) override;
	
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

	virtual void	ReturnInstance( void) override;

	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;

	virtual void	ReturnToReady( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd(void) override;

	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;
	virtual void	OnPlayChangeSound( void) override;
	virtual bool	OnCheckExtensionPossible( bool bVal) override;
	virtual void	OnProcessExtension( bool bVal) override;
	virtual void	OnPlayExtensionAnimation(bool bVal, bool bAnimPlay = true) override;

	bool			SetEnableDualWeapon( bool bUse, bool bPlaySound = false);

	virtual void	OnDrop( void) override;
	virtual void	SetOwner( CGameCharaBase * pOwner) override;

	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos) override;

	virtual void	SetBulletCountForNetwork( INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount) override;

	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;

	void PlayIdleAnim();
	void PlayIdleFoldedAnim();
};

#endif // _WEAPON_DOTLASER_H__