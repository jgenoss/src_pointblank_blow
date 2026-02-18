#pragma once

#include "i3GeometryAttr.h"

class I3_EXPORT_SCENE i3GeometryResolver : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GeometryResolver, i3ElementBase);
protected:
	i3GeometryAttr *		m_pGeoAttr = nullptr;
	INT32					m_idxStart = 0;
	i3VertexArray *			m_pVA = nullptr;
	i3IndexArray *			m_pIA = nullptr;

	bool					m_bLinePrim = false;

public:
	virtual ~i3GeometryResolver(void);

	void			setGeometryAttr( i3GeometryAttr * pAttr);

	I3G_PRIMITIVE	getPrimitiveType(void)								{ return m_pGeoAttr->GetPrimitiveType(); }
	INT32			getPrimitiveCount(void);
	void			getVertexIndices( INT32 idx, INT32 * pIndices);
	void			getPositions( INT32 idx, VEC3D * pPos);
	void			getNormals( INT32 idx, VEC3D * pNormal);
	void			getTexCoords( INT32 idx, VEC2D * pUV);
};
