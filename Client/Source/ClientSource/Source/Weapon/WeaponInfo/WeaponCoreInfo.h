/**
 * \file	Weapon\WeaponInfo\WeaponInfo.h
 *
 * Declares the weapon information class.
 */


#if !defined( _WEAPON_CORE_INFO_H__)
#define _WEAPON_CORE_INFO_H__

#include "../../../../Common/CommonSource/WeaponDefine.h"
#include "WeaponGlobal.h"

class CWeaponCoreInfo :	public i3ElementBase
{
	I3_CLASS_DEFINE( CWeaponCoreInfo, i3ElementBase);

public:
	T_ItemID				m_nItemID;					// 아이템 아이디
	WEAPON_ATTRIBUTE		m_Attribute;
	WEAPON::FIRE_TYPE		m_TypeFire;					//	공격 방식
	WEAPON::TRIGGER_TYPE	m_TypeTrigger;				//	충돌 방식
	WEAPON::DAMAGE_TYPE		m_TypeDamage;				//	피해 방식

	UINT16					m_nDamage;					//	무기 공격력	(기본 공격)
	REAL32					m_nDamageOnlyUI;			//  인벤 표시용
	UINT16					m_nDamageForDino;
	UINT16					m_nMaxDamage;
	UINT16					m_nMaxDamageForDino;

	INT32				m_nCriticalDamageChance;
	REAL32					m_fCriticalDamageRate;
	
	REAL32					m_rRange;					//  유효 사정거리 : 무기를 맞았을때 타격을 입힐수 있는거리
	REAL32					m_rExtDPD_Rate;				// 거리별 데미지 감소율
	REAL32					m_rBulletWeight;			// Helmet penetrate
	REAL32					m_rWeight;					//	무기 무게(무엇에 쓰는물건인고?)
	REAL32					m_rFireDelay;				// 발사 간격
	REAL32					m_rHelmetPenetrate;			// Helmet penetrate
	REAL32					m_rHelmetPenetrate_Str;		// ranforce2 Helmet penetrate

	WEAPON_CLASS_TYPE		m_TypeClass;				//	무기 클래스
	WEAPON::RES_ID			m_nRealNumber;				// D 무기일 경우 진짜 무기 번호.
	
	//Move( %)
	REAL32					m_rMoveNormalFire;			// 무기 발사 시의 이동속도 감속 비율 (Factor)
	REAL32					m_rMoveNormalSit;			// 앉은 상태에서의 이동속도 감속 비율 (Factor)

	REAL32					m_CrossHairSpeedFactor;		// CrossHair가 커지거나 작아지는 속도의 비율 (원래 속도에 곱)
	REAL32					m_CrossHairExpandTime;
	REAL32					m_CrossHairContractTime;

	REAL32					m_rFireDelay2;				// 발사 간격 (근접 무기의 경우 오른쪽 공격)
	REAL32					m_rMoveNormalStand;			// 선 상태에서의 이동속도 감속 비율 (Factor)

	// KEYWORD : WEAPON_DUAL
	WEAPON_CLASS_TYPE		m_LeftHandWeaponClassType;
	INT32					m_nLeftHandWeaponNum;
	INT32					m_DualClassType;

	REAL32					m_rPhysixForceRate;			// 무기의 물리 비율 (계열 절대값 * Damage * Rate)
	
protected:
	UINT8 					m_u8EncryptKey;

	WEAPON_ATTRIBUTE		_getWeaponAttribute( void);

	virtual void			_ReadDamageKey( i3RegKey * pKey);
	virtual void			_ReadMovementKey( i3RegKey * pKey);
	virtual void			_ReadCrosshairKey( i3RegKey * pKey);

public:
	CWeaponCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	bool					isSame(WEAPON_CLASS_TYPE type, WEAPON::RES_ID nNumber)
	{
		return (GetTypeClass() == type && GetNumber() == nNumber);
	}

