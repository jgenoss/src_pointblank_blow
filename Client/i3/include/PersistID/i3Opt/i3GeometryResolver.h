#if !defined( __I3_GEOMETRY_RESOLVER_H)
#define __I3_GEOMETRY_RESOLVER_H

class I3_EXPORT_OPT i3GeometryResolver : public i3ElementBase
{
	I3_CLASS_DEFINE( i3GeometryResolver);
protected:
	i3GeometryAttr *		m_pGeoAttr;
	INT32					m_Index;
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;

public:
	i3GeometryResolver(void);
	virtual ~i3GeometryResolver(void);

	void			setGeometryAttr( i3GeometryAttr * pAttr);

	I3G_PRIMITIVE	getPrimitiveType(void)								{ return m_pGeoAttr->GetPrimitiveType(); }
	INT32			getTriangleCount(void);
	void			getVertexIndices( INT32 idx, INT32 * pIndices);
	void			getPositions( INT32 idx, VEC3D * pPos);
	void			getNormals( INT32 idx, VEC3D * pNormal);
	void			getTexCoords( INT32 idx, VEC2D * pUV);
};

#endif
