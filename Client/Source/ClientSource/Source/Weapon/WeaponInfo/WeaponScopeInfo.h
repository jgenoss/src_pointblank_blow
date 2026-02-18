#if !defined( __SCOPE_INFO_H__)
#define __SCOPE_INFO_H__

#include "WeaponExtGunInfo.h"

class CWeaponScopeInfo : public CWeaponExtGunInfo
{
	I3_CLASS_DEFINE( CWeaponScopeInfo, CWeaponExtGunInfo);
private:
	VAR_LR(REAL32, 119)					m_rScopeFOV = 0.0f;				// 1단 Scope시의 FOV값
	VAR_LR(REAL32, 120)					m_rScopeFOV2 = 0.0f;				// 2단 Scope시의 FOV값
	
	UINT8					m_nMaxScopeLevel = 0;			// 최대 스코프 단계 (WEAPON::ZOOM_SCOPE 타입만 가능)
	INT32					m_nScopeType = -1;				// 신규로 바뀌었다.
	INT32					m_nSubScopeType = -1;			// 스코프 TEXTURE에 필요한 부가정보들 인덱스 (INFODATABASE 꺼)

public:
	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;

	virtual void	OnEncrypt( UINT8 * pCode) override;

	// 속성값 접근
	INT32		GetMaxScopeLevel( UINT8 code) const;			// 최대 스코프 단계 (WEAPON::ZOOM_SCOPE 타입만 가능)

	REAL32		GetScopeFOV( UINT8 code) const;			// 1단 Scope시의 FOV값
	REAL32		GetScopeFOV2( UINT8 code) const;				// 2단 Scope시의 FOV값

	INT32		GetScopeType( UINT8 code) const;
	INT32		GetSubScopeType() const;
};

#endif