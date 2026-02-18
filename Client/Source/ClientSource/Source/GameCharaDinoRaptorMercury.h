#ifndef GAMECHARA_DINO_RAPTOR_MERCURY_H_
#define GAMECHARA_DINO_RAPTOR_MERCURY_H_

#include "GameCharaDinoCCRaptor.h"

class CGameCharaDinoRaptorMercury : public CGameCharaDinoCCRaptor
{
	I3_CLASS_DEFINE( CGameCharaDinoRaptorMercury, CGameCharaDinoCCRaptor);

protected:
	virtual void		_CreateDino( void) override;
};


#endif