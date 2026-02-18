#if !defined( _GAME_CAMERA_FPS_H__)
#define _GAME_CAMERA_FPS_H__

#include "gamecamera.h"

class CGameCamera_FPS :	public CGameCamera
{
	I3_CLASS_DEFINE( CGameCamera_FPS, CGameCamera);

private:
	bool m_bNeckAnim = false;
	
public:
	CGameCamera_FPS(void);

public:
	virtual void	OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke) override;

	virtual void	InitCamera(CGameCharaBase * pChara, REAL32 rInterpolateTerm/*, i3Camera * pCamera = nullptr*/) override;

    virtual void	ResetCamera()  override {}
	virtual void	RespawnCamera() override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ReCalculateMatrix() override;
	virtual void	OnEndInterpolate() override;

	void			SetNeckAnim(bool bNeckAnim) { m_bNeckAnim = bNeckAnim; }
	void			ReCalculateMatrixNormal();
	void			ReCalculateMatrixWithNeckAnim();
	virtual void	AdjustTargetByCamera() override;
};

#endif