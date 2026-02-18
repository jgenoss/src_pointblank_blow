#if !defined( __SCOPE_GUN_INFO_H)
#define __SCOPE_GUN_INFO_H

#include "GunInfo.h"

class CScopeGunInfo :	public CGunInfo
{
	I3_CLASS_DEFINE( CScopeGunInfo, CGunInfo);
private:
	REAL32					m_rScopeFOV;				// 1단 Scope시의 FOV값
	REAL32					m_rScopeFOV2;				// 2단 Scope시의 FOV값
	
	REAL32					m_fThrowSpeed;

	UINT8					m_nMaxScopeLevel;			// 최대 스코프 단계 (WEAPON::ZOOM_SCOPE 타입만 가능)
	UINT8					m_nScopeType;

protected:
	virtual void		_ReadExtensionKey( i3RegKey * pKey, INT32 idx);
	
public:
	CScopeGunInfo();
	virtual ~CScopeGunInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	// 속성값 접근
	virtual INT32			GetMaxScopeLevel();			// 최대 스코프 단계 (WEAPON::ZOOM_SCOPE 타입만 가능)

	virtual REAL32			GetScopeFOV();			// 1단 Scope시의 FOV값
	virtual REAL32			GetScopeFOV2();				// 2단 Scope시의 FOV값

	virtual REAL32			GetThrowSpeed();

	virtual INT32			GetScopeType( void);
};

#endif