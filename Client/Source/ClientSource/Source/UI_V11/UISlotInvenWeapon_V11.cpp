#include "pch.h"

#include "UI_V11/UISlotInvenWeapon_V11.h"
#include "Ui_V11/UIPhaseInventory_V11.h"

I3_CLASS_INSTANCE( UISlotInvenWeapon_V11);

UISlotInvenWeapon_V11::UISlotInvenWeapon_V11()
{
}

UISlotInvenWeapon_V11::~UISlotInvenWeapon_V11()
{
}

/*virtual*/ bool UISlotInvenWeapon_V11::OnSelect(  bool bSound)
{
	INT32 idx = GetInventory()->FindWeaponSlot( this);
	I3ASSERT( idx != -1);

	if( GetInventory()->OnSelectWeapon( idx, bSound) == false)
		return false;

	return UISlotInvenBase_V11::OnSelect( bSound);
}


