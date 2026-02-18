#include "StdAfx.h"
#include ".\viewerinput.h"

CViewerInput::CViewerInput(void)
{
	m_pViewer = NULL;
}

CViewerInput::CViewerInput(i3Viewer * pViewer)
{
	m_pViewer	= pViewer;
	m_hWnd		= m_pViewer->GetWindowHandle();
}

CViewerInput::~CViewerInput(void)
{
	m_pViewer = NULL;
}

void	CViewerInput::OnLButtonDown(UINT nFlags,POINT pt)
{
	if(m_pViewer)
	{
		i3Matrix::Copy( &m_DragStartMatrix, m_pViewer->GetCurrentCamera()->GetMatrix());
		::SetCapture(m_hWnd);
	}

	m_CamMode = I3_VIEWER_CAM_PIVOT_ROTATE;
	m_DragStartPt = pt;
}

void	CViewerInput::OnLButtonUp(UINT nFlags,POINT pt)
{
	m_CamMode = I3_VIEWER_CAM_NONE;
	::ReleaseCapture();
}

void	CViewerInput::OnRButtonDown(UINT nFlags,POINT pt)
{
	if(m_pViewer)
	{
		::SetCapture(m_hWnd);
		i3Matrix::Copy( &m_DragStartMatrix, m_pViewer->GetCurrentCamera()->GetMatrix());
	}

	m_CamMode		= I3_VIEWER_CAM_ROTATE;
	m_DragStartPt	= pt;
}


void	CViewerInput::OnRButtonUp(UINT nFlags,POINT pt)
{
	::ReleaseCapture();

	m_CamMode = I3_VIEWER_CAM_NONE;
}

void	CViewerInput::OnMButtonDown(UINT nFlags,POINT pt)
{	
	if(m_pViewer)
	{
		::SetCapture(m_hWnd);
		i3Matrix::Copy( &m_DragStartMatrix,  m_pViewer->GetCurrentCamera()->GetMatrix());
	}

	m_CamMode = I3_VIEWER_CAM_PAN;
	m_DragStartPt = pt;
}

void	CViewerInput::OnMButtonUp(UINT nFlags,POINT pt)
{
	::ReleaseCapture();

	m_CamMode = I3_VIEWER_CAM_NONE;
}

void	CViewerInput::OnMouseMove(UINT nFlags,POINT pt)
{
	if(m_pViewer == NULL)	return ;

	i3Camera * pCamera = m_pViewer->GetCurrentCamera();

	switch(m_CamMode)
	{
		case I3_VIEWER_CAM_PIVOT_ROTATE :	OnPivotRotate(nFlags , pt);			break;
		case I3_VIEWER_CAM_PAN :			OnPan(nFlags , pt);					break;
		case I3_VIEWER_CAM_ROTATE :			OnRotate(nFlags , pt);				break;
		case I3_VIEWER_CAM_NONE:			break;
	}
}

void	CViewerInput::OnMouseWheel(UINT nFlags,short zDelta , POINT pt)
{
	REAL32			ZoomRate = (REAL32) (zDelta);

	Zoom(ZoomRate);
}

///////////////////////////////////////////////////////////////////////////////////
void	CViewerInput::OnPan(UINT nFlags,POINT point)//평행이동
{
	/*
	VEC3D vec1;
	VEC3D end1;
	VEC3D start1;
	REAL32 dist;

	VEC3D			vecBound = m_pViewer->GetBasePos();
	i3Camera*		pCamera  = m_pViewer->GetCurrentCamera();

	i3Vector::Sub( &vec1, i3Matrix::GetPos(pCamera->GetMatrix()), &vecBound);

	dist = i3Vector::Length( &vec1);
	if( dist <= I3_EPS)
		dist = 1.0f;

	ScreenToWorld( point.x, point.y, &end1, dist);
	ScreenToWorld( m_DragStartPt.x, m_DragStartPt.y, &start1, dist);

	i3Vector::Sub( &vec1, &end1, &start1);
	
	Pan( getX(&vec1), getY(&vec1), getZ(&vec1));
	//*/
}

void CViewerInput::OnPivotRotate(UINT nFlags,POINT point)//물체좌표기준으로 회전
{
	/*
	REAL32 amount = 0.4f;
	REAL32 xDeg, yDeg;

	VEC3D			vecBound = m_pViewer->GetBasePos();
	i3Camera*		pCamera  = m_pViewer->GetCurrentCamera();
	
	if( nFlags & MK_SHIFT)
		amount = 0.8f;
	else if( nFlags & MK_ALT)
		amount = 0.05f;

	xDeg = (REAL32)(point.x - m_DragStartPt.x) * amount;
	yDeg = (REAL32)(point.y - m_DragStartPt.y) * amount;

	PivotRotate( I3_DEG2RAD(xDeg), I3_DEG2RAD(yDeg), 0.0f);
	//*/
}

