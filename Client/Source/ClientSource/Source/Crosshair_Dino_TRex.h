#if !defined( __CROSSHAIR_DINO_TREX_H)
#define __CROSSHAIR_DINO_TREX_H

#include "Crosshair_Dino.h"

class Crosshair_Dino_TRex : public Crosshair_Dino
{
	I3_CLASS_DEFINE( Crosshair_Dino_TRex, Crosshair_Dino);

protected:

protected:
	virtual void		_Update( REAL32 tm) override;

public:
	Crosshair_Dino_TRex(void);
	virtual ~Crosshair_Dino_TRex(void);

	virtual void	Create( void) override;
};

#endif