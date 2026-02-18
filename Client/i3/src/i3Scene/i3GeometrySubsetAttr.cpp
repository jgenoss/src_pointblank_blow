#include "i3SceneDef.h"
#include "i3GeometrySubsetAttr.h"
#include "i3GeometryAttr.h"

I3_CLASS_INSTANCE( i3GeometrySubsetAttr );

i3GeometrySubsetAttr::i3GeometrySubsetAttr(void)
{
	SetID( I3_ATTRID_GEOMETRY);
}

i3GeometrySubsetAttr::~i3GeometrySubsetAttr(void)
{
	I3_SAFE_RELEASE(m_pGeomAttr);
}

void	i3GeometrySubsetAttr::SetGeometryAttr(i3GeometryAttr* geomAttr)
{
	I3_REF_CHANGE(m_pGeomAttr, geomAttr);
}

void	i3GeometrySubsetAttr::Apply( i3RenderContext * pContext)
{
	if (m_pGeomAttr)
	{
		m_pGeomAttr->SetPrimitiveCount(m_PrimCount);
		m_pGeomAttr->SetStartIndex(m_StartIdx);
		m_pGeomAttr->Apply(pContext);
	}
}

