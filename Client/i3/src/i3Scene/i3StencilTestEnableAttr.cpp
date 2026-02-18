#include "i3SceneDef.h"
#include "i3StencilTestEnableAttr.h"

I3_CLASS_INSTANCE( i3StencilTestEnableAttr);

i3StencilTestEnableAttr::i3StencilTestEnableAttr(void)
{
	Set(false);

	SetID( I3_ATTRID_STENCILTEST_ENABLE);
}

void i3StencilTestEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetStencilEnable( m_bState);
}

