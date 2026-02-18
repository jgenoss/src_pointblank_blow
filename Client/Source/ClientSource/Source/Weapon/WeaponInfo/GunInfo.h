#if !defined( __GUN_INFO_H)
#define __GUN_INFO_H

#include "WeaponInfo.h"
#include "GameCharaBoneDefine.h"

class CGameCharaBase;

struct TracerBulletInfo;
struct KILL_INFO;

class CGunInfo : public CWeaponInfo
{
	I3_CLASS_DEFINE( CGunInfo, CWeaponInfo);

private:
	VAR_LR(WEAPON::ZOOM_TYPE,130)			m_TypeZoom = WEAPON::ZOOM_UNKNOWN;					//	줌 방식
	VAR_LR(REAL32, 131)					m_fThrowSpeed = 0.0f;
	VAR_LR(REAL32, 132)					m_rAutoFireDelay = 0.0f;			// 점사 시의 발사 간격

	VAR_LR(REAL32, 133)					m_rLoadBulletTime = 0.0f;			// 장전 손잡이를 당기는 시간
	VAR_LR(REAL32, 134)					m_rReloadTime = 0.0f;				// 재장전에 걸리는 시간

	VAR_LR(REAL32, 135)					m_RecoilVertAngle = 0.0f;			// 1발당 수직 반동각
	VAR_LR(REAL32, 136)					m_RecoilVertMax = 0.0f;			// 최대 누적 반동각

	VAR_LR(REAL32, 137)					m_RecoilHorzAngle = 0.0f;			// 좌우 흔들림 각
	VAR_LR(REAL32, 138)					m_RecoilHorzMax = 0.0f;			// 최대 누적 좌우 흔들림 각

	VAR_LR(REAL32, 139)					m_Deviation = 0.0f;				// 편차각
	VAR_LR(REAL32, 140)					m_DeviationMax = 0.0f;				// 최대 누적 편차각
	REAL32									m_DeviationOnlyUI = 0.0f;			// 인벤토리와 상점에 표기되는 Hit Rate 값으로 기본 0 이상값이 설정되어 있는 경우만 사용한다. 

	VAR_LR(REAL32, 142)					m_CamSyncRate = 0.0f;				// 0:총기반동에 무반응   1:완전반응
	VAR_LR(REAL32, 143)					m_DPD_Rate = 0.0f;					// 거리별 데미지율
	VAR_LR(REAL32, 144)					m_rBulletWeight = 100.0f;			// Helmet penetrate

	VAR_LR(UINT16, 145)					m_RecoilHorzCount = 0;			// 수평 반동이 적용되기 시작하는 연사 총알 개수
	VAR_LR(UINT16, 146)					m_RecoilVertCount = 0;			// 수직 반동이 적용되기 시작하는 연사 총알 개수

	VAR_LR(UINT32, 147)					m_nLoadBullet = 0;				// 장전 가능 탄알 수
	VAR_LR(UINT32, 148)					m_nLoadBulletType = 0;			// 장전 가능 탄알 수
	VAR_LR(UINT32, 149)					m_nGaugeReload = 0;				// Gauge를 이용한 Reload 타입
	VAR_LR(UINT16, 150)					m_nReloadBulletCount = 0;		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	VAR_LR(UINT32, 151)					m_nMaxBullet = 0;				// 최대 소유 탄알 수

	VAR_LR(UINT8, 152)						m_nFireBulletCount = 0;			// 발사체 개수
	VAR_LR(UINT8, 153)						m_nFireDecBulletCount = 1;		// 발사했을 때, 소모하는 총알 개수
	VAR_LR(UINT8, 154)						m_nAutoFireCount = 0;			// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)

	VAR_LR(INT32, 155)						m_OneHorzEnable = 0;			// 수평 반동 적용 방법 변환 구: false 신 : true (광클시 안적용/적용)
	VAR_LR(INT32, 156)						m_OneVertEnable = 0;			// 수직 반동 적용 방법 변환 구: false 신 : true (광클시 안적용/적용)

