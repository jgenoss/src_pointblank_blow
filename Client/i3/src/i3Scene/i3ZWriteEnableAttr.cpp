#include "i3SceneDef.h"
#include "i3ZWriteEnableAttr.h"

I3_CLASS_INSTANCE( i3ZWriteEnableAttr);

i3ZWriteEnableAttr::i3ZWriteEnableAttr(void)
{
	Set(true);
	SetID( I3_ATTRID_Z_WRITE_ENABLE);
}

void i3ZWriteEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetZWriteEnable( m_bState);
}
