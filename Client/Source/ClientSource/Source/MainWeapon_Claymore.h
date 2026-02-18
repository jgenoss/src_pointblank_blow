#pragma once

#include "MainWeapon_Grenade.h"

class MainWeapon_Claymore: public MainWeapon_Grenade
{
protected:
	virtual WeaponGrenade * _CreateThrowWepaon(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx) override;

	virtual void			_TakeObject(void) override;
	virtual void			_UpdateTakeObjectTransform(void) override;

	void					_SetClaymoreMatrix(WeaponGrenade * pGrenade);

	void					_CheckExplosionSensor(void);

public:
	MainWeapon_Claymore(WeaponBase* p);

	virtual void	OnInitVariable(void) override;
	virtual void	Reset(void) override;

	virtual void	OnUpdate_Grenade( REAL32 rDeltaSeconds) override;

	virtual void	CheckWorldCollision(REAL32 rDeltaSeconds) override;
	virtual void	UpdateNetworkData(REAL32 rDeltatime) override;

	virtual void	SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo) override;
	virtual void	UpdateFlying(REAL32 rDeltaSeconds, REAL32 rExplosionRange) override;

	virtual bool	isPlayRadioSound(void) override { return false; }
	virtual bool	isUseTraceEffect(void) override { return false; }

	virtual VEC3D * GetThrowInfo_Normal(void) override { return &m_vLookAt; }

public:
	bool			isHost(void) const;
	void			SetLookAt(VEC3D * pLookAt)	{ i3Vector::Copy(&m_vLookAt, pLookAt); }
	const VEC3D *	GetLookAt(void) const		{ return &m_vLookAt; }

private:
	VEC3D			m_vLookAt;
	MATRIX			m_AttachLocalMatrix;

	REAL32			m_rFloorCheckTime;
	REAL32			m_rExplosionDelay;
};

