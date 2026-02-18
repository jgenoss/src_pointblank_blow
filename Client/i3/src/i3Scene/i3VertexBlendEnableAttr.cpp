#include "i3SceneDef.h"
#include "i3VertexBlendEnableAttr.h"

I3_CLASS_INSTANCE( i3VertexBlendEnableAttr, i3AbstractBoolAttr);

i3VertexBlendEnableAttr::i3VertexBlendEnableAttr(void)
{
	Set( FALSE);

	SetID( I3_ATTRID_VERTEX_BLEND_ENABLE);
}

i3VertexBlendEnableAttr::~i3VertexBlendEnableAttr(void)
{
}

void i3VertexBlendEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetVertexBlendEnable( m_bState);
}
