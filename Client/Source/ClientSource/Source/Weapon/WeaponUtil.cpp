#include "pch.h"
#include "WeaponUtil.h"
#include "WeaponItemID.h"

const char * WEAPON::GetClassName( WEAPON_CLASS_TYPE weaponType)
{
	switch( weaponType)
	{
	case WEAPON_CLASS_KNIFE					:	return "Knife";
	case WEAPON_CLASS_HANDGUN				:	return "HandGun";
	case WEAPON_SUBCLASS_ASSAULT			:	return "AssasultRifle";
	case WEAPON_CLASS_ASSAULT				:	return "AssaultRifle";
	case WEAPON_SUBCLASS_SMG				:	return "SMG";
	case WEAPON_CLASS_SMG					:	return "SMG";				
	case WEAPON_SUBCLASS_SNIPER				:	return "SniperRifle";
	case WEAPON_CLASS_SNIPER				:	return "SniperRifle";
	case WEAPON_SUBCLASS_SHOTGUN			:	return "ShotGun";
	case WEAPON_CLASS_SHOTGUN				:	return "ShotGun";			
	case WEAPON_CLASS_THROWING_GRENADE		:	return "Throwing";			
	case WEAPON_CLASS_THROWING_CONTAIN		:	return "Throwing";			
	case WEAPON_CLASS_MISSION				:	return "Mission";			
	case WEAPON_CLASS_MG					:	return "MG";				
	case WEAPON_CLASS_GRENADESHELL			:	break;
	case WEAPON_CLASS_BOMBTRIGGER			:	return "Throwing";
	case WEAPON_CLASS_CIC					:	return "CIC";
	case WEAPON_CLASS_DUALHANDGUN			:	return "DualHandGun";
	case WEAPON_CLASS_DUALKNIFE				:	return "DualKnife";
	case WEAPON_SUBCLASS_G_LAUNCHER			:	return "RocketLauncher";
	case WEAPON_CLASS_ROCKET_LAUNCHER		:	return "RocketLauncher";
	case WEAPON_CLASS_OBJECT_GUN			:	break;
	case WEAPON_CLASS_DUALSMG				:	return "DualSMG";
	case WEAPON_CLASS_DINO					:	break;
	case WEAPON_CLASS_DINO_LEFT_SCRATCH 	:	break;
	case WEAPON_CLASS_TREX					:	break;
	case WEAPON_CLASS_STING					:	break;
	case WEAPON_CLASS_KNUCKLE				:	return "Knife";
	case WEAPON_CLASS_ELITE_RIGHT_SCRATCH	:	break;
	case WEAPON_CLASS_ELITE_LEFT_SCRATCH	:	break;
	case WEAPON_SUBCLASS_BOW				:	return "Bow";
	case WEAPON_CLASS_BOW					:	return "Bow";
	case WEAPON_CLASS_THROWING_ATTACK		:	return "Throwing";			
	case WEAPON_CLASS_THROWING_HEAL			:	return "Throwing";	
	case WEAPON_CLASS_DUALSHOTGUN			:	return "DualShotGun";
	
	default:
		I3PRINTLOG(I3LOG_FATAL,  "no set ai context from weapon type1");
		break;
	}

	return nullptr;
}


const char * WEAPON::GetBodyUpperName( CHARA_ACTION_BODYUPPER bodyUpper, CWeaponInfo * pWeaponInfo)
{
	switch( bodyUpper)
	{
	case CHARA_BODYUPPER_ATTACKIDLE:		return "AttackIdle";
	case CHARA_BODYUPPER_ZOOM:
		if( pWeaponInfo->isAnimZoomType() ) //	Zoom 가능한 무기만 찾도록 한다.
			return "Zoom";
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_GLAUNCHER ||
			pWeaponInfo->getExtensionType(1) == WEAPON::EXT_GLAUNCHER)
		{
			return "Grenade";
		}
		break;
	case CHARA_BODYUPPER_SHOTGUN:
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_SHOTGUN_LAUNCHER ||
			pWeaponInfo->getExtensionType(1) == WEAPON::EXT_SHOTGUN_LAUNCHER)
		{
			return "ShotGun";
		}
		break;
	case CHARA_BODYUPPER_KNIFE :
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_BAYONET ||
			pWeaponInfo->getExtensionType(1) == WEAPON::EXT_BAYONET)
		{
			return "Knife";
		}
		break;

	case	CHARA_BODYUPPER_CROSSFIRE :
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_CROSSFIRE )
		{
			return "CrossIdle";
		}
		break;
	}

	return nullptr;
}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
T_ItemID WEAPON::MakeRandomWeaponItem( WEAPON_CLASS_TYPE * classList, INT32 listSize)
{
	UINT32 classRand = i3Math::Rand() % listSize;
	WEAPON::RES_ID num = i3Math::Rand() % WEAPON::getItemCount( classList[classRand]);
	
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classList[classRand], num);
		
	while( (pWeaponInfo == nullptr) || (pWeaponInfo->GetOriginalWeaponItemID() > 0)
		|| (g_pWeaponManager->IsExistResource( classList[classRand], num) == false) )
	{
		classRand = i3Math::Rand() % listSize;
		num = i3Math::Rand() % WEAPON::getItemCount( classList[classRand]);
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classList[classRand], num);
	}

	ITEM_TYPE itemType = Weapon::GetItemTypeFromClass( classList[ classRand]);

	return MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, itemType, classList[classRand], num);
}
#endif