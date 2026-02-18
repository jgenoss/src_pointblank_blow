#if !defined( __ONLY_GRENADE_INFO_H)
#define __ONLY_GRENADE_INFO_H

/*
박명진 2008. 6. 25
GrenadeShell이 총기류이 속성도 가지고, 투척류의 속성을 가지고 있기 때문에
두가지 속성을 다 포함해야 하기 때문에 투척류의 속성을 CGrenadeInfo에서 빼낸 클래스
*/

#define  GRENADE_ATTACK_NUM_MAX		1		//	 최대 한번 클릭에 공격 번호

class COnlyGrenadeInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( COnlyGrenadeInfo, i3NamedElement);

private:
	//투척 무기류
	VAR_LR(REAL32, 181)			m_rExplosion_Time = 0.0f;			// 폭발 지연시간( Grenade류 등등)
	VAR_LR(REAL32, 182)			m_rExplosion_Range = 0.0f;			// 폭발 유효 범위 : 일반 범위
	VAR_LR(REAL32, 183)			m_rExplosion_NearRange = 0.0f;		// 폭발 유효 범위 : 주변 범위 (ex. Claymore)
	VAR_LR(REAL32, 184)			m_rExplosion_EffectiveSlope = 0.0f;// 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	VAR_LR(REAL32, 185)			m_rDetected_Range = 0.0f;			// 감지 범위 (Claymore)
	VAR_LR(REAL32, 186)			m_rThrowSpeed = 0.0f;				// 던지는 속도
	VAR_LR(REAL32, 187)			m_rExThrowSpeed = 0.0f;			// 지뢰 익스텐션 던지는 속도
	VAR_LR(REAL32, 188)			m_rEffectiveTime = 0.0f;			// 이펙트 유효 시간  현재 플래쉬에서만 사용

	VAR_LR(REAL32, 189)			m_rAttachLength = 0.0f;			// KEYWORD : WEAPON_C5	벽에 붙는 거리

	VAR_LR(REAL32, 190)			m_rCheckDamageTerm = 0.0f;			// WP_SMOKE : 데미지 계산 텀

	VAR_LR(REAL32, 191)			m_rAttackTimeA[GRENADE_ATTACK_NUM_MAX] = { 0.0f, };	// 왼쪽 클릭 공격시 공격 타이밍. (예: 첫번째 공격은 인덱스 0)
	VAR_LR(REAL32, 192)			m_rAttackTimeB[GRENADE_ATTACK_NUM_MAX] = { 0.0f, };	// 오른쪽 클릭 공격시 공격 타이밍. (예: 첫번째 공격은 인덱스 0)

	// KEYWORD : DECOY_SOUND
	i3::rc_wstring	m_strDefaultDecoySound;


	VAR_LR(UINT8, 193)			m_nFireBulletCount = 0;			// 발사체 개수
	VAR_LR(UINT8, 194)			m_nFireDecBulletCount = 1;		// 발사했을 때, 소모하는 총알 개수

	VAR_LR(UINT16, 195)		m_nLoadBullet = 0;				// 장전 가능 탄알 수
	VAR_LR(UINT16, 196)		m_nReloadBulletCount = 0;
	VAR_LR(UINT16, 197)		m_nMaxBullet = 0;				// 최대 소유 탄알 수

	VAR_LR(UINT8, 198)			m_bDual = 0;					// Dual 수류탄 유무 (K-413)
	VAR_LR(UINT8, 199)			m_bUseThrowDelay = 0;

	VAR_LR(INT32, 200)			m_nIncreaseCount = 0;

	UINT8			m_bUseBombTrigger = 0;		// BombTirgger 사용 유무 (C5 계열)
	INT32			m_nBombtriggerItemID = 0;

protected:
	UINT8 			m_u8EncryptKey = 0;

public:
	void			ReadWeaponInfo( i3RegKey * pKey);
	virtual void	OnEncriptInfo( UINT8 u8Key);

	// 속성값 접근.
	INT32			GetFireBulletCount() const;			// 발사체 개수
	INT32			GetFireDecBulletCount() const;		// 발사했을 때, 소모하는 총알 개수
	INT32			GetLoadBullet() const;				// 장전 가능 탄알 수
	INT32			GetReloadBulletCount() const;			// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	INT32			GetMaxBullet() const;					// 최대 소유 탄알 수

	//투척 무기류
	REAL32			GetExplosion_Time() const;			// 폭발 지연시간( Grenade류 등등)
	REAL32			GetExplosion_Range() const;			// 폭발 유효 범위
	REAL32			GetExplosion_NearRange() const;			// 폭발 유효 범위
	REAL32			GetExplosion_EffectiveSlope() const; // 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	REAL32			GetDetected_Range() const;				// 감지 거리 (Claymore) 
	REAL32			GetThrowSpeed() const;				// 던지는 속도
	REAL32			GetExThrowSpeed();				// 던지는 속도
	REAL32			GetEffectiveTime() const;				// 이펙트 유효 시간

	REAL32			GetAttachLength() const;

	REAL32			GetDamageTerm( void) const;

	INT32			GetIncreaseCount();

	// 왼쪽 클릭 공격시 공격 타이밍. 인자는 공격 횟수째 인덱스 (예: 첫번째 공격은 인덱스 0)
	REAL32			GetAttackTimeA( INT32 num) const;

	// 오른쪽 클릭 공격시 공격 타이밍.
	REAL32			GetAttackTimeB( INT32 num) const;

	bool			GetDual( void) const;
	bool			GetUseThrowDelay( void) const;


	// KEYWORD : DECOY_SOUND
	void			GetDefaultDecoySound(i3::rc_wstring& out);
	
	UINT32			GetBombtriggerItemID() const;
	bool			GetUseBombTrigger( void) const;
};

#endif