	// Fast Zoom
	bool					isEnableFastZoom(void);
	INT32					GetMaxDamage(void);
	INT32					GetDamageForDino();

	// 속성값 접근
public:
	WEAPON::RES_ID			GetNumber();			//	무기 번호
	WEAPON::RES_ID			GetRealNumber();		//	Real 무기 번호
	T_ItemID				GetItemID();

	WEAPON_SLOT_TYPE		GetTypeUsage();			//	소유 분류

	WEAPON_CLASS_TYPE		GetTypeClass();			//	무기 클래스
	WEAPON::FIRE_TYPE		GetTypeFire();			//	공격 방식
	WEAPON::TRIGGER_TYPE	GetTypeTrigger();		//	충돌 방식	

	WEAPON::DAMAGE_TYPE		GetTypeDamage();			//	피해 방식

	REAL32					GetWeight();				//	무기 무게(무엇에 쓰는물건인고?)
	REAL32					GetRange();					//  유효 사정거리 : 무기를 맞았을때 타격을 입힐수 있는거리

	REAL32					GetFireDelay();				// 발사 간격
	REAL32					GetFireDelay2();			// 발사 간격 (근접 무기는 오른쪽 공격)

	REAL32					GetMoveNormalFire();		// 무기 발사 시의 이동속도 감속 비율 (Factor)
	REAL32					GetMoveNormalSit();			// 앉은 상태에서의 이동속도 감속 비율 (Factor)
	REAL32					GetMoveNormalStand();			// 선 상태에서의 이동속도 감속 비율 (Factor)

	REAL32					GetCrossHairExpandTime();
	REAL32					GetCrossHairContractTime();
	REAL32					GetCrossHairSpeedFactor();		// CrossHair가 커지거나 작아지는 속도의 비율 (원래 속도에 곱)

	REAL32					GetHelmetPenetrate();			// Helmet penetrate
	REAL32					GetHelmetPenetrate_Str();		// ranforce2 Helmet penetrate
	REAL32					GetBulletWeight();			// Helmet penetrate

	bool					AttachedSubWeapon();

	virtual WEAPON::ZOOM_TYPE	GetTypeZoom()				{ return WEAPON::ZOOM_UNKNOWN; }		//	줌 방식

	//	하나의 무기내에 여러개의 익스텐션을 가질수 있으므로 기존에 m_pWeaponInfo->getExtensionType() 호출을 자제하세요.
	//	무기의 현재 활성화된 익스텐션을 알기위해선 WeaponInfo에 있는 함수보다 무기에 있는 getCurrentExtensionType() 함수 이용하세요. komet
	virtual WEAPON::EXT_TYPE		getExtensionType( INT32 idx)	{ return WEAPON::EXT_NONE; }		//	무기 Extension 타입 (1~n)

	virtual INT32				GetFireBulletCount()	{ return 0; }			// 발사체 개수
	virtual INT32				GetFireDecBulletCount() { return 1;	}		// 발사했을 때, 소모하는 총알 개수
	virtual INT32				GetAutoFireCount()		{ return 0; }			// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)	왼쪽 공격
	virtual INT32				GetAutoFireCount2()		{ return 0; }			// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)  오른쪽 공격
	virtual REAL32				GetAutoFireDelay()		{ return 0; }			// 점사 시의 발사 간격
	virtual INT32				GetAutoPointShotEnable(){ return 0; }		//신규 점사 작동 방식.

	virtual REAL32				GetLoadBulletTime()		{ return 0;	}		// 장전 손잡이를 당기는 시간
	virtual REAL32				GetReloadTime()			{ return 0;	}			// 재장전에 걸리는 시간
	virtual REAL32				GetReloadTimeDual()		{ return 0; }			// 듀얼 탄창 장전 시간

