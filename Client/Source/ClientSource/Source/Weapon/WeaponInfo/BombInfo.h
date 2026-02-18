#if !defined( __BOMB_INFO_H)
#define __BOMB_INFO_H


#include "WeaponInfo.h"

class CBombInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE(CBombInfo, CWeaponInfo);
private:
	VAR_LR(UINT8,123)		m_nFireBulletCount = 0;			// 발사체 개수
	VAR_LR(UINT8,124)		m_nFireDecBulletCount = 1;		// 발사했을 때, 소모하는 총알 개수

	VAR_LR(UINT16,125)		m_nLoadBullet = 0;				// 장전 가능 탄알 수
	VAR_LR(UINT16,126)		m_nMaxBullet = 0;				// 최대 소유 탄알 수

	VAR_LR(REAL32,127)		m_rExplosion_Time = 0.0f;			// 폭발 지연시간( Grenade류 등등)
	VAR_LR(REAL32,128)		m_rExplosion_Range = 0.0f;			// 폭발 유효 범위
	VAR_LR(REAL32,129)		m_rExplosion_EffectiveSlope = 0.0f;// 피폭 거리에 따라 Damage가 감소하는 비율 Slope

	bool					m_bUseBombTrigger = false;
public:
	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	// 속성값 접근.
	virtual INT32			GetFireBulletCount() const override;			// 발사체 개수
	virtual INT32			GetFireDecBulletCount() const override;	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetLoadBullet() const override;	// 장전 가능 탄알 수
	virtual INT32			GetMaxBullet() const override;	// 최대 소유 탄알 수

	//투척 무기류
	virtual REAL32			GetExplosion_Time() const override;		// 폭발 지연시간( Grenade류 등등)
	virtual REAL32			GetExplosion_Range() const override;			// 폭발 유효 범위
	virtual REAL32			GetExplosion_EffectiveSlope() const override; // 피폭 거리에 따라 Damage가 감소하는 비율 Slope

	virtual bool			GetUseBombTrigger() const override;
};

#endif