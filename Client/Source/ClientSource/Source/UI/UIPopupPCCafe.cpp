#include "pch.h"
#include "UIPopupPCCafe.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "InvenDB.h"
#include "InvenList.h"
#include "UIShopFunction.h"

#include "UIPhaseLobby.h"
#include "UITopMenu.h"

#include "UserInfoContext.h"

#include "i3Base/string/ext/format_rcstring.h"

extern "C" 
{
	int UIPopupPCCafe_ClickInventory( LuaState * L)
	{
		UIPopupPCCafe * pThis = static_cast<UIPopupPCCafe*>(GameUI::GetPopup(UPW_PCCAFE));
		if(pThis != nullptr)
			pThis->ClickInventory();

		return 0;
	}

	int UIPopupPCCafe_ClickItemList( LuaState * L)
	{
		INT32 i32Type = i3Lua::GetIntegerArg(L , 1);

		UIPopupPCCafe * pThis = static_cast<UIPopupPCCafe*>(GameUI::GetPopup(UPW_PCCAFE));
		if(pThis != nullptr)
			pThis->ClickItemListBtn(i32Type);	
		
		return 0;
	}
}

LuaFuncReg UIPopupPCCafe_Glue[] =
{
	{"ClickInventory",		UIPopupPCCafe_ClickInventory},
	{"ClickItemList",		UIPopupPCCafe_ClickItemList	},
	{nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UIPopupPCCafe);

UIPopupPCCafe::UIPopupPCCafe()
{
}

UIPopupPCCafe::~UIPopupPCCafe()
{
}

void UIPopupPCCafe::_ClearPCCafeItemList(INT32 i32Count)
{
	LuaState *  L = _CallLuaFunction( "ClearPCCafeItemList" );
	if(nullptr != L )
	{
		i3Lua::PushInteger( L, i32Count );
		_EndLuaFunction( L, 1 );
	}
}

void UIPopupPCCafe::_SetPCCafeItemList(INVEN::DB_TYPE dbtype)
{
	i3::vector<INVEN_BUFFER*> list;
	
	_GetPCCafeInvenList(list, dbtype);	

	INT32 i32Count = static_cast<INT32>( list.size());
	if( i32Count < DEFAULT_PCCAFE_ITEM_COUNT )
		i32Count = DEFAULT_PCCAFE_ITEM_COUNT;

	_ClearPCCafeItemList(i32Count);

	for(size_t idx = 0 ; idx < static_cast<size_t>(i32Count) ; ++idx )
	{
		INVEN_BUFFER * pBuffer = idx < list.size() ? list.at(idx) : nullptr;
	
		if( pBuffer != nullptr)
			_SetPCCafeItemName(idx, pBuffer->_ui32ItemID);
		else
			_SetPCCafeItemName(idx, 0);
	}
}

void UIPopupPCCafe::_SetPCCafeItemName(INT32 idx, T_ItemID itemID)
{
	LuaState *  L = _CallLuaFunction( "SetPCCafeItem" );
	if(nullptr != L )
	{
		i3::rc_wstring wstrItemName;
		usf::GetShopItemName(wstrItemName, itemID);

		i3Lua::PushInteger( L, idx );
		i3Lua::PushStringUTF16To8(L, wstrItemName.c_str() );
		_EndLuaFunction( L, 2);
	}
}

void UIPopupPCCafe::_GetPCCafeInvenList(i3::vector<INVEN_BUFFER*> & out, INVEN::DB_TYPE dbtype)
{
	INT32 usage = -1;

	switch(dbtype)
	{
	case INVEN::DB_WEAPON	: usage = WEAPON_SLOT_ALL;		break;
	case INVEN::DB_PARTS	: usage = CHARACTER_CLASS_ALL;	break;
	case INVEN::DB_CASHITEM	: usage = ITEM_TYPE_UNKNOWN;	break;
	}

	const i3::vector<INVEN_BUFFER*> & list = CInvenList::i()->get_ref_list(dbtype, usage);

	for(size_t idx = 0 ; idx < list.size() ; ++idx )
	{
		INVEN_BUFFER * pBuffer = list.at(idx);
		I3ASSERT(pBuffer != nullptr);

		if(pBuffer->_ui8ItemType == ITEM_ATTR_TYPE_PC_1)
			out.push_back(pBuffer);
	}
}

void UIPopupPCCafe::_SetBonusEffect(void)
{
	UINT32 bonus_exp = UserInfoContext::i()->GetPCCafeExp(); 
	UINT32 bonus_point = UserInfoContext::i()->GetPCCafePoint();

	i3::rc_wstring wstr_bonus_effect;

	if(bonus_exp > 0)
	{
		wstr_bonus_effect = i3::format_rcstring(GAME_RCSTRING("STR_TBL_GUI_LOBBY_GAIN_PCBENEFIT_EXP"), bonus_exp);
		wstr_bonus_effect += L"\n";
	}

	if(bonus_point > 0)
		wstr_bonus_effect += i3::format_rcstring(GAME_RCSTRING("STR_TBL_GUI_LOBBY_GAIN_PCBENEFIT_POINT"), bonus_point);

	LuaState *  L = _CallLuaFunction( "SetBonusEffect" );
	if(nullptr != L )
	{
		i3Lua::PushStringUTF16To8(L, wstr_bonus_effect.c_str() );
		_EndLuaFunction( L, 1);
	}
}

/*virtual*/ void UIPopupPCCafe::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_IbPcCafeWin.i3UIs", UIPopupPCCafe_Glue);
}

/*virtual*/ bool UIPopupPCCafe::OnKey_Enter( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ void UIPopupPCCafe::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();
	_SetPCCafeItemList(INVEN::DB_WEAPON);
	_SetBonusEffect();
}

/*virtual*/ bool UIPopupPCCafe::OnCloseButton( void)
{
	ConfirmOK();
	return true;
}

void UIPopupPCCafe::ConfirmOK(void)
{
	UIPhaseLobby * pLobby = (UIPhaseLobby*) GetMainFrame()->GetCurrentPhase();

	if( pLobby != nullptr)
	{
		pLobby->ShowNoticePopup();
	}

	g_pFramework->GetUIMainframe()->ClosePopup( UPW_PCCAFE);
}

void UIPopupPCCafe::ClickItemListBtn(INT32 type)
{
	switch(type)
	{
	case TYPE_WEAPON: _SetPCCafeItemList(INVEN::DB_WEAPON);		break;
	case TYPE_PARTS	: _SetPCCafeItemList(INVEN::DB_PARTS);		break;
	case TYPE_ITEM	: _SetPCCafeItemList(INVEN::DB_CASHITEM);	break;
	}
}

void UIPopupPCCafe::ClickInventory(void)
{
	UITopMenu * pTopMenu = GetMainFrame()->GetTopMenu();
	I3ASSERT(pTopMenu != nullptr);
	pTopMenu->ClickInventory();
}