	virtual INT32				GetLoadBullet()			{ return 0; }				// 장전 가능 탄알 수
	virtual INT32				GetReloadBulletCount()	{ return 0; }		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	virtual INT32				GetMaxBullet()			{ return 0; }				// 최대 소유 탄알 수
	virtual INT32				GetMaxScopeLevel()		{ return 0; }			// 최대 스코프 단계 (WEAPON::ZOOM_SCOPE 타입만 가능)
	virtual INT32				GetNeedToLoadBullet()	{ return 0;	}	// Load Magazine 후, Load Bullet을 해야하는가?

	virtual INT32				GetDamage();			//	무기 공격력	
	virtual REAL32				GetDamageOnlyUI();
	INT32						GetMaxDamageForDino(void);
	//투척 무기류
	virtual REAL32				GetExplosion_Time()		{ return 0; }			// 폭발 지연시간( Grenade류 등등)
	virtual REAL32				GetExplosion_Range()	{ return 0; }			// 폭발 유효 범위
	virtual REAL32				GetExplosion_EffectiveSlope() { return 0; } // 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	virtual REAL32				GetThrowSpeed()			{ return 0;	}			// 던지는 속도
	virtual REAL32				GetEffectiveTime()		{ return 0; }			// 이펙트 유효 시간
	virtual REAL32				GetAttachLength()		{ return 0; }

	//Move( %)

	virtual REAL32				GetMoveNormalZoom()		{ return 0;	}		// 견착 상태에서의 이동속도 감속 비율 (Factor)

	//
	virtual REAL32				GetCrossHairExpand()	{ return 4.f; }			// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	virtual REAL32				GetCrossHairMin()		{ return 4.f; }		// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	virtual REAL32				GetCrossHairMax()		{ return 4.f; }
	virtual REAL32				GetCrossHairSizeDamage() { return 0; }		// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32				GetCrossHairSizeJump() { return 0; }		// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
	virtual REAL32				GetCrossHairSizeMove() { return 0; }		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32				GetCrossHairSizeMoveSit() { return 0; }
	virtual REAL32				GetCrossHairSizeWalk() { return 0; }		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32				GetCrossHairSizeWalkSit() { return 0; }
	virtual REAL32				GetCrossHairSizeStand() { return 0;	}	// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32				GetCrossHairSizeSit()	{ return 0;	}	// Sit 동작 시에 CrossHair가 넒어지는 비율 (Factor)	
	virtual REAL32				GetCrossHairSizeZoom()	{ return 0; }		// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
	virtual REAL32				GetCrossHairSizeNoZoom(void)	{ return 0; }

	virtual INT32				GetCrossHairInvisible() { return 0; }		// 0 : All Visible 2:Scope Invisible


	virtual REAL32				GetRecoilVertAngle() { return 0; }			// 1발당 수직 반동각
	virtual REAL32				GetRecoilVertMax() { return 0; }			// 최대 누적 반동각

	virtual REAL32				GetRecoilHorzAngle() { return 0; }			// 좌우 흔들림 각
	virtual REAL32				GetRecoilHorzMax() { return 0; }			// 최대 누적 좌우 흔들림 각
	virtual INT32				GetRecoilHorzCount() { return 0; }			// 수평 반동이 적용되기 시작하는 연사 총알 개수
	virtual INT32				GetOneHorzEnable() { return 0; }			// 수평 반동이 적용되기 시작하는 연사 총알 개수

	virtual REAL32				GetDeviation() { return 0; }				// 편차각
	virtual REAL32				GetDeviationMax() { return 0; }				// 최대 누적 편차각
	virtual REAL32				GetDeviationOnlyUI() { return 0; }
	virtual REAL32				GetDeviation_DiminutionDistance(void)		{ return 0.0f; }
	virtual REAL32				GetDeviation_Decrement(void)				{ return 0.0f; }

	virtual REAL32				GetZoomFOV() { return 0; }					// 1인칭일때 Zoom시의 FOV값
	virtual REAL32				GetScopeFOV() { return 0;	}			// 1단 Scope시의 FOV값
	virtual REAL32				GetScopeFOV2() { return 0; }				// 2단 Scope시의 FOV값

