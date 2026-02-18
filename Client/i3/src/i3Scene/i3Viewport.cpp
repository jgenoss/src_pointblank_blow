#include "i3SceneDef.h"
#include "i3ViewportAttr.h"

I3_CLASS_INSTANCE( i3ViewportAttr, i3RenderAttr);

i3ViewportAttr::i3ViewportAttr(void)
{
	SetID( I3_ATTRID_VIEWPORT);

	m_nStartX = 0;
	m_nStartY = 0;
	m_nWidth = 800;
	m_nHeight = 600;
}

i3ViewportAttr::~i3ViewportAttr(void)
{
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
