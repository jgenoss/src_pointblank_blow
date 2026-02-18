#include "pch.h"

#include "UI_V11/UISlotInvenChara_V11.h"
#include "UI_V11/UIPhaseInventory_V11.h"

I3_CLASS_INSTANCE( UISlotInvenChara_V11);

UISlotInvenChara_V11::UISlotInvenChara_V11()
{
	m_CharaID	= 0;
	m_nTeam 	= 0;
	m_nUseTerm 	= 0;
	m_bNotEquip = FALSE;
}

UISlotInvenChara_V11::~UISlotInvenChara_V11()
{
}

/*virtual*/ bool UISlotInvenChara_V11::OnSelect( bool bSound)
{
	INT32 idx = GetInventory()->FindCharaSlot( this);
	I3ASSERT( idx != -1);

	if( GetInventory()->OnSelectCharacter( idx, bSound) == false)
		return false;

	return UISlotInvenBase_V11::OnSelect( bSound);
}

void UISlotInvenChara_V11::SetNotEquip( bool bVal)
{	
	m_bNotEquip = bVal;
}

