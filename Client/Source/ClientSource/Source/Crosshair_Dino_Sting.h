#if !defined( __CROSSHAIR_DINO_STING_H)
#define __CROSSHAIR_DINO_STING_H

#include "Crosshair_Dino.h"

class Crosshair_Dino_Sting : public Crosshair_Dino
{
	I3_CLASS_DEFINE( Crosshair_Dino_Sting, Crosshair_Dino);

protected:

protected:
	virtual void		_Update( REAL32 tm) override;

public:
	Crosshair_Dino_Sting(void);
	virtual ~Crosshair_Dino_Sting(void);

	virtual void	Create( void) override;
};

#endif