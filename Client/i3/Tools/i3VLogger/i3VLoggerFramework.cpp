#include "stdafx.h"
#include "i3VLoggerFramework.h"

I3_CLASS_INSTANCE( i3VLoggerFramework); //, i3ViewerFramework);

i3VLoggerFramework::i3VLoggerFramework(void)
{
	m_pLineVB = VtxBuffer::new_object();
	m_pTriVB = VtxBuffer::new_object();
	m_BoundSize = 5.0f;
}

i3VLoggerFramework::~i3VLoggerFramework(void)
{
	I3_SAFE_RELEASE( m_pLineVB);
	I3_SAFE_RELEASE( m_pTriVB);
}

void i3VLoggerFramework::_DrawAxis(void)
{
	_VTX * pVB = m_pLineVB->alloc( 6);

	pVB[0].x = 0.0f;	pVB[0].y = 0.0f;	pVB[0].z = 0.0f;
	pVB[0].c = 0xFFFF0000;		// Red

	pVB[1].x = 1.0f;	pVB[1].y = 0.0f;	pVB[1].z = 0.0f;
	pVB[1].c = 0xFFFF0000;		// Red

	pVB[2].x = 0.0f;	pVB[2].y = 0.0f;	pVB[2].z = 0.0f;
	pVB[2].c = 0xFF00FF00;		// Green

	pVB[3].x = 0.0f;	pVB[3].y = 1.0f;	pVB[3].z = 0.0f;
	pVB[3].c = 0xFF00FF00;		// Green

	pVB[4].x = 0.0f;	pVB[4].y = 0.0f;	pVB[4].z = 0.0f;
	pVB[4].c = 0xFF0000FF;		// Blue

	pVB[5].x = 0.0f;	pVB[5].y = 0.0f;	pVB[5].z = 1.0f;
	pVB[5].c = 0xFF0000FF;		// Blue
}

void i3VLoggerFramework::_DrawBox( VEC3D * pCenter, REAL32 sz)
{
	_VTX * pVB = m_pLineVB->alloc(24);
	
	sz *= 0.5f;

	// Upper Rect.
	pVB[0].x = pCenter->x - sz;		pVB[0].y = pCenter->y - sz;		pVB[0].z = pCenter->z - sz; 	pVB[0].c = m_Color;
	pVB[1].x = pCenter->x + sz;		pVB[1].y = pCenter->y - sz;		pVB[1].z = pCenter->z - sz;		pVB[1].c = m_Color;

	pVB[2].x = pCenter->x + sz;		pVB[2].y = pCenter->y - sz;		pVB[2].z = pCenter->z - sz;		pVB[2].c = m_Color;
	pVB[3].x = pCenter->x + sz;		pVB[3].y = pCenter->y - sz;		pVB[3].z = pCenter->z + sz;		pVB[3].c = m_Color;

	pVB[4].x = pCenter->x + sz;		pVB[4].y = pCenter->y - sz;		pVB[4].z = pCenter->z + sz;		pVB[4].c = m_Color;
	pVB[5].x = pCenter->x - sz;		pVB[5].y = pCenter->y - sz;		pVB[5].z = pCenter->z + sz;		pVB[5].c = m_Color;

	pVB[6].x = pCenter->x - sz;		pVB[6].y = pCenter->y - sz;		pVB[6].z = pCenter->z + sz;		pVB[6].c = m_Color;
	pVB[7].x = pCenter->x - sz;		pVB[7].y = pCenter->y - sz;		pVB[7].z = pCenter->z - sz;		pVB[7].c = m_Color;

	// Lower Rect
	pVB[8].x = pCenter->x - sz;		pVB[8].y = pCenter->y + sz;		pVB[8].z = pCenter->z - sz; 	pVB[8].c = m_Color;
	pVB[9].x = pCenter->x + sz;		pVB[9].y = pCenter->y + sz;		pVB[9].z = pCenter->z - sz;		pVB[9].c = m_Color;
																										
	pVB[10].x = pCenter->x + sz;	pVB[10].y = pCenter->y + sz;	pVB[10].z = pCenter->z - sz;	pVB[10].c = m_Color;
	pVB[11].x = pCenter->x + sz;	pVB[11].y = pCenter->y + sz;	pVB[11].z = pCenter->z + sz;	pVB[11].c = m_Color;
																										
	pVB[12].x = pCenter->x + sz;	pVB[12].y = pCenter->y + sz;	pVB[12].z = pCenter->z + sz;	pVB[12].c = m_Color;
	pVB[13].x = pCenter->x - sz;	pVB[13].y = pCenter->y + sz;	pVB[13].z = pCenter->z + sz;	pVB[13].c = m_Color;
																										
	pVB[14].x = pCenter->x - sz;	pVB[14].y = pCenter->y + sz;	pVB[14].z = pCenter->z + sz;	pVB[14].c = m_Color;
	pVB[15].x = pCenter->x - sz;	pVB[15].y = pCenter->y + sz;	pVB[15].z = pCenter->z - sz;	pVB[15].c = m_Color;

	// Vertical Line
	pVB[16].x = pCenter->x - sz;	pVB[16].y = pCenter->y - sz;	pVB[16].z = pCenter->z - sz; 	pVB[16].c = m_Color;
	pVB[17].x = pCenter->x - sz;	pVB[17].y = pCenter->y + sz;	pVB[17].z = pCenter->z - sz;	pVB[17].c = m_Color;
																											
	pVB[18].x = pCenter->x + sz;	pVB[18].y = pCenter->y - sz;	pVB[18].z = pCenter->z - sz;	pVB[18].c = m_Color;
	pVB[19].x = pCenter->x + sz;	pVB[19].y = pCenter->y + sz;	pVB[19].z = pCenter->z - sz;	pVB[19].c = m_Color;
																											
	pVB[20].x = pCenter->x + sz;	pVB[20].y = pCenter->y - sz;	pVB[20].z = pCenter->z + sz;	pVB[20].c = m_Color;
	pVB[21].x = pCenter->x + sz;	pVB[21].y = pCenter->y + sz;	pVB[21].z = pCenter->z + sz;	pVB[21].c = m_Color;
																											
	pVB[22].x = pCenter->x - sz;	pVB[22].y = pCenter->y - sz;	pVB[22].z = pCenter->z + sz;	pVB[22].c = m_Color;
	pVB[23].x = pCenter->x - sz;	pVB[23].y = pCenter->y + sz;	pVB[23].z = pCenter->z + sz;	pVB[23].c = m_Color;
}

void i3VLoggerFramework::PushCmd( I3VLOG_HEADER * pHead, INT32 sz)
{
	switch( pHead->tag[3])
	{
		case I3VLOG_CMD_BEGIN	:
			m_pLineVB->reset();
			m_pTriVB->reset();

			m_BBox.Reset();

			_DrawAxis();
			break;

		case I3VLOG_CMD_END		:
			AfxGetMainWnd()->Invalidate();

			m_pViewerCtrl->Invalidate();
			break;

		case I3VLOG_CMD_COLOR	:
			{
				I3VLOG_PACK_COLOR * pInfo = (I3VLOG_PACK_COLOR *) pHead;

				I3ASSERT( sz == sizeof(I3VLOG_PACK_COLOR));

				m_Color = pInfo->color;
			}
			break;

		case I3VLOG_CMD_POINT	:
			{
				I3VLOG_PACK_POINT * pInfo = (I3VLOG_PACK_POINT *) pHead;

				I3ASSERT( sz == sizeof(I3VLOG_PACK_POINT));

				_DrawBox( (VEC3D *) &pInfo->pos, pInfo->scale);
			}
			break;

		case I3VLOG_CMD_LINE	:
			{
				I3VLOG_PACK_LINE * pInfo = (I3VLOG_PACK_LINE *) pHead;

				I3ASSERT( sz == sizeof(I3VLOG_PACK_LINE));

				_VTX * pVB = m_pLineVB->alloc( 2);

				pVB[0].x = pInfo->start.x;
				pVB[0].y = pInfo->start.y;
				pVB[0].z = pInfo->start.z;
				pVB[0].c = m_Color;

				pVB[1].x = pInfo->end.x;
				pVB[1].y = pInfo->end.y;
				pVB[1].z = pInfo->end.z;
				pVB[1].c = m_Color;

				m_BBox.ExtendByVec( (VEC3D *) &pInfo->start);
				m_BBox.ExtendByVec( (VEC3D *) &pInfo->end);
			}
			break;

		case I3VLOG_CMD_TRI		:
			{
				I3VLOG_PACK_TRI * pInfo = (I3VLOG_PACK_TRI *) pHead;

				I3ASSERT( sz == sizeof(I3VLOG_PACK_TRI));

				_VTX * pVB = m_pTriVB->alloc( 3);

				pVB[0].x = pInfo->p1.x;
				pVB[0].y = pInfo->p1.y;
				pVB[0].z = pInfo->p1.z;
				pVB[0].c = m_Color;

				pVB[1].x = pInfo->p2.x;
				pVB[1].y = pInfo->p2.y;
				pVB[1].z = pInfo->p2.z;
				pVB[1].c = m_Color;

				pVB[2].x = pInfo->p3.x;
				pVB[2].y = pInfo->p3.y;
				pVB[2].z = pInfo->p3.z;
				pVB[2].c = m_Color;

				m_BBox.ExtendByVec( (VEC3D *) &pInfo->p1);
				m_BBox.ExtendByVec( (VEC3D *) &pInfo->p2);
				m_BBox.ExtendByVec( (VEC3D *) &pInfo->p3);
			}
			break;
	}
}

void i3VLoggerFramework::OnRender(void)
{
	i3RenderContext * pCtx = GetViewer()->GetRenderContext();
	COLORREAL c;
	IDirect3DDevice9 * pD3D = (IDirect3DDevice9 *) pCtx->GetNativeDevice();

	i3Color::Set( &c, 1.0f, 1.0f, 1.0f, 1.0f);

	pCtx->SetLightingEnable( FALSE);
	pCtx->SetShader( NULL);
	pCtx->SetColor( &c);
	pCtx->SetBlendEnable( TRUE);
	pCtx->SetSrcBlend( I3G_BLEND_SRCALPHA);
	pCtx->SetDestBlend( I3G_BLEND_INVSRCALPHA);
	pCtx->SetVertexColorEnable( TRUE);
	pCtx->SetTextureEnable( I3G_TEXTURE_BIND_DIFFUSE, FALSE);
	pCtx->SetZWriteEnable( FALSE);

	{
		pD3D->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE);
	}

	if( m_pTriVB->getCount() > 0)
	{
		pCtx->DrawPrimUP( I3G_PRIM_TRILIST, m_pTriVB->getCount() / 3, m_pTriVB->getVertex(0), m_pTriVB->getVertexStride());
	}

	if( m_pLineVB->getCount() > 0)
	{
		pCtx->DrawPrimUP( I3G_PRIM_LINELIST, m_pLineVB->getCount() / 2, m_pLineVB->getVertex(0), m_pLineVB->getVertexStride());
	}
}

void	i3VLoggerFramework::OnKeyDown( UINT32 nKey)
{
	REAL32 step = 10.0f;
	bool bSetBound = false;

	if( ::GetKeyState( VK_SHIFT) & 0x8000)
		step = 100.0f;
	else if( ::GetKeyState( VK_CONTROL) & 0x8000)
		step = 1.0f;

	switch( nKey)
	{
		case VK_PRIOR :
			m_BoundSize += step;
			bSetBound = true;
			break;

		case VK_NEXT :
			m_BoundSize -= step;
			bSetBound = true;
			break;

		case 'Z' :
			SetBound( m_BBox.GetMin(), m_BBox.GetMax());
			break;
	}

	if( bSetBound)
	{
		VEC3D vmin, vmax;

		if( m_BoundSize < 1.0f)
			m_BoundSize = 1.0f;

		i3Vector::Set( &vmin, -m_BoundSize, -m_BoundSize, -m_BoundSize);
		i3Vector::Set( &vmax, m_BoundSize, m_BoundSize, m_BoundSize);

		SetBound( &vmin, &vmax);
	}

	i3ViewerFramework::OnKeyDown( nKey);
}
