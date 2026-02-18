#if !defined ( _CGAMECAMERA_FLY_H__)
#define  _CGAMECAMERA_FLY_H__

#include "GameCamera.h"

class CGameCamera_FLY :	public CGameCamera
{
	I3_CLASS_DEFINE( CGameCamera_FLY, CGameCamera);

private :
	REAL32		m_MoveSpeed = 6.7f;
	REAL32		m_RotateSpeed = 0.1f;
	REAL32		m_MoveDelta = 0.15f;

	REAL32		m_rMoveSpeedMax;		// [initialize at constructor]

	REAL32		m_rRotateY = 0.0f;
	REAL32		m_rPreRotateY = 0.0f;

	REAL32		m_rBaseRotateY = I3_DEG2RAD(180.f);

public:
	void			setMoveSpeed( REAL32 rSpeed)				{ m_MoveSpeed = rSpeed; }
	void			setRotateSpeed( REAL32 rSpeed)				{ m_RotateSpeed = rSpeed; }
	void			setMoveSpeedMax( REAL32 rMax)				{ m_rMoveSpeedMax = rMax; }

	REAL32			getMoveSpeed()								{ return m_MoveSpeed; }
	REAL32			getRotateSpeed()							{ return m_RotateSpeed; }

public:
	CGameCamera_FLY(void);

	virtual void	InitCamera( CGameCharaBase * pChar, REAL32 rInterpolateTime) override;
	virtual void	OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke) override;
	virtual void	OnInputMouse( VEC3D * pVecDelta, UINT32 nState) override;
	virtual	void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ReCalculateMatrix() override;
	void			InitTopViewCamera();
	void			SetRotateY(REAL32 rTemp) { m_rRotateY = rTemp; }
	REAL32			GetRotateY() const { return m_rRotateY; }
	void			SetPreRotateY(REAL32 rTemp) {m_rPreRotateY = rTemp; }
	REAL32			GetPreRotateY() const { return m_rPreRotateY; }
	void			SetBaseRotationY(const REAL32 y);
};

namespace FlyCameraUtil
{
	void SaveFlyCameraLocation(I3I_KEY inputKey);
	void MoveFlyCameraLocation(I3I_KEY inputKey);
}

#endif