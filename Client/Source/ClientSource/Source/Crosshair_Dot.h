#if !defined( __CROSSHAIR_DOT_H)
#define __CROSSHAIR_DOT_H

#include "Crosshair.h"

class Crosshair_Dot : public Crosshair
{
	I3_CLASS_DEFINE( Crosshair_Dot, Crosshair);

protected:
	i3AttrSet *		m_pAttrSet;
	i3ColorAttr *	m_pColorAttr;
	i3GeometryAttr *	m_pGeoAttr;

protected:
	void	_CreateVertex(void);

public:
	Crosshair_Dot(void);
	virtual ~Crosshair_Dot(void);

	virtual void	Create( void) override;
	virtual void	OnUpdate( REAL32 tm) override;

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif