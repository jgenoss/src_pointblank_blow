#include "i3SceneDef.h"
#include "i3ShadowMapEnableAttr.h"

I3_CLASS_INSTANCE( i3ShadowMapEnableAttr);

i3ShadowMapEnableAttr::i3ShadowMapEnableAttr(void)
{
	Set(false);

	SetID( I3_ATTRID_SHADOWMAP_ENABLE);
}


void i3ShadowMapEnableAttr::Apply( i3RenderContext * pContext)
{
	//pContext->SetShadowMapEnable( Get());
}
