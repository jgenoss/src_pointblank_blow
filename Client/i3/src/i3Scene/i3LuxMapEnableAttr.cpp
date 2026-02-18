#include "i3SceneDef.h"
#include "i3LuxMapEnableAttr.h"

I3_CLASS_INSTANCE( i3LuxMapEnableAttr);

i3LuxMapEnableAttr::i3LuxMapEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_LUXMAP_ENABLE);
}


void i3LuxMapEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_LUX, Get());
}
