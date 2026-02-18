#if !defined( __GRID_H)
#define __GRID_H

class CGrid : public i3GameObj
{
	I3_CLASS_DEFINE( CGrid);

protected:
	i3AttrSet *			m_pAttrSet;
	i3Geometry *		m_pGeo;
	i3GeometryAttr *	m_pGeoAttr;
	i3VertexArray *		m_pVA;
public:
	CGrid(void);
	virtual ~CGrid(void);

	void				Create(void);

	void				SetSize( REAL32 cx, REAL32 cy);

};

#endif
