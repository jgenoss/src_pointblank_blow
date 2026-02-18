#include "pch.h"
#include "UIPhaseInventory.h"

#include "UIPhaseInventorySlot.h"
#include "UICharaInfoMgr.h"
#include "UIClanFunction.h"
#include "ItemImageMgr.h"
#include "UIUtil.h"
#include "UIMath.h"
#include "UIMainFrame.h"
#include "UIMainFrame_V10.h"
#include "UI_V10/UITopMenu_V10.h"
#include "UISideMenu.h"
#include "UIPopupBuyItem.h"
#include "Batmanager.h"
#include "PurchaseMgr.h"
#include "BuyUtil.h"
#include "UIFloatCharacterView.h"
#include "UserInfoContext.h"
#include "UIShopFunction.h"
#include "ItemConsumer.h"
#include "ShopContext.h"
#include "UIImgSetCtrl.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

//------------------------------------------------------------------------------//
I3_CLASS_INSTANCE( UIPhaseInventory);//, UIPhaseSubBase);

namespace
{
	enum	{	
		//UITap에 enum 값과 같이 사용한다.
		EXTEND = 6, APPLY, eDELETE,  CASH
	};

	UIPhaseInventory* g_this = 0;
}

void UIPhaseInventory::open_menu(INT32 idx) const
{
	iMultiTap::open_menu(idx);

	LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "Subopen");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushBoolean(L, true);
	UILua::EndLuaFunction(L, 2);
}

void UIPhaseInventory::clicked_menu_impl(INT32 idx)
{
	switch( idx )
	{
	case EXTEND: static_cast<InvenSlot*>(m_pSlot)->clicked(1, true); break;
	case APPLY: static_cast<InvenSlot*>(m_pSlot)->clicked(2, true); break;
	case eDELETE: static_cast<InvenSlot*>(m_pSlot)->clicked(3, true); break;
	case CASH:
	{
		//	캐시 충전 웹페이지 호출
#if defined(LOCALE_TAIWAN)
		if (g_pFramework->OpenWebBrowser(UserInfoContext::i()->GetBillingURL(), nullptr, false))
			return;
#else
		if (::ExcuteCashWeb())
			return;
#endif
	}
	}
}

void UIPhaseInventory::clicked_tap_impl(INT32 idx1, INT32 idx2)
{
	LuaState* L = UILua::CallLuaFunction( m_menu_scn_info.scn, "EnableAccept" );	//장착 표시 유무
	i3Lua::PushBoolean(L, idx1 != item_def::CTG_CHARA );		//UITap.cpp enum 과 동일
	UILua::EndLuaFunction(L, 1);

	clicked_tap_UseInvenInfo(idx1, idx2);
}

