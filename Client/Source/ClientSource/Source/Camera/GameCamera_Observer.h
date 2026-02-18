#ifndef _GameCamera_Observer_h
#define _GameCamera_Observer_h

#include "GameCamera.h"

class CGameCamera_Observer: public CGameCamera
{
	I3_CLASS_DEFINE(CGameCamera_Observer, CGameCamera);
public:
	virtual void InitCamera(CGameCharaBase* pChar,REAL32 rInterpolateTime) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void ReCalculateMatrix(void) override;
	virtual void OnEndInterpolate(void) override;

	void		CalculateNoneTargetPosition( CGameCharaBase* pPreviousChara);
	
protected:
	void _UpdateInterpolateObserver(REAL32 rDeltaSeconds);
};

#endif //_GameCamera_Observer_h
