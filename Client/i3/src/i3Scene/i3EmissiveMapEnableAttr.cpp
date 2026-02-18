#include "i3SceneDef.h"
#include "i3EmissiveMapEnableAttr.h"

I3_CLASS_INSTANCE( i3EmissiveMapEnableAttr);

i3EmissiveMapEnableAttr::i3EmissiveMapEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_EMISSIVEMAP_ENABLE);
}

void i3EmissiveMapEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_EMISSIVE, Get());
}
