#include "pch.h"
#include "ViewportControl.h"
#include "../StageMapEditor.h"

I3_ABSTRACT_CLASS_INSTANCE( CViewportControl, i3GameNode);

static CGizmo *		s_pDefaultGizmo = NULL;
static HCURSOR		s_hCursor		= NULL;

CViewportControl::CViewportControl(void)
{
	if( s_pDefaultGizmo == NULL)
	{
		s_pDefaultGizmo	= CGizmo::NewObject();
		s_pDefaultGizmo->Create();

		s_hCursor	= ::LoadCursor( NULL, MAKEINTRESOURCE( IDC_ARROW));
	}

	m_pGizmo		= NULL;
	m_hCursor		= NULL;

	m_pMap			= NULL;
	m_pStage		= NULL;
	m_pCtrlCtx		= NULL;

	m_bMovingEnable = false;
}

CViewportControl::~CViewportControl(void)
{
	I3_SAFE_RELEASE( m_pGizmo);

	if( m_hCursor != NULL)
	{
		DestroyCursor( m_hCursor);
		m_hCursor = NULL;
	}
}

void CViewportControl::AttachGizmo(void)
{
	DetachGizmo();

	if( m_pGizmo != NULL)
	{
		if( m_pGizmo->getSceneObject() != NULL)
		{
			g_pFramework->Get3DRoot()->AddChild( m_pGizmo->getSceneObject());
		}
	}
}

void CViewportControl::DetachGizmo(void)
{
	if( m_pGizmo != NULL)
	{
		if( m_pGizmo->getSceneObject() != NULL)
		{
			m_pGizmo->getSceneObject()->RemoveFromParent();
		}
	}
}

void CViewportControl::Create( CStageMapEditor * pStage)
{
	m_pStage = pStage;

	m_pMap = m_pStage->getMap();

	if( m_pStage != NULL)
	{
		m_pCtrlCtx = m_pStage->getViewportCtrlCtx();
	}
}

CGizmo *	CViewportControl::GetDefaultGizmo(void)
{
	return s_pDefaultGizmo;
}

HCURSOR CViewportControl::GetDefaultCursor(void)
{
	return s_hCursor;
}

void CViewportControl::LockFocus(bool bLock)
{
	if( m_pCtrlCtx->m_bLockedFocus == bLock)	return;

	if( bLock)
	{
		g_pFramework->SetCapture( this);

		SetCapture( g_pViewer->GetWindowHandle());
	}
	else
	{
		g_pFramework->ReleaseCapture();

		ReleaseCapture();
	}

	m_pCtrlCtx->m_bLockedFocus = bLock;
}

void CViewportControl::OnLButtonDown( UINT nFlags, POINT point)
{
	m_pCtrlCtx->m_bLBDragging = true;

	m_pCtrlCtx->m_DragStartPt = point;
	m_pCtrlCtx->m_OldDragPt = point;

	if( m_pGizmo != NULL && m_pGizmo->isCollided())
	{
		LockFocus( true);
	}
}

void CViewportControl::OnLButtonUp( UINT nFlags, POINT point)
{
	OnEnd();
}

void CViewportControl::OnMButtonDown( UINT nFlags, POINT point)
{
}

void CViewportControl::OnMButtonUp( UINT nFlags, POINT point)
{
	
}

void CViewportControl::OnRButtonDown( UINT nFlags, POINT point)
{
}

void CViewportControl::OnRButtonUp( UINT nFlags, POINT point)
{
}

void CViewportControl::OnMouseMove( UINT nFlags, POINT point)
{
	if( m_pCtrlCtx->m_bLockedFocus == false)
	{
		MATRIX viewproj;
		VEC3D vStart, vEnd;
		m_pStage->GetViewProjectionMatrix( &viewproj);
		m_pStage->ScreenToWorld( point, &vStart, &vEnd);
		
		m_pGizmo->OnCollision( &vStart, &vEnd, &viewproj);
	}
	else
	{
		if( m_bMovingEnable == false)
		{
			INT32 x = point.x - m_pCtrlCtx->m_DragStartPt.x;
			INT32 y = point.y - m_pCtrlCtx->m_DragStartPt.y;

			if( i3Math::abs( x) > 1 || i3Math::abs( y) > 1)
			{
				OnStart();
			}
		}
		else
		{

		}
	}
}

void CViewportControl::OnMouseWheel( UINT nFlags, short zDelta, POINT pt)
{
	
}

void CViewportControl::OnKeyDown(  UINT nChar)
{
	
}

void CViewportControl::OnKeyUp( UINT nChar)
{
}

bool CViewportControl::OnStart()
{
	RECT rt;
	GetWindowRect( g_pViewer->GetWindowHandle(), &rt);

	SetMovingEnable();

	ClipCursor( &rt);

	return true;
}

bool CViewportControl::OnEnd()
{
	m_pCtrlCtx->m_bLBDragging = false;

	SetMovingEnable( false);

	LockFocus( false);

	ClipCursor( NULL);

	return false;
}

bool CViewportControl::OnPopupMenu( CMenu * pMenu)
{
	return false;
}
