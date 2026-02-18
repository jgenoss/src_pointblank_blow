#if !defined( __GAME_CHARA_DINO_ACID_H__)
#define __GAME_CHARA_DINO_ACID_H__

#include "GameCharaDino.h"

class CGameCharaDinoAcid : public CGameCharaDino
{
	I3_CLASS_DEFINE( CGameCharaDinoAcid, CGameCharaDino);

protected:

	REAL32				m_rElapsedCrashTime = 0.0f;
	bool				m_isAvailDash = false;
	bool				m_isAvailCrash = false;

	INT32				m_idxButtFireEffect = 0;
	INT32				m_idxExplosionFireEffect = 0;
	INT32				m_idxExplosionIdleEffect1pv = 0;
	INT32				m_idxExplosionIdleEffect3pv = 0;


protected:
	virtual void			_CreateDino( void) override;

	virtual void			_PlayDownfallSound( void) override;
	virtual void			_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;
public:
	CGameCharaDinoAcid();

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;


	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;
	virtual void				_PlayBreathSnd(void) override;

	virtual void				ResetChara() override;
	void						OnDinoExplosionSign();


	REAL32						m_rExplosionSpeedGauge = 0.0f;
	bool						m_bExplosionPenalty = false;
	bool						m_bExplosionIdle = false;
	bool						m_bExplosionOK = false;
	REAL32						m_rOldExplosionSpeedGauge = 0.0f;
	REAL32						m_rNoPressAccuTime = 0.0f;
	REAL32						m_rPressAccuTime = 0.0f;
	REAL32						m_rAccuTimeForPenalty = 0.0f;
	REAL32						m_rExplosionHP = 0.0f;

	REAL32						m_rExplosionSign = 0.0f;
	bool						m_bIncreaseSign = false;
	bool						m_bEnableSign = false;

	void						UpdateFlag();
	void						UpdateExplosionGuage();
public:
	REAL32						getExplosionGauge( void)				{ return m_rExplosionSpeedGauge; }
	void						Effect();

	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;
	virtual bool				isSkipAttack(INT32 idx) override;
};

#endif //__GAME_CHARA_DINO_RAPTOR_H__
