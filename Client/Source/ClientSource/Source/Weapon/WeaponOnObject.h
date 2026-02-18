#if !defined( _WEAPON_ON_OBJECT_H__)
#define _WEAPON_ON_OBJECT_H__

#include "WeaponBase.h"

class CGameObjectWithWeapon;

class WeaponOnObject :	public WeaponBase
{
private:
	INT32 m_iCameraLookAtIdx;
	CGameObjectWithWeapon* m_pObjectWithWeapon;
	

protected:
	REAL32 m_rObjectRho;
	REAL32 m_rObjectTheta;
	bool m_bActive;

	void					SetObjectWithWeapon(CGameObjectWithWeapon* pWeapon) { m_pObjectWithWeapon = pWeapon; }
	CGameObjectWithWeapon*	GetObjectWithWeapon() { return m_pObjectWithWeapon; }

public:
	WeaponOnObject();
	virtual ~WeaponOnObject();

	virtual void	Reset( void) override;
	void			InitVariables();
	virtual void	InitWeaponOnObject();
	virtual void	ActiveWeaponOnObject(CGameObjectWithWeapon* pWeapon);
	virtual void	DeactiveWeaponOnObject();
	INT32			GetCameraLookAtIdx() { return m_iCameraLookAtIdx; }
	
	MATRIX*			GetCameraLookAtCacheMatrix();
	MATRIX*			GetCameraLookAtMatrix();
	void			SetCameraLookAtIdx(INT32 iIdx) { m_iCameraLookAtIdx = iIdx; }
	virtual VEC3D*	GetMuzzlePos() = 0;
	virtual VEC3D*	GetMuzzleDir() = 0;

	INT32			GetEnterCharaIdx();
	
	bool			IsControlUser();
	

	virtual bool	FireWeaponOnObject() = 0;

	REAL32			getTheta( void)					{ return m_rObjectTheta; }
};

#endif // _WEAPON_ON_OBJECT_H__