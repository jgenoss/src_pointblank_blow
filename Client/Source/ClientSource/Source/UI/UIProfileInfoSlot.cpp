#include "pch.h"
#include "UIMainFrame.h"
#include "UIProfileInfoSlot.h"
#include "UITabProfileInfo.h"

static UIProfileInfoSlot * GetThis( LuaState * L)
{
	//UISkillSelectionSlot * pSelector = GetSelectStagePopup();
	//INT32 idx = pSelector->FindSlot( L);
	//return pSelector->GetSlot( idx);
}

//--------------------------------------------------------------------------//
//								Lua Glue									//
extern "C" {

}

LuaFuncReg UIProfileInfoSlot_Glue[] = {
	{NULL,					NULL}
};

I3_CLASS_INSTANCE( UIProfileInfoSlot, UISlotBase);

UIProfileInfoSlot::UIProfileInfoSlot()
{
}

UIProfileInfoSlot::~UIProfileInfoSlot()
{
}

/*virtual*/ bool UIProfileInfoSlot::OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */)
{
	if( UISlotBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	return true;
}

/*virtual*/ void UIProfileInfoSlot::OnCreate( i3GameNode * pParent)
{
	UISlotBase::OnCreate( pParent);

	// load scene
	AddScene( "UIRe/PointBlankRe_Profile_Info_Slot.i3UIe", UIProfileInfoSlot_Glue);
}