void UIPhaseInventory::clicked_tap_UseInvenInfo(INT32 idx1, INT32 idx2)
{
	i3::wstring TitleName[USE_INVEN_SLOT_COUNT];

	for (INT32 i = 0; i < USE_INVEN_SLOT_COUNT; i++)
	{
		m_UseInvenInfo[i].m_m_UseInvenFrm->EnableCtrl(false);
	}

	switch (idx1)
	{
	case item_def::CTG_WEAPON:
	{
		const ItemInfoList& weapon_list = CharaInfoMgr::instance()->get_item_info_list(CCharaInfoContext::i()->GetMainCharaSlotIdx(), item_def::SCTG_WEAPON);
		size_t size = weapon_list.size();

		for (size_t i = 0; i < size; ++i)
		{
			GetItemImageMgr()->SelectImage(m_UseInvenInfo[i].m_m_UseInvenImg, weapon_list[i].m_TItemID);
		}

		m_iUseInvenSlotCount = 5;
		for (INT32 i = 0; i < m_iUseInvenSlotCount; i++)
		{
			m_UseInvenInfo[i].m_m_UseInvenFrm->EnableCtrl(m_bEnableUseInven);
		}

	}
	break;
	case item_def::CTG_PARTS:
	{
		ItemInfoList list = CharaInfoMgr::instance()->get_item_info_list(CCharaInfoContext::i()->GetMainCharaSlotIdx(), item_def::SCTG_PARTS);
		list.default_part_id_make_to_mainchara();
		size_t size = list.size();

		INT32 count = 0;
		for (size_t i = 0; i < size; ++i)
		{
			ITEM_TYPE itemtype = item_def::get_type(list[i].m_TItemID);
			if(itemtype == ITEM_TYPE_HEADGEAR
				|| itemtype == ITEM_TYPE_BERET
				|| itemtype == ITEM_TYPE_FACEGEAR
				|| itemtype == ITEM_TYPE_UPPER
				|| itemtype ==ITEM_TYPE_LOWER
				|| itemtype == ITEM_TYPE_GLOVE
				|| itemtype == ITEM_TYPE_BELT
				|| itemtype == ITEM_TYPE_HOLSTER)
				GetItemImageMgr()->SelectImage(m_UseInvenInfo[count++].m_m_UseInvenImg, list[i].m_TItemID);
		}

		m_iUseInvenSlotCount = USE_INVEN_SLOT_COUNT;
		for (INT32 i = 0; i < m_iUseInvenSlotCount; i++)
		{
			m_UseInvenInfo[i].m_m_UseInvenFrm->EnableCtrl(m_bEnableUseInven);
		}
	}
	break;
	case item_def::CTG_CHARA:
	{
		if (g_pEnvSet->IsV2Version() == false)
		{
			m_iUseInvenSlotCount = 5;
			for (INT32 idx = 0; idx < m_iUseInvenSlotCount; ++idx)
			{
				const ITEM_INFO & info = CharaInfoMgr::instance()->get_current_equip_item(static_cast<UserEquipInfo::ShareUserEquip>(idx));
				GetItemImageMgr()->SelectImage(m_UseInvenInfo[idx].m_m_UseInvenImg, info.m_TItemID);
				m_UseInvenInfo[idx].m_m_UseInvenFrm->EnableCtrl(m_bEnableUseInven);
			}
		}
	}
	break;
	default:
		break;
	}
}

void UIPhaseInventory::cb_finish_buy_extend_item(bool arg1)
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false) 
		return;

	UIPopupBuyItem* popup = (UIPopupBuyItem*)GetMainFrame()->GetPopup(UPW_BUYITEM);
	popup->ClosePopup();
	
	m_pSlot->update_slot(true, false); //(void*)TRUE );

	_RestoreOldSlotClick();
}

void UIPhaseInventory::cb_finish_remove_item(bool arg1, const ITEM_INFO* arg2)
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false)
		return;

	switch( iMultiTap::get_current_menu_idx() )
	{
	case item_def::CTG_CHARA:
		m_pCharaInfoMgr->list_up(); //캐릭터 제거시엔 chara manager 정보를 갱신한다.
	case item_def::CTG_WEAPON:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			ITEM_INFO item_info = *arg2; //		*((ITEM_INFO*)arg2);
			m_pCharaInfoMgr->replace_default(item_info);

			CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;
			if(state.ctg == state.chara_menu_ctg)
			{
				UISideMenu::instance()->smenu_click_equip( false, 3); //캐릭터 정보를 다시 얻는다.

				//if( iMultiTap::get_current_menu_idx() == item_def::CTG_CHARA )//주석풀면 캐릭터 삭제할때 장비중인 캐릭터가 변경됩니다 ㅜㅜ
				//{
				//	UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
				//	if( pCharaViewWin != nullptr)
				//		pCharaViewWin->SetChangeChara(0, false); //캐릭터 상태 변경
				//}
			}
		}
		break;
	case item_def::CTG_ITEM:
		break;
	}

	m_pSlot->update_slot((void*)TRUE, (void*)TRUE);
}

void UIPhaseInventory::cb_finish_repair_item(bool bAck)
{
	if( bAck == FALSE ) return;

	m_pSlot->Entrance();
	static_cast<InvenSlot*>(m_pSlot)->update_dp_item_info();

	_RestoreOldSlotClick();
}

void UIPhaseInventory::cb_finish_auth_item(bool arg1, const SHOP_GOODS*)
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false)
		return;

	static_cast<InvenSlot*>(m_pSlot)->double_clicked();	//double_clicked 행동이 인증된 아이템을 사용하는거라. 호출해도 무관할것 같다.
}


