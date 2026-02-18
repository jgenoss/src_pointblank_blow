#include "pch.h"

#include "UI_V11/UISlotInvenItem_V11.h"
#include "UI_V11/UIPhaseInventory_V11.h"

#include "UI/UIUtil.h"

I3_CLASS_INSTANCE( UISlotInvenItem_V11);

UISlotInvenItem_V11::UISlotInvenItem_V11()
{
}

UISlotInvenItem_V11::~UISlotInvenItem_V11()
{	
}

/*virtual*/ bool UISlotInvenItem_V11::OnSelect( bool bSound)
{
	INT32 idx = GetInventory()->FindItemSlot( this);
	I3ASSERT( idx != -1);

	if( GetInventory()->OnSelectItem( idx, bSound) == false)
		return false;

	return UISlotInvenBase_V11::OnSelect( bSound);
}

