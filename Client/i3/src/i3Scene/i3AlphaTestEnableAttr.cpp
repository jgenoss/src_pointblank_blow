#include "i3SceneDef.h"
#include "i3AlphaTestEnableAttr.h"

I3_CLASS_INSTANCE( i3AlphaTestEnableAttr);

i3AlphaTestEnableAttr::i3AlphaTestEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_ALPHA_TEST_ENABLE);
}

void i3AlphaTestEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetAlphaTestEnable( Get());
}