	VAR_LR(REAL32, 157)					m_RecoilVertAngle_Recovertime = 1.0f;	//수평반동 복구속도
	VAR_LR(REAL32, 158)					m_RecoilHorzAngle_Recovertime = 1.0f;	//수평반동 복구속도

	INT32					m_FireDelayOnlyUI = 0;			// 인벤토리와 상점에 표기되는 연사력 값으로 기본 0 이상값이 설정되어 있는 경우만 사용한다. 
	INT32					m_RecoilVertOnlyUI = 0;
	INT32					m_RecoilHorzOnlyUI = 0;

	INT32					m_nAutoPointShotEnable = 0;		// 신규 점사 방식 작동 ( 리얼 점사 )
	// 속성값 접근
public:
	//	줌 방식
	virtual WEAPON::ZOOM_TYPE	GetTypeZoom() const override;
	
	virtual INT32			GetFireBulletCount() const override;		// 발사체 개수
	virtual INT32			GetFireDecBulletCount() const override;	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetAutoFireCount() const override;		// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)	
	virtual REAL32			GetAutoFireDelay() const override;		// 점사 시의 발사 간격
	virtual INT32			GetAutoPointShotEnable() const override; // 신규 점사 방식 작동

	virtual REAL32			GetLoadBulletTime() const override;		// 장전 손잡이를 당기는 시간
	virtual REAL32			GetReloadTime() const override;			// 재장전에 걸리는 시간
	
	virtual INT32			GetLoadBullet() const override;			// 장전 가능 탄알 수
	virtual INT32			GetLoadBulletType() const override;		// 재장전 타입
	virtual INT32			GetGaugeReload() const override;			// Gauge로 리로드 하는 타입(WaterGun)
	virtual INT32			GetReloadBulletCount() const override;	// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	
	virtual INT32			GetMaxBullet() const override;			// 최대 소유 탄알 수
	virtual INT32			GetNeedToLoadBullet() const override;	// Load Magazine 후, Load Bullet을 해야하는가?
	virtual REAL32			GetThrowSpeed() const override;
	
	virtual REAL32			GetRecoilVertAngle() const override;		// 1발당 수직 반동각
	virtual REAL32			GetRecoilVertMax() const override;		// 최대 누적 반동각
	virtual INT32			GetRecoilVertCount() const override;		// 수평 반동이 적용되기 시작하는 연사 총알 개수

	virtual REAL32			GetRecoilVertAngle_Recovertime() const override;
	virtual REAL32			GetRecoilHorzAngle_Recovertime() const override;
	
	virtual REAL32			GetRecoilHorzAngle() const override;		// 좌우 흔들림 각
	virtual REAL32			GetRecoilHorzMax() const override;		// 최대 누적 좌우 흔들림 각
	virtual INT32			GetRecoilHorzCount() const override;		// 수평 반동이 적용되기 시작하는 연사 총알 개수
	
	virtual REAL32			GetDeviation() const override;			// 편차각
	virtual REAL32			GetDeviationMax() const override;		// 최대 누적 편차각
	virtual REAL32			GetDeviationOnlyUI() const override;		// 인벤토리와 상점에 표기되는 Hit Rate 값으로 기본 0 이상값이 설정되어 있는 경우만 사용한다.
	virtual INT32			GetFireDelayOnlyUI() const override;		// 인벤토리와 상점에 표기되는 FireDelay 값으로 기본 0 이상값이 설정되어 있는 경우만 사용한다.
	virtual	INT32			GetRecoilVertOnlyUI() const override;
	virtual	INT32			GetRecoilHorzOnlyUI() const override;
		
	virtual REAL32			GetCamSyncRate() const override;			// 0:총기반동에 무반응   1:완전반응
	
	virtual INT32			GetOneHorzEnable() const override;		// 수평 반동 계산 방식 0: 구 1: 신
	virtual INT32			GetOneVertEnable() const override;		// 수직 반동 계산 방식 0: 구 1: 신

	// 상태 체크..
	virtual bool			isNeedToActivate(void) const override
	{
		return ( ( getExtensionType(0) & WEAPON_EXT_MASK_NEED_TO_ACTIVATE) != 0 );
	}

