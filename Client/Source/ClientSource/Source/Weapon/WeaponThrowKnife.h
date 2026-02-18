#if !defined( __WEAPON_THROW_KNIFE_H)
#define __WEAPON_THROW_KNIFE_H

#include "WeaponBase.h"

class WeaponThrowKnife : public WeaponKnife
{
	I3_CLASS_DEFINE( WeaponThrowKnife, WeaponKnife);

private:
	virtual MainWeapon*		CreateMainWeapon() override;

public:
	virtual bool	isBulletFireWeapon( void) override { return false; }

public:
	WeaponThrowKnife( void);
	virtual ~WeaponThrowKnife( void);

	virtual void	OnBindResource(void) override;
	virtual void	Reset( void) override;

	virtual void	ReturnInstance() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnSwaping( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/
};

#endif //__WEAPON_KNIFE_H

