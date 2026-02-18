#ifndef _GAMECHARA_DINO_CCACID_H_
#define _GAMECHARA_DINO_CCACID_H_

#include "GameCharaDinoAcid.h"

class CGameCharaDinoCCAcid : public CGameCharaDinoAcid
{
	I3_CLASS_DEFINE( CGameCharaDinoCCAcid, CGameCharaDinoAcid);

protected:
	virtual void	_CreateDino( void) override;

public:
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ResetChara() override;
	virtual void	OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm) override;
};

#endif // _GAMECHARA_DINO_CCACID_H_

