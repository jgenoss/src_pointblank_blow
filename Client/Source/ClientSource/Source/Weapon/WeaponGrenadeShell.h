#if !defined( __WEAPON_GRENADESHELL_H__)
#define __WEAPON_GRENADESHELL_H__

#include "weaponbase.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 유탄입니다.
// 유탄발사기에 대한 클래스는 아닙니다.
// 월드에서 폭발하는 유탄만 처리됩니다.

#define GRENADESHELL_STATE_LAUNCH			0x0001
#define GRENADESHELL_STATE_EXPLOSION		0x0002
#define GRENADESHELL_STATE_MISFIRE			0x0004


class WeaponGrenadeShell : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponGrenadeShell, WeaponBase);

private:
	virtual MainWeapon*	CreateMainWeapon() override;
protected:
	virtual void	_InitCollideeGroup( void) override;

private:
	//	네트워크용
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	//virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos ) override;
	virtual void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType ) override;
	virtual void	NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx ) override {}

public:
	WeaponGrenadeShell();
	virtual ~WeaponGrenadeShell();
	



	void			SetGrenadeOwner( CGameCharaBase * pOwner, CWeaponInfo * pOwnerWeaponInfo);

//	VEC3D *			getThrowDirection( void)								{ return &m_vDirection; }
//	REAL32			getThrowSpeed( void)									{ return m_rSpeed; }
	
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;

	virtual void	Reset() override;
	//virtual void	Fire(void);
	virtual void	FireEnd(void) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override {}
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	//
	virtual void	ReturnInstance() override;

	virtual bool	IsGrenade(void) const override { return true; }
};

#endif
