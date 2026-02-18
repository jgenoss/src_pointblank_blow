#ifndef _GAMECHARA_DINO_STING_MARS_H_
#define _GAMECHARA_DINO_STING_MARS_H_

#include "GameCharaDinoSting.h"

class CGameCharaDinoStingMars : public CGameCharaDinoSting
{
	I3_CLASS_DEFINE( CGameCharaDinoStingMars, CGameCharaDinoSting);

protected:
	virtual void		_CreateDino( void) override;

	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;

	// АјАн
public:
	ID_UPPER_DINO_AI			DinoAttack(WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void				ResetChara() override;
};

#endif

