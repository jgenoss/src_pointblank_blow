#if !defined( __CROSSHAIR_DINO_RAPTOR_H)
#define __CROSSHAIR_DINO_RAPTOR_H

#include "Crosshair_Dino.h"

class Crosshair_Dino_Raptor : public Crosshair_Dino
{
	I3_CLASS_DEFINE( Crosshair_Dino_Raptor, Crosshair_Dino);

protected:

protected:
	virtual void		_Update( REAL32 tm) override;

public:
	Crosshair_Dino_Raptor(void);
	virtual ~Crosshair_Dino_Raptor(void);

	virtual void	Create( void) override;
};

#endif