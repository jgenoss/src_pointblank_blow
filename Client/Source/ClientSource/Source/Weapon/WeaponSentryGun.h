#ifndef _WEAPON_CENTRYGUN_H_
#define _WEAPON_CENTRYGUN_H_

#include "weaponbase.h"
#include "WeaponM197.h"

class SentryGunMode;
class WeaponSentryGun : public WeaponBase
{
	I3_CLASS_DEFINE(WeaponSentryGun, WeaponBase);

	enum ROTATE_AXIS_X
	{
		ROTATE_UP,
		ROTATE_DOWN,
	};

	enum ROTATE_AXIS_Y
	{
		ROTATE_LEFT,
		ROTATE_RIGHT,
	};

public:
	WeaponSentryGun();
	virtual ~WeaponSentryGun();

	virtual void	Create(CGameCharaBase* owner, INT32 Itemid);
	virtual void	OnIdle(i3AIContext* ctx, REAL32 tm);
	virtual void	Explosion();
	virtual void	Reset();	
	virtual void	ReturnToReady();
	virtual void	OnUpdate(REAL32 deltaSeconds);
	virtual void	UpdateAfterAnim( REAL32 rDeltaSeconds);

	void			Fire();
	void			Install(i3Node* sceneNode, i3GameNode* owner, VEC3D* installPosisiton);
	void			Uninstall();
	void			Destroy();
	BOOL			IsInstalled() { return m_isInstalled; }

	void			RotateAxisX(ROTATE_AXIS_X rotDir, REAL32 degreeX = 0.0f, REAL32 speed = 1.0f);
	void			RotateAxisY(ROTATE_AXIS_Y rotDir, REAL32 degreeY = 0.0f, REAL32 speed = 1.0f);

	virtual void	OnLostDevice(bool isDeviceLost);
	virtual void	OnRevive(i3RenderContext* ctx);

protected:
	void			_NetFire();
	VEC3D*			_GetMuzzleDir();
	VEC3D*			_GetMuzzlePos();

private:
	WeaponSentryGun(const WeaponSentryGun& src);
	WeaponSentryGun& operator=(const WeaponSentryGun& src);

private:
	BOOL	m_isInstalled;
	VEC3D	m_bulletTraceStartPos;

};


#endif