#include "pch.h"
#include "UIPopupMedalRestriction.h"
#include "UIUtil.h"
#include "UIMainFrame.h"
#include "MedalManager.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "GameGUI.h"
#include "i3Base/string/compare/generic_is_equal.h"


I3_CLASS_INSTANCE( UIPopupMedalRestriction);//, UIPopupClanBase);

extern "C" 
{
	int UIPopupMedalRestriction_Cancel( LuaState * L)
	{
		UIPopupMedalRestriction * p = static_cast<UIPopupMedalRestriction*>(GameUI::GetPopup( UPW_MEDAL_ITEM));
		if( p != nullptr)
			p->Close();
		return 0;
	}
}

LuaFuncReg UIPBRe_PopUp_MedalRestriction_Glue[] =
{
	{ "Cancel",					UIPopupMedalRestriction_Cancel},
	{nullptr, nullptr}
};

UIPopupMedalRestriction::UIPopupMedalRestriction()
{
}

UIPopupMedalRestriction::~UIPopupMedalRestriction()
{
}

/*virtual*/ void UIPopupMedalRestriction::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PBRe_PopUp_Buy_fail.i3UIs", UIPBRe_PopUp_MedalRestriction_Glue);
}


/*virtual*/ bool UIPopupMedalRestriction::OnEntranceStart(void * pArg1 /* = nullptr  */, void * pArg2 /* = nullptr */ )
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	_InitControl();

	MedalViewInfo *pMedalInfo = static_cast<MedalViewInfo *>(pArg1);
	UINT8 lv = *static_cast<UINT8 *>(pArg2);
	UINT8 Currentlv = pMedalInfo->getCurrentLevel();

	INT32 dest_cnt = 0;
	if(lv == MEDAL_LEVEL_MAX)
	{
		dest_cnt = pMedalInfo->getLevelProcessCnt( lv);
	}
	else
	{
		dest_cnt = pMedalInfo->getLevelProcessCnt(lv + 1) - pMedalInfo->getLevelProcessCnt(Currentlv);
	}

	LuaState * L = _CallLuaFunction("InitControl");

	if(L != nullptr)
	{
		i3Lua::PushInteger(L, pMedalInfo->getImageIdx());
		i3Lua::PushStringUTF16To8(L, L"");
		i3Lua::PushStringUTF16To8(L, pMedalInfo->getMedalName());
		_EndLuaFunction(L , 3);
	}

	return true;
}

void UIPopupMedalRestriction::_InitControl( void)
{
	LuaState * L = _CallLuaFunction("__init");

	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_WEAPONBUY_FAIL"));
		i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_WEAPONBUY_TERMS"));
		_EndLuaFunction(L , 2);
	};
}

void UIPopupMedalRestriction::Close(void)
{
	GetMainFrame()->ClosePopup(UPW_MEDAL_ITEM);
}
