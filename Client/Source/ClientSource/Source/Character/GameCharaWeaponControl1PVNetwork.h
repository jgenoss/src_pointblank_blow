#if !defined( __GAME_CHARA_WEAPON_CONTROL_1PV_NETWORK_H__)
#define __GAME_CHARA_WEAPON_CONTROL_1PV_NETWORK_H__

#include "GameCharaWeaponControl1PV.h"

class CGameCharaWeaponControl1PVNetwork : public CGameCharaWeaponControl1PV
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl1PVNetwork);
protected:
public:
	CGameCharaWeaponControl1PVNetwork();
	virtual ~CGameCharaWeaponControl1PVNetwork();

	//virtual void	OnUpdate( REAL32 rDeltatime);
	//virtual bool	OnSwapWeapon( INT32 nSlot);
	/*virtual bool	OnFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL);
	virtual bool	OnSecFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL);*/
	
	//virtual bool	OnZoom( void);
	/*virtual bool	OnExtActivate( void);
	virtual bool	OnExtDeactivate( void);*/
	//virtual bool	OnLoadMagazine( void);
	//virtual bool	OnLoadMagazineDual( void);
	//virtual bool	OnLoadBullet( void);

	//virtual bool	OnLoadMagazine_Ready( void);
};

#endif
