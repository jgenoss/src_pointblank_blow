#if !defined( _CGAME_CAMERA_FOLLOW_H__)
#define	_CGAME_CAMERA_FOLLOW_H__

#include "GameCamera.h"

class CGameCamera_Follow :	public CGameCamera
{
	I3_CLASS_DEFINE( CGameCamera_Follow, CGameCamera);

public:
	CGameCamera_Follow(void);

	virtual void	ToggleCamSidePos() override;
	virtual void	InitCamera( CGameCharaBase * pChar, REAL32 rInterpolateTime) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ReCalculateMatrix() override;
};

#endif