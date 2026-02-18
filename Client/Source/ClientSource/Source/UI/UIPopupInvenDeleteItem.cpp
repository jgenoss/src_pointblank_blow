#include "pch.h"
#include "UIPopupInvenDeleteItem.h"
#include "UIMainFrame.h"
#include "UIUtil.h"
#include "ItemConsumer.h"
#include "UICharaInfoMgr.h"

extern "C" 
{
	int item_click_delete(LuaState * L)
	{
		UIPopupInvenDeleteItem * pThis = static_cast<UIPopupInvenDeleteItem*>(GameUI::GetPopup( UPW_INVEN_DELETE_ITEM_NEW));
		if( pThis != nullptr)
			pThis->clicked(1);
		return 0;
	}

	int item_click_cancel( LuaState * L)
	{
		UIPopupInvenDeleteItem * pThis = static_cast<UIPopupInvenDeleteItem*>(GameUI::GetPopup( UPW_INVEN_DELETE_ITEM_NEW));
		if( pThis != nullptr)
			pThis->clicked(0);
		return 0;
	}
}

LuaFuncReg UIPopupInvenDeleteItem_Glue[] =
{
	{ "Click_Delete", item_click_delete	},
	{ "Click_Cancel", item_click_cancel	},
	{ nullptr, nullptr}
};

I3_CLASS_INSTANCE(UIPopupInvenDeleteItem);

UIPopupInvenDeleteItem::UIPopupInvenDeleteItem()
{
}

UIPopupInvenDeleteItem::~UIPopupInvenDeleteItem() 
{
}

void UIPopupInvenDeleteItem::OnCreate(i3GameNode* parent)
{
	UIPopupBase::OnCreate(parent);
	
	AddScene("Scene/Popup/PBRe_PopUp_ItemQuery_delete.i3UIs", UIPopupInvenDeleteItem_Glue );

}

void UIPopupInvenDeleteItem::_InitializeAtLoad(i3UIScene* scn)
{
	UIPopupBase::_InitializeAtLoad(scn);
}

bool UIPopupInvenDeleteItem::OnEntranceStart(void* arg1, void* arg2)
{
	if( UIPopupBase::OnEntranceStart( arg1, arg1) == false)
		return false;

	const ITEM_INFO* info = static_cast<const ITEM_INFO*>( arg1 );
	m_item_id = info->m_TItemID;
	m_item_db_idx = info->m_TItemDBIdx;

	GetSlide()->ForceEnd();

	return true;
}

void UIPopupInvenDeleteItem::clicked(INT32 bDelete)
{
	if (bDelete == 1)
	{
		//false면 변환된게 없는거임.
		if (!(CharaInfoMgr::instance()->send_event_change_equip()))
		{
			ItemConsumer::instance()->remove(GetMainFrame(), m_item_id, m_item_db_idx);
			GetMainFrame()->ClosePopup(UPW_INVEN_DELETE_ITEM_NEW);
		}
	}
	else
	{
		GetMainFrame()->ClosePopup(UPW_INVEN_DELETE_ITEM_NEW);
	}
}

void UIPopupInvenDeleteItem::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData);
	
	switch (event)
	{
		case EVENT_INVEN_CHANGE_CHARA_EQUIP:
			ItemConsumer::instance()->remove(GetMainFrame(), m_item_id, m_item_db_idx);
			GetMainFrame()->ClosePopup(UPW_INVEN_DELETE_ITEM_NEW);
			break;
	}

	//UIMainFrame* frame = g_pFramework->GetUIMainframe();
}

bool UIPopupInvenDeleteItem::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code )
{
	return UIPopupBase::OnEvent(event, pObj, param2, code);
}