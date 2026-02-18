#if !defined( _WEAPON_DINO_HOWL_H__)
#define _WEAPON_DINO_HOWL_H__

#include "WeaponDino.h"

class WeaponDinoHowl :	public WeaponDino
{
	I3_CLASS_DEFINE( WeaponDinoHowl, WeaponDino);

public:
	WeaponDinoHowl( void);
	virtual ~WeaponDinoHowl( void);

	virtual CHARA_HIT_PART	GetHitRagdollPart(void) override;

protected:
	bool	_IsValidChara(CGameCharaBase * pChara, WEAPON::DAMAGE_TYPE damageType);
	void	_SetTrexTrembleCamera(REAL32 rRange, REAL32 rSlope, REAL32 rEffectiveTime );

public:
	void			ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo);
	virtual bool	DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale = 0.0f) override;
};

#endif // _WEAPON_DINO_HOWL_H__