	virtual bool			isDirectUse(void) const override
	{
		return ( ( getExtensionType(0) & WEAPON_EXT_MASK_DIRECT_USE) != 0 );
	}

	virtual bool			isExtensible(void) const override { return ( getExtensionType(0) != WEAPON::EXT_NONE); }
	// 카메라 줌인 경우는 세가지
	virtual	bool			isCameraZoomType() const override
	{
		WEAPON::ZOOM_TYPE type = GetTypeZoom();
		return (type == WEAPON::ZOOM_ZOOM || type == WEAPON::ZOOM_SCOPE || type == WEAPON::ZOOM_STEADY_AIM);
	}
	// 애니메이션 줌인 경우는 두가지, WEAPON::ZOOM_STEADY_AIM같은 경우는 idle과 같은 애니메이션을 쓴다.
	virtual	bool			isAnimZoomType() const override
	{
		WEAPON::ZOOM_TYPE type = GetTypeZoom();
		return (type == WEAPON::ZOOM_ZOOM || type == WEAPON::ZOOM_SCOPE);
	}

	//--------------------------------------------------------------------------------------//
	//										Crosshair 정보									//
	//--------------------------------------------------------------------------------------//
private:
	VAR_LR(REAL32, 159)			m_rCrossHairExpand = 0.0f;			// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	VAR_LR(REAL32, 160)			m_rCrossHairMin = 0.0f;			// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	VAR_LR(REAL32, 161)			m_rCrossHairMax = 0.0f;

	VAR_LR(REAL32, 162)			m_rCrossHairSizeDamage = 0.0f;		// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
	VAR_LR(REAL32, 163)			m_rCrossHairSizeJump = 0.0f;		// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
	VAR_LR(REAL32, 164)			m_rCrossHairSizeMove = 0.0f;		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	VAR_LR(REAL32, 165)			m_rCrossHairSizeMoveSit = 0.0f;
	VAR_LR(REAL32, 166)			m_rCrossHairSizeWalk = 0.0f;		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	VAR_LR(REAL32, 167)			m_rCrossHairSizeWalkSit = 0.0f;

	VAR_LR(REAL32, 168)			m_rCrossHairSizeStand = 0.0f;		// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	VAR_LR(REAL32, 169)			m_rCrossHairSizeSit = 0.0f;		// Sit 동작 시에 CrossHair가 넒어지는 비율 (Factor)	
	VAR_LR(REAL32, 170)			m_rCrossHairSizeZoom = 0.0f;		// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
	VAR_LR(REAL32, 171)			m_rCrossHairSizeNoZoom = 0.0f;		// Sniper 총에 한함.
	VAR_LR(INT32,	172)			m_CrossHairInvisible = 0;		// 0 : All Visible 2:Scope Invisible

	VAR_LR(REAL32, 173)			m_CrossHairExpandTime = 0.3f;
	VAR_LR(REAL32, 174)			m_CrossHairContractTime = 0.4f;
	VAR_LR(REAL32, 175)			m_CrossHairSpeedFactor = 1.0f;		// CrossHair가 커지거나 작아지는 속도의 비율 (원래 속도에 곱)

protected:
	void				_ReadCrosshairKey( i3RegKey * pKey);
	void				_ReadAccuracyKey( i3RegKey * pKey);

public:
	virtual REAL32		GetCrossHairExpand() const override;				// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	virtual REAL32		GetCrossHairMin() const override;				// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	virtual REAL32		GetCrossHairMax() const override;
	virtual REAL32		GetCrossHairSizeDamage() const override;			// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32		GetCrossHairSizeJump() const override;			// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
	virtual REAL32		GetCrossHairSizeMove() const override;			// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32		GetCrossHairSizeMoveSit() const override;
	virtual REAL32		GetCrossHairSizeWalk() const override;			// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32		GetCrossHairSizeWalkSit() const override;
	virtual REAL32		GetCrossHairSizeStand() const override;			// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32		GetCrossHairSizeSit() const override;
	virtual REAL32		GetCrossHairSizeZoom() const override;			// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
	virtual REAL32		GetCrossHairSizeNoZoom(void) const override;
	virtual INT32		GetCrossHairInvisible() const override;			// 0 : All Visible 2:Scope Invisible

