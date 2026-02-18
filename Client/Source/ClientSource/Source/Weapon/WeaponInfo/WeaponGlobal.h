#if !defined( _WEAPON_GLOBAL_H__)
#define _WEAPON_GLOBAL_H__

typedef UINT32 WEAPON_ATTRIBUTE;
#define WEAPON_ATTRIBUTE_FASTZOOM				0x0001			// Fast Zoom을 사용하는 무기
#define WEAPON_ATTRIBUTE_USINGPAIRWEAPON		0x0002			// Dual Weapon의 Info를 사용하는 무기
#define WEAPON_ATTRIBUTE_ATTACHEDSUB			0x0004			// 메인 무기에 부착된 익스텐션 무기일 경우 (예: LSS)
#define WEAPON_ATTRIBUTE_ALPHABLEND				0x0008			// Alpha blend를 키는 무기 (유리제질이 있는 무기들)
#define WEAPON_ATTRIBUTE_USINGDESCRIPTION		0x0010			// 상세설명이 있는 무기(상세보기 가능)
#define WEAPON_ATTRIBUTE_PUMPACTION				0x0020			// Pump Action 방식 무기
#define WEAPON_ATTRIBUTE_FASTARELOAD			0x0040			// 빠른 재장전 애니메이션 유무
#define WEAPON_ATTRIBUTE_SHOTGUNMASTERC			0x0080			// ShotGun master C 애니메이션 유무
#define WEAPON_ATTRIBUTE_LOADBULLETLEFTBARREL	0x0100			// 노리쇠 전후진 시 바렐이 붙는 손이 왼손인지에 대한 유무
#define WEAPON_ATTRIBUTE_LOADMAGLEFTBARREL		0x0200			// 탄창 교체 시 바렐이 붙는 손이 왼손인지에 대한 유무 
#define WEAPON_ATTRIBUTE_LOADMAGREADY			0x0400			// 재장전 전에 재장전 준비모션을 한다. (예: M1887 재장전 참조)
#define WEAPON_ATTRIBUTE_CIC					0x0800			// CIC 무기
#define WEAPON_ATTRIBUTE_LOCKEDDOTLASER			0x1000			// DotLaser를 항시 On
#define WEAPON_ATTRIBUTE_NEEDTOLOADBULLET		0x2000			// Load Magazine 후, Load Bullet을 해야하는가?
#define WEAPON_ATTRIBUTE_THROW_WEAPON			0x4000			// 던지는 무기 여부
#define WEAPON_ATTRIBUTE_MODE_WEAPON			0x8000			// 모드 전용 무기 여부
#define WEAPON_ATTRIBUTE_EXCEPTION_ENDURANCE	0x00010000		// 내구도 무시형
#define WEAPON_ATTRIBUTE_WORLD_WEAPON			0x00020000		// 월드형 무기
#define WEAPON_ATTRIBUTE_NO_EXJECTSHELL			0x00040000		// 탄피 배출을 안함
#define WEAPON_ATTRIBUTE_NOTOFF_EXTENSION_AT_CHANGE 0x00080000	// 무기 스왑시 익스텐션 Off 처리 여부
#define WEAPON_ATTRIBUTE_DUAL_WEAPON			0x00100000		// 듀얼형 무기
#define WEAPON_ATTRIBUTE_HITED_ANIMATION		0x00200000		// 공격 히트시 애니메이션을 따로 플레이(M-7)
#define WEAPON_ATTRIBUTE_BOUNCED_ATTACK			0x00400000		// 범위 공격형
#define WEAPON_ATTRIBUTE_DELAYED_FIRE_SOUND		0x00800000		// 지연된 공격 사운드 플레이  (for Melee)
#define WEAPON_ATTRIBUTE_NOMORE_SECONDARY_SOUND	0x01000000		// 2번째 공격 사운드가 없다. (for Melee)
#define WEAPON_ATTRIBUTE_DO_NOT_THROW_LAST		0x02000000		// 1발 남은 무기의 경우 던질 수 없다. (for Melee)
#define WEAPON_ATTRIBUTE_SPRAY_SECONDARYATTACK	0x04000000		// 보조 공격시 범위 컬리젼 처리 (for Melee)
#define WEAPON_ATTRIBUTE_DELAYED_THROWING		0x08000000		// 연속 던지기 불가 (for Grenade)
#define WEAPON_ATTRIBUTE_USE_BOMBTRIGGER		0x10000000		// Bomb Trigger 사용 여부(for C5)
#define WEAPON_ATTRIBUTE_LOADMAG_TO_LOADBULLET 	0x20000000		// Load Mag 이후 LoadBullet을 사용해야 하는 무기
#define WEAPON_ATTRIBUTE_RELOAD_LOOP_ANIMATION	0x40000000		// Reload Animation이 Loop되는 Animation인가 하는 무기.

