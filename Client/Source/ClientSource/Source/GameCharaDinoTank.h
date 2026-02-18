#if !defined( __GAME_CHARA_DINO_TANK_H__)
#define __GAME_CHARA_DINO_TANK_H__

#include "GameCharaDino.h"

class CGameCharaDinoTank : public CGameCharaDino
{
	I3_CLASS_DEFINE( CGameCharaDinoTank, CGameCharaDino);

protected:

	REAL32				m_rElapsedCrashTime = 0.0f;
	bool				m_isAvailDash = false;
	bool				m_isAvailCrash = false;
	INT32				m_idxButtFireEffect = 0;
	INT32				m_idxDashEffect = 0;
	INT32				m_idxHitDashEffect = 0;

	virtual void		_CreateDino( void) override;
	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;

public:
	CGameCharaDinoTank();

	virtual void				OnUpdate( REAL32 rDeltaSeconds) override;

	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;

	virtual void				_PlayBreathSnd(void) override;

	virtual void				ResetChara() override;

	virtual void				DoFireHitEffect( void) override;


	REAL32						m_rDashSpeedGauge = 0.0f;
	bool						m_bDashPenalty = false;
	REAL32						m_rOldDashSpeedGauge = 0.0f;
	REAL32						m_rNoPressAccuTime = 0.0f;
	REAL32						m_rPressAccuTime = 0.0f;
	REAL32						m_rAccuTimeForPenalty = 0.0f;

	void						UpdateFlag();
	void						UpdateDashGuage();

public:
	REAL32						getDashGauge( void)				{ return m_rDashSpeedGauge; }
	bool						isDashCrash(void) const			{ return m_isAvailCrash; }

	virtual bool				isWalk(INT32 idx) override;
	virtual bool				isSkipAttack(INT32 idx) override;
	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;
};

#endif //__GAME_CHARA_DINO_RAPTOR_H__
