#include "pch.h"
#include "UI_V11/UIPopupUseItemChangeCrosshairColor_V11.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/ItemConsumer.h"

#include "UserInfoContext.h"

#include "i3Base/string/ext/contain_string.h"


extern "C" 
{
	int UIPopupUseItemChangeCrosshairColor_V11_SelectColor( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		ItemConsumer::instance()->m_nSelected = idx;
		return 0;
	}
}

LuaFuncReg UIPopupUseItemChagneCrosshairColor_V11_Glue[] = {
	{"SelectColor",					UIPopupUseItemChangeCrosshairColor_V11_SelectColor},
	{NULL,							NULL}
};


I3_CLASS_INSTANCE( UIPopupUseItemChangeCrosshairColor_V11);//, UIPopupColorItemBase);


LuaState * UIPopupUseItemChangeCrosshairColor_V11::_SetItemInfo( void)
{
	LuaState * L = UIPopupColorItemBase::_SetItemInfo();

	if( L != NULL)
	{
		I3COLOR* pColor = GetColor(-1);	//	기본 흰색

		if( CInvenList::i()->IsUsingCashItem_Maintenance(CASHITEM_GROUP_COLOR_CROSSHAIR) == true)
		{
			USER_INFO_INVITEM_DATA Data;
			UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);

			pColor = GetColor( Data._ui8CrossHairColor - 1);	//	서버는 1부터 색상, GUI는 0부터 색상
		}

		i3Lua::PushColor( L, pColor);
		_EndLuaFunction( L, 5);
	}

	return L;
}

/*virtual*/ void UIPopupUseItemChangeCrosshairColor_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupColorItemBase::OnCreate( pParent);

	AddScene( "UIRe1/Scene/Popup/PBRe_PopUp_useItemChangeColorCrosshair.i3UIs", UIPopupUseItemChagneCrosshairColor_V11_Glue);
}

/*virtual*/ bool UIPopupUseItemChangeCrosshairColor_V11::OnClick( i3UIControl * pControl)
{
	INT32 i;
	char szCon[ 32];

	if( UIPopupColorItemBase::OnClick( pControl) == false)
		return false;

	if( i3::same_of<i3UIUserControl*>(pControl)) //->IsExactTypeOf( i3UIUserControl::static_meta() ) )
	{
		for( i = 0; i < UI_COLOR_SLOT_MAX; i++)
		{
			i3::snprintf( szCon, sizeof( szCon), "ColorBox%d", i);

			if( i3::contain_string( pControl->GetName(), szCon) >= 0)
			{
				LuaState * L = _CallLuaFunction( "SetSelection");
				if( L != NULL)
				{
					i3Lua::PushInteger( L, i);
					_EndLuaFunction( L, 1);
				}
				return true;
			}
		}
	}

	return true;
}
