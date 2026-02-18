/**
 * \file	Weapon\WeaponInfo\WeaponInfo.h
 *
 * Declares the weapon information class.
 */


#if !defined( _WEAPON_INFO_H__)
#define _WEAPON_INFO_H__

#include "WeaponGlobal.h"
#include "GameCharaBoneDefine.h"

#define MAX_FIRE_EFFECT		3
#define MAX_WEAPON_EXTENSION	4

struct CompatibleWeaponInfo
{
	T_ItemID	_nItemID;	// 호환 무기의 ItemID
	bool		_bUseShape;	// 호환 무기 Shape 사용 Flag

	void Reset(void)
	{
		_nItemID = 0;
		_bUseShape = false;
	}

	void SetCompatibleInfo(T_ItemID ItemID, bool _useCurshape)
	{
		_nItemID = ItemID;
		_bUseShape = _useCurshape;
	}
};

class CGameCharaBase;
class WeaponBase;
class COnlyGrenadeInfo;
class CWeaponExtInfoBase;

struct TracerBulletInfo;
struct KILL_INFO;

extern const char g_chBlank;

class CWeaponInfo :	public i3NamedElement
{
	I3_CLASS_DEFINE( CWeaponInfo, i3NamedElement);

private:
	VAR_LR(WEAPON::DAMAGE_TYPE,203)		m_TypeDamage = WEAPON::DAMAGE_UNKNOWN;	//	피해 방식
	VAR_LR(WEAPON::FIRE_TYPE,204)			m_TypeFire = WEAPON::FIRE_UNKNOWN;		//	공격 방식
	VAR_LR(WEAPON::TRIGGER_TYPE,205)		m_TypeTrigger = WEAPON::TRIGGER_UNKNOWN;	//	충돌 방식

	VAR_LR(UINT32,206)				m_nRealNumber = 0;	// D 무기일 경우 진짜 무기 번호.
															// 인도네시아에 경우 같은 무기이지만 판매방식 때문에 D 무기가 추가됩니다.
															// * RealNumber로 외형이나 판매방식만 다른 아이템의 코드 중복을 해결하려 했으나.
															// 현재 구조상 RealNumber로 전부 교체할 경우 문제가 발생할 수 있기 때문에 (패킷)
															// Number와 RealNumber 병행해서 사용합니다.

	i3::rc_wstring			m_strWeaponName;		//	무기 이름
	i3::rc_wstring			m_strLinkedToCharaAI;	//	캐릭터의 AI노드와 연결되는 이름
	i3::rc_wstring			m_strFireSound;			// 발사 사운드
	i3::rc_wstring			m_strSwapSound;
	i3::rc_wstring			m_strIdleSound;			// Idle Sound

	// 2015-01-20 특수 피격 HUD 처리 - 김대영
	INT32					m_nSpecialDamageHUDIndex = -1;	// 특수 데미지 처리 허드 인덱스 입니다. ( 초기화 : -1 )
	INT32					m_nSpecialDieHUDIndex = -1;		// 특수 다이 처리 허드 인덱스 입니다. ( 초기화 : -1 )
														// WeaponInfoDataBase에 텍스쳐 패스를 저장하고 해당 인덱스를 여기에 저장합니다. 

	// 2008.4.22 GrenadeShell fire sound
	
	i3::rc_wstring			m_strFireSoundExt;		// 익스텐션 기능에서의 발사 사운드
	
	bool					m_bExtFireEffectOffset = false;		// 익스텐션 기능에서의 발사 이펙트 오프셋 사용 유무
	VEC3D					m_vPosExtFireEffectOffset;	// 익스텐션 기능에서의 발사 이펙트 오프셋 값

	i3::rc_wstring			m_strClassMetaName;		// 무기 인스턴스 생성시 클래스 메타 지정.
	
	REAL32					m_nDamageOnlyUI = 0.0f;			//  인벤 표시용

	VAR_LR(UINT16, 207)	m_nDamage = 0;					//	무기 공격력	(기본 공격)
	VAR_LR(UINT16,208)		m_nDamageForDino = 0;			//	무기 공격력	(공룡  모드일 때 적용)

	VAR_LR(UINT16,209)		m_nMaxDamage = 0;
	VAR_LR(UINT16,210)		m_nMaxDamageForDino = 0;

	VAR_LR(INT32,211)		m_nCriticalDamageChance = 0;
	VAR_LR(REAL32,212)		m_fCriticalDamageRate = 0.0f;

	VAR_LR(UINT32,213)		m_WeaponOriginalItemID = 0;

	REAL32					m_rWeight = 0.0f;					//	무기 무게(무엇에 쓰는물건인고?)
	VAR_LR(REAL32,214)		m_rFireDelay = 0.0f;				// 발사 간격
	VAR_LR(REAL32,215)		m_rFireDelay2 = 0.0f;				// 발사 간격 (근접 무기의 경우 오른쪽 공격)
	VAR_LR(REAL32,216)		m_rRange = 0.0f;					//  유효 사정거리 : 무기를 맞았을때 타격을 입힐수 있는거리
	
