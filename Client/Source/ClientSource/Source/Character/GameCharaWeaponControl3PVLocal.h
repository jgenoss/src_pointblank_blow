#if !defined( __GAME_CHARA_WEAPON_CONTROL_3PV_LOCAL_H__)
#define __GAME_CHARA_WEAPON_CONTROL_3PV_LOCAL_H__

#include "GameCharaWeaponControl3PV.h"

class CGameCharaWeaponControl3PVLocal : public CGameCharaWeaponControl3PV
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl3PVLocal );
protected:
public:
	CGameCharaWeaponControl3PVLocal();
	virtual ~CGameCharaWeaponControl3PVLocal();

	//virtual void	OnUpdate( REAL32 rDeltatime);
	//virtual bool	OnSwapWeapon( INT32 nSlot);
	/*virtual bool	OnDropedWeaponSearching( REAL32 rDeltatime);
	virtual bool	OnFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL);
	virtual bool	OnSecFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL);*/

	//virtual bool	OnExtActivate( void);
	//virtual bool	OnExtDeactivate( void);
	//virtual bool	OnLoadMagazine( void);
	//virtual bool	OnLoadMagazineDual( void);
	//virtual bool	OnLoadBullet( void);

	//virtual bool	OnLoadMagazine_Ready( void);
};

#endif