	virtual REAL32				GetCamSyncRate() { return 0; }				// 0:총기반동에 무반응   1:완전반응

	virtual REAL32				GetDPD_Rate() { return 0; }

	virtual I3COLOR*			GetBulletSmokeColor() { return nullptr;	}	//	총알 궤적 연기 색상	
	virtual REAL32				GetBulletSmokeRepeatTexSize() const { return 1.f; }	//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
	virtual REAL32				GetBulletSmokeGrowThickness() const { return 0;	}	//	총알 궤적 연기 알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)
	virtual REAL32				GetBulletSmokeLifeTime() const { return 0; }		//	총알 궤적 연기 생명 시간 (단위: 초) 0.0이면 비활성화된다.
	virtual REAL32				GetBulletSmokeThickness() const { return 0;	}//	총알 궤적 연기 두께 (단위: m)
	virtual REAL32				GetBulletSmokeStartSpeed() const { return 0; }	//	총알 궤적 연기 시작점 이동 속도 (총구) (단위: 초당 m)
	virtual REAL32				GetBulletSmokeEndSpeed() const { return 0; }		//	총알 궤적 연기 끝점 이동 속도 (타격점) (단위: 초당 m)

	virtual bool				GetPumpAction()	{ return 0;	}		//	펌프액션 방식 여부	
	virtual INT32				GetFireDelayOnlyUI() const { return 0; }
	virtual	INT32				GetRecoilVertOnlyUI() const { return 0; }
	virtual	INT32				GetRecoilHorzOnlyUI() const { return 0; }
	virtual REAL32				GetRecoilVertAngle_Recovertime() const {return 1.0f;}
	virtual REAL32				GetRecoilHorzAngle_Recovertime() const {return 1.0f;}

	// KEYWORD : WEAPON_DUALKNIFE
	virtual REAL32				GetAttackTimeA( INT32 num)		{ return 0;}
	virtual REAL32				GetAttackTimeB( INT32 num)		{ return 0;}
	virtual REAL32				GetDamageA( INT32 num)			{ return 0;}
	virtual REAL32				GetDamageB( INT32 num)			{ return 0;}

	// KEYWORD : PHYSIX_CHARA_FORCE
	REAL32						GetPhysixForceRate();

	virtual bool				IsFastReloadAnimation( void)	{ return false; }
	virtual bool				IsAlphablendUsed( void);
	virtual bool				IsLoadBulletLeftBarrel( void)	{ return false; }
	virtual bool				IsLoadMagazineLeftBarrel( void)	{ return false; }
	virtual bool				IsLoadMagazineReady(void)		{ return false; }

	virtual INT32				GetScopeType( void)		{ return -1; }

private:

#if defined( I3_DEBUG )
	void						CheckRegDataType();	// 레지스트리 데이터 타입 검사
#endif
	
};

class CMeleeWeaponCoreInfo : public CWeaponCoreInfo
{
	I3_CLASS_DEFINE(CMeleeWeaponCoreInfo, CWeaponCoreInfo);

private:
	REAL32					m_rAttackDamageA[ 1];					// 왼쪽 클릭 공격 데미지. (예: 첫번째 공격은 인덱스 0)
	REAL32					m_rAttackDamageA_ForDino[ 1];			// 왼쪽 클릭 공격 데미지. (예: 첫번째 공격은 인덱스 0)
	INT32					m_nAutoFireCount;				// 왼쪽 공격 최대 횟수
	VEC3D					m_v3CollisionBoxExtent;			//  밀리 공격일 때 범위 충돌을 위한 박스 크기
	INT32					m_nLoadBullet;				// 장전 가능 탄알 수
	INT32					m_nMaxBullet;				// 최대 소유 탄알 수	
	INT32					m_bEnableBounce;

public:
	CMeleeWeaponCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	virtual void			GetCollisionBoxExtent(VEC3D* pVec);
	virtual INT32			GetDamage();			//	무기 공격력	
	virtual INT32			GetAutoFireCount();
	virtual INT32			GetLoadBullet();			// 장전 가능 탄알 수
	virtual INT32			GetMaxBullet();
	virtual bool			IsEnableBounce();
};

