#if !defined( __GAMECAMERA_OBSERVER2_H__)
#define __GAMECAMERA_OBSERVER2_H__

#include "GameCamera.h" 

class CGameCamera_Observer2: public CGameCamera
{
	I3_CLASS_DEFINE( CGameCamera_Observer2, CGameCamera);

private:
	REAL32		m_rRotateY = 0.0f;
	REAL32		m_rPreRotateY = 0.0f;

public:
	virtual void InitCamera(CGameCharaBase* pChar,REAL32 rInterpolateTime) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void ReCalculateMatrix(void) override;
	virtual void OnEndInterpolate(void) override;
};

#endif //__GAMECAMERA_OBSERVER2_H__
