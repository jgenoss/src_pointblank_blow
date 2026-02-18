#if !defined( __CROSSHAIR_RING_H)
#define __CROSSHAIR_RING_H

#include "Crosshair.h"

class Crosshair_Ring : public Crosshair
{
	I3_CLASS_DEFINE( Crosshair_Ring, Crosshair);

protected:
	i3AttrSet *			m_pAttrSet;
	i3VertexArray *		m_pVA;
	i3GeometryAttr *	m_pGeoAttr;

protected:
	void			_CreateVertex(void);

public:
	Crosshair_Ring(void);
	virtual ~Crosshair_Ring(void);

	virtual void	Create( void) override;
	virtual void	OnUpdate( REAL32 tm) override;

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif