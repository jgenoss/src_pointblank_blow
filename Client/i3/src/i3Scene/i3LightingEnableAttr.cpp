#include "i3SceneDef.h"
#include "i3LightingEnableAttr.h"

I3_CLASS_INSTANCE( i3LightingEnableAttr);

i3LightingEnableAttr::i3LightingEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_LIGHTING_ENABLE);
}

void i3LightingEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetLightingEnable( m_bState);
}
