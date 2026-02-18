#if !defined( __WEAPON_SHOTGUN_WITH_BUTT_H__)
#define __WEAPON_SHOTGUN_WITH_BUTT_H__


class WeaponShotGunWithButt : public WeaponShotGun
{
	I3_CLASS_DEFINE(WeaponShotGunWithButt, WeaponShotGun);

public:
	WeaponShotGunWithButt();
	virtual ~WeaponShotGunWithButt();

	virtual void PostCreate( void) override;
	virtual void OnBindResource() override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/

	virtual void OnIdle(i3AIContext * pCtx, REAL32 tm) override;
	virtual void OnSecondaryFire(i3AIContext* pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual bool Fire(WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void FireEnd() override;
	void MeleeCollision(VEC3D* pStartPos, VEC3D* pTargetPos);

	virtual REAL32 GetMoveSpeed(WEAPON_MOVE_SPEED_TYPE type) override;
	virtual REAL32 GetRange() override;
	virtual REAL32 GetDPD_Rate() override;
	virtual REAL32 GetDamage(bool bRandomDamage = true) override;

protected:
	virtual	void _OnInitVairable() override;
	virtual void _OnPlayWeaponSecondFireAnim() override;
	virtual void _OnPlayTerrainHitSound(WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D* pPos) override;
	virtual void _OnAddTerrainEffect(WEAPON_TERRAIN_EFFECT_INFO* pInfo) override;

private:
	INT8 m_aii_SwingButt;
};

#endif