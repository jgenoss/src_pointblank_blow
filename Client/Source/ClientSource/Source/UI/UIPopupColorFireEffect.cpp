#include "pch.h"
#include "UIPopupColorFireEffect.h"
#include "UIUtil.h"
#include "ItemConsumer.h"
#include "UserInfoContext.h"
#include "UI/ItemConsumer.h"
#include "UIShopFunction.h"
#include "UIItemInfo_Def.h"

extern "C" 
{
	int UIPopupColorFireEffect_SelectColor( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		ItemConsumer::instance()->m_nSelected = idx;
		return 0;
	}
}

LuaFuncReg UIPopupColorFireEffect_Glue[] = {
	{"SelectColor",					UIPopupColorFireEffect_SelectColor},
	{nullptr,							nullptr}
};

I3_CLASS_INSTANCE(UIPopupColorFireEffect);

UIPopupColorFireEffect::UIPopupColorFireEffect() 
{
	m_pFmwColor = nullptr;
	m_SelectedIndex = -1;
}

UIPopupColorFireEffect::~UIPopupColorFireEffect()
{
}

void UIPopupColorFireEffect::OnCreate(i3GameNode* parent)
{
	UIPopupItemBase::OnCreate(parent);

	AddScene("Scene/Popup/PBRe_PopUp_Item_FireEffect.i3UIs", UIPopupColorFireEffect_Glue);
}

void UIPopupColorFireEffect::_InitializeAtLoad(i3UIScene* scene)
{
	UIPopupBase::_InitializeAtLoad(scene);

	m_pFmwColor = (i3UIFrameWnd*)scene->FindChildByName("Fmw_Color");

	// UI ColorSlot 에 색상 설정
	USER_INFO_INVITEM_DATA InvData;
	UserInfoContext::i()->GetMyUserInfoInvItemData(&InvData);
	m_SelectedIndex = InvData._ui16MuzzleFlashColor;

	for(int i = 0; i < FIRE_EFFECT_COLORIDX_MAX; ++i)
	{
		i3::string BtnName;
		i3::sprintf(BtnName, "btn_color%d", i);

		i3UIButton* pBtn = (i3UIButton*)scene->FindChildByName(BtnName.c_str());
		m_colorboxes[i] = pBtn;

		I3COLOR *pColor = const_cast<I3COLOR*>(GetColorbyIdx(0, i));
		m_colorboxes[i]->setColor(pColor);
	}
	_UpdateSelectedColor();
}
/*virtual */bool UIPopupColorFireEffect::OnClick(i3UIControl* pControl)
{
	UIPopupItemBase::OnEnterMouse(pControl);

	if(_IsColorbox(pControl))
	{
		for(int i = 0; i < FIRE_EFFECT_COLORIDX_MAX; ++i)
		{
			i3::string BtnName;
			i3::sprintf(BtnName, "btn_color%d", i);

			if( i3::contain_string( pControl->GetName(), BtnName) >= 0)
			{
				m_SelectedIndex = i;
				ItemConsumer::instance()->m_nSelected = m_SelectedIndex;
				_UpdateSelectedColor();
				return true;
			}
		}
	}

	return true;
}

/*virtual*/ bool UIPopupColorFireEffect::OnOKButton( void)
{
	USER_INFO_INVITEM_DATA Data;
	UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);

	SetInputDisable( true);

	UINT8 ui8Size = sizeof(INT32);
	INT32 idx = ItemConsumer::instance()->m_nSelected;

	// 컬러 박스를 선택하지 않고 확인 버튼을 누르게 될 경우(동일 색상 선택)
	if(idx < 0)
		idx = m_SelectedIndex; 

	if (usf::GetItemStatus(m_ItemID, m_AuthWareDBIndex) == ITEM_STATUS_USED)
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA_CHAGNE, &m_AuthWareDBIndex, &ui8Size, &idx);
	else
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);


	// 서버 event를 받으면 닫는다.
	return false;
}

bool UIPopupColorFireEffect::_IsColorbox(i3UIControl* pControl) const
{
	return i3::same_of<i3UIButton*>(pControl) && (pControl->getParent() == m_pFmwColor);
}

void UIPopupColorFireEffect::_UpdateSelectedColor()
{
	if(m_SelectedIndex >= 0 && m_SelectedIndex < FIRE_EFFECT_COLORIDX_MAX)
	{		
		LuaState* L = _CallLuaFunction("SetSelectedColor");
		if(L != nullptr)
		{
			i3Lua::PushInteger(L, m_SelectedIndex);

			I3COLOR *pColor = const_cast<I3COLOR*>(GetColorbyIdx(0, m_SelectedIndex));
			i3Lua::PushColor(L, pColor);
			_EndLuaFunction(L, 5);
		}
	}

}

LuaState * UIPopupColorFireEffect::_SetItemInfo( void)
{
	LuaState * L = UIPopupColorItemBase::_SetItemInfo();

	if( L != nullptr)
	{
		int DefaultIdx = 0;

		if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_COLOR_MUZZLE_FLASH) == true)
		{
			USER_INFO_INVITEM_DATA Data;
			UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);

			DefaultIdx = Data._ui16MuzzleFlashColor; 
		}

		I3COLOR *pColor = const_cast<I3COLOR*>(GetColorbyIdx(0, DefaultIdx));

		i3Lua::PushInteger( L, DefaultIdx);
		i3Lua::PushColor(L, pColor);
		_EndLuaFunction( L, 6);
	}

	return L;
}