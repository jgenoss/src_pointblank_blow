#if !defined( __UI_SLOT_INVENWEAPON_H__)
#define __UI_SLOT_INVENWEPAON_H__

#include "UI_V11/UISlotInvenBase_V11.h"

class UISlotInvenWeapon_V11 : public UISlotInvenBase_V11
{
	I3_CLASS_DEFINE( UISlotInvenWeapon_V11, UISlotInvenBase_V11);


public:
	UISlotInvenWeapon_V11();
	virtual ~UISlotInvenWeapon_V11();

	virtual bool	OnSelect( bool bSound = true);

	i3UIButtonImageSet *	GetWeaponImage( INT32 idx) {
		I3_BOUNDCHK( idx, (INT32)m_ControlList.size());
		return m_ControlList[ idx];
	}
};

#endif