//	공용 무기 정보
class CWeaponGlobal : public i3NamedElement
{
	I3_CLASS_DEFINE( CWeaponGlobal, i3NamedElement);

public:

	REAL32		m_rTracerBulletLength;		//	총알 궤적 길이
	REAL32		m_rTracerBulletThickness;	//	총알 궤적 굵기
	REAL32		m_rTracerBulletSpeed;		//	총알 궤적 속도
	INT32		m_iTracerBulletRate;		//	총알 발사 확률 ( Max: 100%)
	i3::rc_wstring	m_strTracerBulletEffectFile;	//	총알 이펙트 파일 이름
	i3::rc_wstring	m_strTracerHandGrenadeEffectFile;	//	수류탄 궤적 파일 이름
	i3::rc_wstring	m_strTracerShellGrenadeEffectFile;	//	유탄 궤적 파일 이름
	i3::rc_wstring	m_strTracerArrowEffectFile;	//	화살 궤적 파일 이름
	i3::rc_wstring	m_strTracerSmokeEffectFile;		//	연막탄 궤적 파일 이름
	i3::rc_wstring	m_strTracerFlashEffectFile;		//	섬광탄 궤적 파일 이름

	REAL32		m_rTracerHandGrenadeThickness;	//	수류탄 궤적 길이 (단위:m)
	REAL32		m_rTracerShellGrenadeThickness;	//	유탄 궤적 길이 (단위:m)
	REAL32		m_rTracerArrowThickness;	//	화살 궤적 길이 (단위:m)
	REAL32		m_rTracerSmokeThickness;	//	연막탄 궤적 길이 (단위:m)
	REAL32		m_rTracerFlashThickness;	//	섬광탄 궤적 길이 (단위:m)
	REAL32		m_rTracerHandGrenadePieceLifeTime;	//	수류탄 궤적 한토막 보존 시간 (단위: 초)
	REAL32		m_rTracerShellGrenadePieceLifeTime;	//	유탄 궤적 한토막 보존 시간 (단위: 초)
	REAL32		m_rTracerArrowPieceLifeTime;	//	화살 궤적 한토막 보존 시간 (단위: 초)
	REAL32		m_rTracerSmokePieceLifeTime;	//	연막탄 궤적 한토막 보존 시간 (단위: 초)
	REAL32		m_rTracerFlashPieceLifeTime;	//	섬광탄 궤적 한토막 보존 시간 (단위: 초)
	REAL32		m_rTracerHandGrenadePieceCreateTime;	//	수류탄 궤적 한토막 생성 시간 (단위: 초)
	REAL32		m_rTracerShellGrenadePieceCreateTime;	//	유탄 궤적 한토막 생성 시간 (단위: 초)
	REAL32		m_rTracerArrowPieceCreateTime;	//	화살 궤적 한토막 생성 시간 (단위: 초)
	REAL32		m_rTracerSmokePieceCreateTime;		//	연막탄 궤적 한토막 생성 시간 (단위: 초)
	REAL32		m_rTracerFlashPieceCreateTime;		//	섬광탄 궤적 한토막 생성 시간 (단위: 초)
	I3COLOR		m_TracerHandGrenadeColor;		//	수류탄 궤적 색상
	I3COLOR		m_TracerShellGrenadeColor;		//	유탄 궤적 색상
	I3COLOR		m_TracerArrowColor;		//	화살 궤적 색상
	I3COLOR		m_TracerSmokeColor;			//	연막탄 궤적 색상
	I3COLOR		m_TracerFlashColor;			//	섬광탄 궤적 색상
	I3COLOR		m_DamageEffectColor;		//	데미지 이펙트 색상 (데미지 적용시)
	REAL32		m_DamageEffectBlurStart;	//	데미지 이펙트 블러 시작값 (데미지 적용시)
	REAL32		m_DamageEffectBlurEnd;		//	데미지 이펙트 블러 종료값 (데미지 적용시)	
	
public:
	CWeaponGlobal( void);
	virtual ~CWeaponGlobal( void);

	void	ReadInfo( i3RegKey * pKey);
};


class CUITemplateInfo_Weapon : public i3NamedElement
{
	I3_CLASS_DEFINE(CUITemplateInfo_Weapon, i3NamedElement);

public:
	CUITemplateInfo_Weapon();
	virtual ~CUITemplateInfo_Weapon();

	void ReadInfo(i3RegKey* rk);
	const i3::rc_wstring& GetName() const;

private:
	i3::rc_wstring m_name;
};
#endif