#if !defined( __WEAPON_MEDICAL_KIT_H__)
#define __WEAPON_MEDICAL_KIT_H__

#include "WeaponBase.h"

class WeaponMedicalKit :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponMedicalKit, WeaponGrenade );

protected:
	virtual MainWeapon*	CreateMainWeapon() override;

	
protected:
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void	Reset() override;


	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx ) override;


public:
	WeaponMedicalKit( void );
	virtual ~WeaponMedicalKit( void );

};

#endif
