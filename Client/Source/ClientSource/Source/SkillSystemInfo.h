#pragma once

// 스킬시스템 구조체 정의
//sever 코드 적용 합니다. 
// 스킬 시스템 메인 카테고리 스킬 리스트

#include "../../LeucoShell/Include/Client/Lib/LeucoShell.h"

enum REQUISTE
{
	REQUISTE_UNKNOWN	= -1,
	REQUISTE_CLASS		= 0,
	REQUISTE_TIME
};

enum SKILL_MAINCATEGORY
{
	MAINCATEGORY_UNKNOWN	= -1,
	MAINCATEGORY_ASSAULT	=0,
	MAINCATEGORY_SMG,
	MAINCATEGORY_RIFLE,
	MAINCATEGORY_SHOTGUN,
	MAINCATEGORY_MG,
	MAINCATEGORY_MAX,
};

enum SKILL_WEAPONTYPE
{
	SKILL_WEAPONTYPE_UNKNOWN	= -1,
	SKILL_WEAPONTYPE_ASSAULT	=0,
	SKILL_WEAPONTYPE_SMG,
	SKILL_WEAPONTYPE_RIFLE,
	SKILL_WEAPONTYPE_SHOTGUN,
	SKILL_WEAPONTYPE_MG,
	SKILL_WEAPONTYPE_HANDGUN,
	SKILL_WEAPONTYPE_KNIFE,
	SKILL_WEAPONTYPE_THROWING,
	SKILL_WEAPONTYPE_MAX,
};

enum SKILL_CATEGORYLIST
{
	CATEGORYLIST_UNKNOWN	= -1,
	CATEGORYLIST_COMMON		= 0,
	CATEGORYLIST_MAIN,
	CATEGORYLIST_ASSIST,
	CATEGORYLIST_MAX,
};

// 제대로 쓰이지 않는 Get/SetWeaponThrowSpeed및 멤버변수 삭제합니다...(2015.05.22.수빈)

class SKILLSYSTEM_APPLY_INFO
{
private:
	VAR_LR(INT32, 249)				_iCategoryIdx;				//스킬 카테고리
	VAR_LR(INT32, 250)				_iSkillIdx;					//스킬 인덱스
	VAR_LR(SKILL_TYPE, 251)			_iSkillType;				//스킬 타입
	VAR_LR(INT32, 252)				_iSkillLevel;				//스킬 레벨
	VAR_LR(SKILL_WEAPONTYPE,253)	_iWeaponType;				//무기 타입
	VAR_LR(bool,254)				_bActivate;					//스킬 활성화 

	// Weapon
	VAR_LR(REAL32, 255) _fWeaponMoveSpeed;				// 이동속도
	VAR_LR(REAL32, 256) _fWeaponFireSpeed;				// 발사속도
	VAR_LR(REAL32, 257) _fWeaponReloadTime;				// 재장전 시간
	VAR_LR(REAL32, 258) _fWeaponHorizontalReaction;		// 수평반동
	VAR_LR(REAL32, 259) _fWeaponVerticalReaction;		// 수직반동
	VAR_LR(REAL32, 260) _fWeaponDeclination;				// 목표편차
	VAR_LR(REAL32, 261) _fWeaponAttackRange;				// 사정거리
	VAR_LR(REAL32, 262) _fWeaponDamage;					// 데미지
	VAR_LR(REAL32, 263) _fWeaponScoopeRange;				// 스쿠프시야
	VAR_LR(REAL32, 264) _fWeaponCrossHairRestore;		// 크로스헤어복구시간
	VAR_LR(REAL32, 265) _fWeaponBombTime;				// 폭파지연시간
	VAR_LR(REAL32, 266) _fWeaponBombRange;				// 폭파범위
	VAR_LR(REAL32, 267) _fWeaponBombLength;				// 던질수잇는거리 (투척속도와 동일하게 적용함!)
	VAR_LR(REAL32, 268) _fWeaponChange;					// 무기교체속도
	VAR_LR(REAL32, 269) _fWeaponPenetrate;				// 무기관통력(헬멧)

	// character
	VAR_LR(REAL32, 271) _fCharaWalkSpeed;				// 캐릭터 걷기능력
	VAR_LR(REAL32, 272) _fCharaMoveSpeed;				// 캐릭터 이동능력
	VAR_LR(REAL32, 273) _fCharaAddSpeed;					// 캐릭터 가속도
	VAR_LR(REAL32, 274) _fCharaJump;						// 캐릭터 점프딜레이
	VAR_LR(REAL32, 275) _fCharaFallDown;					// 캐릭터 낙하
	VAR_LR(REAL32, 276) _fCharaDefense;					// 캐릭터 방어력
	VAR_LR(REAL32, 277) _fCharaHelmetDefense;			// 캐릭터 헬멧 방어력

	// 추가.
	VAR_LR(REAL32, 278)	_fWeaponReaction;			// 총기 반동( 기존것 통합)
	VAR_LR(REAL32, 279)	_fFirstShot_Rate;			// 초탄 정확도(CorssHair가 최소로 작을 때의 크기)
	VAR_LR(REAL32, 280)	_fDPD_Rate;					// 원거리 데미지 (DPD_Rate의 (Damage per Distance))


public:
	SKILLSYSTEM_APPLY_INFO()
	{
		Reset();
		setCategory(-1);
		setIndex(-1);
		setLevel(0);
		seActivate(false);
	}