	REAL32					m_rValue = 0.0f;					// 무기의 값어치

		
	i3::rc_wstring			m_strUiPath;				// GUI resource path
	i3::rc_wstring			m_strDescription;			// 무기 설명
	INT32					m_nUIShapeIndex = 0;			// UIShapeIndex
	
	// Damage
	VAR_LR(REAL32,217)		m_rHelmetPenetrate = 0.0f;			// Helmet penetrate
	VAR_LR(REAL32,218)		m_rHelmetPenetrate_Str = 0.0f;		// ranforce2 Helmet penetrate

	INT32					m_nLevel = 0;
	INT32					m_nMaxLevel = 0;

	i3::rc_wstring			m_strQuestPostfix_Use;	//Quest에서 사용되는 접미어, 로/으로 구분문자열. 예) mp7으로, k-2로 적을 사살하라.
	i3::rc_wstring			m_strQuestPostfix_Do;	//Quest에서 사용된느 접미어, 을/를 구분 문자열. 예) c4를, m4a1을 바닥에 버려라!!

	REAL32					m_rPhysixForceRate = 1.0f;			// 무기의 물리 비율 (계열 절대값 * Damage * Rate)
		
	WEAPON_INDICATOR_TYPE	m_nIndicatorType = WEAPON_INDICATOR_UNKNOWN;			// 무기의 지시자 타입 (현재 HUD 수류탄 표기시 형식에 사용)
	REAL32					m_rIndicateDistance = 0.0f;		// 표시기 거리

	VAR_LR(T_ItemID,219)		m_nItemID = 0;

	TRACER_TYPE				m_TracerType = TRACER_TYPE_BULLET;

	// 마비효과
	VAR_LR(bool,220)	m_bParalysis = false;
	VAR_LR(REAL32,221)	m_rJumpParalysis = 0.0f;
	VAR_LR(REAL32,222)	m_rWalkParalysis = 0.0f;
	VAR_LR(REAL32,223)	m_rRunParalysis = 0.0f;
	VAR_LR(REAL32,224)	m_rCrouchParalysis = 0.0f;

	// revision 61755 KEYWORD : ROTATE_GRENADE
	INT32					m_RotateAngleTable[4] = { 0, 0, 0, 0 };		// 벽X , 벽Y , 바닥X , 바닥Y

	INT32					m_TerrainHitSoundIndex = -1;		// 월드 히트 사운드 인덱스
	INT32					m_CharaHitSoundIndex = -1;		// 캐릭터 히트 사운드 인덱스
	INT32					m_CharaHitSoundIndex2 = -1;		// 캐릭터 히트 사운드 인덱스

	bool					m_bModeWeapon = false;				// 특정 모드용 무기

	QUATERNION				m_Shop3DViewObjQuat = { 0.0f, 0.0f, 0.0f, 1.0f };
	VEC3D					m_Shop3DViewObjPos;
	INT32					m_Shop3DViewSilencer = 0;
	REAL32					m_Shop3DViewDist = 0.0f;

	INT32					m_IsLimitedByDesigSlot = 0;

	INT32					m_KillSoundIndex = -1;

protected:

	//--------------------------------------------------------------------------------------//
	//										암호화											//
	//--------------------------------------------------------------------------------------//
protected:
	// Shift 값 (외부에서 설정되는 값)
	UINT8 					m_u8EncryptKey[VALUETYPE_COUNT] = { 0, };

	// "Damage" field값 load
	virtual void			_ReadDamageKey( i3RegKey * pKey);

	//--------------------------------------------------------------------------------------//
	//										Movement										//
	//--------------------------------------------------------------------------------------//
private:
	REAL32					m_rMoveNormalFire = 0.0f;			// 무기 발사 시의 이동속도 감속 비율 (Factor)
	REAL32					m_rMoveNormalSit = 0.0f;			// 앉은 상태에서의 이동속도 감속 비율 (Factor)
	REAL32					m_rMoveNormalStand = 0.0f;			// 선 상태에서의 이동속도 감속 비율 (Factor)

protected:
	// "Movement" field값 load
	virtual void			_ReadMovementKey( i3RegKey * pKey);

	//--------------------------------------------------------------------------------------//
	//										Extension 정보									//
	//--------------------------------------------------------------------------------------//
protected:
	CWeaponExtInfoBase *	m_pExtInfo[MAX_WEAPON_EXTENSION] = { nullptr, };

	i3::rc_wstring				m_strExtName;		// 익스텐션 명
	i3::rc_wstring				m_strExtFeatures;	// 익스텐션 기능

protected:
	virtual void		_ReadExtensionKey( i3RegKey * pKey, INT32 idx);


