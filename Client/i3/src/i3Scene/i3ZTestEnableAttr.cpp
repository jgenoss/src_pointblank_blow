#include "i3SceneDef.h"
#include "i3ZTestEnableAttr.h"

I3_CLASS_INSTANCE( i3ZTestEnableAttr);

i3ZTestEnableAttr::i3ZTestEnableAttr(void)
{
	Set(true);
	SetID( I3_ATTRID_Z_TEST_ENABLE);
}

void i3ZTestEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetZTestEnable( Get());
}
