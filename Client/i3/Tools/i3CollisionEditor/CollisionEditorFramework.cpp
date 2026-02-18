#include "stdafx.h"
#include "CollisionEditorFramework.h"
#include "GlobalVar.h"

I3_CLASS_INSTANCE( CollisionEditorFramework, i3ViewerFramework);

CollisionEditorFramework::CollisionEditorFramework(void)
{
	m_bMultiSelect = false;
}

CollisionEditorFramework::~CollisionEditorFramework(void)
{
}

void CollisionEditorFramework::_DrawSelectRect(void)
{
	HWND hwnd = GetViewer()->GetWindowHandle();
	HDC dc = ::GetDC( hwnd);
	int old;

	old = ::SetROP2( dc, R2_XORPEN);

	::DrawFocusRect( dc, &m_OldDragRect);

	::SetROP2( dc, old);
	::ReleaseDC( hwnd, dc);
}

void	CollisionEditorFramework::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3ViewerFramework::OnLButtonDown( nFlag, point);

	if( !(nFlag & MK_CONTROL))
	{
		g_pProject->UnselectAllTri();
	}

	GetViewer()->SetPendingUpdateEnable( true);

	m_bMultiSelect = true;
	m_OldDragRect.SetRect( point.x, point.y, point.x, point.y);
	_DrawSelectRect();
}

void	CollisionEditorFramework::OnLButtonUp( UINT32 nFlag, POINT point)
{
	BOOL bSelect, bBothSide;

	i3ViewerFramework::OnLButtonUp( nFlag, point);

	_DrawSelectRect();

	GetViewer()->SetPendingUpdateEnable( false);

	{
		I3RECT rt;

		rt.left		= (REAL32) m_OldDragRect.left;
		rt.top		= (REAL32) m_OldDragRect.top;
		rt.right	= (REAL32) m_OldDragRect.right;
		rt.bottom	= (REAL32) m_OldDragRect.bottom;

		rt.left = (rt.left / GetViewer()->GetViewWidth() * 2.0f) - 1.0f;
		rt.top = (rt.top / GetViewer()->GetViewHeight() * -2.0f) + 1.0f;
		rt.right = (rt.right / GetViewer()->GetViewWidth() * 2.0f) - 1.0f;
		rt.bottom = (rt.bottom / GetViewer()->GetViewHeight() * -2.0f) + 1.0f;

		i3Math::NormalizeRect( &rt);

		MATRIX mtx;
		i3Camera * pCam = GetCurrentCamera();

		i3Matrix::Mul( &mtx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectMatrixAttr()->GetMatrix());

		bSelect = TRUE;
		bBothSide = TRUE;

		if( nFlag & MK_SHIFT)
			bBothSide = FALSE;	

		if( GetKeyState( VK_MENU) & 0x8000 )
			bSelect = FALSE;

		g_pProject->SelectTri( &rt, &mtx, bSelect, bBothSide);
	}

	m_bMultiSelect = false;
}

void	CollisionEditorFramework::OnMouseMove( UINT32 nFlag, POINT point)
{
	i3ViewerFramework::OnMouseMove( nFlag, point);

	if( m_bMultiSelect)
	{
		_DrawSelectRect();

		m_OldDragRect.left		= MIN( m_DragStartPt.x, point.x);
		m_OldDragRect.top		= MIN( m_DragStartPt.y, point.y);
		m_OldDragRect.right		= MAX( m_DragStartPt.x, point.x);
		m_OldDragRect.bottom	= MAX( m_DragStartPt.y, point.y);

		_DrawSelectRect();
	}	
}