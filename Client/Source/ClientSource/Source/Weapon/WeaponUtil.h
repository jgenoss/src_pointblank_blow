#if !defined( __WEAPON_UTIL_H__)
#define __WEAPON_UTIL_H__

namespace WEAPON {
	const char * GetClassName( WEAPON_CLASS_TYPE weaponType);

	const char * GetBodyUpperName( CHARA_ACTION_BODYUPPER bodyUpper, CWeaponInfo * pWeaponInfo);

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	T_ItemID	MakeRandomWeaponItem( WEAPON_CLASS_TYPE * classList, INT32 listSize);
#endif
};

#endif