class CGunCoreInfo : public CWeaponCoreInfo
{
	I3_CLASS_DEFINE(CGunCoreInfo, CWeaponCoreInfo);

private:
	INT32					m_nReloadBulletCount;		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	INT32					m_nLoadBullet;				// 장전 가능 탄알 수
	INT32					m_nLoadBulletType;				// 장전 가능 탄알 수
	INT32					m_nMaxBullet;				// 최대 소유 탄알 수
	INT32					m_nFireBulletCount;			// 발사체 개수
	INT32					m_nFireDecBulletCount;		// 발사했을 때, 소모하는 총알 개수

	REAL32					m_rCrossHairExpand;			// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	REAL32					m_rCrossHairMin;			// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	REAL32					m_rCrossHairMax;

	REAL32					m_rCrossHairSizeDamage;		// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
	REAL32					m_rCrossHairSizeJump;		// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
	REAL32					m_rCrossHairSizeMove;		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	REAL32					m_rCrossHairSizeMoveSit;
	REAL32					m_rCrossHairSizeWalk;		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	REAL32					m_rCrossHairSizeWalkSit;

	REAL32					m_rCrossHairSizeStand;		// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	REAL32					m_rCrossHairSizeSit;		// Sit 동작 시에 CrossHair가 넒어지는 비율 (Factor)	
	REAL32					m_rCrossHairSizeZoom;		// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
	REAL32					m_rCrossHairSizeNoZoom;		// Sniper 총에 한함.

	REAL32					m_RecoilVertAngle;			// 1발당 수직 반동각
	REAL32					m_RecoilVertMax;			// 최대 누적 반동각

	REAL32					m_RecoilHorzAngle;			// 좌우 흔들림 각
	REAL32					m_RecoilHorzMax;			// 최대 누적 좌우 흔들림 각
	INT32					m_RecoilHorzCount;			// 수평 반동이 적용되기 시작하는 연사 총알 개수
	INT32					m_OneHorzEnable;			// 수평 반동 적용방법 0: 구 1: 신

	INT32					m_nAutoFireCount;			// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)
	INT32					m_bNeedToLoadBullet;

	REAL32					m_rReloadTime;				// 재장전에 걸리는 시간

	REAL32					m_DPD_Rate;

	REAL32					m_rReloadTimeDual;			// 듀얼 탄창 장전 시간

	REAL32					m_RecoilVertAngle_Recovertime;	//수평반동 복구속도
	REAL32					m_RecoilHorzAngle_Recovertime;	//수평반동 복구속도

protected:
	virtual void			_ReadCrosshairKey( i3RegKey * pKey);
	void					_ReadAccuracy( i3RegKey * pKey);
	virtual void			_ReadDamageKey( i3RegKey * pKey);

public:
	CGunCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	virtual INT32			GetLoadBullet();			// 장전 가능 탄알 수
	virtual INT32			GetLoadBulletType();		// 재장전 탄알 수
	virtual INT32			GetReloadBulletCount();		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	virtual INT32			GetMaxBullet();				// 최대 소유 탄알 수
	virtual INT32			GetFireBulletCount();		// 발사체 개수
	virtual INT32			GetFireDecBulletCount();	// 발사했을 때, 소모하는 총알 개수

	virtual REAL32			GetCrossHairExpand();			// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	virtual REAL32			GetCrossHairMin();				// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	virtual REAL32			GetCrossHairMax();
	virtual REAL32			GetCrossHairSizeDamage();		// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeJump();			// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeMove();		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeMoveSit();
	virtual REAL32			GetCrossHairSizeWalk();		// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeWalkSit();
	virtual REAL32			GetCrossHairSizeStand();	// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeZoom();		// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
	virtual REAL32			GetCrossHairSizeNoZoom(void);

