#include "i3SceneDef.h"
#include "i3TextureEnableAttr.h"

I3_CLASS_INSTANCE( i3TextureEnableAttr);

i3TextureEnableAttr::i3TextureEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_TEXTURE_ENABLE);
}


void i3TextureEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_DIFFUSE, Get());
}
