#include "pch.h"
#include "UISlotStage.h"

#include "UIMainFrame.h"
#include "UIPopupCreateRoom.h"
#include "UIPopupSelectStage.h"

static UIPopupSelectStage * GetSelectStagePopup( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupSelectStage*) pFrame->GetPopup( UPW_SELECTSTAGE);
}

static UISlotStage * GetThis( LuaState * L)
{
	UIPopupSelectStage * pSelector = GetSelectStagePopup();
	INT32 idx = pSelector->FindSlot( L);
	return pSelector->GetSlot( idx);
}

//--------------------------------------------------------------------------//
//								Lua Glue									//
extern "C" {
	int UISlotStage_SelectMap( LuaState * L)
	{
		UIPopupSelectStage * pSelector = GetSelectStagePopup();

		INT32 slot = pSelector->FindSlot( L);
		pSelector->SelectSlot( slot);
		return 0;
	}

	int UISlotStage_SelectOK( LuaState * L)
	{
		UIPopupSelectStage * pSelector = GetSelectStagePopup();
		INT32 slot = pSelector->FindSlot( L);
		pSelector->SelectSlot( slot);
		pSelector->SelectOK();
		return 0;
	}
}

LuaFuncReg UISlotStage_Glue[] = {
	{ "SelectMap",			UISlotStage_SelectMap},
	{ "SelectOK",			UISlotStage_SelectOK},
	{NULL,					NULL}
};

I3_CLASS_INSTANCE( UISlotStage, UISlotBase);

UISlotStage::UISlotStage()
{
}

UISlotStage::~UISlotStage()
{
}

/*virtual*/ void UISlotStage::OnCreate( i3GameNode * pParent)
{
	UISlotBase::OnCreate( pParent);

	// load scene
	AddScene( "UIRe/pb_Lobby_CreatRoom_StageSelect_MapSlot.i3UIe", UISlotStage_Glue);
}

/*virtual*/ void UISlotStage::SetEmptySlot(void)
{
	LuaState * L = _CallLuaFunction( "SetEmpty");
	_EndLuaFunction( L, 0);
}

/*virtual*/ void UISlotStage::OnSelect( void)
{
	LuaState * L = _CallLuaFunction( "Select");
	_EndLuaFunction( L, 0);
}

/*virtual*/ void UISlotStage::OnUnselect( void)
{
	LuaState * L = _CallLuaFunction( "Unselect");
	_EndLuaFunction( L, 0);
}

/*virtual*/ void UISlotStage::OnWheel( i3UIControl * pControl, INT32 zDelta)
{
	UIPopupSelectStage * pSelectPopup = GetSelectStagePopup();

	if( pSelectPopup != NULL)
		pSelectPopup->OnWheel( pControl, zDelta);
}

void UISlotStage::SetInfo( const char * pszName, INT32 imageIdx, bool bNew, bool bEvent)
{
	LuaState * L;

	L = _CallLuaFunction( "SetInfo");
	if( L != NULL)
	{
		i3Lua::PushA2UTF8String( L, pszName);
		i3Lua::PushInteger( L, imageIdx);
		i3Lua::PushBoolean( L, bNew?TRUE:FALSE);
		i3Lua::PushBoolean( L ,bEvent? TRUE:FALSE);
		_EndLuaFunction( L, 4);
	}
}

bool UISlotStage::IsSelect()
{
	LuaState * L = _CallLuaFunction( "isSelect");
	bool bEnable = _ReturnLuaBoolean(L , 0);
//	_EndLuaFunction(L , 0);
	return bEnable;
}

bool UISlotStage::IsEmpty()
{
	LuaState * L = _CallLuaFunction( "isEmpty");
	bool bEnable = _ReturnLuaBoolean(L , 0);
	return bEnable;	
}