	void		Reset();

	INT32		getCategory( void) const;
	void		setCategory( INT32 idx);

	INT32		getIndex( void) const;
	void		setIndex( INT32 idx);

	SKILL_TYPE	getSkillType( void) const;
	void		setSkillType( SKILL_TYPE idx);
	void		getSkillTypeStr( char* strtemp) const;

	INT32		getLevel( void) const;
	void		setLevel( INT32 idx);

	bool		getActivate( void) const;
	void		seActivate( bool idx);

	void		setSkillData( INT32 _category, INT32 _idx, INT32 _level);
	REAL32		getSkillValue(void) const;
	REAL32		getSkillValue( WEAPON_CLASS_TYPE weapontype, SKILL_TYPE skilltype) const;
	REAL32		getSkillValue(  SKILL_TYPE skilltype) const;
	REAL32		getDataValue( SKILL_TYPE skilltype) const;

	// 무기타입 
	SKILL_WEAPONTYPE getSkillWeaponType( void);
	bool		isWeaponType( WEAPON_CLASS_TYPE _weapon) const;
	void		getWeaponTypeStr( char* strtemp) const;
	void		setWeaponType( SKILL_WEAPONTYPE type);

	//삭제 무기 무기 업
	REAL32		getWeaponMoveSpeed( void) const;
	void		setWeaponMoveSpeed( REAL32 rVal);

	//무기 연사력
	REAL32		getWeaponFireSpeed( void) const;
	void		setWeaponFireSpeed( REAL32 rVal);

	//무기 재정전속도
	REAL32		getWeaponReloadTime( void) const;
	void		setWeaponReloadTime( REAL32 rVal);

	//삭제 무기 수평반동  안쓰임 삭제예정
	REAL32		getWeaponHorizontalReaction( void) const;
	void		setWeaponHorizontalReaction( REAL32 rVal);

	//삭제 무기 수직반동 안쓰임 삭제예정
	REAL32		getWeaponVerticalReaction( void) const;
	void		setWeaponVerticalReaction( REAL32 rVal);

	//무기 명중률
	REAL32		getWeaponDeclination( void) const;
	void		setWeaponDeclination( REAL32 rVal);

	//무기 사정거리
	REAL32		getWeaponAttackRange( void) const;
	void		setWeaponAttackRange( REAL32 rVal);

	//무기 데미지
	REAL32		getWeaponDamage( void) const;
	void		setWeaponDamage( REAL32 rVal);

	//삭제 스코프거리  안쓰임 삭제예정
	REAL32		getWeaponScoopeRange( void) const;
	void		setWeaponScoopeRange( REAL32 rVal);

	//삭제 안쓰임
	REAL32		getWeaponCrossHairRestore( void) const;
	void		setWeaponCrossHairRestore( REAL32 rVal);

	//무기 투척물 폭발시간단축
	REAL32		getWeaponBombTime( void) const;
	void		setWeaponBombTime( REAL32 rVal);

	//무기 수류탄 폭발범위
	REAL32		getWeaponBombRange( void) const;
	void		setWeaponBombRange( REAL32 rVal);

	//무기 교체 속도
	REAL32		getWeaponChange( void) const;
	void		setWeaponChange( REAL32 rVal);

	//무기 헬멧 관통력 
	REAL32		getWeaponPenetrate( void) const;
	void		setWeaponPenetrate( REAL32 rVal);

	//무기 투척물 던지는 거리
	REAL32		getWeaponBombLength( void) const;
	void		setWeaponBombLength( REAL32 rVal);

	//무기 총기반동제어
	REAL32		getWeaponReaction( void) const;
	void		setWeaponReaction( REAL32 rVal);

	//캐릭터 이동속도
	REAL32		getCharaMoveSpeed( void) const;
	void		setCharaMoveSpeed( REAL32 rVal);

	//캐릭터 걷기속도
	REAL32		getCharaWalkSpeed( void) const;
	void		setCharaWalkSpeed( REAL32 rVal);

	//캐릭터 가속도
	REAL32		getCharaAddSpeed( void) const;
	void		setCharaAddSpeed( REAL32 rVal);

	//캐릭터 점프
	REAL32		getCharaJump( void) const;
	void		setCharaJump( REAL32 rVal);

	//캐릭터 방어력
	REAL32		getCharaDefense( void) const;
	void		setCharaDefense( REAL32 rVal);

	//캐릭터 헬멧 방어력
	REAL32		getCharaHelmetDefense( void) const;
	void		setCharaHelmetDefense( REAL32 rVal);

	//삭제 캐릭터 낙하데미지
	REAL32		getCharaFallDown( void) const;
	void		setCharaFallDown( REAL32 rVal);

	//무기 초탄 정확도 향상
	REAL32		getFirstShotRate( void) const;
	void		setFirstShotRate( REAL32 rVal);

	//삭제 원거리 데미지 향상
	REAL32		getDPDRate( void) const;
	void		setDPDRate( REAL32 rVal);

};
