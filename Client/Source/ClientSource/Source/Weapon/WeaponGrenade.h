#if !defined( _WEAPON_GRENADE_H__)
#define _WEAPON_GRENADE_H__

#include "weaponbase.h"
#include "Network/ClientP2PDef.h"

//#define NO_GRENADE_TRACE_EFFECT

class WeaponGrenade :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponGrenade, WeaponBase);

protected:

	virtual MainWeapon*	CreateMainWeapon() override;

protected:
	virtual void	_InitCollideeGroup( void) override;


protected:
	//	네트워크용
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx ) override;
	virtual void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType ) override;
	virtual void	NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx ) override;

public:
	WeaponGrenade(void);
	virtual ~WeaponGrenade(void);

	void	Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx);
	void	CheckWorldCollision( REAL32 rDeltaSeconds);

	virtual bool	isBulletFireWeapon( void) override { return false; }
	virtual bool	IsGrenade(void) const override { return true; }


	
	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	void	Explosion();		// 가상 제거..
	virtual void	Reset() override;
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireReady( void) override;
	virtual void	FireEnd(void) override;

	// Drive function
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	//
	virtual void	ReturnInstance() override;

	virtual void	SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo);

	virtual void	ApplyBulletWithDropItem(REAL32 rRate) override;


protected:

	
};

#endif