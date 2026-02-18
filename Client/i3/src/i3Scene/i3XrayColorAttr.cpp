#include "i3SceneDef.h"
#include "i3Math.h"
#include "i3XrayColorAttr.h"

I3_CLASS_INSTANCE( i3XrayColorAttr);

i3XrayColorAttr::i3XrayColorAttr(void)
{
	SetID( I3_ATTRID_XRAYCOLOR);
}

void i3XrayColorAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetXrayColor( &m_Color);
}

void i3XrayColorAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3XrayColorAttr *) pObj)->Set( Get());
}

bool i3XrayColorAttr::IsSame( i3RenderAttr * pAttr)
{
	return i3Color::IsSame( &m_Color, ((i3XrayColorAttr *)pAttr)->Get());
}

