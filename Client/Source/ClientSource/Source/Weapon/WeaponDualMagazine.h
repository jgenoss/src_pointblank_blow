#if !defined( __WEAPON_DUALMAGAZINE_H__)
#define __WEAPON_DUALMAGAZINE_H__

#include "WeaponBase.h"


class WeaponDualMagazine : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDualMagazine, WeaponBase);
protected:
	INT8				m_aiiFire_Dual;					// 듀얼 탄장이 우측으로 간 경우
	INT8				m_aiiIdle_Dual;
	INT8				m_aiiLoadMagazine_Dual;			// 듀얼 탄창 좌측에서 우측으로
	INT8				m_aiiLoadMagazine_DualShift;

protected:
	virtual void	_OnPlayWeaponIdleAnim( void) override;
	virtual void	_OnPlayWeaponFireAnim( void) override;

	virtual void	_OnProcessFireEndAnim( REAL32 rDelay) override;

public:
	WeaponDualMagazine();
	virtual ~WeaponDualMagazine();

	virtual void	Reset( void) override;
	virtual void	OnBindResource( void) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;

	virtual void	OnPlayLoadMagAnimation( void) override;
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;
	virtual bool	OnCheckExtensionPossible( bool bVal) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnExtensionActivate( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnQuickLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndQuickLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual INT32	_OnProcessFireBulletCount( void) override;

};



#endif
