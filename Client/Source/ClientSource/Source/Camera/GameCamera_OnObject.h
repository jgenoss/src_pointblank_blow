#if !defined( _GAME_CAMERA_ON_OBJECT_H__)
#define _GAME_CAMERA_ON_OBJECT_H__

#include "gamecamera.h"

class CGameCharaBase;

class CGameCamera_OnObject :	public CGameCamera
{
	I3_CLASS_DEFINE( CGameCamera_OnObject, CGameCamera);

private:
	REAL32 m_rCameraTheta = 0.0f;
	REAL32 m_rCameraRho = 0.0f;

protected:
	VAR_LR(i3Object*,6) m_pi3Obj = nullptr;
	
public:
	CGameCamera_OnObject(void);

public:
	virtual void	OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke) override;
	virtual void	InitCamera(CGameCharaBase * pChar = nullptr, REAL32 rInterpolateTime = 0.0f) override;
	void			SetCameraFromTrans(i3Body* pBody, INT32 idx, REAL32 rInterpolateTerm, i3Object* pObj);
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ReCalculateMatrix() override;
	virtual void	OnEndInterpolate() override;

	i3Object *	getOnObject( void) { return m_pi3Obj; }
};

#endif // _GAME_CAMERA_ON_OBJECT_H__