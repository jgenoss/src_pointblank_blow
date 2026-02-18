#include "i3SceneDef.h"
#include "i3BlendEnableAttr.h"

I3_CLASS_INSTANCE( i3BlendEnableAttr);

i3BlendEnableAttr::i3BlendEnableAttr(void)
{
	Set(false);
	SetID( I3_ATTRID_BLEND_ENABLE);
}

void i3BlendEnableAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetBlendEnable( Get());
}
