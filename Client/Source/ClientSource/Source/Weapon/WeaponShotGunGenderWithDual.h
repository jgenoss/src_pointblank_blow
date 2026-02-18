#if !defined( __WEAPONSHOTGUNGENDERWITHDUAL_H__)
#define __WEAPONSHOTGUNGENDERWITHDUAL_H__

#include "WeaponShotGun.h"

namespace 
{
	enum { BULLET_COUNT = 2,	LOD_COUNT = 3 };
	enum { GENDER_COUNT = 2,	AI_COUNT = 13 } ;
};

class WeaponShotGunGenderWithDual : public WeaponShotGun
{
	I3_CLASS_DEFINE( WeaponShotGunGenderWithDual, WeaponShotGun);

private:

	i3Node *		m_pBullet[LOD_COUNT][BULLET_COUNT];
	INT8			m_i8AIIndex[GENDER_COUNT][AI_COUNT];	

	INT32			m_iLoadedBulletCount;
	INT32			m_iLoadableBulletCount;

	bool			m_bFirstLoadMag;

	WeaponBase *	m_pDualWeapon;
	bool			m_bUseDualWeapon;

private:
	void			_CreateDualWeapon(CGameCharaBase * pOwner, T_ItemID dual_itemID);	
	void			_OnChangeAnimProcess(REAL32 rTimeScale = 1.f);

protected:
	virtual	void	_OnInitVairable( void) override;
	virtual void	_OnProcessFireMiddleAnim( REAL32 rDelay) override;

	void			_VisibleBullet( bool bEnable);
	void			_BindAnimationIndex(CGameCharaBase * pOwner);

	bool			_SwitchingWeapon( WeaponBase * pPrevWeapon, WeaponBase * pNextWeapon, bool bPlaySound = false );			
	
public:
	WeaponShotGunGenderWithDual();
	virtual ~WeaponShotGunGenderWithDual();

	virtual void	PostCreate( void) override;
	virtual void	Reset( void) override;
	virtual void	OnBindResource(void) override;
	virtual void	ReturnInstance( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	SetOwner( CGameCharaBase * pOwner) override;
	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;
	bool			SetEnableDualWeapon( bool bUse, bool bPlaySound = false);

	virtual void	SetBulletCountForNetwork(INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount) override;
	virtual void	OnPlayChangeSound( void) override;


	/************************************************************************/
	/*							Material Functions							*/
	/************************************************************************/
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;

	/************************************************************************/
	/*							View Process Functions						*/
	/************************************************************************/
	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;
	virtual void	OnSetVisibleExtension( bool bVal) override;

	/****************************************************************************/
	/*							Weapon Order Functions							*/
	/****************************************************************************/

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd(void) override;
	virtual void	ReturnToReady( void) override;
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos) override;
	virtual void	LoadMagazine_Ready( void) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

	virtual void	OnDrop( void) override;

	/****************************************************************************/
	/*							Animation Play Functions						*/
	/****************************************************************************/
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;
	virtual void	OnPlayLoadMagReadyAnimation( void) override;

	/****************************************************************************/
	/*						Weapon Extension Functions							*/
	/****************************************************************************/
	virtual bool	OnCheckExtensionPossible( bool bVal) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true ) override;
	virtual void	OnProcessExtension( bool bVal) override;

	/****************************************************************************/
	/*								Network Functions							*/
	/****************************************************************************/
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm) override;

	/************************************************************************/
	/*							Dual Weapon Func							*/
	/************************************************************************/
	WeaponBase *	getDualWeapon(void)							{ return m_pDualWeapon; }
	virtual INT32	getLoadedBulletDualCount( void) override { if( m_pDualWeapon != nullptr) return m_pDualWeapon->getLoadedBulletCount(); return 0;}
	virtual void	setLoadedBulletDualCount( INT32 nCount ) override { if( m_pDualWeapon != nullptr) m_pDualWeapon->setLoadedBulletCount( nCount); }

	const bool				isUsedDualWeapon( void)	const { return m_bUseDualWeapon; }
	virtual WeaponBase *	getUsedWeapon( void) override { if( m_bUseDualWeapon) return m_pDualWeapon; return this; }

};

#endif	// __WEAPONSHOTGUNGENDERWITHDUAL_H__