	virtual REAL32			GetRecoilVertAngle();		// 1발당 수직 반동각
	virtual REAL32			GetRecoilVertMax();			// 최대 누적 반동각
	
	virtual REAL32			GetRecoilHorzAngle();		// 좌우 흔들림 각
	virtual REAL32			GetRecoilHorzMax();			// 최대 누적 좌우 흔들림 각
	virtual INT32			GetRecoilHorzCount();		// 수평 반동이 적용되기 시작하는 연사 총알 개수
	virtual INT32			GetOneHorzEnable();

	virtual INT32			GetAutoFireCount();			// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)
	virtual INT32			GetNeedToLoadBullet();	// Load Magazine 후, Load Bullet을 해야하는가?

	virtual REAL32			GetReloadTime();			// 재장전에 걸리는 시간
	virtual REAL32			GetReloadTimeDual();		// 듀얼 탄창 장전 시간
	virtual REAL32			GetDPD_Rate();

	virtual REAL32			GetRecoilVertAngle_Recovertime() const;
	virtual REAL32			GetRecoilHorzAngle_Recovertime() const;
};

class CGrenadeCoreInfo :	public CWeaponCoreInfo
{
	I3_CLASS_DEFINE( CGrenadeCoreInfo, CWeaponCoreInfo);

private:
	COnlyGrenadeInfo*		m_pOnlyGreandeInfo;

public:
	CGrenadeCoreInfo();
	virtual ~CGrenadeCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	// 속성값 접근.
	virtual INT32			GetFireBulletCount(); 			// 발사체 개수
	virtual INT32			GetFireDecBulletCount(); 	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetLoadBullet();				// 장전 가능 탄알 수
	virtual INT32			GetReloadBulletCount();		// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	virtual INT32			GetMaxBullet();				// 최대 소유 탄알 수

	//투척 무기류
	virtual REAL32			GetExplosion_Time(); 		// 폭발 지연시간( Grenade류 등등)
	virtual REAL32			GetExplosion_Range();			// 폭발 유효 범위
	virtual REAL32			GetExplosion_EffectiveSlope(); // 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	virtual REAL32			GetThrowSpeed(); 			// 던지는 속도
	virtual REAL32			GetEffectiveTime(); 			// 이펙트 유효 시간

	virtual REAL32			GetAttachLength();

	virtual REAL32			GetDamageTerm();
};

class CBombCoreInfo : public CWeaponCoreInfo
{
	I3_CLASS_DEFINE(CBombCoreInfo, CWeaponCoreInfo);
private:
	INT32					m_nFireBulletCount;			// 발사체 개수
	INT32					m_nFireDecBulletCount;		// 발사했을 때, 소모하는 총알 개수

	INT32					m_nLoadBullet;				// 장전 가능 탄알 수
	INT32					m_nMaxBullet;				// 최대 소유 탄알 수

	REAL32					m_rExplosion_Time;			// 폭발 지연시간( Grenade류 등등)
	REAL32					m_rExplosion_Range;			// 폭발 유효 범위
	REAL32					m_rExplosion_EffectiveSlope;// 피폭 거리에 따라 Damage가 감소하는 비율 Slope

public:
	CBombCoreInfo();
	virtual ~CBombCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	// 속성값 접근.
	virtual INT32			GetFireBulletCount();			// 발사체 개수
	virtual INT32			GetFireDecBulletCount();	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetLoadBullet();				// 장전 가능 탄알 수
	virtual INT32			GetMaxBullet();				// 최대 소유 탄알 수

	//투척 무기류
	virtual REAL32			GetExplosion_Time();		// 폭발 지연시간( Grenade류 등등)
	virtual REAL32			GetExplosion_Range();			// 폭발 유효 범위
	virtual REAL32			GetExplosion_EffectiveSlope(); // 피폭 거리에 따라 Damage가 감소하는 비율 Slope
};

#endif
