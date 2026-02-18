#if !defined( __CROSSHAIR_SCOPE_H)
#define __CROSSHAIR_SCOPE_H

#include "Crosshair.h"

class Crosshair_Scope : public Crosshair
{
	I3_CLASS_DEFINE( Crosshair_Scope, Crosshair);

protected:
	i3AttrSet *			m_pAttrSet;
	i3VertexArray *		m_pVA;
	i3GeometryAttr *	m_pGeoAttr;

protected:
	void			_CreateVertex(void);

public:
	Crosshair_Scope(void);
	virtual ~Crosshair_Scope(void);

	virtual void	Create( void) override;
	virtual void	OnUpdate( REAL32 tm) override;

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif