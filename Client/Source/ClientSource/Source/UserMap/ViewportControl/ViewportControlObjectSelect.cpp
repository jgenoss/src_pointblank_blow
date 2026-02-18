#include "pch.h"
#include "ViewportControlObjectSelect.h"

I3_CLASS_INSTANCE( CViewportControlObjectSelect, i3ElementBase);

CViewportControlObjectSelect::CViewportControlObjectSelect(void)
{
}

CViewportControlObjectSelect::~CViewportControlObjectSelect(void)
{
}

void CViewportControlObjectSelect::Create(void)
{
//	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_BANDSELECT));
}

void CViewportControlObjectSelect::DrawSelectRect( POINT pt)
{
	/*
	HDC dc;
	CRect rect;

	dc = ::GetDC( pCtx->m_pViewport->m_hWnd);

	rect.left = MIN( pCtx->m_DragStartPt.x, pt.x);
	rect.top = MIN( pCtx->m_DragStartPt.y, pt.y);
	rect.right = MAX( pCtx->m_DragStartPt.x, pt.x);
	rect.bottom = MAX( pCtx->m_DragStartPt.y, pt.y);

	::DrawFocusRect( dc, &rect);

	::ReleaseDC( pCtx->m_pViewport->m_hWnd, dc);
	*/
}

void CViewportControlObjectSelect::BeginMultiSelect( POINT pt)
{
	/*
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	pFramework->PendingUpdate();

	pCtx->m_OldDragPt = pt;

	pFramework->SaveMatrix();

	DrawSelectRect( pCtx, pt);
	*/
}

void CViewportControlObjectSelect::MultiSelecting( POINT pt)
{
	/*
	DrawSelectRect( pCtx, pCtx->m_OldDragPt);

	pCtx->m_OldDragPt = pt;

	DrawSelectRect( pCtx, pt);
	*/
}

void CViewportControlObjectSelect::EndMultiSelect( POINT pt)
{
	/*
	PICK_TYPE type = PICK_SELECT;
	SEL_MODE mode = SEL_EXCLUSIVE;

	if( GetKeyState( VK_LCONTROL) & 0x8000)
	{
		mode = SEL_INCLUSIVE;
	}
	else if( GetKeyState( VK_SHIFT) & 0x8000)
	{
		mode = SEL_ALTERNATIVE;
	}
	else if( GetKeyState( VK_LMENU) & 0x8000)
	{
		type = PICK_UNSELECT;
	}

	i3LevelScene * pScene = i3Level::GetScene();

	pScene->PickBand( pCtx->m_pViewport, pCtx->m_DragStartPt, pt, type, mode);
	*/
}


void CViewportControlObjectSelect::OnLButtonDown( UINT nFlags, POINT point)
{
	//BeginMultiSelect( pCtx, point);
}

void CViewportControlObjectSelect::OnLButtonUp( UINT nFlags, POINT point)
{
	/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3LevelFramework * pFramework = pViewport->getFramework();

	DrawSelectRect( pCtx, pCtx->m_OldDragPt);
	pFramework->RestoreUpdate();

	INT32 ddx, ddy;

	ddx = pCtx->m_DragStartPt.x - point.x;	ddx *= ddx;
	ddy = pCtx->m_DragStartPt.y - point.y;	ddy *= ddy;

	if( (ddx <= 9) && (ddy <= 9))
	{
		PICK_TYPE type	= PICK_SELECT;
		SEL_MODE mode	= SEL_EXCLUSIVE;

		if( nFlags & MK_CONTROL)
			mode = SEL_INCLUSIVE;
		
		if( nFlags & MK_SHIFT)
			mode = SEL_ALTERNATIVE;
			
		if( GetKeyState( VK_LMENU) & 0x8000)
			type = PICK_UNSELECT;

		i3LevelScene * pScene = i3Level::GetScene();

		if( pScene != NULL)
		{
			pScene->Pick( point, type, mode);
		}
	}
	else
	{
		EndMultiSelect( pCtx, point);
	}

	pViewport->Invalidate();
	*/
}

void CViewportControlObjectSelect::OnMouseMove( UINT nFlags, POINT point)
{
	/*
	if( pCtx->m_bDragging)
	{
		MultiSelecting( pCtx, point);
	}*/
}