void	CViewerInput::OnRotate(UINT nFlags, POINT point)
{
	/*
	REAL32 amount = 0.1f;
	REAL32 xDeg, yDeg;
	i3Camera*		pCamera = m_pViewer->GetCurrentCamera();

	if( nFlags & MK_SHIFT)
		amount = 0.2f;
	else if( nFlags & MK_ALT)
		amount = 0.01f;

	xDeg = -(REAL32)(point.y - m_DragStartPt.y) * amount;
	yDeg = (REAL32)(point.x - m_DragStartPt.x) * amount;

	Rotate( I3_DEG2RAD(-xDeg), I3_DEG2RAD(yDeg), 0.0f);
	//*/
}


////////////////////////////////////////////////////////////////////////////
void	CViewerInput::Pan(REAL32  dx, REAL32  dy, REAL32 dz)
{
	VEC3D vecRight, vecUp, vecAt;
	MATRIX * pCam;
	MATRIX * pMatCam = m_pViewer->GetCurrentCamera()->GetMatrix();
	pCam = &m_DragStartMatrix;

	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCam));
	i3Vector::Copy( &vecUp, i3Matrix::GetUp( pCam));
	i3Vector::Copy( &vecAt, i3Matrix::GetAt( pCam));

	i3Vector::Normalize( &vecRight, &vecRight);
	i3Vector::Normalize( &vecUp, &vecUp);
	i3Vector::Normalize( &vecAt, &vecAt);

	i3Vector::Scale( &vecRight, &vecRight, -dx);
	i3Vector::Scale( &vecUp, &vecUp, dy);
	i3Vector::Scale( &vecAt, &vecAt, dz);

	i3Vector::Add( &vecRight, &vecRight, &vecUp);
	i3Vector::Add( &vecRight, &vecRight, &vecAt);

	{
		MATRIX tmp;
		
		i3Matrix::Identity( &tmp);

		i3Matrix::SetPos( &tmp, &vecRight);

		i3Matrix::Mul( pMatCam, &m_DragStartMatrix, &tmp);
	}
}

void CViewerInput::Rotate(REAL32 RotX,	REAL32 RotY , REAL32 RotZ)
{
	MATRIX * pOldCam	= &m_DragStartMatrix;
	MATRIX * pCam		= m_pViewer->GetCurrentCamera()->GetMatrix();

//	VEC3D  * vecBeginPos = i3Matrix::GetPos(pOldCam);
	
	VEC3D  * vecEndPos	 = i3Matrix::GetAt(pCam);

///	i3Vector::Normalize(vecViewDir,vecEndPos);
	
   // INT32	Dist = 


	//i3Vector::Sub(

	i3Matrix::Copy( pCam, pOldCam);

	i3Matrix::PostRotateAxis( pCam, i3Matrix::GetUp( pOldCam), RotY);
	i3Matrix::PostRotateAxis( pCam, i3Matrix::GetRight( pOldCam), RotX);
}

void CViewerInput::PivotRotate(REAL32 RotX,	REAL32 RotY , REAL32 RotZ)
{
	/*
	// Camera matrix
	VEC3D vecRight, localPos;
	MATRIX * pCam	 = m_pViewer->GetCurrentCamera()->GetMatrix();
	VEC3D	 BasePos = m_pViewer->GetBasePos();

	RotX *= 5.0f;
	RotY *= 5.0f;

	i3Matrix::Copy( pCam, &m_DragStartMatrix);

	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCam), &BasePos);

	// make it in local space
	i3Matrix::SetPos( pCam, &localPos);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCam));
	i3Vector::Normalize( &vecRight, &vecRight);
	i3Matrix::PostRotateAxis( pCam, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCam, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCam), &BasePos);
	i3Matrix::SetPos( pCam, &localPos);

	//i3System::DCacheFlushAll();

	//i3Matrix::Dump( pCamera);
	//*/
}

void CViewerInput::Zoom( REAL32 rate)
{/*
	VEC3D vec;
	MATRIX tmp;
	
	MATRIX * pCam	 = m_pViewer->GetCurrentCamera()->GetMatrix();

	rate *= 2.0f;

	i3Vector::Copy( &vec, i3Matrix::GetAt( pCam));
	i3Vector::Normalize( &vec, &vec);
	i3Vector::Scale( &vec, &vec, rate);

	i3Matrix::SetTranslate( &tmp, getX(&vec), getY(&vec), getZ(&vec));

	i3Matrix::Mul( pCam, pCam, &tmp);
	*/
	m_pViewer->Zoom(rate);
}

void CViewerInput::ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec1, REAL32 zDistance)
{
	MATRIX mat;
	VEC3D vec1, vec2, dir;
	REAL32 dx, dy;

	i3Matrix::Inverse( &mat, NULL, m_pViewer->GetCurrentCamera()->GetProjectionMatrix());

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)scrX / m_pViewer->GetViewWidth() * 2.0f) - 1.0f;
	dy = ((REAL32)scrY / m_pViewer->GetViewHeight() * 2.0f) - 1.0f;

	i3Vector::Set( &vec1, dx, dy, 0.1f);
	i3Vector::Set( &vec2, dx, dy, 0.5f);

	i3Vector::TransformCoord( &vec1, &vec1, &mat);
	i3Vector::TransformCoord( &vec2, &vec2, &mat);

	i3Vector::Sub( &dir, &vec2, &vec1);

	i3Vector::Normalize( &dir, &dir);

	i3Vector::Scale( pVec1, &dir, zDistance);
}