	//--------------------------------------------------------------------------------------//
	//										유효 무기 정보									//
	//--------------------------------------------------------------------------------------//
protected:
	CompatibleWeaponInfo	m_stCompatibleInfo[MAX_WEAPON_EXTENSION];

public:
	void					SetCompatibleItemID(INT32 idx, T_ItemID nItemID, bool bUseExtShape);
	T_ItemID				GetCompatibleItemID(INT32 idx) const;
	INT32					GetCompatibleWeaponIdx(T_ItemID nItemID) const;
	bool					isUseCompatibleShape(INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_WEAPON_EXTENSION)
		{
			I3_BOUNDCHK(idx, MAX_WEAPON_EXTENSION);
			return false;
		}
		return m_stCompatibleInfo[idx]._bUseShape; 
	}

protected:
	bool				_FindAttributeKey( i3RegKey * pKey, const char * pszFieldName);

public:
	//	하나의 무기내에 여러개의 익스텐션을 가질수 있으므로 기존에 m_pWeaponInfo->getExtensionType() 호출을 자제하세요.
	//	무기의 현재 활성화된 익스텐션을 알기위해선 WeaponInfo에 있는 함수보다 무기에 있는 getCurrentExtensionType() 함수 이용하세요. komet
	WEAPON::EXT_TYPE	getExtensionType(INT32 idx) const;
	T_ItemID			GetExtensionItemID( INT32 idx ) const;
	bool				isUseExtShape(INT32 idx) const;

	bool				isExtScopeType( INT32 idx) const
	{
		WEAPON::EXT_TYPE type = getExtensionType( idx);
		return type == WEAPON::EXT_DOT_SIGHT || type == WEAPON::EXT_SCOPE;
	}

	bool				isExtFireType( INT32 idx) const
	{
		WEAPON::EXT_TYPE type = getExtensionType( idx);
		return (type & WEAPON_EXT_MASK_FIRE) == WEAPON_EXT_MASK_FIRE;
	}

	bool				availableExtType( WEAPON::EXT_TYPE type) const {
		for( INT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
			if( getExtensionType(i) == type)
				return true;
		return false;
	}

	void				GetExtensionName( i3::rc_wstring& out) const;		// 익스텐션 명
	void				GetExtensionFeatures( i3::rc_wstring& out) const; // { return &g_chBlank; }	// 익스텐션 기능

	// 익스텐션 기능이 있는 무기의 활성화 상태의 정보를 가져오는 무기
	T_ItemID				GetSubExtensionItemID( WEAPON::EXT_TYPE ExtType = WEAPON::EXT_NONE ) const;

	// addable info
	REAL32				GetExtRange( void) const;
	REAL32				GetExtDamage( void) const;
	REAL32				GetExtAttackTime( void) const;
	REAL32				GetExt_DPD_Rate( void) const;

	// Gun
	REAL32				GetExtRecoilHorzFactor(void) const;
	INT32				GetExtRecoilHorzCount(void) const;
	REAL32				GetExtRecoilVertFactor(void) const;
	INT32				GetExtRecoilVertCount(void) const;

	REAL32				GetExtDeviationFactor(void) const;
	INT32				GetExtAutoFireCount(void) const;
	REAL32				GetExtFireDelay(void) const;
	INT32				GetExtOneHorzEnable(void) const;
	INT32				GetExtOneVertEnable(void) const;
	INT32				GetExtAutoPointShotEnable(void) const;

	// Zoom
	REAL32				GetZoomFOV(void) const;		// 1인칭일때 Zoom시의 FOV값

	// Scope
	INT32				GetScopeType( void) const;
	INT32				GetScopeSubType(void) const;
	REAL32				GetScopeFOV(void) const;
	REAL32				GetScopeFOV2(void) const;
	INT32				GetMaxScopeLevel(void) const;			// 최대 스코프 단계 (WEAPON::ZOOM_SCOPE 타입만 가능)

	// Silencer
	REAL32				GetExtSilencerDmgRate() const;	// Extension : Damage Rate


	// Dual Magazine
	REAL32				GetReloadTimeDual() const;		// 듀얼 탄창 장전 시간

	INT32				GetTerrainHitSoundIndex( void) const			{ return m_TerrainHitSoundIndex; }
	INT32				GetCharaHitSoundIndex( void) const				{ return m_CharaHitSoundIndex; }
	INT32				GetCharaHitSoundIndex2( void) const				{ return m_CharaHitSoundIndex2; }

	bool				isNoExjectShell( void) const					{ return _isAttribute(WEAPON_ATTRIBUTE_NO_EXJECTSHELL); }
	bool				isNoOffExtAtChange( void) const					{ return _isAttribute(WEAPON_ATTRIBUTE_NOTOFF_EXTENSION_AT_CHANGE); }
	
	bool				GetLoadMagToLoadBullet() const					{ return _isAttribute(WEAPON_ATTRIBUTE_LOADMAG_TO_LOADBULLET);	}

	// 속성값 접근
public:
	WEAPON::RES_ID			GetNumber() const;	//	무기 번호
	WEAPON::RES_ID			GetRealNumber() const;//	Real 무기 번호
	T_ItemID				GetItemID() const;

	T_ItemID				GetLeftHandWeaponItemID() const;	// 왼손 무기

	//	무기 이름
	void					GetWeaponName( i3::rc_wstring& strOut) const;


	//---------------------------------------------------------------------------------//
	// 2015. 01. 13 EFFECT 처리 - 노현서
protected:
	i3::rc_wstring	m_strFireEffect[ MAX_FIRE_EFFECT];			// 발사 이펙트
	i3::rc_wstring	m_strFireEffectExt;				// 익스텐션 기능에서의 발사 이펙트
	i3::rc_wstring	m_strObjHitEffect;			// 월드/오브젝트 히트 이펙트
	i3::rc_wstring	m_strCharaHitEffect;		// 캐릭터 피격 이펙트
	i3::rc_wstring	m_strCharaHitEffect1PV;	// 캐릭터 1인칭 피격 이펙트
	UINT16			m_UnDecalEffFlag = 0;
	UINT16			m_UnBlowEffFlag = 0;
	i3::rc_wstring	m_strDecalEffectExt;
	UINT16			m_UnColorEffectFlag = 0;		// Color Fire Muzzle Flash 사용여부. 사용하지 않는 경우를 체크함(ColorFireEffect)
	bool			m_bUseHazeEffect = false;

	i3::rc_wstring	m_strIdleEffect;			// Idle 이펙트
	REAL32			m_rIdleEffectLoopTime = 0.0f;		// Idle 이펙트가 루프되는 시간

	void		_ReadEffectKey( i3RegKey * pRootKey, i3RegKey * pEffRegKey);

public:
	// 발사 이펙트
	void		GetFireEffect( i3::rc_wstring& strOut, INT32 idx = 0) const;
	// 히트 이펙트
	void		GetObjHitEffect( i3::rc_wstring& strOut ) const;
	void		GetCharaHitEffect( i3::rc_wstring& strOut ) const;
	void		GetCharaHitEffect1PV( i3::rc_wstring& strOut ) const;
	UINT16		GetUnDecalEffFlag() const;
	UINT16		GetUnBlowEffFlag() const;
	void		GetDecalEffectExt( i3::rc_wstring& strOut ) const;
	UINT16		GetUnColorEffectFlag() const;
	//아이들 이펙트
	void		GetIdleEffect(i3::rc_wstring& strOut) const;
	REAL32		GetIdleEffectLoopTime() const;

	bool		IsHazeEffect() { return m_bUseHazeEffect; }
	//--------------------------------------------------------------------------------//

	// 발사 사운드
	void					GetFireSound( i3::rc_wstring& strOut) const;
	// 교체 사운드
	void					GetSwapSound( i3::rc_wstring& strOut) const;
	// Idle 사운드
	void					GetIdleSound( i3::rc_wstring& strOut) const;

	void					GetLinkedToCharaAI( i3::rc_wstring& strOut) const;
	
	// 2008.4.22 GrenadeShell fire sound
	// 익스텐션 기능에서의 발사 이펙트
	void					GetFireEffectExt( i3::rc_wstring& strOut) const;
	// 익스텐션 기능에서의 발사 사운드
	void					GetFireSoundExt( i3::rc_wstring& strOut) const;

	void					GetClassMetaName( i3::rc_wstring& strOut) const;
	
	bool					GetEnableExtFireEffectOffSet() const		{ return m_bExtFireEffectOffset; }
	void					GetPosExtFireEffectOffSet( VEC3D* vecOut );

	WEAPON_SLOT_TYPE		GetTypeUsage() const;				//	소유 분류
	WEAPON_CLASS_TYPE		GetTypeClass() const;				//	무기 클래스
	WEAPON::FIRE_TYPE		GetTypeFire() const;			//	공격 방식
	WEAPON::TRIGGER_TYPE	GetTypeTrigger() const;		//	충돌 방식	
	WEAPON::DAMAGE_TYPE		GetTypeDamage() const;		//	피해 방식
		
	
	// Kill Sound
	INT32					GetKillSoundIndex() { return m_KillSoundIndex; }

	//	무기 무게(무엇에 쓰는물건인고?)
	REAL32					GetWeight() const;
	//  유효 사정거리 : 무기를 맞았을때 타격을 입힐수 있는거리
	REAL32					GetRange() const;

	// 발사 간격
	REAL32					GetFireDelay() const;
	// 발사 간격 (근접 무기는 오른쪽 공격)
	REAL32					GetFireDelay2() const;

	// 무기 발사 시의 이동속도 감속 비율 (Factor)
	REAL32					GetMoveNormalFire() const;
	// 앉은 상태에서의 이동속도 감속 비율 (Factor)
	REAL32					GetMoveNormalSit() const;
	// 선 상태에서의 이동속도 감속 비율 (Factor)
	REAL32					GetMoveNormalStand() const;

	virtual REAL32					GetCrossHairExpandTime() const		{ return 0.f;}
	virtual REAL32					GetCrossHairContractTime() const	{ return 0.f;}
	// CrossHair가 커지거나 작아지는 속도의 비율 (원래 속도에 곱)
	virtual REAL32					GetCrossHairSpeedFactor() const		{ return 0.f;}

	// GUI resource path (MAX_PATH)
	void					GetUiPath( i3::rc_wstring& strOut) const;
	
	// UI shape
	INT32					GetUIShapeIndex() const;

	// 무기의 값어치
	REAL32					GetValue() const;
	// 무기 설명 (2048bytes)
	void					GetDescription( i3::rc_wstring& strOut) const;

	// 상세설명이미지 존재여부
	bool					isDescriptionImage() const;

	// Helmet penetrate
	REAL32					GetHelmetPenetrate() const;
	// ranforce2 Helmet penetrate
	REAL32					GetHelmetPenetrate_Str() const;
	// Helmet penetrate
	virtual REAL32			GetBulletWeight() const		{ return 100.f; }

	void					GetQuestPostfix_Use( i3::rc_wstring& strOut) const;
	void					GetQuestPostfix_Do( i3::rc_wstring& strOut) const;

	bool					UsePairData(void) const 			{ return _isAttribute(WEAPON_ATTRIBUTE_USINGPAIRWEAPON); }
	
	bool					AttachedSubWeapon();

	WEAPON_INDICATOR_TYPE	GetIndicatorType( void) const;
	REAL32					GetIndicateDistance( void) const;

	REAL32					GetRotateAngle( INT32 nIndex ) const;

	bool					IsThrowWeapon( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_THROW_WEAPON); }
	bool					IsExceptionEndurance( void) const	{ return _isAttribute(WEAPON_ATTRIBUTE_EXCEPTION_ENDURANCE); }
	bool					IsWorldWeapon( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_WORLD_WEAPON); }

	virtual WEAPON::ZOOM_TYPE	GetTypeZoom() const				{ return WEAPON::ZOOM_UNKNOWN; }		//	줌 방식
	
	virtual bool			isMultipleExtension(void) const		{ return false;	}	//	두가지 Extension 기능 가지고 있는 여부
	
	virtual INT32			GetFireBulletCount() const			{ return 0; }	// 발사체 개수
	virtual INT32			GetFireDecBulletCount() const		{ return 1;	}	// 발사했을 때, 소모하는 총알 개수
	virtual INT32			GetAutoFireCount() const			{ return 0; }	// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)	왼쪽 공격
	virtual INT32			GetAutoFireCount2() const			{ return 0; }	// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)  오른쪽 공격
	virtual REAL32			GetAutoFireDelay() const			{ return 0; }	// 점사 시의 발사 간격
	virtual INT32			GetAutoPointShotEnable() const		{ return 0; }	// 신규 점사 작동 방식.

	virtual REAL32			GetLoadBulletTime() const			{ return 0;	}	// 장전 손잡이를 당기는 시간
	virtual REAL32			GetReloadTime() const				{ return 0;	}	// 재장전에 걸리는 시간

	virtual INT32			GetLoadBullet() const				{ return 0; }	// 장전 가능 탄알 수
	virtual INT32			GetLoadBulletType() const			{ return 0; }	// 장전 가능 탄알 수
	virtual INT32			GetGaugeReload() const				{ return 0; }	// Gauge로 리로드 하는 타입(WaterGun)
	virtual INT32			GetReloadBulletCount() const		{ return 0; }	// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
	virtual INT32			GetMaxBullet() const				{ return 0; }	// 최대 소유 탄알 수
	virtual INT32			GetNeedToLoadBullet() const			{ return 0;	}	// Load Magazine 후, Load Bullet을 해야하는가?
	
	//	무기 공격력	
	virtual INT32			GetDamage() const;
	virtual REAL32			GetDamageOnlyUI() const;
	virtual INT32			GetDamageForDino() const;

	INT32					GetMaxDamage(void) const;
	INT32					GetMaxDamageForDino(void) const;

	INT32					GetCriticalDamageChance() const;
	REAL32					GetCriticalDamageRate() const;

	
	UINT32					GetOriginalWeaponItemID(void) const;

	//투척 무기류
	virtual REAL32			GetExplosion_Time() const				{ return 0; }			// 폭발 지연시간( Grenade류 등등)
	virtual REAL32			GetExplosion_Range() const				{ return 0; }			// 폭발 유효 범위 : 일반 범위
	virtual REAL32			GetExplosion_NearRange() const			{ return 0; }			// 폭발 유효 범위 : 주변 범위 (ex. Claymore)
	virtual REAL32			GetExplosion_EffectiveSlope() const		{ return 0; }			// 피폭 거리에 따라 Damage가 감소하는 비율 Slope
	virtual REAL32			GetDetected_Range() const				{ return 0; }			// 감지 범위 (Claymore)
	virtual REAL32			GetThrowSpeed() const					{ return 0;	}			// 던지는 속도
	virtual REAL32			GetExThrowSpeed()			{ return 0;	}			// 지뢰 익스텐션 던지는 속도
	virtual REAL32			GetEffectiveTime() const				{ return 0; }			// 이펙트 유효 시간
	virtual REAL32			GetAttachLength() const					{ return 0; }
	virtual bool			GetUseBombTrigger( void) const			{ return false;}
	virtual bool			GetDual( void) const					{ return false; }
	virtual bool			GetUseThrowDelay( void) const			{ return false; }		// 던지기에 대한 딜레이 여부
	virtual UINT32			GetBombtriggerItemID() const			{ return 0; }

	virtual INT32			GetIncreaseCount() { return 0; }

	//Move( %)
	
	virtual REAL32			GetMoveNormalZoom() const				{ return 0;	}		// 견착 상태에서의 이동속도 감속 비율 (Factor)

	//
	virtual REAL32			GetCrossHairExpand() const				{ return 4.f; }// Crosshiar가 넓어질 때, 길이가 길어지는 비율
	virtual REAL32			GetCrossHairMin() const		 			{ return 4.f; }	// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	virtual REAL32			GetCrossHairMax() const		 			{ return 4.f; }
	virtual REAL32			GetCrossHairSizeDamage() const			{ return 0; }	// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeJump() const	 		{ return 0; }	// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeMove() const	 		{ return 0; }	// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeMoveSit() const			{ return 0; }
	virtual REAL32			GetCrossHairSizeWalk() const			{ return 0; }	// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeWalkSit() const			{ return 0; }
	virtual REAL32			GetCrossHairSizeStand() const		 	{ return 0;	}	// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
	virtual REAL32			GetCrossHairSizeSit() const				{ return 0;	}	// Sit 동작 시에 CrossHair가 넒어지는 비율 (Factor)	
	virtual REAL32			GetCrossHairSizeZoom() const			{ return 0; }	// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
	virtual REAL32			GetCrossHairSizeNoZoom(void) const		{ return 0; }
	
	virtual INT32			GetCrossHairInvisible() const			{ return 0; }		// 0 : All Visible 2:Scope Invisible
	

	virtual REAL32			GetRecoilVertAngle() const				{ return 0; }			// 1발당 수직 반동각
	virtual REAL32			GetRecoilVertMax() const				{ return 0; }			// 최대 누적 반동각
	virtual INT32			GetRecoilVertCount() const				{ return 0; }			// 수직 반동이 적용되기 시작하는 연사 총알 개수
	virtual INT32			GetOneVertEnable()	const				{ return 0; }			// 수평 반동 계산 방식 0: 구 1: 신

	virtual REAL32			GetRecoilHorzAngle() const				{ return 0; }			// 좌우 흔들림 각
	virtual REAL32			GetRecoilHorzMax() const				{ return 0; }			// 최대 누적 좌우 흔들림 각
	virtual INT32			GetRecoilHorzCount() const				{ return 0; }			// 수평 반동이 적용되기 시작하는 연사 총알 개수
	virtual INT32			GetOneHorzEnable()	const				{ return 0; }			// 수평 반동 계산 방식 0: 구 1: 신

	virtual REAL32			GetDeviation() const					{ return 0; }			// 편차각
	virtual REAL32			GetDeviationMax() const					{ return 0; }			// 최대 누적 편차각
	virtual REAL32			GetDeviationOnlyUI() const				{ return 0; }
	virtual REAL32			GetDeviation_DiminutionDistance(void) const	{ return 0.0f; }
	virtual REAL32			GetDeviation_Decrement(void) const			{ return 0.0f; }
	
	virtual REAL32			GetCamSyncRate() const					{ return 0; }				// 0:총기반동에 무반응   1:완전반응

	virtual INT32			GetFireDelayOnlyUI() const				{ return 0; }
	virtual	INT32			GetRecoilVertOnlyUI() const				{ return 0; }
	virtual	INT32			GetRecoilHorzOnlyUI() const				{ return 0; }
	virtual REAL32			GetRecoilVertAngle_Recovertime() const	{ return 1.0f; }
	virtual REAL32			GetRecoilHorzAngle_Recovertime() const	{ return 1.0f; }

	virtual REAL32			GetDPD_Rate() const						{ return 0; }

	virtual I3COLOR*		GetBulletSmokeColor() const				{ return nullptr;	}	//	총알 궤적 연기 색상	
	virtual REAL32			GetBulletSmokeRepeatTexSize() const		{ return 1.f; }	//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
	virtual REAL32			GetBulletSmokeGrowThickness() const		{ return 0;	}	//	총알 궤적 연기 알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)
	virtual REAL32			GetBulletSmokeLifeTime() const			{ return 0; }		//	총알 궤적 연기 생명 시간 (단위: 초) 0.0이면 비활성화된다.
	virtual REAL32			GetBulletSmokeThickness() const			{ return 0;	}//	총알 궤적 연기 두께 (단위: m)
	virtual REAL32			GetBulletSmokeStartSpeed() const			{ return 0; }	//	총알 궤적 연기 시작점 이동 속도 (총구) (단위: 초당 m)
	virtual REAL32			GetBulletSmokeEndSpeed() const			{ return 0; }		//	총알 궤적 연기 끝점 이동 속도 (타격점) (단위: 초당 m)

	bool					GetPumpAction() const					{ return (_getAttribute() & WEAPON_ATTRIBUTE_PUMPACTION) == WEAPON_ATTRIBUTE_PUMPACTION;	}		//	펌프액션 방식 여부	

	virtual INT32			GetLevel() const					{ return m_nLevel; }
	virtual INT32			GetMaxLevel() const					{ return m_nMaxLevel; }

	// KEYWORD : WEAPON_DUALKNIFE
	virtual REAL32			GetAttackTimeA( INT32 num) const		{ return 0;}
	virtual REAL32			GetAttackTimeB( INT32 num) const		{ return 0;}
	virtual REAL32			GetDamageA( INT32 num) const			{ return 0;}
	virtual REAL32			GetDamageB( INT32 num) const			{ return 0;}

	// KEYWORD : DECOY_SOUND
	virtual void			GetDefaultDecoySound(i3::rc_wstring& out) {};

	// KEYWORD : PHYSIX_CHARA_FORCE
	REAL32			GetPhysixForceRate() const;

	TRACER_TYPE		GetTracerType( void) const						{ return m_TracerType; }

	// KEYWORD : WEAPON_DINO
	virtual REAL32			GetAttackFreezeTime(void) const			{ return 0.0f; }
	virtual REAL32			GetSkillDuration(void) const			{ return 0.0f; }
	virtual REAL32			GetSkillCoolTime(void) const			{ return 0.0f; }
	virtual REAL32			GetSkillSkillMovementRate(void) const	{ return 0.0f; }

	// revision 53433 마비시간
	bool					IsParalysis( void) const				{ return m_bParalysis; }
	REAL32					GetJumpParalysis() const;
	REAL32					GetWalkParalysis() const;
	REAL32					GetRunParalysis() const;
	REAL32					GetCrouchParalysis() const;

	QUATERNION				GetShop3DViewObjQuat()		{ return m_Shop3DViewObjQuat; }
	VEC3D					GetShop3DViewObjPos()		{ return m_Shop3DViewObjPos; }
	INT32					IsShop3DViewSilencer()		{ return m_Shop3DViewSilencer; }
	REAL32					GetShop3DViewDist()			{ return m_Shop3DViewDist; }
	bool					IsShop3DViewDualWeapon()	{ return _isAttribute( WEAPON_ATTRIBUTE_DUAL_WEAPON); }
	
	// 특수 피격 HUD
	INT32					GetWeaponSpecialHitHudIndex( void ) const;
	INT32					GetWeaponSpecialDieHudIndex( void ) const;

	virtual WEAPON::ATTACK_TYPE	GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const;

	//			Gatcha2
