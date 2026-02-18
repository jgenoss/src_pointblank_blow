#include "pch.h"
#include "MapEditorCamera.h"

I3_CLASS_INSTANCE( CMapEditorCamera, i3EventReceiver);

CMapEditorCamera::CMapEditorCamera(void)
{
	m_pCamera		= NULL;
	m_LBDragging	= false;
	m_RBDragging	= false;
	m_bMoving		= false;

	m_posDragStart.x = m_posDragStart.y	= 0;
	m_posOld.x = m_posOld.y	= 0;
}

CMapEditorCamera::~CMapEditorCamera(void)
{
	I3_SAFE_RELEASE( m_pCamera);
}

void CMapEditorCamera::InitCamera( i3Camera * pCam)
{
	I3_REF_CHANGE( m_pCamera, pCam);

	m_pCamera->SetWindowSize( (REAL32) g_pViewer->GetViewWidth(), (REAL32) g_pViewer->GetViewHeight());
	m_pCamera->SetFOV( 45.0f);
	m_pCamera->SetNear( 0.1f);
	m_pCamera->SetFar( 1000.0f);

	MATRIX * pMtx = m_pCamera->GetMatrix();

	VEC3D vPos, vDir;
	i3Vector::Set( &vPos, -20.0f, 10.0f, -20.0f);

	i3Vector::Scale( &vDir, &vPos, - 1.0f);
	i3Vector::Normalize( &vDir, &vDir);

	REAL32 Theta	= i3Math::atan2( getX( &vDir), getZ( &vDir));
	REAL32 fLen		= i3Math::Sqrt( (getX(&vDir) * getX(&vDir)) + (getZ(&vDir) * getZ(&vDir)) );
	REAL32 Rho		= atan2f( getY(&vDir), fLen);

	i3Matrix::PreRotateY( pMtx, Theta + I3_DEG2RAD( 180.0f));
	i3Matrix::PostRotateAxis( pMtx, i3Matrix::GetRight( pMtx), Rho);

	i3Matrix::SetPos( pMtx, &vPos);
}

void CMapEditorCamera::OnChangeVideo( i3VideoInfo * pInfo)
{
	m_pCamera->SetWindowSize( (REAL32) pInfo->m_Width, (REAL32) pInfo->m_Height);
}

void CMapEditorCamera::OnMoveXY( REAL32 X, REAL32 Y)
{
	MATRIX * pCamMtx = m_pCamera->GetMatrix();

	VEC3D	dirUp, dirRight;

	i3Vector::Copy( &dirRight, i3Matrix::GetRight( pCamMtx));
	i3Vector::Scale( &dirRight, &dirRight, X * - 0.1f);

	i3Vector::Scale( &dirUp, &I3_YAXIS, Y * 0.1f);

	i3Vector::Add( i3Matrix::GetPos( pCamMtx), i3Matrix::GetPos( pCamMtx), &dirUp);
	i3Vector::Add( i3Matrix::GetPos( pCamMtx), i3Matrix::GetPos( pCamMtx), &dirRight);
}

void CMapEditorCamera::OnMoveAt( REAL32 rDelta)
{
	MATRIX * pCamMtx = m_pCamera->GetMatrix();

	VEC3D vAt;
	i3Vector::Copy( &vAt, i3Matrix::GetAt( pCamMtx));

	REAL32 div = i3Math::div( (REAL32)rDelta, 30.0f);

	i3Vector::Scale( &vAt, &vAt, - div * 0.5f);
	i3Vector::Add( &vAt, &vAt, i3Matrix::GetPos( pCamMtx));

	i3Matrix::SetPos( pCamMtx, &vAt);
}

void CMapEditorCamera::OnMoveZRotateX( REAL32 X, REAL32 Z)
{
	MATRIX * pCamMtx = m_pCamera->GetMatrix();

	VEC3D	vAt;
	i3Vector::Copy( &vAt, i3Matrix::GetAt( pCamMtx));
	i3Vector::SetY( &vAt, 0.0f);
	i3Vector::Normalize(&vAt, &vAt);

	i3Vector::Scale( &vAt, &vAt, Z * 0.2f);
	i3Vector::Add( i3Matrix::GetPos( pCamMtx), i3Matrix::GetPos( pCamMtx), &vAt);

	VEC3D	vUp;
	i3Vector::Set( &vUp, pCamMtx->m01, pCamMtx->m11, pCamMtx->m21);

	i3Matrix::PreRotateAxis( pCamMtx, &vUp, I3_DEG2RAD( - X) * 0.5f);
}

