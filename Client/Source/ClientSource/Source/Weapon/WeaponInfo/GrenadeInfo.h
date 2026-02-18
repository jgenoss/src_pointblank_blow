#if !defined( __GRENADE_INFO_H)
#define __GRENADE_INFO_H

#include "WeaponInfo.h"

class COnlyGrenadeInfo;

class CGrenadeInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE( CGrenadeInfo, CWeaponInfo);

private:
	COnlyGrenadeInfo*		m_pOnlyGrenadeInfo;
	TRACER_TYPE				m_TraceType;

private:
	void					_SetTracerType(i3RegKey* pKey);

public:
	CGrenadeInfo();
	virtual ~CGrenadeInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	virtual void			FillBulletCountString(i3::stack_wstring& out_wstrBulletCount) override;

	// 속성값 접근.
	virtual INT32			GetFireBulletCount() const override; 			// 발사체 개수
	virtual INT32			GetFireDecBulletCount() const override; 	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetLoadBullet() const override;				// 장전 가능 탄알 수
	virtual INT32			GetReloadBulletCount() const override;		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	virtual INT32			GetMaxBullet() const override;				// 최대 소유 탄알 수
	
	//투척 무기류
	virtual REAL32			GetExplosion_Time() const override; 				// 폭발 지연시간( Grenade류 등등)
	virtual REAL32			GetExplosion_Range() const override;				// 폭발 유효 범위 : 일반 범위
	virtual REAL32			GetExplosion_NearRange() const override;			// 폭발 유효 범위 : 주변 범위 (ex. Claymore)
	virtual REAL32			GetExplosion_EffectiveSlope() const override;	// 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	virtual REAL32			GetDetected_Range() const override;				// 감지 거리 (Claymore)
	virtual REAL32			GetThrowSpeed() const override; 					// 던지는 속도
	virtual REAL32			GetExThrowSpeed() override; 						// 지뢰 익스텐션 던지는 속도
	virtual REAL32			GetEffectiveTime() const override; 				// 이펙트 유효 시간

	virtual REAL32			GetAttachLength() const override;

	virtual REAL32			GetDamageTerm() const;

	virtual INT32			GetIncreaseCount() override;

	// 왼쪽 클릭 공격시 공격 타이밍. 인자는 공격 횟수째 인덱스 (예: 첫번째 공격은 인덱스 0)
	virtual REAL32			GetAttackTimeA( INT32 num) const override;

	// 오른쪽 클릭 공격시 공격 타이밍.
	virtual REAL32			GetAttackTimeB( INT32 num) const override;

	// KEYWORD : DECOY_SOUND
	virtual void			GetDefaultDecoySound(i3::rc_wstring& out) override;

	virtual bool			GetDual( void) const override;

	virtual bool			GetUseThrowDelay( void) const override;

	virtual UINT32			GetBombtriggerItemID() const override;

	virtual bool			GetUseBombTrigger( void) const override;
};

#endif