#pragma once

#include "stdafx.h"

/*
enum	I3_VIEWER_CAMMODE
{
	I3_VIEWER_CAM_NONE			= 0,
	I3_VIEWER_CAM_PIVOT_ROTATE,
	I3_VIEWER_CAM_PAN,
	I3_VIEWER_CAM_ROTATE,
};
//*/

class CViewerInput
{
public:
	CViewerInput(void);
	CViewerInput(i3Viewer*	pViewer);
	~CViewerInput(void);

private:
	i3Viewer				*	m_pViewer;
	I3_VIEWER_CAMMODE			m_CamMode;

	HWND						m_hWnd;

	POINT						m_DragStartPt;
	MATRIX						m_DragStartMatrix;

public:
	void	OnLButtonDown	(UINT nFlags,POINT pt);
	void	OnLButtonUp		(UINT nFlags,POINT pt);

	void	OnRButtonDown	(UINT nFlags,POINT pt);
	void	OnRButtonUp		(UINT nFlags,POINT pt);

	void	OnMButtonDown	(UINT nFlags,POINT pt);
	void	OnMButtonUp		(UINT nFlags,POINT pt);

	void	OnMouseMove		(UINT nFlags,POINT pt);
	void	OnMouseWheel	(UINT nFlags,short zDelta , POINT pt);

protected:
	void	OnPan			(UINT nFlags , POINT pt);
	void	OnRotate		(UINT nFlags , POINT pt);
	void	OnPivotRotate	(UINT nFlags , POINT pt);

private:
	void	Pan				(REAL32	dx	 , REAL32	dy , REAL32 dz);
	void	Rotate			(REAL32	RotX , REAL32 RotY , REAL32 RotZ);
	void	PivotRotate		(REAL32 RotX,	REAL32 RotY , REAL32 RotZ);
	void	ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec1, REAL32 zDistance );
	void	Zoom			( REAL32 rate);
};
