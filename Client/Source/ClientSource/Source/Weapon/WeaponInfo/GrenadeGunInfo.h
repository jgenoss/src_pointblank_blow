#if !defined( __GRENADE_GUN_INFO_H)
#define __GRENADE_GUN_INFO_H

#include "GunInfo.h"
#include "GrenadeInfo.h"

class COnlyGrenadeInfo;

class CGrenadeGunInfo :	public CGrenadeInfo
{
	I3_CLASS_DEFINE( CGrenadeGunInfo, CGrenadeInfo);

private:
	COnlyGrenadeInfo*		m_pOnlyGreandeInfo;

	REAL32					m_Homing;

public:
	CGrenadeGunInfo();
	virtual ~CGrenadeGunInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	REAL32					getHoming(void) const;

	// 속성값 접근.
	virtual INT32			GetFireBulletCount() const override; 			// 발사체 개수
	virtual INT32			GetFireDecBulletCount() const override; 	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetLoadBullet() const override;				// 장전 가능 탄알 수
	virtual INT32			GetReloadBulletCount() const override;		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	virtual INT32			GetMaxBullet() const override;				// 최대 소유 탄알 수

	//투척 무기류
	virtual REAL32			GetExplosion_Time() const override; 		// 폭발 지연시간( Grenade류 등등)
	virtual REAL32			GetExplosion_Range() const override;			// 폭발 유효 범위
	virtual REAL32			GetExplosion_EffectiveSlope() const override; // 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	virtual REAL32			GetThrowSpeed() const override; 			// 던지는 속도
	virtual REAL32			GetEffectiveTime() const override; 			// 이펙트 유효 시간
	
	virtual REAL32			GetAttachLength() const override;				// KEYWORD : WEAPON_C5	벽에 붙는 거리

	virtual REAL32			GetDamageTerm() const override;

};

#endif