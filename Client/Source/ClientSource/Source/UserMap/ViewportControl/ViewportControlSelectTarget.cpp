#include "pch.h"
#include "ViewportControlSelectTarget.h"

I3_CLASS_INSTANCE( CViewportControlSelectTarget, CViewportControl);

CViewportControlSelectTarget::CViewportControlSelectTarget(void)
{
}

CViewportControlSelectTarget::~CViewportControlSelectTarget(void)
{
}

void CViewportControlSelectTarget::Create(void)
{
//	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_SELECT_TARGET));
}

void CViewportControlSelectTarget::DrawSelectRect( POINT pt)
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

void CViewportControlSelectTarget::BeginMultiSelect( POINT pt)
{
	//i3LevelViewport * pViewport = pCtx->m_pViewport;
	/*
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	pFramework->PendingUpdate();

	pCtx->m_OldDragPt = pt;

	pFramework->SaveMatrix();

	DrawSelectRect( pCtx, pt);*/
}

void CViewportControlSelectTarget::MultiSelecting( POINT pt)
{
	/*
	DrawSelectRect( pCtx, pCtx->m_OldDragPt);

	pCtx->m_OldDragPt = pt;

	DrawSelectRect( pCtx, pt);*/
}

void CViewportControlSelectTarget::EndMultiSelect( POINT pt)
{
	/*
	SEL_MODE mode = SEL_EXCLUSIVE;
	PICK_TYPE type = PICK_TARGET;

	if( GetKeyState( VK_LCONTROL) & 0x8000)
	{
		mode = SEL_INCLUSIVE;
	}
	else if( GetKeyState( VK_LMENU) & 0x8000)
	{
		type = PICK_UNTARGET;
	}

	i3LevelScene * pScene = i3Level::GetScene();

	pScene->PickBand( pCtx->m_pViewport, pCtx->m_DragStartPt, pt, type, mode);
	*/
}


void CViewportControlSelectTarget::OnLButtonDown( UINT nFlags, POINT point)
{
	//BeginMultiSelect( pCtx, point);
}

void CViewportControlSelectTarget::OnLButtonUp( UINT nFlags, POINT point)
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
		PICK_TYPE type	= PICK_TARGET;
		SEL_MODE mode	= SEL_EXCLUSIVE;

		if( nFlags & MK_CONTROL)
			mode = SEL_INCLUSIVE;
		
		if( nFlags & MK_SHIFT)
			mode = SEL_ALTERNATIVE;
			
		if( GetKeyState( VK_LMENU) & 0x8000)
			type = PICK_UNTARGET;

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

void CViewportControlSelectTarget::OnMouseMove( UINT nFlags, POINT point)
{
	/*
	if( pCtx->m_bDragging)
	{
		MultiSelecting( pCtx, point);
	}*/
}

void CViewportControlSelectTarget::OnRButtonDown( UINT nFlags, POINT point)
{
	/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3LevelFramework * pFramework = pViewport->getFramework();

	pFramework->EndSelectTarget();
	*/
}