	virtual REAL32		GetCrossHairExpandTime( void) const override;
	virtual REAL32		GetCrossHairContractTime( void) const override;
	virtual REAL32		GetCrossHairSpeedFactor( void) const override;

	//--------------------------------------------------------------------------------------//
	//										Movement										//
	//--------------------------------------------------------------------------------------//
private:
	VAR_LR(REAL32, 176)			m_rMoveNormalZoom = 0.0f;			// 견착 상태에서의 이동속도 감속 비율 (Factor)

protected:
	virtual void		_ReadMovementKey( i3RegKey * pKey) override;

public:
	virtual REAL32		GetMoveNormalZoom() const override;		// 견착 상태에서의 이동속도 감속 비율 (Factor)

	//--------------------------------------------------------------------------------------//
	//										궤적 정보										//
	//--------------------------------------------------------------------------------------//
private:
	I3COLOR				m_cBulletSmokeColor = { 255, 255, 255, 255 };		//	총알 궤적 연기 색상	
	REAL32				m_fBulletSmokeRepeatTexSize = 1.0f;	//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
	REAL32				m_fBulletSmokeGrowThickness;		//	총알 궤적 연기 알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)
	REAL32				m_fBulletSmokeLifeTime;		//	총알 궤적 연기 생명 시간 (단위: 초) 0.0이면 비활성화된다.
	REAL32				m_fBulletSmokeThickness;	//	총알 궤적 연기 두께 (단위: m)
	REAL32				m_fBulletSmokeStartSpeed;	//	총알 궤적 연기 시작점 이동 속도 (총구) (단위: 초당 m)
	REAL32				m_fBulletSmokeEndSpeed;		//	총알 궤적 연기 끝점 이동 속도 (타격점) (단위: 초당 m)

protected:
	void				_ReadBulletTracerKey( i3RegKey * pKey);

public:
	//	총알 궤적 연기 색상	
	virtual void			GetBulletSmokeColor( I3COLOR * pOut) const;

	//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
	virtual REAL32			GetBulletSmokeRepeatTexSize() const override;
	virtual REAL32			GetBulletSmokeGrowThickness() const override;	//	총알 궤적 연기 알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)
	virtual REAL32			GetBulletSmokeLifeTime() const override;			//	총알 궤적 연기 생명 시간 (단위: 초) 0.0이면 비활성화된다.
	virtual REAL32			GetBulletSmokeThickness() const override;		//	총알 궤적 연기 두께 (단위: m)
	virtual REAL32			GetBulletSmokeStartSpeed() const override;		//	총알 궤적 연기 시작점 이동 속도 (총구) (단위: 초당 m)
	virtual REAL32			GetBulletSmokeEndSpeed() const override;			//	총알 궤적 연기 끝점 이동 속도 (타격점) (단위: 초당 m)

	//					무기 데미지
protected:
	virtual void			_ReadDamageKey( i3RegKey * pKey) override;

public:
	virtual REAL32			GetDPD_Rate() const override;

	// Helmet penetrate
	virtual REAL32			GetBulletWeight() const override;

public:
	virtual void			FillBulletCountString(i3::stack_wstring& out_wstrBulletCount) override;
	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	virtual INT32			GetPercentage( WEAPON::INFO_TYPE nWeaponInfoType) const override;

	virtual void			SetRecoilVert(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 fRate) override;
	virtual void			SetRecoilHorz(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 rTheta, REAL32 fRate) override;
	// 다른 클래스 값 세팅..
	//virtual void			SetDropWeaponInfo(N_PCINFO_DROPWEAPON* pIn);
	virtual void			SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo) override;
	// 계산..
	virtual REAL32			CalcDeviationByAction(CGameCharaBase* pOwner, WeaponBase * pWeapon, REAL32 fAddDeviation = 0.f) override;
	
	const char*				GetStringExtensionType(WEAPON::EXT_TYPE type) const;
};

#endif