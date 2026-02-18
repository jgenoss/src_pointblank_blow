// #include "pch.h"
// #include "CharaAbility.h"
// 
// void CCharaAbility::Reset()
// {
// 	ResetEquipInfo();
// 	ResetSkillInfo();
// }
// 
// CCharaAbility::~CCharaAbility(void)
// {
// }
// 
// REAL32 CCharaAbility::GetAbilityValue( EQUIP::eABILITY ability) const
// {
// 	I3ASSERT( ability < EQUIP::eABILITY_COUNT );
// 	return m_arAbilityValue[ ability ];
// }
// 
// void CCharaAbility::AddAbilityValue( EQUIP::eABILITY ability, REAL32 rValue )
// {
// 	I3ASSERT( ability < EQUIP::eABILITY_COUNT );
// 	m_arAbilityValue[ ability ] += rValue;
// 
// #ifdef _DEBUG
// 	if( rValue > I3_EPS )
// 	{
// 		//DSMSG("[CCharaAbility::AddAbilityValue] ability=%s(%d), value=%f, accumulated value=%f\n", sEquipAbilityProperty[ability]._pszFieldName, (UINT32)ability, rValue, m_arAbilityValue[ ability ] );
// 	}
// #endif 
// }
// 
// //cf: CGameCharaEquipContext::GetDefenseRate
// //Note. 서버에서는 배틀 입장시 누적값으로 셋팅함.
// REAL32	CCharaAbility::GetDefenseRate( CHARA::eDAMAGEPART damagePart)
// {
// 	switch( damagePart)
// 	{
// 	case CHARA::eDAMAGEPART_BODY :
// 		return GetAbilityValue( EQUIP::eABILITY_DEFENSE_BODY);
// 	case CHARA::eDAMAGEPART_ARM:
// 	case CHARA::eDAMAGEPART_HAND:
// 		return GetAbilityValue( EQUIP::eABILITY_DEFENSE_ARM);
// 	case CHARA::eDAMAGEPART_LEG:
// 	case CHARA::eDAMAGEPART_FOOT:
// 		return GetAbilityValue( EQUIP::eABILITY_DEFENSE_LEG);
// 	}
// 
// 	return 0.0f;
// }
// 
// void CCharaAbility::ResetSkillInfo()
// {
// }
// 
// void CCharaAbility::ResetEquipInfo()
// {
// 	i3mem::FillZero( m_arAbilityValue, sizeof( m_arAbilityValue ));
// }
// 
// void CCharaAbility::AddSkillValue( CHARA_SLOT_SKILL* pSkillInfo, T_ItemID PrimaryID, T_ItemID SecondaryID)
// {
// 	UINT8 ui8WeaponClass;
// 
// 	if( pSkillInfo->m_MainSkill != MAIN_SKILL_NONE )
// 	{
// 		ui8WeaponClass = GET_ITEM_SUBTYPE( PrimaryID );
// 
// 		CSkillInfo SkillInfo;
// 		SkillTable::GetInstance().FindSkillInfo(SKILL_TYPE_MAIN, pSkillInfo->m_MainSkill, SkillInfo);
// 
// 		switch( pSkillInfo->m_MainSkill )
// 		{
// 		case MAIN_ASSAULT_FIREDELAY:
// 		case MAIN_SMG_FIREDELAY:
// 		case MAIN_SHOTGUN_FIREDELAY:
// 			{
// 				if( ui8WeaponClass == WEAPON_CLASS_ASSAULT ||
// 					ui8WeaponClass == WEAPON_CLASS_SMG ||
// 					ui8WeaponClass == WEAPON_CLASS_SHOTGUN )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		case MAIN_ASSAULT_DAMAGE:
// 		case MAIN_MG_DAMAGE:
// 			{
// 				if( ui8WeaponClass == WEAPON_CLASS_ASSAULT ||
// 					ui8WeaponClass == WEAPON_CLASS_MG )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		case MAIN_ASSAULT_CONTROL:
// 		case MAIN_SNIPER_CONTROL:
// 		case MAIN_SHOTGUN_CONTROL:
// 		case MAIN_MG_CONTROL:
// 		case MAIN_SMG_CONTROL:
// 			{
// 				if( ui8WeaponClass == WEAPON_CLASS_ASSAULT ||
// 					ui8WeaponClass == WEAPON_CLASS_MG ||
// 					ui8WeaponClass == WEAPON_CLASS_SHOTGUN ||
// 					ui8WeaponClass == WEAPON_CLASS_SMG ||
// 					ui8WeaponClass == WEAPON_CLASS_SNIPER  )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		case MAIN_ASSAULT_RANGE:
// 		case MAIN_SNIPER_RANGE:
// 			{
// 				if( ui8WeaponClass == WEAPON_CLASS_ASSAULT ||
// 					ui8WeaponClass == WEAPON_CLASS_SNIPER  )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		case MAIN_SMG_PENETRATE:
// 		case MAIN_SNIPER_PENETRATE:
// 		case MAIN_MG_PENETRATE:
// 			{
// 				if( ui8WeaponClass == WEAPON_CLASS_MG ||
// 					ui8WeaponClass == WEAPON_CLASS_SMG ||
// 					ui8WeaponClass == WEAPON_CLASS_SNIPER  )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		case MAIN_SMG_RELOAD:
// 		case MAIN_SHOTGUN_RELOAD:
// 		case MAIN_MG_RELOAD:
// 			{
// 				if( ui8WeaponClass == WEAPON_CLASS_MG ||
// 					ui8WeaponClass == WEAPON_CLASS_SHOTGUN ||
// 					ui8WeaponClass == WEAPON_CLASS_SMG )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		case MAIN_SNIPER_CHANGE:
// 		case MAIN_SHOTGUN_CHANGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_SHOTGUN ||
// 					ui8WeaponClass == WEAPON_CLASS_SNIPER  )
// 				{
// 					m_arSkillValue[ eABILITY_PRIM_FIREDELAY ] = SkillInfo.m_ar32Value[pSkillInfo->m_ui8MainSkillLv];
// 				}
// 			}
// 			break;
// 		}
// 	}
// 
// 	if( pSkillInfo->m_AssistSkill != ASSIST_SKILL_NONE )
// 	{
// 		ui8WeaponClass = GET_ITEM_SUBTYPE( SecondaryID );
// 
// 		CSkillInfo SkillInfo;
// 		SkillTable::GetInstance().FindSkillInfo(SKILL_TYPE_ASSIST, pSkillInfo->m_AssistSkill, SkillInfo);
// 
// 		switch( pSkillInfo->m_AssistSkill )
// 		{
// 		case ASSIST_HANDGUN_DAMAGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_SUB_DAMAGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_THROW_DAMAGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_THROWING_DAMAGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_MELEE_DAMAGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_MELEE_DAMAGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_HANDGUN_CONTROL:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_SUB_CONTROL ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_HANDGUN_FIREDELAY:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_SUB_FIREDELAY ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_HANDGUN_RELOAD:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_SUB_RELOAD ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_MELEE_RANGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_MELEE_RANGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_MELEE_CHANGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_MELEE_CHANGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_THROW_EXPLOSION_RANGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_EXPLOSION_RANGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_THROW_THROW_RANGE:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_THROWING_RANGE ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		case ASSIST_THROW_EXPLOSION_TIME:
// 			{
// 				if(	ui8WeaponClass == WEAPON_CLASS_HANDGUN )
// 				{
// 					m_arSkillValue[ eABILITY_EXPLOSION_TIME ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8AssistSkillLv ];
// 				}
// 			}
// 			break;
// 		}
// 	}
// 
// 	if( pSkillInfo->m_CommonSkill != COMMON_SKILL_NONE )
// 	{
// 		CSkillInfo SkillInfo;
// 		SkillTable::GetInstance().FindSkillInfo(SKILL_TYPE_COMMON, pSkillInfo->m_CommonSkill, SkillInfo);
// 
// 		switch( pSkillInfo->m_CommonSkill )
// 		{
// 		case COMMON_WALK:
// 			{
// 				m_arSkillValue[ eABILITY_WALK ]	= SkillInfo.m_ar32Value[ pSkillInfo->m_ui8CommonSkillLv ];
// 			}
// 			break;
// 		case COMMON_RUN:
// 			{
// 				m_arSkillValue[ eABILITY_RUN ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8CommonSkillLv ];
// 			}
// 			break;
// 		case COMMON_DAMAGE_RATE:
// 			{
// 				m_arSkillValue[ eABILITY_DEF ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8CommonSkillLv ];
// 			}
// 			break;
// 		case COMMON_HELMET_DEF:
// 			{
// 				m_arSkillValue[ eABILITY_HELMET ] = SkillInfo.m_ar32Value[ pSkillInfo->m_ui8CommonSkillLv ];
// 			}
// 			break;
// 		case COMMON_JUMP:
// 			{
// 				m_arSkillValue[ eABILITY_JUMP ]	= SkillInfo.m_ar32Value[ pSkillInfo->m_ui8CommonSkillLv ];
// 			}
// 			break;
// 		}
// 	}
// }
// 
// REAL32 CCharaAbility::GetSkillValue( SKILL_ABILITY eIdx )
// {
// 	if( eIdx <	MAX_ABILITY_COUNT )
// 		return m_arSkillValue[ eIdx ];
// 	else return 0.0f;
// }
// 
// REAL32 CCharaAbility::GetDamage_Skill( WEAPON_USAGE_TYPE usageType )
// {
// 	switch( usageType )
// 	{
// 	case WEAPON_USAGE_PRIMARY:
// 		return m_arSkillValue[ eABILITY_PRIM_DAMAGE ];			
// 	case WEAPON_USAGE_SECONDARY:
// 		return m_arSkillValue[ eABILITY_SUB_DAMAGE ];
// 	case WEAPON_USAGE_MELEE:
// 		return m_arSkillValue[ eABILITY_MELEE_DAMAGE ];
// 	case WEAPON_USAGE_THROWING:
// 		return m_arSkillValue[ eABILITY_THROWING_DAMAGE ];
// 	default :
// 		return 0.0f;
// 	}
// }
// 
// REAL32 CCharaAbility::GetDefenseRate_Skill()
// {
// 	return m_arSkillValue[ eABILITY_DEF ];
// }
// 
// REAL32 CCharaAbility::GetFireDelay_Skill( WEAPON_USAGE_TYPE usageType )
// {
// 	switch( usageType )
// 	{
// 	case WEAPON_USAGE_PRIMARY:
// 		return m_arSkillValue[ eABILITY_PRIM_FIREDELAY ];
// 	case WEAPON_USAGE_SECONDARY:
// 		return m_arSkillValue[ eABILITY_SUB_FIREDELAY ];
// 	default:
// 		return 0.0f;
// 	}
// }
// 
// REAL32 CCharaAbility::GetExplosionRange_Skill()
// {
// 	return m_arSkillValue[ eABILITY_EXPLOSION_RANGE ];
// }