//------------------------------------------------------------------------------//
UIPhaseInventory::UIPhaseInventory() : iMultiTap(), m_oldClickSlot(-1)
{
	g_this = this;
	m_phase_type = USP_INVENTORY;

	m_pSlot = new InvenSlot(this);
	MEMDUMP_NEW( m_pSlot, sizeof(InvenSlot));

	m_slide_offset_x = 0;
	m_slide_offset_y = SLIDE_OFFSET;
	slot_scene_name = "Scene/Main/PointBlankRe_Inven_Slot_V3.i3UIs";

	for (INT32 i = 0; i < USE_INVEN_SLOT_COUNT; i++)
	{
		m_UseInvenInfo[i].m_m_UseInvenImg = nullptr;
		m_UseInvenInfo[i].m_m_UseInvenFrm = nullptr;
		m_UseInvenInfo[i].m_UseInvenEdit = nullptr;
	}
}

UIPhaseInventory::~UIPhaseInventory()
{
	I3_SAFE_DELETE( m_pSlot);
	m_pSlot = 0;
}


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseInventory::OnCreate( i3GameNode * pParent)
{
	iMultiTap::OnCreate( pParent);
}

/*virtual*/ void UIPhaseInventory::OnLoadAllScenes()
{
	iMultiTap::OnLoadAllScenes();

//	UI::insert_cb( "finish_buy_extend_item", this, &UIPhaseInventory::cb_finish_buy_extend_item);
//	UI::insert_cb( "finish_remove_item", this, &UIPhaseInventory::cb_finish_remove_item);
	m_SigConnection += UISignal::FinishRemoveItem.connect(i3::bind(&UIPhaseInventory::cb_finish_remove_item, this, i3::_1, i3::_2));
	m_SigConnection += UISignal::FinishRepairItem.connect(i3::bind(&UIPhaseInventory::cb_finish_repair_item, this, i3::_1));
	m_SigConnection += UISignal::FinishBuyExtendItem.connect(i3::bind(&UIPhaseInventory::cb_finish_buy_extend_item, this, i3::_1));
	m_SigConnection += UISignal::FinishAuthItem.connect(i3::bind(&UIPhaseInventory::cb_finish_auth_item, this, i3::_1, i3::_2));

	for (INT32 i = 0; i < USE_INVEN_SLOT_COUNT; i++)
	{
		i3::string strCtlName[3];
		strCtlName[0] = i3::format_string("i3UIImageBoxInven%d", i);
		strCtlName[1] = i3::format_string("i3UIStaticTextInven%d", i);
		strCtlName[2] = i3::format_string("i3UIButtonImageSetInven%d", i);

		m_UseInvenInfo[i].m_m_UseInvenFrm = (i3UIImageBox*)m_menu_scn_info.scn->FindChildByName(strCtlName[0].c_str());
		m_UseInvenInfo[i].m_UseInvenEdit = (i3UIEditBox*)m_menu_scn_info.scn->FindChildByName(strCtlName[1].c_str());
		i3UIButtonImageSet * pImg= (i3UIButtonImageSet*)m_menu_scn_info.scn->FindChildByName(strCtlName[2].c_str());
		 m_UseInvenInfo[i].m_m_UseInvenImg = GetItemImageMgr()->CreateImgSetCtrl(pImg);
	}

	LuaState * L = nullptr;
	L = _CallLuaFunction(m_menu_scn_info.idx, "UseInvenBtn");
	I3ASSERT( L != nullptr);
	i3Lua::PushBoolean(L, true);
	UILua::EndLuaFunction(L , 1);

	L = _CallLuaFunction(m_menu_scn_info.idx, "TabEnable");
	i3Lua::PushInteger(L, 0);
	UILua::EndLuaFunction(L, 1);

	//모든 무기를 읽어 놓는다.
	clicked_tap(item_def::CTG_WEAPON, -1); // -1 for 전체무기
	hide_display_info();

	OnRevive(0);
	open_menu(0);
	// 장성급 베레모 지급여부 확인
	if(UserInfoContext::i()->IsGeneralRecvData())
	{
		if(UserInfoContext::i()->IsGeneralMsgBox())
			return;
		else
			UserInfoContext::i()->SetGeneralMsgBox();

		INVEN_BUFFER itemInfo = UserInfoContext::i()->GetGeneralRankInfo();

		// 장착 체크
		if(usf::GetItemStatus(itemInfo._ui32ItemID, itemInfo._TItemWareDBIdx) == ITEM_STATUS_EQUIPPED)
		{
			g_pFramework->EquipWeaponToMyInven(itemInfo._ui32ItemID, true);
		}

		GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION4"));
	}
	m_bEnableUseInven = false;
	clicked_UseInven();
}

