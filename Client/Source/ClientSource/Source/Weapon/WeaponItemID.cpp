#include "pch.h"
#include "WeaponItemID.h"

static WEAPON::RES_ID sWeaponItemID[WEAPON::eINDEX_MAX];
static UINT32 sWeaponItemCount[ WEAPON_CLASS_COUNT];
static i3::vector<WEAPON::RES_ID>	sSettableWeaponID;

void WEAPON::Initialize( void)
{
	sWeaponItemID[ C4 ]							= (WEAPON::RES_ID)GetItemIndex( "MISSION_C4");
	sWeaponItemID[ SHUTTLEX]					= (WEAPON::RES_ID)GetItemIndex( "MISSION_SHUTTLEX");
	sWeaponItemID[ SP1]							= (WEAPON::RES_ID)GetItemIndex( "MISSION_SUPPLYBASE_LV1");
	sWeaponItemID[ SP2]							= (WEAPON::RES_ID)GetItemIndex( "MISSION_DINO_DUMMY_LV1");
	sWeaponItemID[ SP3]							= (WEAPON::RES_ID)GetItemIndex( "MISSION_SENTRYGUN_LV1");
	sWeaponItemID[ RPG7]						= (WEAPON::RES_ID)GetItemIndex( "ROCKET_LAUNCHER_RPG7");
	sWeaponItemID[ RPG7_OUTPOST]				= (WEAPON::RES_ID)GetItemIndex( "ROCKET_LAUNCHER_RPG7_OUTPOST");
	sWeaponItemID[ RPG7_BOSS]					= (WEAPON::RES_ID)GetItemIndex( "ROCKET_LAUNCHER_RPG7_BOSS");
	sWeaponItemID[ K201GL]						= (WEAPON::RES_ID)GetItemIndex( "ASSAULT_K201_GL");
	sWeaponItemID[ S870MCS_IN_MODE]				= (WEAPON::RES_ID)GetItemIndex( "SHOTGUN_870MCS_IN_MODE");
	sWeaponItemID[ SSG69_IN_MODE]				= (WEAPON::RES_ID)GetItemIndex( "SNIPER_SSG69_IN_MODE");
	sWeaponItemID[ BAREFIST_IN_MODE]			= (WEAPON::RES_ID)GetItemIndex( "KNIFE_BAREFIST_IN_MODE");
	sWeaponItemID[ KNIFE_M7]					= (WEAPON::RES_ID)GetItemIndex( "KNIFE_M7");
	sWeaponItemID[ KNIFE_M7_GOLD]				= (WEAPON::RES_ID)GetItemIndex( "KNIFE_M7_GOLD");
	sWeaponItemID[ GH5007]						= (WEAPON::RES_ID)GetItemIndex( "KNIFE_GH5007");
	sWeaponItemID[ GSHELL_POISON]				= (WEAPON::RES_ID)GetItemIndex( "GRENADESHELL_POISON");
	sWeaponItemID[ GSHELL_K201]					= (WEAPON::RES_ID)GetItemIndex( "GRENADESHELL_K201");
	sWeaponItemID[ GSHELL_K201_BOSS]			= (WEAPON::RES_ID)GetItemIndex( "GRENADESHELL_K201_BOSS");
	sWeaponItemID[ GSHELL_ARROW]				= (WEAPON::RES_ID)GetItemIndex( "GRENADESHELL_ARROW");
	sWeaponItemID[ GSHELL_SMOKE_ARROW]			= (WEAPON::RES_ID)GetItemIndex( "GRENADESHELL_SMOKE_ARROW");
	sWeaponItemID[ THROWING_K400]				= (WEAPON::RES_ID)GetItemIndex( "THROWING_K400");
	sWeaponItemID[ THROWING_FLASHBANG]			= (WEAPON::RES_ID)GetItemIndex( "THROWING_FLASHBANG");
	sWeaponItemID[ THROWING_FLASHBANG_PLUS]		= (WEAPON::RES_ID)GetItemIndex( "THROWING_FLASHBANG_PLUS");
	sWeaponItemID[ KNIFE_KUKRII]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_KUKRII");
	sWeaponItemID[ FANG_BLADE]					= (WEAPON::RES_ID)GetItemIndex("KNIFE_FANG_BLADE");
	sWeaponItemID[ KNIFE_KERIS]					= (WEAPON::RES_ID)GetItemIndex("KNIFE_KERIS");
	sWeaponItemID[ KNIFE_BALLISTIC]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_BALLISTIC");
	sWeaponItemID[ KNIFE_BALLISTIC_S]			= (WEAPON::RES_ID)GetItemIndex("KNIFE_BALLISTIC_I3S");
	sWeaponItemID[ KNIFE_BALLISTIC_GRS]			= (WEAPON::RES_ID)GetItemIndex("KNIFE_BallisticKnife_GRS");
	sWeaponItemID[ KNIFE_BALLISTIC_GRS_S]		= (WEAPON::RES_ID)GetItemIndex("KNIFE_BallisticKnife_GRS_I3S");
	sWeaponItemID[ CANDYCANE]					= (WEAPON::RES_ID)GetItemIndex("KNIFE_CANDYCANE");
	sWeaponItemID[ MINIAXE]						= (WEAPON::RES_ID)GetItemIndex("KNIFE_MINIAXE");
	sWeaponItemID[ SHOVEL]						= (WEAPON::RES_ID)GetItemIndex("KNIFE_SHOVEL");
	sWeaponItemID[ SHOVEL_ROYAL]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_SHOVEL_ROYAL");
	sWeaponItemID[ DUALKNIFE_BONE]				= (WEAPON::RES_ID)GetItemIndex("DUALKNIFE_BONE");
	sWeaponItemID[ BRASS_KNUCKLE]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_BRASS_KNUCKLE");
	sWeaponItemID[ PUMPKIN_KNUCKLE]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_PUMPKIN_KNUCKLE");
	sWeaponItemID[ GARENA_KNUCKLE]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_GARENA_KNUCKLE");
	sWeaponItemID[ SNIPER_SVU]					= (WEAPON::RES_ID)GetItemIndex("SNIPER_SVU");
	sWeaponItemID[ SNIPER_SCAR_LB]				= (WEAPON::RES_ID)GetItemIndex("SNIPER_SCAR_LB_SNIPER");
	sWeaponItemID[ BOMBTRIGGER_DEFAULT]			= (WEAPON::RES_ID)GetItemIndex("BOMBTRIGGER_DEFAULT");
	sWeaponItemID[ SHOTGUN_M26_LSS]				= (WEAPON::RES_ID)GetItemIndex("SHOTGUN_M26_LSS");

	sWeaponItemID[ CHINESECLEAVER]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_CHINESE_CLEAVER");
	sWeaponItemID[ CHINESECLEAVER_DUMMY]		= (WEAPON::RES_ID)GetItemIndex("KNIFE_CHINESE_CLEAVER_DUMMY");
	sWeaponItemID[ CHINESECLEAVER_SEIZED]		= (WEAPON::RES_ID)GetItemIndex("KNIFE_CHINESE_CLEAVER_SEIZED");
	sWeaponItemID[ CHINESECLEAVER_SEIZED_DUMMY] = (WEAPON::RES_ID)GetItemIndex("KNIFE_CHINESE_CLEAVER_SEIZED_DUMMY");
	sWeaponItemID[ FRYINGPAN]					= (WEAPON::RES_ID)GetItemIndex("KNIFE_FRYINGPAN");
	sWeaponItemID[ FRYINGPAN_DUMMY]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_FRYINGPAN_DUMMY");
	sWeaponItemID[ HAIRDRYER]					= (WEAPON::RES_ID)GetItemIndex("KNIFE_HAIRDRYER");
	sWeaponItemID[ HAIRDRYER_DUMMY]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_HAIRDRYER_DUMMY");
	sWeaponItemID[ GOAT_HAMMER ]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_GOAT_HAMMER");

	sWeaponItemID[ CROWNWATERMELON]				= (WEAPON::RES_ID)GetItemIndex("CROWNWATERMELON");
	sWeaponItemID[ ON_OBJECT_M197]				= (WEAPON::RES_ID)GetItemIndex("ON_OBJECT_M197");
	sWeaponItemID[ SUB_BOW_COMPOUND_BLUE]		= (WEAPON::RES_ID)GetItemIndex("SUB_BOW_COMPOUND_BLUE");

	sWeaponItemID[ DINO_TREX_CRASH]				= (WEAPON::RES_ID)GetItemIndex( "DINO_TREX_CRASH");
	sWeaponItemID[ DINO_TREX_HOWL]				= (WEAPON::RES_ID)GetItemIndex( "DINO_TREX_HOWL");
	sWeaponItemID[ DINO_STING_SPIT]				= (WEAPON::RES_ID)GetItemIndex( "DINO_STING_SPIT");
	sWeaponItemID[ DINO_TANK_BUTT]				= (WEAPON::RES_ID)GetItemIndex( "DINO_TANK_BUTT");
	sWeaponItemID[ DINO_TANK_CRASH]				= (WEAPON::RES_ID)GetItemIndex( "DINO_TANK_CRASH");
	sWeaponItemID[ DINO_ACID_EXPLOSION] 		= (WEAPON::RES_ID)GetItemIndex( "DINO_ACID_EXPLOSION");
	sWeaponItemID[ DINO_CC_ACID_EXPLOSION]		= (WEAPON::RES_ID)GetItemIndex( "DINO_CC_ACID_EXPLOSION");
	sWeaponItemID[ DINO_ACID_VULCAN_EXPLOSION]	= (WEAPON::RES_ID)GetItemIndex( "DINO_ACID_VULCAN_EXPLOSION");
	sWeaponItemID[ DOMI_MEATBOMB]				= (WEAPON::RES_ID)GetItemIndex( "THROWING_MEATBOMB");
	sWeaponItemID[ SENTRYGUN_TRANSPARENT]		= (WEAPON::RES_ID)GetItemIndex( "SENTRYGUN_TRANSPARENT");
	sWeaponItemID[ SENTRYGUN_LV1]				= (WEAPON::RES_ID)GetItemIndex( "SENTRYGUN_LV1");
	sWeaponItemID[ SENTRYGUN_LV2]				= (WEAPON::RES_ID)GetItemIndex( "SENTRYGUN_LV2");
	sWeaponItemID[ SENTRYGUN_LV3]				= (WEAPON::RES_ID)GetItemIndex( "SENTRYGUN_LV3");
	sWeaponItemID[ SENTRYGUN_LV4]				= (WEAPON::RES_ID)GetItemIndex( "SENTRYGUN_LV4");
	sWeaponItemID[ DUMMY_TRANSPARENT]			= (WEAPON::RES_ID)GetItemIndex( "DUMMY_TRANSPARENT");
	sWeaponItemID[ DUMMY_LV1]					= (WEAPON::RES_ID)GetItemIndex( "DUMMY_LV1");
	sWeaponItemID[ DUMMY_LV2]					= (WEAPON::RES_ID)GetItemIndex( "DUMMY_LV2");
	sWeaponItemID[ DUMMY_LV3]					= (WEAPON::RES_ID)GetItemIndex( "DUMMY_LV3");
	sWeaponItemID[ DUMMY_LV4]					= (WEAPON::RES_ID)GetItemIndex( "DUMMY_LV4");
	sWeaponItemID[ SUPPLY_HP]					= (WEAPON::RES_ID)GetItemIndex( "SUPPLY_HP");
	sWeaponItemID[ SUPPLY_AMMO]					= (WEAPON::RES_ID)GetItemIndex( "SUPPLY_AMMO");
	sWeaponItemID[ SKILLSHOP]					= (WEAPON::RES_ID)GetItemIndex( "SKILLSHOP");
	sWeaponItemID[ SNOW_BALL_BLASTER]			= (WEAPON::RES_ID)GetItemIndex( "ASSAULT_SNOW-BALL_BLASTER");
	sWeaponItemID[ HANDGUN_MGL32]				= (WEAPON::RES_ID)GetItemIndex( "HANDGUN_MGL32");
	sWeaponItemID[ KNIFE_ICICLE]				= (WEAPON::RES_ID)GetItemIndex( "KNIFE_ICICLE");
	sWeaponItemID[ KNIFE_ICE_FORK]				= (WEAPON::RES_ID)GetItemIndex( "KNIFE_ICE_FORK");
	sWeaponItemID[ CHINESECLEAVER_CNY2015]		= (WEAPON::RES_ID)GetItemIndex("KNIFE_CHINESE_CLEAVER_CNY2015");
	sWeaponItemID[ CHINESECLEAVER_CNY2015_DUMMY]= (WEAPON::RES_ID)GetItemIndex("KNIFE_CHINESE_CLEAVER_CNY2015_DUMMY");
	sWeaponItemID[ DUAL_KUNAI]					= (WEAPON::RES_ID)GetItemIndex("DUALKNIFE_KUNAI");
	sWeaponItemID[ KUNAI ]						= (WEAPON::RES_ID)GetItemIndex("KNIFE_KUNAI");
	sWeaponItemID[ HALLOWEEN_HAMMER]				= (WEAPON::RES_ID)GetItemIndex("KNIFE_HALLOWEEN_HAMMER");

	sWeaponItemCount[ WEAPON_CLASS_KNIFE]			= GetItemIndex("KNIFE_MAX");
	sWeaponItemCount[ WEAPON_CLASS_HANDGUN]			= GetItemIndex("HANDGUN_MAX");
	sWeaponItemCount[ WEAPON_CLASS_ASSAULT]			= GetItemIndex("ASSAULT_MAX");
	sWeaponItemCount[ WEAPON_CLASS_SMG]				= GetItemIndex("SMG_MAX");
	sWeaponItemCount[ WEAPON_CLASS_SNIPER]			= GetItemIndex("SNIPER_MAX");
	sWeaponItemCount[ WEAPON_CLASS_SHOTGUN]			= GetItemIndex("SHOTGUN_MAX");
	sWeaponItemCount[ WEAPON_CLASS_THROWING_GRENADE]= GetItemIndex("THROWING_GRENADE_MAX");
	sWeaponItemCount[ WEAPON_CLASS_THROWING_CONTAIN]= GetItemIndex("THROWING_CONTAIN_MAX");
	sWeaponItemCount[ WEAPON_CLASS_MISSION]			= GetItemIndex("MISSION_MAX");
	sWeaponItemCount[ WEAPON_CLASS_MG]				= GetItemIndex("MG_MAX");
	sWeaponItemCount[ WEAPON_CLASS_GRENADESHELL]	= GetItemIndex("GRENADESHELL_MAX");
	sWeaponItemCount[ WEAPON_CLASS_BOMBTRIGGER]		= GetItemIndex("BOMBTRIGGER_MAX");
	sWeaponItemCount[ WEAPON_CLASS_CIC]				= GetItemIndex("CIC_MAX");
	sWeaponItemCount[ WEAPON_CLASS_DUALHANDGUN]		= GetItemIndex("DUALHANDGUN_MAX");
	sWeaponItemCount[ WEAPON_CLASS_DUALKNIFE]		= GetItemIndex("DUALKNIFE_MAX");
	sWeaponItemCount[ WEAPON_CLASS_ROCKET_LAUNCHER]	= GetItemIndex("ROCKET_LAUNCHER_MAX");
	sWeaponItemCount[ WEAPON_CLASS_OBJECT_GUN]		= GetItemIndex("ON_OBJECT_MAX");
	sWeaponItemCount[ WEAPON_CLASS_DUALSMG]			= GetItemIndex("DUALSMG_MAX");
	sWeaponItemCount[ WEAPON_CLASS_DINO]			= GetItemIndex("DINO_MAX");
	sWeaponItemCount[ WEAPON_CLASS_DINO_LEFT_SCRATCH] = 0;
	sWeaponItemCount[ WEAPON_CLASS_TREX]			= 0;
	sWeaponItemCount[ WEAPON_CLASS_STING]			= 0;
	sWeaponItemCount[ WEAPON_CLASS_KNUCKLE]			= GetItemIndex("KNUCKLE_MAX");
	sWeaponItemCount[ WEAPON_CLASS_ELITE_RIGHT_SCRATCH] = 0;
	sWeaponItemCount[ WEAPON_CLASS_ELITE_LEFT_SCRATCH] = 0;
	sWeaponItemCount[ WEAPON_CLASS_BOW]				= GetItemIndex("BOW_MAX");
	sWeaponItemCount[ WEAPON_CLASS_THROWING_ATTACK]	= GetItemIndex("THROWING_ATTACK_MAX");
	sWeaponItemCount[ WEAPON_CLASS_THROWING_HEAL]	= GetItemIndex("THROWING_HEAL_MAX");
	sWeaponItemCount[ WEAPON_SUBCLASS_ASSAULT]		= GetItemIndex("SUB_ASSAULT_MAX");
	sWeaponItemCount[ WEAPON_SUBCLASS_SHOTGUN]		= GetItemIndex("SUB_SHOTGUN_MAX");
	sWeaponItemCount[ WEAPON_SUBCLASS_SNIPER]		= GetItemIndex("SUB_SNIPER_MAX");
	sWeaponItemCount[ WEAPON_SUBCLASS_SMG]			= GetItemIndex("SUB_SMGMAX");
	sWeaponItemCount[ WEAPON_SUBCLASS_G_LAUNCHER]	= GetItemIndex("SUB_G_LAUNCHER_MAX");
	sWeaponItemCount[ WEAPON_SUBCLASS_BOW]			= GetItemIndex("SUB_BOW_MAX");
	sWeaponItemCount[ WEAPON_CLASS_DUALSHOTGUN]		= GetItemIndex("DUALSHOTGUN_MAX");
}

WEAPON::RES_ID WEAPON::getItemIndex( eINDEX idx)
{
	I3_BOUNDCHK( idx, eINDEX_MAX);
	return sWeaponItemID[ idx];
}

UINT32 WEAPON::getItemCount( WEAPON_CLASS_TYPE type)
{
	return sWeaponItemCount[ type];
}

void WEAPON::addSettableItem( WEAPON::RES_ID id)
{
	sSettableWeaponID.push_back( id);
}
UINT32 WEAPON::getSettableItemCount( void)
{
	return sSettableWeaponID.size();
}

WEAPON::RES_ID WEAPON::getSettableItemIndex( UINT32 cntIndex)
{
	I3ASSERT( cntIndex < sSettableWeaponID.size());
	return sSettableWeaponID.at( cntIndex);
}
