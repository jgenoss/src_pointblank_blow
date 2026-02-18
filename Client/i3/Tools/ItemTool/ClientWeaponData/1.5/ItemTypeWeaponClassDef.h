#ifndef __ITEMTYPE_WEAPONCLASS_DEF_H
#define	__ITEMTYPE_WEAPONCLASS_DEF_H

//////////////////////////////////////////////////////////////////////////
// MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, WEAPON_CLASS_TYPE, WEAPON_IDX)
//////////////////////////////////////////////////////////////////////////
//
//	무기 분류 방식
//
// 아이템 아이디 세번째에 들어갑니다. ( 3 )
//
// 유효 값 범위 : 99
// 아이템 아이디에서 저장되는 자릿수 : 
//
// 99000
//~ 1000
//
enum WEAPON_CLASS_TYPE
{
	WEAPON_CLASS_UNKNOWN	= 0,

	WEAPON_CLASS_KNIFE,		//	나이프
	WEAPON_CLASS_HANDGUN,	//	권총
	WEAPON_CLASS_ASSAULT,	//	소총
	WEAPON_CLASS_SMG,		//	서브머신건
	WEAPON_CLASS_SNIPER,	//	저격총
	WEAPON_CLASS_SHOTGUN,	//	산탄총
	WEAPON_CLASS_THROWING1,	//	투척 무기
	WEAPON_CLASS_THROWING2,	//	특수무기(C4,연막)
	WEAPON_CLASS_MISSION,	//	미션 아이템
	WEAPON_CLASS_MG,		//	머신건

	// 게임에서만 사용하는 클래스 ( 서버에는 없습니다.)
	WEAPON_CLASS_GRENADESHELL,		// 유탄
	WEAPON_CLASS_BOMBTRIGGER,		// 폭탄 스위치

	// 새로 추가되는 무기 2008.07.09
	WEAPON_CLASS_CIC,
	WEAPON_CLASS_DUALHANDGUN,
	WEAPON_CLASS_DUALKNIFE,

	WEAPON_CLASS_ROCKET_LAUNCHER,
	WEAPON_CLASS_OBJECT_GUN,		// 특정 오브젝트에 붙어 있는 총 ( M197....)

	WEAPON_CLASS_DUALSMG,

	WEAPON_CLASS_DINO,		
	WEAPON_CLASS_DINO_LEFT_SCRATCH,	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
	WEAPON_CLASS_TREX,				// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
	WEAPON_CLASS_STING,
	WEAPON_CLASS_KNUCKLE,			// 너클
	WEAPON_CLASS_ELITE_RIGHT_SCRATCH,
	WEAPON_CLASS_ELITE_LEFT_SCRATCH,
	WEAPON_CLASS_BOW,
	WEAPON_CLASS_COUNT,


	WEAPON_CLASS_WOODEN_ARROW = 99
};

enum C5_TYPE
{
	C5_NORMAL	= 0,
	C5_D		= 1,
	NUM_C5_MAX
};

enum WEAPON_INDICATOR_TYPE
{
	WEAPON_INDICATOR_UNKNOWN = 0,

	WEAPON_INDICATOR_GRENADE,
	WEAPON_INDICATOR_C5,
};

#define WEAPON_CLASS_NPC_ARROW		99

//////////////////////////////////////////////////////////////////////////

#endif