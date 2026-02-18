#include "i3SceneDef.h"
#include "i3ReflectMaskMapEnableAttr.h"

I3_CLASS_INSTANCE( i3ReflectMaskMapEnableAttr);

i3ReflectMaskMapEnableAttr::i3ReflectMaskMapEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_REFLECTMASKMAP_ENABLE);
}

void i3ReflectMaskMapEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureEnable( I3G_TEXTURE_BIND_REFLECT_MASK, Get());
}
