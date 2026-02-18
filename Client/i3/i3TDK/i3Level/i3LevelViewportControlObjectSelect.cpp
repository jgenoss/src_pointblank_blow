#include "stdafx.h"
#include "i3LevelViewportControlObjectSelect.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "../i3TDKGlobalVariable.h"
#include "i3NavMeshDataMgr.h"

I3_CLASS_INSTANCE( i3LevelViewportControlObjectSelect);

void i3LevelViewportControlObjectSelect::Create(void)
{
	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_BANDSELECT));
}

void i3LevelViewportControlObjectSelect::DrawSelectRect( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt)
{
	HDC dc;
	CRect rect;

	dc = ::GetDC( pCtx->m_pViewport->m_hWnd);

	rect.left = MIN( pCtx->m_DragStartPt.x, pt.x);
	rect.top = MIN( pCtx->m_DragStartPt.y, pt.y);
	rect.right = MAX( pCtx->m_DragStartPt.x, pt.x);
	rect.bottom = MAX( pCtx->m_DragStartPt.y, pt.y);

	::DrawFocusRect( dc, &rect);

	::ReleaseDC( pCtx->m_pViewport->m_hWnd, dc);
}

void i3LevelViewportControlObjectSelect::BeginMultiSelect( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt)
{
	//i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	m_bMultiSelecting = TRUE;

	pFramework->PendingUpdate();

	pCtx->m_OldDragPt = pt;

	pFramework->SaveMatrix();

	//DrawSelectRect( pCtx, pt);
}

void i3LevelViewportControlObjectSelect::MultiSelecting( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt)
{
	if( m_bMultiSelecting == TRUE)
	{
		DrawSelectRect( pCtx, pCtx->m_OldDragPt);

		pCtx->m_OldDragPt = pt;

		DrawSelectRect( pCtx, pt);
	}
}

void i3LevelViewportControlObjectSelect::EndMultiSelect( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt)
{
	if( m_bMultiSelecting == TRUE)
	{
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

		m_bMultiSelecting = FALSE;
	}
}


bool i3LevelViewportControlObjectSelect::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	BeginMultiSelect( pCtx, point);

	return true;
}

bool i3LevelViewportControlObjectSelect::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
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

		if( pScene != nullptr)
		{
			pScene->Pick( point, type, mode);
		}
	}
	else
	{
		EndMultiSelect( pCtx, point);
	}

	pViewport->Invalidate();

	return false;
}

bool i3LevelViewportControlObjectSelect::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	if( pCtx->m_bDragging)
	{
		MultiSelecting( pCtx, point);
	}

	return true;
}
