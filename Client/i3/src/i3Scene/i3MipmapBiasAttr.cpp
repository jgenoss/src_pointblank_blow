#include "i3SceneDef.h"
#include "i3MipmapBiasAttr.h"

I3_CLASS_INSTANCE(i3MipmapBiasAttr);

i3MipmapBiasAttr::i3MipmapBiasAttr(void)
{
	SetID(I3_ATTRID_MIPMAPBIAS);
}


void i3MipmapBiasAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetMipmapBias( m_fBias);
}

void i3MipmapBiasAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3MipmapBiasAttr *) pObj)->Set( Get());
}

bool i3MipmapBiasAttr::IsSame( i3RenderAttr * pAttr)
{
	I3ASSERT(i3::same_of<i3MipmapBiasAttr*>(pAttr));

	i3MipmapBiasAttr * pBiasAttr = (i3MipmapBiasAttr *)pAttr;

	return pBiasAttr->Get() == Get();
}