private:
	REAL32					m_rScaleRate = 0.0f;
	REAL32					m_rXAxis = 0.0f;
	REAL32					m_rYAxis = 0.0f;
	REAL32					m_rZAxis = 0.0f;
	REAL32					m_rBrightness = 1.0f;				//	뽑기상점2 모델링 밝기 조정

	void		__ReadGatchaKey( i3RegKey * pKey);

public:
	REAL32					GetWeaponBrightness(void) const;
	REAL32					GetWeaponScale(void) const;
	REAL32					GetWeaponXRotate(void) const;
	REAL32					GetWeaponYRotate(void) const;
	REAL32					GetWeaponZRotate(void) const;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//											상태 체크..													//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	WEAPON_ATTRIBUTE		m_Attribute = 0;			// 무기 속성
	WEAPON_ATTRIBUTE		_getAttribute( void) const;
	bool					_isAttribute( WEAPON_ATTRIBUTE attr) const	{ return (_getAttribute() & attr) == attr; }

public:

	/** \brief Extension 무기 여부 */
	virtual bool			isExtensible(void) const			{ return false; }
	virtual bool			isNeedToActivate(void) const		{ return false; }
	virtual bool			isDirectUse(void) const				{ return false; }
	
	bool					isBombTypeClass() const;
	bool					isUseExplosionRange() const;
	bool					hasAction() const;
	bool					isRange() const;
	bool					isMelee() const;
	bool					isThrowing() const;
	bool					isItem() const;
	bool					isThrowingClass() const;
	bool					isMission() const;
	bool					isNotMission() const;
	bool					isEnableHeadShot() const;
	bool					isEnableCrossHairEffect() const;
	bool					isEnableDrop() const;
	bool					isNotNeedContactBox() const;
	bool					isNotPenetrate() const;
	bool					isExplosionPhysics() const;
	bool					isValidUiPath() const;
	bool					isUnconditionalNeutralColorCrossHair() const;		// 무조건 자연 칼라가 설정되는 무기
	bool					isUnableSwapIfZero() const;
	bool					isGun() const;
	bool					isCompoundBow() const;
	bool					isC4() const;
	bool					isOverFireDelay(REAL32 rDelay) const;
	bool					hasExtensionInfo() const;
	bool					hasFireEffect() const;
	bool					hasFireEffectExt() const;
	bool					hasIdleEffect() const;
	bool					isModeWeapon( void) const				{ return _isAttribute( WEAPON_ATTRIBUTE_MODE_WEAPON); }

	/** \brief Fast Zoom 가능 무기*/
	bool					isEnableFastZoom(void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_FASTZOOM); }
	/** \brief Shotgun Maser C 애니메이션을 하는 총기 */
	bool					isShotgunMasterC( void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_SHOTGUNMASTERC); }
	/** \brief CIC 무기 판별 */
	bool					isCIC( void) const						{ return _isAttribute( WEAPON_ATTRIBUTE_CIC); }
	/** \brief DotLaser를 항시 On */
	bool					isLockedDotLaser( void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_LOCKEDDOTLASER); }
	virtual	bool			isCameraZoomType() const				{ return false; }

	// 애니메이션 줌인 경우는 두가지, WEAPON::ZOOM_STEADY_AIM같은 경우는 idle과 같은 애니메이션을 쓴다.
	virtual	bool			isAnimZoomType() const					{ return false; }

	bool					IsReloadLoopAnimation(void) const		{ return _isAttribute( WEAPON_ATTRIBUTE_RELOAD_LOOP_ANIMATION); }
	bool					IsFastReloadAnimation( void) const		{ return _isAttribute( WEAPON_ATTRIBUTE_FASTARELOAD); }
	bool					IsAlphablendUsed( void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_ALPHABLEND); }
	bool					IsLoadBulletLeftBarrel( void) const		{ return _isAttribute( WEAPON_ATTRIBUTE_LOADBULLETLEFTBARREL); }
	bool					IsLoadMagazineLeftBarrel( void) const	{ return _isAttribute( WEAPON_ATTRIBUTE_LOADMAGLEFTBARREL); }
	bool					IsLoadMagazineReady(void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_LOADMAGREADY); }

	bool					IsDelayedThrowingType() const			{ return _isAttribute( WEAPON_ATTRIBUTE_DELAYED_THROWING); }

	bool					isSame(WEAPON_CLASS_TYPE type, WEAPON::RES_ID nNumber) const;

	bool					IsLimitedByDesigSlot() const;

