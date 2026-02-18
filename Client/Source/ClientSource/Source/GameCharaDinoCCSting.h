#ifndef _GAMECHARA_DINO_CCSTING_H_
#define _GAMECHARA_DINO_CCSTING_H_

#include "GameCharaDinoSting.h"

class CGameCharaDinoCCSting : public CGameCharaDinoSting
{
	I3_CLASS_DEFINE( CGameCharaDinoCCSting, CGameCharaDinoSting);

protected:
	virtual void		_CreateDino( void) override;
	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;

public:
	// АјАн
	ID_UPPER_DINO_AI	DinoAttack(WEAPON::FIRE_ORDER fireOrder);
	void				OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);

	virtual void		OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void		ResetChara() override;
};

#endif // _GAMECHARA_DINO_CCSTING_H_

