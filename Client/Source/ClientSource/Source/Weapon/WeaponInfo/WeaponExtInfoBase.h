#if !defined( __WEAPON_EXT_INFO_BASE_H__)
#define __WEAPON_EXT_INFO_BASE_H__

#include "../../../../Common/CommonSource/WeaponDefine.h"

class CWeaponExtInfoBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CWeaponExtInfoBase, i3ElementBase);

private:
	WEAPON::EXT_TYPE	m_ExtType;		// Extension Type. (1 ~ n)
	T_ItemID			m_ItemID;		// 익스텐션 기능이 있는 무기의 활성화 상태의 정보를 가져오는 무기

	bool				m_bUseExtShape;	// ExtShape 사용 Flag (false : 원본무기 ShapeIdx 사용, true : 확장무기 ShapeIdx 사용)

public:
	CWeaponExtInfoBase();
	virtual ~CWeaponExtInfoBase();

	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType);
	virtual void	OnEncrypt( UINT8 * pCode);

	WEAPON::EXT_TYPE	GetExtType( UINT8 code) const;
	T_ItemID			GetItemID( UINT8 code) const;

	bool				IsUseExtShape(void) const		{ return m_bUseExtShape; }
};

#endif
