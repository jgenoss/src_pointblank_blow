#include "i3SceneDef.h"
#include "i3FogEnableAttr.h"

I3_CLASS_INSTANCE( i3FogEnableAttr);

i3FogEnableAttr::i3FogEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_FOG_ENABLE);
}

void i3FogEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetFogEnable( m_bState);
}

