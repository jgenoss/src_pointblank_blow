#include "stdafx.h"
#include "i3LevelViewportControlObjectRotate.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "../i3TDKGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelViewportControlObjectRotate);

i3LevelViewportControlObjectRotate::~i3LevelViewportControlObjectRotate(void)
{
	I3_SAFE_NODE_RELEASE( m_pTextNode);
}

void i3LevelViewportControlObjectRotate::Create(void)
{
	m_pAxis = i3LevelAxisRotate::new_object();
	m_pAxis->Create();

	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_ROTATE));
}

bool i3LevelViewportControlObjectRotate::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonDown( pCtx, nFlags, point );

	i3LevelFramework * pFramework = pCtx->m_pFramework;
	pFramework->SaveMatrix();

	if( m_pTextNode == nullptr)
	{
		i3Font * pFont;

		pFont = pFramework->getDebugFont();

		m_pTextNode = i3TextNode::new_object();
		m_pTextNode->Create( pFont, 64);
		m_pTextNode->SetEnable( TRUE);
		m_pTextNode->SetSize( 128, 32);

		pFramework->Get2DRoot()->AddChild( m_pTextNode);
	}

	{
		m_pTextNode->RemoveFlag( I3_NODEFLAG_DISABLE);

		m_pTextNode->SetText( L"0");

		// 위치
		{
			VEC3D pos = pCtx->m_pViewport->GetScreenPos( *m_pAxis->getPos());

			m_pTextNode->SetPos( pos.x, pos.y);
		}
	}

	return true;
}

bool i3LevelViewportControlObjectRotate::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonUp( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta());

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelUndo::ADD_Transform( pFramework->getScene());

	if( m_pTextNode != nullptr)
	{
		m_pTextNode->AddFlag( I3_NODEFLAG_DISABLE);
	}

	return false;
}

bool i3LevelViewportControlObjectRotate::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	if( pCtx->m_bDragging)
	{
		// World-Space의 Point로 변환
		MATRIX mtx, inv;
		i3LevelViewport * pView;

		pView = i3LevelViewport::GetCurrentFocusViewport();

		pFramework->GetViewProjectionMatrix( &mtx);
		i3Matrix::Inverse( &inv, nullptr, &mtx);

		VEC3D s, e;
		REAL32 rw, rh, hw, hh;

		hw = (REAL32)(pView->GetViewWidth() >> 1);
		hh = (REAL32)(pView->GetViewHeight() >> 1);

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &s, (pCtx->m_DragStartPt.x - hw) * rw, (pCtx->m_DragStartPt.y - hh) * rh, 0.0f);
		i3Vector::Set( &e, (point.x - hw) * rw, (point.y - hh) * rh, 0.0f);

		// World-Space에서의 시작점 끝점.
		i3Vector::TransformCoord( &s, &s, &inv);
		i3Vector::TransformCoord( &e, &e, &inv);



		i3LevelAxisRotate * pAxis = (i3LevelAxisRotate *) getAxis();

		VEC3D diff, axis;
		i3Vector::Sub( &diff, pAxis->getPos(), pAxis->getPickedPos());

		i3Vector::Normalize( &diff, &diff);

		// 현재 보는 View에서 회전축에 맞추어 Axis가 회전하는 Moment Vector를 구한다.
		i3Vector::Cross( &axis, &diff, pFramework->GetAxisMask());
		i3Vector::Normalize( &axis, &axis);

		// Dragging Vector를 axis Vector 방향에서의 길이로 변환한다.
		i3Vector::Sub( &diff, &e, &s);

		REAL32 rot = i3Vector::Dot( &axis, &diff) * 10.0f;

		VEC3D rotVec;
		i3Vector::Scale( &rotVec, pFramework->GetAxisMask(), rot);

		bool bSnap = (GetAsyncKeyState( VK_LMENU) & 0x8000) == 0;

		g_pScene->Rotating( pFramework->getSpaceMode(), rotVec.x, rotVec.y, rotVec.z, bSnap);

		pFramework->OnSelect( nullptr );

		{
			wchar_t conv[256];

			if( bSnap )
			{
				rot = g_pScene->getSnappedValue_Rotation( rot);
			}

			swprintf_s( conv, _countof( conv), L"%.1f", I3_RAD2DEG( rot));

			m_pTextNode->SetText( conv);
		}
	}

	return false;
}
