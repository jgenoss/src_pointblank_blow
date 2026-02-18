#pragma once

#include "MainWeapon_Grenade.h"

// 일단 급한대로 원래와 비슷하게 수류탄을 상속한다..

// 나중에, 쓰로잉쪽으로 루틴을 좀더 옮기고, 수류탄대신 쓰로잉을 상속받게 조정해야한다.
class ProcessMeatBombLight;


class MainWeapon_MeatBomb : public MainWeapon_Grenade
{
public:
	MainWeapon_MeatBomb(WeaponBase* p); 
	virtual ~MainWeapon_MeatBomb();

	virtual void	OnInitVariable() override;
	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	Reset() override;

	virtual void	CheckWorldCollision(REAL32 rDeltaSeconds) override;
	virtual void	UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange) override;

	virtual void	UpdateNetworkData(REAL32 rDeltatime) override;

	virtual bool	isPlayRadioSound(void) override { return false; }
	virtual VEC3D * GetThrowInfo_Normal(void) override { return &m_vAttachNormal; }

protected:
	virtual void	_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0) override;

private:

	virtual void	OnUpdate_Grenade(REAL32 rDeltaSeconds) override;

	virtual void Explosion( void ) override;

	void _CheckExplosionCollision(REAL32 rRange);		// virtual제거..쓰면 안된다..



	REAL32			m_rRotateY;	
	bool			m_bFirstBound;
	VEC3D			m_vAttachNormal;		// 벽에 붙는 노멀

	ProcessMeatBombLight* m_cbLight;

};