public:
	CWeaponInfo(void);
	virtual ~CWeaponInfo(void);

	virtual void			ReadWeaponInfo( i3RegKey * pKey);
	void					ReadShop3DViewKey( i3RegKey * pKey );
	/** \brief 암호화 합니다.
		\param[in] pShiftCode Bit Shift count입니다. 0 ~ 7*/
	virtual void			OnEncriptInfo( UINT8 u8Key);
	virtual void			FillBulletCountString(i3::stack_wstring& out_wstrBulletCount );
	virtual INT32			GetPercentage( WEAPON::INFO_TYPE nWeaponInfoType) const;
	void					MakeBulletCountString(i3::stack_wstring& out_wstrBulletCount); 
	void					MakeWeaponRangeString(i3::stack_wstring& out_wstrRange);

	void					SetKillInfo(KILL_INFO* pKillInfo);

	// 다른 클래스 값 세팅..
	virtual void			GetDamageInfo(CHARA_DAMAGEINFO * pDamageInfo);
	UINT8					GetWeaponFireState(T_ItemID itemID) const;

	virtual void			SetRecoilVert(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 fRate) { return; }
	virtual void			SetRecoilHorz(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 rTheta, REAL32 fRate) { return; }
	//virtual void			SetDropWeaponInfo(N_PCINFO_DROPWEAPON* pIn) { return; }
	virtual void			SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo) { return; }
	
	// 계산..
	virtual REAL32			CalcDeviationByAction(CGameCharaBase* pOwner, WeaponBase * pWeapon, REAL32 fAddDeviation = 0.f) { return 0; }

	static void				InitializeAttributeKey( void);

private:

#if defined( I3_DEBUG )
	void					CheckRegDataType();	// 레지스트리 데이터 타입 검사
#endif
};


#endif
