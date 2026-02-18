#if !defined( __CROSSHAIR_CROSS_H)
#define __CROSSHAIR_CROSS_H

#include "Crosshair.h"

class Crosshair_Cross : public Crosshair
{
	I3_CLASS_DEFINE( Crosshair_Cross, Crosshair);

protected:
	i3Sprite2D	*		m_pSprite;

protected:
	void			_UpdatePos(void);

public:
	Crosshair_Cross(void);
	virtual ~Crosshair_Cross(void);

	virtual void	Create( void) override;
	virtual void	OnUpdate( REAL32 tm) override;
};

#endif