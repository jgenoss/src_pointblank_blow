#pragma once

#include "i3RenderAttr.h"

class i3GeometryAttr;

class I3_EXPORT_SCENE i3GeometrySubsetAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3GeometrySubsetAttr, i3RenderAttr);

protected:
	i3GeometryAttr*		m_pGeomAttr = nullptr;
	UINT32				m_PrimCount = 0;
	UINT32				m_StartIdx = 0;
public:
	i3GeometrySubsetAttr(void);
	virtual ~i3GeometrySubsetAttr(void);
	
	void			SetGeometryAttr(i3GeometryAttr* geomAttr);

	UINT32			GetPrimitiveCount(void)						{ return m_PrimCount; }
	void			SetPrimitiveCount( UINT32 nCount )			{ m_PrimCount = nCount; }

	UINT32			GetStartIndex(void)							{ return m_StartIdx; }
	void			SetStartIndex( UINT32 idx)					{ m_StartIdx = idx; }
	// for i3RenderAttr
	virtual void	Apply( i3RenderContext * pContext) override;
};
