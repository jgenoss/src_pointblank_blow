#if !defined( __WEAPON_HANDGRENADE_H__)
#define __WEAPON_HANDGRENADE_H__

	
class WeaponHandGrenade : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponHandGrenade, WeaponBase);
private:
	virtual MainWeapon*	CreateMainWeapon()  override;
	
public:
	WeaponHandGrenade();
	virtual ~WeaponHandGrenade();

	virtual bool	isBulletFireWeapon( void) override { return false; }

	virtual void	PostCreate( void) override;
	virtual void	Reset() override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual	 void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

};

#endif
