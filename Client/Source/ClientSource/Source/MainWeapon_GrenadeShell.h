#pragma once

//
// 말이 그리네이드이지, 이건 수류탄상속을 받지 않는다..
//
#include "MainWeapon_Throw.h"

class MainWeapon_GrenadeShell : public MainWeapon_Throw
{
public:
	MainWeapon_GrenadeShell(WeaponBase* p);
	virtual ~MainWeapon_GrenadeShell() {}
	
	virtual void	Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin);			// 여기저기서 많이 쓰이는것 같다..

	virtual void	OnUpdate_GrenadeShell( REAL32 rDeltaSeconds);
	void			NET_Bound_GrenadeShell( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );

protected:
	virtual void	OnInitVariable() override;
	virtual void	Reset() override;

	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);

	virtual void	Explosion();					// virtual을 뗌..
	void			UpdateNetworkData( REAL32 rDeltatime);		// virtual 뗌...가상일 필요가 없음..
	void			_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0);


	REAL32			m_rElapsedExplosionTime;
	bool			m_bExplosion;
	VEC3D			m_vHitNormal;
};