void	UIPhaseInventory::OnUnloadAllScenes()
{
	m_SigConnection.clear();
//	UI::remove_cb(this);

	for (INT32 i = 0; i < USE_INVEN_SLOT_COUNT; i++)
	{
		UIImgSetCtrl* imgsetCtrl = m_UseInvenInfo[i].m_m_UseInvenImg;
		if (imgsetCtrl)
			imgsetCtrl->RestoreSizePos();

		i3::safe_destroy_instance(m_UseInvenInfo[i].m_m_UseInvenImg);
	}


	iMultiTap::OnUnloadAllScenes();
}

void UIPhaseInventory::OnUpdate( REAL32 rDeltaSeconds)
{
	iMultiTap::OnUpdate(rDeltaSeconds);

	if( GetMainFrame()->IsPopupFocused() == false )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_DEL) == true )
		{
			clicked_menu_impl(eDELETE);
		}
	}
}

void UIPhaseInventory::init_slide_impl()
{
	UI::tmSlideAttContainer list(m_menu_scn_info.scn);
	list.add("i3UIFrameWnd");
	m_tmSlide.add(list);
}

void UIPhaseInventory::clicked_UseInven()
{
	m_bEnableUseInven = !m_bEnableUseInven;
	for (INT32 i = 0; i < m_iUseInvenSlotCount; i++)
	{
		m_UseInvenInfo[i].m_m_UseInvenFrm->EnableCtrl(m_bEnableUseInven);
	}
}

/*virtual*/ void UIPhaseInventory::Enter(void)
{

}

/*virtual*/ void UIPhaseInventory::Leave(void)
{
	GameEventSender::i()->SetEventOnce( EVENT_LEAVE_INVENTORY);
	CharaInfoMgr::instance()->send_event_change_equip();
}


void UIPhaseInventory::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	iMultiTap::ProcessGameEvent(event, Arg, UserData);

	switch (event)
	{
	case EVENT_LEAVE_INVENTORY:
	{
		m_pCharaInfoMgr->change_equip();

		if (EV_FAILED(Arg))
		{
			UISideMenu::instance()->smenu_click_equip(false, 1);
			UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
			if (pCharaViewWin != nullptr)
				pCharaViewWin->RevertChara();

			switch (Arg)
			{
			case EVENT_ERROR_INVENTORY_LEAVE_MAIN_SLOT:
			{
				/*대표 캐릭터 설정이 잘 못 되었습니다.*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INCORRECT_MAIN_CHARACTER"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
			}
			break;
			default: // 에러코드가 언제 확장될지 모르므로. 2015.08.13 어깨깡패
			//case EVENT_ERROR_INVENTORY_LEAVE_EQUIPMENT:
			//case EVENT_ERROR_EQUIPMENT_DUPLICATE_PARTS:
			{
				GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_EQUIP_FAIL_BY_NETWORK_PROBLEM"));
			}
			break;
			}
		}

		ItemConsumer::instance()->InitItemConsumer();
	}
	break;
	case EVENT_INVEN_GETINFO:
		{
			// 이 이벤트가 오는 것들은 서버에서 일방적으로 Inven 리스트를 갱신하는 경우
			// 선택되어 있다면 그 선택값을 가진다. 클릭 된 인덱스가 없으면 -1을 반환. Jinsik.kim(2015.02.02)
			m_oldClickSlot = m_pSlot->get_old_slot();
			m_pSlot->update_slot(true, false); // (void*)TRUE);
		}
		break;
	case EVENT_INVEN_REFRESH:
	{
		clicked_tap_UseInvenInfo((INT32)m_pSlot->GetCategory(), (INT32)m_pSlot->GetType());
	}
	break;
	}
}

void UIPhaseInventory::_RestoreOldSlotClick()
{
	// Slot을 기억해 두고 초기화 후인 지금 Slot을 다시 세팅합니다.
	if(m_oldClickSlot != -1)
	{
		m_pSlot->set_select(m_oldClickSlot);
		m_oldClickSlot = -1;
		static_cast<InvenSlot*>(m_pSlot)->clicked(0, false);
	}
}