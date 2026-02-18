#if !defined( __CROSSHAIR_CROSSDOT_H)
#define __CROSSHAIR_CROSSDOT_H

#include "Crosshair.h"

class Crosshair_CrossDot : public Crosshair
{
	I3_CLASS_DEFINE( Crosshair_CrossDot, Crosshair);

protected:
	i3Sprite2D	*		m_pSprite;
	
protected:
	void			_UpdatePos(void);

public:
	Crosshair_CrossDot(void);
	virtual ~Crosshair_CrossDot(void);

	virtual void	Create( void) override;
	virtual void	OnUpdate( REAL32 tm) override;
};

#endif