void CMapEditorCamera::OnRotateXY( REAL32 X, REAL32 Y)
{
	MATRIX * pMat = m_pCamera->GetMatrix();

	VEC3D  localPos, vecRight;
	i3Vector::Copy( &localPos, i3Matrix::GetPos( pMat));
	i3Matrix::SetPos( pMat, &localPos);

	i3Vector::Copy( &vecRight, i3Matrix::GetRight(pMat));
	i3Vector::Normalize( &vecRight, &vecRight);

	i3Matrix::PostRotateAxis( pMat, &vecRight, I3_DEG2RAD( - Y) * 0.5f);
	i3Matrix::PostRotateAxis( pMat, &I3_YAXIS, I3_DEG2RAD( - X) * 0.5f);

	i3Matrix::SetPos( pMat, &localPos);
}

void CMapEditorCamera::_SetMoveMode( bool bEnable)
{
	if( m_bMoving == bEnable)	return;

	if( bEnable )
	{
		g_pFramework->SetCapture( this);
		SetCapture( g_pViewer->GetWindowHandle());
		
		g_pFramework->SetGameCursor( FALSE);
	}
	else
	{
		g_pFramework->ReleaseCapture();
		ReleaseCapture();

		g_pFramework->SetGameCursor( TRUE);
	}

	m_bMoving = bEnable;
}

void CMapEditorCamera::_SetCursorPosition( POINT point)
{
	POINT moveTo;
	moveTo.x = point.x;
	moveTo.y = point.y;

	ClientToScreen( g_pViewer->GetWindowHandle(), &moveTo);

	SetCursorPos( moveTo.x, moveTo.y);
}

void CMapEditorCamera::OnLButtonDown( UINT32 nFlags, POINT point)
{
	m_LBDragging = true;

	m_posDragStart	= point;
	m_posOld		= point;
};

void CMapEditorCamera::OnLButtonUp( UINT32 nFlags, POINT point)
{
	if( m_LBDragging == true)
	{
		m_LBDragging = false;

		if( m_RBDragging == false)
		{
			_SetMoveMode( false);

			_SetCursorPosition( m_posDragStart);
		}
	}
};

void CMapEditorCamera::OnMButtonDown( UINT32 nFlags, POINT point)
{
	m_nMouseButtonState |= I3I_MOUSE_MBUTTON;
};

void CMapEditorCamera::OnMButtonUp( UINT32 nFlags, POINT point)
{
	m_nMouseButtonState &= ~I3I_MOUSE_MBUTTON;
};

void CMapEditorCamera::OnRButtonDown( UINT32 nFlags, POINT point)
{
	m_RBDragging = true;

	m_posDragStart	= point;
	m_posOld		= point;
};

void CMapEditorCamera::OnRButtonUp( UINT32 nFlags, POINT point)
{
	if( m_RBDragging == true)
	{
		m_RBDragging = false;

		if( m_LBDragging == false)
		{
			_SetMoveMode( false);

			_SetCursorPosition( m_posDragStart);
		}
	}
};

void CMapEditorCamera::OnMouseMove( UINT32 nFlags, POINT point)
{
	POINT ptDelta, ptDragStart;

	//이동 초기에 일정 이상의 움직임이 있는경우에만 이동하도록 체크
	if( m_bMoving == false && ( m_LBDragging || m_RBDragging))
	{
		ptDragStart.x = point.x - m_posDragStart.x;
		ptDragStart.y = point.y - m_posDragStart.y;

		if( i3Math::abs( ptDragStart.x) >= 2 || i3Math::abs( ptDragStart.y) >= 2)
		{
			_SetMoveMode( true);

			_SetCursorPosition( m_posDragStart);
		}

		return ;
	}

	// 이동
	ptDelta.x = point.x - m_posDragStart.x;
	ptDelta.y = point.y - m_posDragStart.y;
/*
	if( m_LBDragging )
	{
		if( m_RBDragging)
		{
			OnMoveXY( ptDelta.x, ptDelta.y);
		}
		else
		{
			OnMoveZRotateX( ptDelta.x, ptDelta.y);
		}
	}
	else if( m_RBDragging)
	{
		OnRotateXY( ptDelta.x, ptDelta.y);
	}
*/
	// 이동 시킨후 원래의 위치로 이동 : 화면 밖이라거나, 모니터 해상도 밖의 경우 절대좌표를 사용한 이동은 한계가 생긴다.
	if( m_bMoving && ( m_LBDragging || m_RBDragging))
	{
		_SetCursorPosition( m_posDragStart);
	}

	m_posOld = point;
};

