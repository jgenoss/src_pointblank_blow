#if !defined( _WEAPON_DINO_CRASH_H__)
#define _WEAPON_DINO_CRASH_H__

#include "WeaponDino.h"

class WeaponDinoCrash :	public WeaponDino
{
	I3_CLASS_DEFINE( WeaponDinoCrash, WeaponDino);

public:
	WeaponDinoCrash( void);
	virtual ~WeaponDinoCrash( void);

	virtual CHARA_HIT_PART	GetHitRagdollPart(void) override { return CHARA_HIT_DEATH_TRAMPLE ; }

protected:
	bool	_IsDashCrash(void);
	bool	_CheckRange(VEC3D & outDir, REAL32 & outLength, VEC3D * pCharaPos, REAL32 rRange);


public:
	void			ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo);
	virtual bool	DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale = 0.0f) override;
};

#endif // _WEAPON_DINO_CRASH_H__