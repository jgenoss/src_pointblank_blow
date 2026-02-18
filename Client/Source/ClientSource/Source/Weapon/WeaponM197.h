#if !defined( _WEAPON_M197_H__)
#define _WEAPON_M197_H__

#include "GameCharaBoneContext.h"
#include "WeaponOnObject.h"

class WeaponM197 :	public WeaponOnObject
{
	I3_CLASS_DEFINE(WeaponM197, WeaponOnObject);

private:
	
	MATRIX m_matOrgGun;
	
	bool m_bNeedPrehit;
	bool m_bFirePenalty;
	REAL32 m_rNoPressAccuTime;
	REAL32 m_rPressAccuTime;
	REAL32 m_rAccuTimeForPenalty;
	VEC3D m_vMuzzleDir;
	VEC3D m_vBulletTraceStartPos;
	bool m_bFired;

	//	Recoil
	CHARAPLAYER_RECOIL	m_RecoilData;
	//	Deviation
	CHARAPLAYER_DEVIATION	m_DeviationData;

	REAL32				m_rCrossHitTimer;
	CGameCharaBase *	m_pCrossHitChara;

	REAL32 m_rOldFireSpeedGauge;

	i3SoundPlayState* m_pTurnOnSnd;
	i3SoundPlayInfo * m_pSndInfo;

protected:
	REAL32 m_rFireSpeedGauge;					// 발사 속도 게이지 시간 단위
	REAL32 m_rElapsedLastFire;

public:
	WeaponM197();
	virtual ~WeaponM197();

	void	InitVariables();
	virtual void	InitWeaponOnObject() override;
	virtual void	ActiveWeaponOnObject(CGameObjectWithWeapon* pWeapon) override;
	virtual void	DeactiveWeaponOnObject() override;
	virtual void	Reset( void) override;
	virtual void	_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type = WEAPON::FIRE_SOUND_NORMAL) override;
	
	virtual void	_OnEmitFireEffect() override;

	VEC3D*			GetMuzzlePos() { return i3Matrix::GetPos( getFXMatrix()); }
	VEC3D*			GetMuzzleDir();

	virtual REAL32	GetFireDelay() override;
	
	virtual bool	FireWeaponOnObject() override;
	virtual void	UpdateAfterAnim(REAL32 rDeltaSeconds) override;
	void			FireM197();
	void			RotateM197();
	void			ReceiveAngle();
	void			UpdateFlag();

	REAL32			getFireGauge( void)				{ return m_rFireSpeedGauge; }
	virtual VEC3D*	GetBulletTraceStartPos() override;

	void			SetDeviation();
	void			CalcDeviation( REAL32 rDeltatime);

	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos) override;

	void			CheckCrossHit( void);

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;

};

#endif // _WEAPON_M197_H__