void CMapEditorCamera::OnMouseWheel( UINT nFlags, short zDelta, POINT pt)
{
	OnMoveAt( zDelta);
};

void CMapEditorCamera::OnKeyDown( UINT32 nKey)
{
};

void CMapEditorCamera::OnKeyUp( UINT32 nKey)
{
};

void CMapEditorCamera::OnInput(void)
{
	i3InputDeviceManager * pInput = g_pViewer->GetInputDeviceManager();
	REAL32 rDeltaSeconds = g_pViewer->getDeltaTime();
	i3InputMouse * pMouse = pInput->GetMouse();

	if( pInput != NULL)
	{
		OnMouseMove( pMouse, rDeltaSeconds);
	}

	i3InputKeyboard * pKey = pInput->GetKeyboard();
	if( pKey != NULL)
	{
		OnKeyInput( pKey, rDeltaSeconds);
	}
}

void CMapEditorCamera::OnKeyInput( i3InputKeyboard * pKey, REAL32 rDeltaSeconds)
{
	REAL32 side = 0.0f, updown = 0.0f, frontback = 0.0f, siderot = 0.0f;

	if( pKey->GetPressState( I3I_KEY_A) == TRUE)		siderot -= 1.0f;
	if( pKey->GetPressState( I3I_KEY_D) == TRUE) 		siderot += 1.0f;
	if( pKey->GetPressState( I3I_KEY_W) == TRUE) 		frontback -= 1.0f;	
	if( pKey->GetPressState( I3I_KEY_S) == TRUE) 		frontback += 1.0f;
	if( pKey->GetPressState( I3I_KEY_SPACE) == TRUE) 	updown += 1.0f;
	if( pKey->GetPressState( I3I_KEY_X) == TRUE) 		updown -= 1.0f;
	if( pKey->GetPressState( I3I_KEY_Q) == TRUE) 		side += 1.0f;
	if( pKey->GetPressState( I3I_KEY_E) == TRUE)		side -= 1.0f;

	siderot		*= rDeltaSeconds * 60.0f;
	updown		*= rDeltaSeconds * 60.0f;
	frontback	*= rDeltaSeconds * 60.0f;
	siderot		*= rDeltaSeconds * 60.0f;

	if( i3Math::abs( siderot) > 0 || i3Math::abs( frontback) > 0)
	{
		OnMoveZRotateX( siderot, frontback);
	}

	if( i3Math::abs( side) > 0 || i3Math::abs( updown) > 0)
	{
		OnMoveXY( side, updown);
	}
}

void CMapEditorCamera::OnMouseMove( i3InputMouse * pMouse, REAL32 rDeltaSeconds)
{
	REAL32 X = pMouse->GetDeltaX();
	REAL32 Y = pMouse->GetDeltaY();

	if( i3Math::abs( X) > I3_EPS || i3Math::abs( Y) > I3_EPS)
	{
		X = X * rDeltaSeconds / 0.032f;
		Y = Y * rDeltaSeconds / 0.032f;

		if( m_bMoving)
		{
			if( m_LBDragging )
			{
				if( m_RBDragging)
				{
					OnMoveXY( X, Y);
				}
				else
				{
					OnMoveZRotateX( X, Y);
				}
			}
			else if( m_RBDragging)
			{
				OnRotateXY( X, Y);
			}
		}
	}
/*
	if( i3Math::abs( Z) > 30.0f)
	{
		OnMoveAt( Z);
	}*/
}