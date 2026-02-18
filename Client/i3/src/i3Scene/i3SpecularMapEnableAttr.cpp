#include "i3SceneDef.h"
#include "i3SpecularMapEnableAttr.h"

I3_CLASS_INSTANCE( i3SpecularMapEnableAttr);

i3SpecularMapEnableAttr::i3SpecularMapEnableAttr(void)
{
	Set(false);

	SetID( I3_ATTRID_SPECULARMAP_ENABLE);
}


void i3SpecularMapEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_SPECULAR, Get());
}
