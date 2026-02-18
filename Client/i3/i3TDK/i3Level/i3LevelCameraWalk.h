#if !defined( __I3_Level_CAMERA_WALK_H)
#define __I3_Level_CAMERA_WALK_H

#include "i3LevelCamera.h"

class I3_EXPORT_TDK i3LevelCameraWalk : public i3LevelCamera
{
	I3_EXPORT_CLASS_DEFINE( i3LevelCameraWalk, i3LevelCamera);

protected:
	bool	m_bDrag = false;

	bool	m_bMoveAhead = false;
	REAL32	m_MoveAheadFactor = 0.0f;

	bool	m_bMoveSide = false;
	REAL32	m_MoveSideFactor = 0.0f;

	VEC3D	m_DragStartRot;

protected:
	void	MoveAhead( REAL32 factor);
	void	MoveSide( REAL32 factor);
	void	LookAt( REAL32 xDeg, REAL32 yDeg);

public:
	i3LevelCameraWalk(void);
};

#endif