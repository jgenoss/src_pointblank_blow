#include "i3SceneDef.h"
#include "i3NormalMapEnableAttr.h"

I3_CLASS_INSTANCE( i3NormalMapEnableAttr);

i3NormalMapEnableAttr::i3NormalMapEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_NORMALMAP_ENABLE);
}

void i3NormalMapEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_NORMAL, Get());
}
