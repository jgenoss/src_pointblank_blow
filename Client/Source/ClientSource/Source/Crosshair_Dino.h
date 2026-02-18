#if !defined( __CROSSHAIR_DINO_H)
#define __CROSSHAIR_DINO_H

#include "Crosshair.h"

class Crosshair_Dino : public Crosshair
{
	I3_ABSTRACT_CLASS_DEFINE( Crosshair_Dino, Crosshair);

protected:

protected:
	i3Sprite2D	*		m_pSprite;
	i3Sprite2D	*		m_pSprite2;

	REAL32				m_timeLocal;
	REAL32				m_timeTarget;

	bool				m_bBite;
	bool				m_bSpit;
	bool				m_bScratch;

protected:
	virtual void		_Update( REAL32 tm) = 0;

public:
	Crosshair_Dino(void);
	virtual ~Crosshair_Dino(void);

	virtual void	StartBite(void);
	virtual void	Bite();
	virtual void	Spit();
	virtual void	Scratch();

	virtual void	Create( void) override;
	virtual void	OnUpdate( REAL32 tm) override;

	virtual void	ResetCrosshair() override;
};

#endif