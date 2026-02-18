#if !defined( __GAME_CHARA_DINO_STING_H__)
#define __GAME_CHARA_DINO_STING_H__

#include "GameCharaDino.h"

class CGameCharaDinoSting : public CGameCharaDino
{
	I3_CLASS_DEFINE( CGameCharaDinoSting, CGameCharaDino);

protected:
	INT32						m_idxBlazeEffect = 0;
	INT32						m_idxPoisonFireEffect = 0;
	INT32						m_idxSpitFireEffect = 0;

protected:
	virtual void		_CreateDino( void) override;

	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;
	virtual void		_PlayDownfallSound( void) override;

public:
	CGameCharaDinoSting();

	// АјАн
private:
	bool						m_isAvailPoison = true;
	REAL32						m_rElapsedPoisonTime = 0.0f;

public:
	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;

	void						ThrowPoisonGrenade();
	REAL32						GetPoisonCooldownRate(void);

	virtual void				_PlayBreathSnd(void) override;
	virtual void				OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void				ResetChara() override;

	virtual void				SetDinoFireStartPos(VEC3D * pSrc) override;

	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;
	virtual bool				isSkipAttack(INT32 idx) override;

private:
	void UpdatePoisonGuage();

};

#endif //__GAME_CHARA_DINO_RAPTOR_H__
