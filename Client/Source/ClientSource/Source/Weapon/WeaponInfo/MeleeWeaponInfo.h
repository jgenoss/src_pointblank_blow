#if !defined( __KNIFE_INFO_H)
#define __KNIFE_INFO_H

#include "WeaponInfo.h"

#define  ATTACK_NUM_MAX		4		//	 최대 한번 클릭에 공격 번호

class CMeleeWeaponInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE(CMeleeWeaponInfo, CWeaponInfo);

private:
	REAL32					m_rExplosion_Time = 0.0f;			// 폭발 지연시간( Grenade류 등등)

	VAR_LR(UINT8,177)		m_nAutoFireCount = 1;			// 왼쪽 공격 최대 횟수
	VAR_LR(UINT8,178)		m_nAutoFireCount2 = 1;			// 오른쪽 공격 최대 횟수

	VAR_LR(REAL32, 179)	m_rAttackTimeA[ATTACK_NUM_MAX] = { 0.0f, };	// 왼쪽 클릭 공격시 공격 타이밍. (예: 첫번째 공격은 인덱스 0)
	VAR_LR(REAL32, 180)	m_rAttackTimeB[ATTACK_NUM_MAX] = { 0.0f, };	// 오른쪽 클릭 공격시 공격 타이밍. (예: 첫번째 공격은 인덱스 0)

	REAL32					m_rAttackDamageA[ATTACK_NUM_MAX] = { 0.0f, };	// 왼쪽 클릭 공격 데미지. (예: 첫번째 공격은 인덱스 0)
	REAL32					m_rAttackDamageB[ATTACK_NUM_MAX] = { 0.0f, };	// 오른쪽 클릭 공격 데미지. (예: 첫번째 공격은 인덱스 0)

	REAL32					m_rAttackDamageA_ForDino[ATTACK_NUM_MAX] = { 0.0f, };	// 왼쪽 클릭 공격 데미지. (예: 첫번째 공격은 인덱스 0)
	REAL32					m_rAttackDamageB_ForDino[ATTACK_NUM_MAX] = { 0.0f, };	// 오른쪽 클릭 공격 데미지. (예: 첫번째 공격은 인덱스 0)

	REAL32					m_rThrowSpeed = 0.0f;				// 던지는 속도							

	INT32					m_iInstanceCount = 16;			// 던져지는 인스턴스 최대 갯수
	REAL32					m_rHideTime = 5.0f;				// 던져진 인스턴스가 사라지는 시간

	VEC3D					m_v3CollisionBoxExtent;		//  밀리 공격일 때 범위 충돌을 위한 박스 크기

	UINT16					m_nLoadBullet = 0;				// 장전 가능 탄알 수
	UINT16					m_nMaxBullet = 0;				// 최대 소유 탄알 수

	REAL32					m_rThrowDelayTime = 0.0f;			// 던지기 전까지의 동작 시간

	REAL32					m_RotateFront = -1.0f;				// 박히는 곳의 Weapon 방향
	REAL32					m_WallBetweenDistance = 0.12f;		// 벽과 무기 사이의 거리

public:
	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	
	void			GetCollisionBoxExtent(VEC3D* pVec) const;

	// 속성값
	virtual REAL32			GetExplosion_Time() const override;	// 폭발 지연시간( Grenade류 등등)

	// 왼쪽 클릭 공격시 공격 타이밍. 인자는 공격 횟수째 인덱스 (예: 첫번째 공격은 인덱스 0)
	virtual REAL32			GetAttackTimeA( INT32 num) const override;

	// 오른쪽 클릭 공격시 공격 타이밍.
	virtual REAL32			GetAttackTimeB( INT32 num) const override;
	
	//	무기 공격력
	virtual INT32			GetDamage() const override;

	// 왼쪽 클릭 공격 데미지
	virtual REAL32			GetDamageA( INT32 num) const override;

	// 오른쪽 클릭 공격 데미지
	virtual REAL32			GetDamageB( INT32 num) const override;

	// 던지는 속도
	virtual REAL32			GetThrowSpeed() const override;

	virtual INT32			GetLoadBullet() const override;			// 장전 가능 탄알 수

	REAL32			GetRotateFront();
	REAL32			GetWallBetweenDistance();

	virtual INT32			GetAutoFireCount() const override;
	virtual INT32			GetAutoFireCount2() const override;

	virtual INT32			GetMaxBullet() const override;
	
	virtual WEAPON::ATTACK_TYPE	GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const override;

	INT32					GetInstanceCount( void) const		{ return m_iInstanceCount; }
	REAL32					GetHideTime( void) const			{ return m_rHideTime; }

	bool					IsEnableBounce() const				{ return _isAttribute(WEAPON_ATTRIBUTE_BOUNCED_ATTACK); }

	REAL32					GetThrowDelayTime( void) const		{ return m_rThrowDelayTime; }

	bool					isDelayedFireSound( void) const		{ return _isAttribute(WEAPON_ATTRIBUTE_DELAYED_FIRE_SOUND); }
	bool					NoMoreSecSound( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_NOMORE_SECONDARY_SOUND); }
	bool					DoNotThrowLast( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_DO_NOT_THROW_LAST); }
	bool					UseHitedAnimation( void) const		{ return _isAttribute(WEAPON_ATTRIBUTE_HITED_ANIMATION); }
	BOOL					IsSpraySecAttack( void) const		{ return _isAttribute(WEAPON_ATTRIBUTE_SPRAY_SECONDARYATTACK); }
};

#endif