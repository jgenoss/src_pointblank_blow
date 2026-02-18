#if !defined( _GAME_CHARA_DINO_TREX__)
#define _GAME_CHARA_DINO_TREX__

#include "GameCharaDino.h"

class CGameCharaDinoTRex : public CGameCharaDino
{
	I3_CLASS_DEFINE( CGameCharaDinoTRex, CGameCharaDino);

protected:
	//EREX_ATTACK_TYPE	m_AttackType;

	REAL32				m_rElapsedCrashTime = 0.0f;

	bool				m_isAvailHowl = true;
	REAL32				m_rElapsedHowlTime = 0.1f;

	WeaponDino *		m_pTrexCrash = nullptr;

protected:
	/** \brief 상체 Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const override { return IsRex()?CHARA_UPPER_TREX_BONE_COUNT:CHARA_UPPER_MUTANT_TREX_BONE_COUNT;}

	/** \brief 하체 Bone Count */
	virtual INT32		_GetLowerBoneCount( void) const override { return CHARA_LOWER_TREX_BONE_COUNT; }

	virtual void		_CreateDino( void) override;
	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;

public:
	CGameCharaDinoTRex(void);
	virtual ~CGameCharaDinoTRex(void);

public:
	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;

public:
	REAL32						GetHowlCooldownRate(void);

	virtual void				OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void				_PlayBreathSnd(void) override;
	virtual void				ResetChara() override;

	virtual void				SetDinoFireStartPos(VEC3D * pSrc) override;

	virtual bool				isWalk(INT32 idx) override { return false; }
	virtual bool				isSkipAttack(INT32 idx) override;
	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;

private:
	void				UpdateHowlGuage();
};

#endif // _GAME_CHARA_DINO_TREX__