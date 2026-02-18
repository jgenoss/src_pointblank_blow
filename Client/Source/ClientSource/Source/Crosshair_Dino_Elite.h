#if !defined( __CROSSHAIR_DINO_ELITE_H)
#define __CROSSHAIR_DINO_ELITE_H

#include "Crosshair_Dino.h"

class Crosshair_Dino_Elite : public Crosshair_Dino
{
	I3_CLASS_DEFINE( Crosshair_Dino_Elite, Crosshair_Dino);

protected:

protected:
	virtual void		_Update( REAL32 tm) override;

public:
	Crosshair_Dino_Elite(void);
	virtual ~Crosshair_Dino_Elite(void);


	virtual void	Create( void) override;
};

#endif