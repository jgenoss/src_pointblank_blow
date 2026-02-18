// #pragma once
// 
// #include "pch.h"
// 
// #include "EquipmentTable.h"
// #include "SkillTable.h"
// 
// namespace CHARA {
// 	enum eDAMAGEPART
// 	{
// 		eDAMAGEPART_ARM,
// 		eDAMAGEPART_BODY,
// 		eDAMAGEPART_FOOT,
// 		eDAMAGEPART_HAND,
// 		eDAMAGEPART_HEAD,
// 		eDAMAGEPART_LEG,
// 		eDAMAGEPART_NECK,
// 		eDAMAGEPART_OTHER,
// 		eDAMAGEPART_VITAL,
// 
// 		eDAMAGEPART_COUNT
// 	};
// 
// 	inline
// 	eDAMAGEPART		GetDamagePartByHit( CHARA_HIT_PART part)
// 	{
// 		switch( part)
// 		{
// 		case CHARA_HIT_JAW:
// 		case CHARA_HIT_HEAD:		return eDAMAGEPART_HEAD;	// 머리
// 		case CHARA_HIT_NECK	:
// 		case CHARA_HIT_LCLAVICLE :
// 		case CHARA_HIT_RCLAVICLE :
// 		case CHARA_HIT_BODY1 :
// 		case CHARA_HIT_BODY2 :
// 		case CHARA_HIT_BODY3 :
// 		case CHARA_HIT_ROOT	:		
// 		case CHARA_HIT_DEATH_TRAMPLE:
// 		case CHARA_HIT_DEATH_HOWL:	return eDAMAGEPART_BODY;	// 몸통
// 		case CHARA_HIT_RUARM:
// 		case CHARA_HIT_RDARM:
// 		case CHARA_HIT_LUARM:
// 		case CHARA_HIT_LDARM:		return eDAMAGEPART_ARM;		// 팔
// 		case CHARA_HIT_LHAND:
// 		case CHARA_HIT_RHAND:		return eDAMAGEPART_HAND;	// 손
// 		case CHARA_HIT_LULEG:
// 		case CHARA_HIT_LDLEG:
// 		case CHARA_HIT_RULEG:
// 		case CHARA_HIT_RDLEG:		return eDAMAGEPART_LEG;		// 다리
// 		case CHARA_HIT_LFOOT:
// 		case CHARA_HIT_RFOOT:		return eDAMAGEPART_FOOT;	// 발
// 		case CHARA_HIT_NECK_LOWER:	return eDAMAGEPART_NECK;	// 목	// 공룡용
// 		case CHARA_HIT_VITAL_EYE:	return eDAMAGEPART_VITAL;	// 급소
// 		}
// 
// 		return eDAMAGEPART_OTHER;
// 	}
// 
// 	enum eSEX_TYPE {
// 		eSEX_TYPE_MALE = 0,
// 		eSEX_TYPE_FEMALE,
// 		eSEX_TYPE_DINO,
// 	};
// };
// 
// //////////////////////////////////////////////////////////////////////////////////
// //
// // A Character Ability by Equip
// //
// //////////////////////////////////////////////////////////////////////////////////
// 
// enum SKILL_ABILITY
// {
// 	eABILITY_PRIM_DAMAGE,
// 	eABILITY_SUB_DAMAGE,
// 	eABILITY_MELEE_DAMAGE,	
// 	eABILITY_THROWING_DAMAGE,
// 
// 	eABILITY_PRIM_FIREDELAY,
// 	eABILITY_SUB_FIREDELAY,
// 
// 	eABILITY_PRIM_CONTROL,
// 	eABILITY_SUB_CONTROL,
// 
// 	eABILITY_PRIM_RANGE,
// 	eABILITY_SUB_RANGE,
// 	eABILITY_MELEE_RANGE,
// 	eABILITY_THROWING_RANGE,
// 
// 	eABILITY_PENETRATE,
// 
// 	eABILITY_PRIM_RELOAD,
// 	eABILITY_SUB_RELOAD,
// 
// 	eABILITY_PRIM_CHANGE,
// 	eABILITY_MELEE_CHANGE,
// 
// 	eABILITY_EXPLOSION_RANGE,
// 	eABILITY_EXPLOSION_TIME,
// 	eABILITY_WALK,
// 	eABILITY_RUN,
// 	eABILITY_DEF,
// 	eABILITY_HELMET,
// 	eABILITY_JUMP,
// 
// 	MAX_ABILITY_COUNT
// };
// 
// class CCharaAbility
// {
// public:
// 	void				ResetEquipInfo();
// 	void				ResetSkillInfo();
// 	
// 	void				AddSkillValue( CHARA_SLOT_SKILL* pSkillInfo, T_ItemID PrimaryID, T_ItemID SecondaryID );
// 	REAL32				GetSkillValue( SKILL_ABILITY eIdx ); 
// 	//equip
// 	REAL32	GetAbilityValue( EQUIP::eABILITY ability) const;
// 	void	AddAbilityValue( EQUIP::eABILITY ability, REAL32 rValue );
// 	REAL32  GetDefenseRate ( CHARA::eDAMAGEPART damagePart );
// 
// 	REAL32	GetDamage_Skill( WEAPON_USAGE_TYPE usageType );
// 	REAL32	GetFireDelay_Skill( WEAPON_USAGE_TYPE usageType );
// 	REAL32	GetDefenseRate_Skill();
// 	REAL32	GetExplosionRange_Skill();
// 
// 	// Constructor
// 	void	Reset();
// 	CCharaAbility() { Reset(); }
// 	~CCharaAbility();
// 
// private:
// 	REAL32					m_arAbilityValue [ EQUIP::eABILITY_COUNT ];	// 누적값
// 	REAL32					m_arSkillValue [ MAX_ABILITY_COUNT ];
// };
