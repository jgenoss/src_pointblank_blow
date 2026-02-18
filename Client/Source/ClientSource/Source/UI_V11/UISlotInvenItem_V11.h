#if !defined( __UI_SLOT_INVEN_ITEM_H__)
#define __UI_SLOT_INVEN_ITEM_H__

#include "UI_V11/UISlotInvenBase_V11.h"

class UISlotInvenItem_V11 : public UISlotInvenBase_V11
{
	I3_CLASS_DEFINE( UISlotInvenItem_V11, UISlotInvenBase_V11);

public:
	UISlotInvenItem_V11();
	virtual ~UISlotInvenItem_V11();

	virtual bool	OnSelect( bool bSound = true);
};

#endif
