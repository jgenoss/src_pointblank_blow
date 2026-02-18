#if !defined( __WEAPON_DINO_INFO_H)
#define __WEAPON_DINO_INFO_H

#include "WeaponInfo.h"
#include "MeleeWeaponInfo.h"

class CWeaponDinoInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE(CWeaponDinoInfo, CWeaponInfo);

private:
	REAL32		m_rAttackTime[ATTACK_NUM_MAX];
	REAL32		m_rAttackDamage[ATTACK_NUM_MAX];
	REAL32		m_rAttackFreezeTime;					

	REAL32		m_rDPD_Rate;							// 거리별 데미지율

	UINT8		m_nAutoFireCount;						// 자동 발사 횟수
	UINT8		m_nFireBulletCount;						// 발사체 개수

	REAL32		m_rDeviation;							// 편차각

	REAL32		m_rExplosion_Range;						// 폭파 유효 범위
	REAL32		m_rExplosion_EffectiveSlope;			// 거리별 데미지 감소율 Slop

	REAL32		m_rEffectiveTime;						// 이펙트 유효 시간

	REAL32		m_rSkillDuration;						// 스킬 지속 시간
	REAL32		m_rSkillCoolTime;						// 스킬 쿨타임
	REAL32		m_rSkillMovementRate;					// 스킬 이동속도율

	INT32		m_AssistKillImageIndex;			// Assist Kill Image Index
	INT32		m_HitSoundType;					// 공룡 히트 사운드 종류
	INT32		m_HitSoundIndex;				// 히트 사운드 인덱스
	INT32		m_TerrainHitSoundIndex;			// 월드 히트 사운드

	REAL32		m_rCrossHairExpand;			// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	REAL32		m_rCrossHairMin;			// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	REAL32		m_rCrossHairMax;
	//--------------------------------------------------------------------------------------//
	//										궤적 정보										//
	//--------------------------------------------------------------------------------------//
private:
	I3COLOR		m_cBulletSmokeColor;			//	총알 궤적 연기 색상	
	REAL32		m_rBulletSmokeRepeatTexSize;	//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
	REAL32		m_rBulletSmokeGrowThickness;	//	총알 궤적 연기 알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)
	REAL32		m_rBulletSmokeLifeTime;			//	총알 궤적 연기 생명 시간 (단위: 초) 0.0이면 비활성화된다.
	REAL32		m_rBulletSmokeThickness;		//	총알 궤적 연기 두께 (단위: m)
	REAL32		m_rBulletSmokeStartSpeed;		//	총알 궤적 연기 시작점 이동 속도 (총구) (단위: 초당 m)
	REAL32		m_rBulletSmokeEndSpeed;			//	총알 궤적 연기 끝점 이동 속도 (타격점) (단위: 초당 m)

protected:
	void				_ReadBulletTracerKey( i3RegKey * pKey);
	virtual void		_ReadDamageKey( i3RegKey * pKey) override;
	virtual void		_ReadCrosshairKey( i3RegKey * pKey);

public:
	CWeaponDinoInfo();
	virtual ~CWeaponDinoInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	virtual REAL32			GetDamageA( INT32 num) const override;				// 데미지
	virtual REAL32			GetAttackTimeA( INT32 num) const override;			// 공격 지연 속도
	
	virtual INT32			GetAutoFireCount(void) const override;				// 자동 발사 횟수
	virtual INT32			GetFireBulletCount(void) const override;			// 발사체 개수
	virtual REAL32			GetDeviation(void) const override;					// 편차각

	virtual REAL32			GetExplosion_Range(void) const override;			// 폭파 유효 범위
	virtual REAL32			GetExplosion_EffectiveSlope(void) const override;	// 폭파 거리별 데미지 감소율

	virtual REAL32			GetEffectiveTime(void) const override;				// 이펙트 유효 시간

	virtual REAL32			GetAttackFreezeTime(void) const override;

	virtual REAL32			GetSkillDuration(void) const override;				// 스킬 지속시간
	virtual REAL32			GetSkillCoolTime(void) const override;				// 스킬 쿨타임
	virtual REAL32			GetSkillSkillMovementRate(void) const override;	// 스킬 이동속도율

	virtual WEAPON::ATTACK_TYPE	GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const override;

	virtual void			SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo) override;

	//	총알 궤적 연기 색상	
	virtual void			GetBulletSmokeColor( I3COLOR * pOut);

	//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
	virtual REAL32			GetBulletSmokeRepeatTexSize() const override;
	virtual REAL32			GetBulletSmokeGrowThickness() const override;	//	총알 궤적 연기 알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)
	virtual REAL32			GetBulletSmokeLifeTime() const override;			//	총알 궤적 연기 생명 시간 (단위: 초) 0.0이면 비활성화된다.
	virtual REAL32			GetBulletSmokeThickness() const override;		//	총알 궤적 연기 두께 (단위: m)
	virtual REAL32			GetBulletSmokeStartSpeed() const override;		//	총알 궤적 연기 시작점 이동 속도 (총구) (단위: 초당 m)
	virtual REAL32			GetBulletSmokeEndSpeed() const override;			//	총알 궤적 연기 끝점 이동 속도 (타격점) (단위: 초당 m)

	INT32					GetAssistKillImageIndex( void) const	{ return m_AssistKillImageIndex; }
	INT32					GetHitSoundType( void) const			{ return m_HitSoundType; }
	INT32					GetHitSoundIndex( void) const			{ return m_HitSoundIndex; }
	INT32					GetTerrainHitSoundIndex( void) const	{ return m_TerrainHitSoundIndex; }

	virtual REAL32			GetCrossHairExpand() const override;			// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	virtual REAL32			GetCrossHairMin() const override;				// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	virtual REAL32			GetCrossHairMax() const override;

	virtual REAL32			GetDPD_Rate() const override { return m_rDPD_Rate; }
};

#endif // __WEAPON_DINO_INFO_H