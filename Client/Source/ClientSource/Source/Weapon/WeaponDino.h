#if !defined( _WEAPON_DINO_H__)
#define _WEAPON_DINO_H__

#include "WeaponBase.h"

class WeaponDino :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDino, WeaponBase);

protected:
	WeaponBase *	m_pExtWeapon;

	REAL32			m_rAttakActiveTime;

protected:
	virtual void	_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos) override;
	virtual CHARA_HIT_PART	GetHitRagdollPart(void) { return CHARA_HIT_ROOT; }

public:
	WeaponDino( void);
	virtual ~WeaponDino( void);

	virtual void			Create(CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	virtual REAL32			GetDamage( bool bRandomDamage = true) override;
	virtual REAL32			GetRange() override;
	virtual REAL32			GetFireDelay() override;

	virtual void			OnUpdate( REAL32 rDeltaSeconds) override;
	virtual REAL32			GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type) override;

protected:
	bool					_DoFireCollisionDino (i3CollideeLine * pCol, INT32 idxGroup);
	void					_PlayFireEffectDino(void);

	CHARA_DEATH_TYPE		_GetDeathType(WEAPON::DAMAGE_TYPE damageType);
public:
	virtual void			FireCollisionDino(VEC3D * pStartPos, VEC3D * pTargetPos);
	virtual void			GetShotTargetPos(VEC3D * pDest, VEC3D * pStart = nullptr ) override;

	void					ProcessAreaHitDino(void);	
	virtual void			ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo) {};

	REAL32					GetDinoAttackTime(INT32 i32FireCount);
	virtual bool			DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale = 0.0f);
};

#endif // _WEAPON_DINO_H__