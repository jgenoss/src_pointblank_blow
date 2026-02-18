#include "i3SceneDef.h"
#include "i3VertexColorEnableAttr.h"

I3_CLASS_INSTANCE( i3VertexColorEnableAttr, i3AbstractBoolAttr);

i3VertexColorEnableAttr::i3VertexColorEnableAttr(void)
{
	Set( TRUE);

	SetID( I3_ATTRID_VERTEX_COLOR_ENABLE);
}

i3VertexColorEnableAttr::~i3VertexColorEnableAttr(void)
{
}

void i3VertexColorEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetVertexColorEnable( m_bState);
}

