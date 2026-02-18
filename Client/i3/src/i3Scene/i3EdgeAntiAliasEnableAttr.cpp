#include "i3SceneDef.h"
#include "i3EdgeAntiAliasEnableAttr.h"

I3_CLASS_INSTANCE( i3EdgeAntiAliasEnableAttr, i3AbstractBoolAttr);

i3EdgeAntiAliasEnableAttr::i3EdgeAntiAliasEnableAttr(void)
{
	Set( FALSE);

	SetID( I3_ATTRID_EDGEANTIALIAS_ENABLE);
}

i3EdgeAntiAliasEnableAttr::~i3EdgeAntiAliasEnableAttr(void)
{
}

void i3EdgeAntiAliasEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetEdgeAntiAliasEnable( m_bState);
}

