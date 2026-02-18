#if !defined( __GRENADE_CHECK_H)
#define __GRENADE_CHECK_H


//수류탄 (핵대응) 관리용 구조체
struct GrenadeCheck
{
	i3::vector<VEC3D>	GrenadePosList;		// 2개의 사이즈같음..
	i3::vector<VEC3D>	CharaPosList;
	WEAPON_CLASS_TYPE	WeaponClass;
	WEAPON::RES_ID		WeaponNumber;

	void Reset()
	{
		GrenadePosList.clear();
		CharaPosList.clear();
		WeaponClass = WEAPON_CLASS_UNKNOWN;
		WeaponNumber = 0;
	}
};

class GrenadeContext : public i3::shared_ginst<GrenadeContext> 
{
public:
	GrenadeContext() {}
	~GrenadeContext() {}

	GrenadeCheck		m_GrenadeCheck[MAX_THROWWEAPON_SLOP];

	void ResetGrenadeCheck();
};


//////////////////////////////////////////////
//struct _GrenadeCheck_
//{
//	i3::vector<VEC3D> GrenadePosList;		// 2개의 사이즈같음..
//	i3::vector<VEC3D> CharaPosList;
//	INT32 WeaponClass;
//	INT32 WeaponNumber;
//};

//void ResetGrenadeCheck(GrenadeCheck* out) 
//{
//
//}
///////////////////////////////////////////////
#endif

