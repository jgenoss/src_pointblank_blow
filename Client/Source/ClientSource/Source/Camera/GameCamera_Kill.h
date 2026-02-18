#if !defined( _CGAMECAMERA_KILL_H__)
#define _CGAMECAMERA_KILL_H__

#include "GameCamera.h"

class CGameCamera_Kill :	public CGameCamera
{
	I3_CLASS_DEFINE( CGameCamera_Kill, CGameCamera);

protected:
	bool	m_bCollision = false;

	REAL32	m_rRotation = 0.0f;
	REAL32	m_rElapsedNoCollision = 0.0f;

	MATRIX	m_mtxInterpolateTarget;
	VEC3D	m_vDamagedDir;

	INT32	m_nEnemyIdx = 0;
	bool	m_bEnabledAutoCam = false;

	REAL32	m_rFreezeTime = 0.0f;
	bool	m_bFreeze = false;

	bool	m_bFPS = false;
	
	VEC3D	m_vLastTargetPos;
	VEC3D	m_vLastEnemyPos;
	REAL32	m_rXTheta = 0.0f;
	REAL32	m_rYTheta = 0.0f;

	REAL32	m_rCollisionTimer = 0.0f;
		
public:
	void			SetEnemyIdx(INT32 nIdx);
	INT32			GetEnemyIdx(void);
	void			SetEnableAutoCam( bool bEnable);
	void			CalcDirection();

	void			SetDamagedDirection( VEC3D * pDamagedDir);
	virtual void	OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke) override;
	virtual void	InitCamera(CGameCharaBase * pChara, REAL32 rInterpolateTerm) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	OnEndInterpolate() override;
	virtual void	ReCalculateMatrix() override;

	bool			isFPS(void)			{	return m_bFPS;		}
	void			setFPS(bool bFPS)	{	m_bFPS = bFPS;		}

	bool			isFreeze( void )	{ return m_bFreeze; }
};


#endif