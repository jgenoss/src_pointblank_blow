#pragma once

#include "MainWeapon_Throw.h"

class MainWeapon_RLShell : public MainWeapon_Throw
{
public:
	MainWeapon_RLShell(WeaponBase* p);

	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin);
	bool			isPlayedGrazeSound(void)								{ return m_bGrazeSoundPlayed;	}
	void			PlayGrazeSound(void);

	void			OnUpdate_RLShell( REAL32 rDeltaSeconds);
	
	void			NET_Bound_RLShell( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );


private:
	virtual void	OnInitVariable() override;
	virtual void	Reset() override;

	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);
	bool			isBeing(VEC2D* pVec);

	void			UpdateNetworkData( REAL32 rDeltatime);			// 가상일 필요가 없어서 뗀다..
	void			Explosion();									// 가상일 필요가 없어서 뗀다..

	void			_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0);


	REAL32			m_rElapsedExplosionTime;
	REAL32			m_fSmokeLocalTime;
	REAL32			m_rElapsedFlyingTime;			// 로켓이 발사된후의 경과시간.

	VEC2D			m_vRandDirection[4];			//랜덤방향(포지션)
	VEC2D			m_vRandTangent[2];				//랜덤방향(탄젠트)
	VEC2D			m_vRandSub[2];					//랜덤방향

	bool			m_bExplosion;
	INT32			m_nFlyingStep;					// 로켓의 발사경로 스텝
	bool			m_bGrazeSoundPlayed;
};
