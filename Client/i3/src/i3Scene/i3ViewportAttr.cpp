#include "i3SceneDef.h"
#include "i3ViewportAttr.h"

I3_CLASS_INSTANCE( i3ViewportAttr);

i3ViewportAttr::i3ViewportAttr(void)
{
	SetID( I3_ATTRID_VIEWPORT);
}


void i3ViewportAttr::SetViewportRect( INT32 nStartX, INT32 nStartY, INT32 nWidth, INT32 nHeight)
{
	m_nStartX = nStartX;
	m_nStartY = nStartY;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void i3ViewportAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetViewport( m_nStartX, m_nStartY, m_nWidth, m_nHeight);
}

void  i3ViewportAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	I3ASSERT( i3::same_of<i3ViewportAttr*>(pObj));

	i3ViewportAttr * pDest = (i3ViewportAttr *) pObj;

	pDest->m_nStartX = m_nStartX;
	pDest->m_nStartY = m_nStartY;
	pDest->m_nWidth = m_nWidth;
	pDest->m_nHeight = m_nHeight;
}

bool  i3ViewportAttr::IsSame( i3RenderAttr * pAttr)
{
	I3ASSERT( i3::same_of<i3ViewportAttr*>(pAttr));

	i3ViewportAttr * pDest = (i3ViewportAttr *) pAttr;

	return	(pDest->m_nStartX == m_nStartX) &&
			(pDest->m_nStartY == m_nStartY) &&
			(pDest->m_nWidth == m_nWidth) &&
			(pDest->m_nHeight == m_nHeight);
}

UINT32	 i3ViewportAttr::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32	 i3ViewportAttr::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
