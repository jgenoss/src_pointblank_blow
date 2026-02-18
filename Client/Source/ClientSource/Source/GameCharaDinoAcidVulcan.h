#ifndef _GAMECHARA_DINO_ACID_VULCAN_H_
#define _GAMECHARA_DINO_ACID_VULCAN_H_

#include "GameCharaDinoAcid.h"

class CGameCharaDinoAcidVulcan : public CGameCharaDinoAcid
{
	I3_CLASS_DEFINE( CGameCharaDinoAcidVulcan, CGameCharaDinoAcid);

protected:
	virtual void	_CreateDino( void) override;

public:
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ResetChara() override;
	virtual void	OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm) override;
};

#endif // _GAMECHARA_DINO_ACID_VULCAN_H_

