#include "pch.h"
#include "UIPopup_Clan_ChangeMark.h"
#include "UIMainFrame.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( UIPopup_Clan_ChangeMark, UIPopupBase);

static UIPopup_Clan_ChangeMark* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopup_Clan_ChangeMark*) pFrame->GetPopup( UPW_CLAN_MARKCHANGE);
}

extern "C" {
	int UIPopup_Clan_ChangeMark_Confirm(LuaState * L)
	{
		GetThis()->ChangeMark_Confirm();
		return 0;
	}

	int UIPopup_Clan_ChangeMark_Cancel(LuaState * L)
	{
		GetThis()->ChangeMark_Cancel();
		return 0;
	}
}

LuaFuncReg UIPBRe_PopUp_Clan_ChangeMark_Glue[] = {
	{"UIPopup_Clan_ChangeMark_Confirm",		UIPopup_Clan_ChangeMark_Confirm	},
	{"UIPopup_Clan_ChangeMark_Cancel",		UIPopup_Clan_ChangeMark_Cancel	},
	{NULL,						NULL}
};


UIPopup_Clan_ChangeMark::UIPopup_Clan_ChangeMark()
{
	
}

UIPopup_Clan_ChangeMark::~UIPopup_Clan_ChangeMark()
{
}

void UIPopup_Clan_ChangeMark::ChangeMark_Confirm()
{
	
	GetMainFrame()->TogglePopup( UPW_CLAN_MARKCHANGE);
}
void UIPopup_Clan_ChangeMark::ChangeMark_Cancel()
{
	GetMainFrame()->TogglePopup( UPW_CLAN_MARKCHANGE);
}



/*virtual*/ void UIPopup_Clan_ChangeMark::ProcessGameEvent(UINT32 event, INT32 Arg)
{
}

BOOL UIPopup_Clan_ChangeMark::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{

	return UIPopupBase::OnEvent(event, pObj, param2, code);
}


/*virtual*/ void UIPopup_Clan_ChangeMark::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	// Load Scene
	AddScene( "UIRe/PBRe_PopUp_ClanMark_Change.i3UIe", UIPBRe_PopUp_Clan_ChangeMark_Glue);
}