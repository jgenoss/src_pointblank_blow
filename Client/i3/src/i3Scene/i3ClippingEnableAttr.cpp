#include "i3SceneDef.h"
#include "i3ClippingEnableAttr.h"

I3_CLASS_INSTANCE( i3ClippingEnableAttr);

i3ClippingEnableAttr::i3ClippingEnableAttr(void)
{
	Set(true);
	SetID( I3_ATTRID_CLIPPING_ENABLE);
}

void i3ClippingEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetClippingEnable( m_bState);
}

