#include "i3SceneDef.h"
#include "i3ReflectMapEnableAttr.h"

I3_CLASS_INSTANCE( i3ReflectMapEnableAttr);

i3ReflectMapEnableAttr::i3ReflectMapEnableAttr(void)
{
	Set(false);

	SetID( I3_ATTRID_REFLECTMAP_ENABLE);
}

void i3ReflectMapEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_REFLECT, Get());
}
