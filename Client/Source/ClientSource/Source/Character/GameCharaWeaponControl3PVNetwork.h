#if !defined( __GAME_CHARA_WEAPON_CONTROL_3PV_NETWORK_H__)
#define __GAME_CHARA_WEAPON_CONTROL_3PV_NETWORK_H__

#include "GameCharaWeaponControl3PV.h"

class CGameCharaWeaponControl3PVNetwork : public CGameCharaWeaponControl3PV
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl3PVNetwork);
protected:
public:
	CGameCharaWeaponControl3PVNetwork();
	virtual ~CGameCharaWeaponControl3PVNetwork();

	//virtual void	OnUpdate( REAL32 rDeltatime);

	/*virtual bool	OnFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL);
	virtual bool	OnSecFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL);*/
	//virtual bool	OnSwapWeapon( INT32 nSlot);

	/*virtual bool	OnExtActivate( void);
	virtual bool	OnExtDeactivate( void);*/
	//virtual bool	OnLoadMagazine( void);
	//virtual bool	OnLoadMagazineDual( void);
	//virtual bool	OnLoadBullet( void);

	//virtual bool	OnLoadMagazine_Ready( void);
};

#endif
