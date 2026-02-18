#include "stdafx.h"
#include "i3LevelCameraWalk.h"
#include "i3LevelViewport.h"

I3_CLASS_INSTANCE( i3LevelCameraWalk);

i3LevelCameraWalk::i3LevelCameraWalk(void)
{
	SetName( "Walk");
}

void	i3LevelCameraWalk::MoveAhead( REAL32 factor)
{
	MATRIX * pCam = GetTransform();
	VEC3D At;
	VEC3D diff;

	i3Vector::Copy( &At, i3Matrix::GetAt( pCam));
	i3Vector::SetY( &At, 0.0f);
	i3Vector::Normalize( &At, &At);

	i3Vector::Copy( &diff, getPos());
	i3Vector::ScaleAdd( &diff, &At, factor);

	setPos( &diff);
}

void	i3LevelCameraWalk::MoveSide( REAL32 factor)
{
	MATRIX * pCam = GetTransform();
	VEC3D Right;
	VEC3D diff;

	i3Vector::Copy( &Right, i3Matrix::GetRight( pCam));
	i3Vector::SetY( &Right, 0.0f);
	i3Vector::Normalize( &Right, &Right);

	i3Vector::Copy( &diff, getPos());
	i3Vector::ScaleAdd( &diff, &Right, factor);

	setPos( &diff);
}

void i3LevelCameraWalk::LookAt( REAL32 xDeg, REAL32 yDeg)
{
	VEC3D rot;

	i3Vector::Set( &rot, xDeg, yDeg, 0.0f);
	i3Vector::Add( &rot, &m_DragStartRot, &rot);

	SetRotate( rot.x, rot.y, rot.z);

	i3LevelViewport::GetCurrentFocusViewport()->InvalidateRect( nullptr);
}

/*
void i3LevelCameraWalk::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void i3LevelCameraWalk::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void i3LevelCameraWalk::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_bDrag = true;

	i3Vector::Copy( &m_DragStartRot, GetRotate());
	m_DragStartPt = point;
}

void i3LevelCameraWalk::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = false;
}

void i3LevelCameraWalk::OnRButtonDown(UINT nFlags, CPoint point)
{
	
}

void i3LevelCameraWalk::OnRButtonUp(UINT nFlags, CPoint point)
{
	
}

void i3LevelCameraWalk::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		REAL32 amount = 0.3f;
		REAL32 xDeg, yDeg;

		if( nFlags & MK_SHIFT)
			amount = 0.4f;

		xDeg = (REAL32)(point.y - m_DragStartPt.y) * amount;
		yDeg = (REAL32)(point.x - m_DragStartPt.x) * amount;

		LookAt( I3_DEG2RAD(-xDeg), I3_DEG2RAD(-yDeg));
	}

	i3LevelCamera::OnMouseMove( nFlags, point);
}

BOOL i3LevelCameraWalk::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return i3LevelCamera::OnMouseWheel( nFlags, zDelta, pt);
}

void i3LevelCameraWalk::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar)
	{
		case VK_UP :	
			m_MoveAheadFactor = -1.0f;	
			m_bMoveAhead = true;
			break;

		case VK_DOWN :	
			m_MoveAheadFactor = 1.0f;
			m_bMoveAhead = true;
			break;

		case VK_LEFT :	
			m_MoveSideFactor = -1.0f;	
			m_bMoveSide = true;
			break;

		case VK_RIGHT :	
			m_MoveSideFactor = 1.0f;
			m_bMoveSide = true;
			break;
	}
}

void i3LevelCameraWalk::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar)
	{
		case VK_UP :	
		case VK_DOWN :	m_bMoveAhead = false;
			break;

		case VK_LEFT :	
		case VK_RIGHT :	m_bMoveSide = false;
			break;
	}
}

#define MOVE_SPEED	(20.0f)

void i3LevelCameraWalk::OnDrive( REAL32 time)
{
	if( m_bMoveAhead)
	{
		MoveAhead( m_MoveAheadFactor * time * MOVE_SPEED);
	}

	if( m_bMoveSide)
	{
		MoveSide( m_MoveSideFactor * time * MOVE_SPEED);
	}
}
*/