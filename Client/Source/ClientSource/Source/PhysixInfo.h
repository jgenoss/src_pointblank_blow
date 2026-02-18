#if !defined ( _PHYSIX_INFO_H__)
#define _PHYSIX_INFO_H__

struct PHYSIXINFODATA_CHARA
{
	REAL32		m_rRoot = 0.0f;
	REAL32		m_rHead = 0.0f;
	REAL32		m_rNeck = 0.0f;
	REAL32		m_rClavicle = 0.0f;
	REAL32		m_rBody = 0.0f;
	REAL32		m_rUpperLeg = 0.0f;
	REAL32		m_rLowLeg = 0.0f;
	REAL32		m_rFoot = 0.0f;
	REAL32		m_rUpperArm = 0.0f;
	REAL32		m_rLowArm = 0.0f;
	REAL32		m_rHand = 0.0f;
} ;

struct PHYSIXINFODATA_WEAPON
{
	REAL32		m_rWeapon_Assult = 0.0f;
	REAL32		m_rWeapon_Shotgun = 0.0f;
	REAL32		m_rWeapon_SMG = 0.0f;
	REAL32		m_rWeapon_Pistol = 0.0f;
	REAL32		m_rWeapon_Rifle = 0.0f;
	REAL32		m_rWeapon_Knife = 0.0f;
	REAL32		m_rWeapon_Grenade = 0.0f;
} ;

class CPhysixInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( CPhysixInfo, i3NamedElement);

public:
	// global
	REAL32		m_rGravity = 0.0f;
	REAL32		m_rFXtoForce = 0.0f;
	REAL32		m_rSleepLinVelocity = 0.0f;
	REAL32		m_rSleepAngVelocity = 0.0f;
	REAL32		m_rBounceThresHold = 0.0f;
	REAL32		m_rMaxAngVelocity = 0.0f;
	REAL32		m_rSimTimeSlop = 0.0f;
	REAL32		m_rStaticFriction = 0.0f;
	REAL32		m_rDynamicFriction = 0.0f;

	REAL32		m_rObjectLinSlop = 0.0f;
	REAL32		m_rObjectAngSlop = 0.0f;

	// chara default variable
	PHYSIXINFODATA_CHARA	m_Damping;
	
	// chara death action physix variable
	PHYSIXINFODATA_WEAPON	m_WeaponDeath;
	REAL32		m_rCharaLinSlop = 0.0f;
	REAL32		m_rCharaAngSlop = 0.0f;
	
	VEC3D		m_vCharaAngForceFeed;

	// object에 가해지는 충격량 데이타
	PHYSIXINFODATA_WEAPON	m_WeaponObject;

	REAL32		m_rHelmet_Feed = 0.0f;
	REAL32		m_rHelmet_Y = 0.0f;
	
	void	ReadInfo( i3RegKey * pKey);
};

class PhysixInfoDataBase : public i3GameNode
{
	I3_CLASS_DEFINE( PhysixInfoDataBase, i3GameNode );

protected:
	CPhysixInfo	*	m_pGlobalInfo = nullptr;

public:
	virtual ~PhysixInfoDataBase();

	bool	Create();

	CPhysixInfo *	getInfo( void)			{	return m_pGlobalInfo;				}

	#if !defined( I3_NO_PROFILE)
	virtual void	OnReloadProperty() override;
	#endif
};

#endif