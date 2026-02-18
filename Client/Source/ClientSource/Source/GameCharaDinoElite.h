#if !defined( __GAME_CHARA_DINO_ELITE_H__)
#define __GAME_CHARA_DINO_ELITE_H__

#include "GameCharaDino.h"

class CGameCharaDinoElite : public CGameCharaDino
{
	I3_CLASS_DEFINE( CGameCharaDinoElite, CGameCharaDino);

private:
	bool				m_isAvailHowl = false;
	REAL32				m_rElapsedHowlTime = 0.0f;

	bool				m_isAvailRecoveryHp = false;
	REAL32				m_rElapsedRecoveryTime = 0.0f;

	//ø§∏Æ∆Æ πˆ«¡ ¿Ã∆Â∆Æ
	INT32				m_idxEliteBuffEffect = 0;
	INT32				m_idxEliteScratchEffect = 0;

protected:
	/** \brief ªÛ√º Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const override { return CHARA_UPPER_ELITE_BONE_COUNT;}

	virtual void		_CreateDino( void) override;
	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;

public:
	CGameCharaDinoElite();

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;


	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;
	virtual void				_PlayBreathSnd(void) override;

	virtual void				ResetChara() override;

	virtual void				DoFireHitEffect( void) override;


	REAL32						m_rExplosionSpeedGauge = 0.0f;
	bool						m_bExplosionPenalty = false;
	REAL32						m_rOldExplosionSpeedGauge = 0.0f;
	REAL32						m_rNoPressAccuTime = 0.0f;
	REAL32						m_rPressAccuTime = 0.0f;
	REAL32						m_rAccuTimeForPenalty = 0.0f;

	void						UpdateFlag();
	void						UpdateHowlGuage();

	REAL32						GetHowlCooldownRate(void);

	void						HP_Recovery(REAL32 rRecoveryHP = 0.0f);

	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;
	virtual bool				isSkipAttack(INT32 idx) override;
};

#endif //__GAME_CHARA_DINO_RAPTOR_H__
