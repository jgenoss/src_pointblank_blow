#include "pch.h"
#include "UIPhaseInventorySlot.h"

#include "PurchaseMgr.h"
#include "UIPopupBuyItem.h"
#include "BuyUtil.h"
#include "ItemConsumer.h"
#include "UIPhaseInventory.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"
#include "UIUtil.h"
#include "UICharaInfoMgr.h"
#include "UIMainFrame.h"
#include "UISideMenu.h"
#include "UIDefine.h"
#include "UIPopupItemBase.h"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
#include "UIFloatCharacterView.h"
#include "UISMenu_Equip.h"

#include "MyRoomInfoContext.h"
#include "ShopContext.h"
#include "i3Base/string/ext/format_rcstring.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "../Designation/DesigUtil.h"
#include "../Designation/DesigSet.h"

namespace
{
	UISecCount sec_counter;		// 시간재는 유틸..

	InvenSlot* g_this = 0;
	bool is_clicked_slot = false;

	enum	{	
		ROW = 3, COL = 3, SLOT = ROW * COL,
		CLICK = 0,	EXTEND,	APPLY, eDELETE, REPAIR,
	};

	enum	{ FRM_DN = 4, FRM_UP, };
	
	bool is_enable_equip_parts( T_ItemID itemid, T_ItemDBIdx ware_idx);

	bool is_possible_purchase( T_ItemID itemid );
	bool is_possible_expend_item( T_ItemID itemid);
}

extern "C" 
{
	int inven_menu_slot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked(idx);
		return 0;
	}
	int inven_menu_slot2( LuaState * L)
	{
		g_this->double_clicked();
		return 0;
	}
}

LuaFuncReg inven_slot_glue[] =
{
	{ "Click",			inven_menu_slot	},
	{ "Click2",		inven_menu_slot2	},
	{ nullptr,							nullptr}
};

InvenSlot::InvenSlot(iMultiTap *parent) : iMultiTabSlot(parent)
{
	g_this = this;

	pf_update_item_info_arr[0] = &my_type::update_item_info_normal;
	pf_update_item_info_arr[1] = &my_type::update_item_info_front;

	pf_update_item_info_normal_arr[0] = &my_type::update_item_info_normal_impl;
	pf_update_item_info_normal_arr[1] = &my_type::update_item_info_normal_impl2;

	sec_count_cb.SetObj(this);
	sec_counter.SetCallback(&sec_count_cb);
}

void InvenSlot::clicked(INT32 idx, bool from_parent)
{
	if(from_parent)
	{
		if(is_clicked_slot == false) return; //delete 키를 누를 경우 clicked 효과를 나타낸다.하지만 자식이 클릭된 후에 가능하다.

		iSlot::set_select( iSlot::get_old_slot() );
	}

	is_clicked_slot = true;

	INT32 slot_idx = iSlot::apply_select();
	if( slot_idx == -1 ) return; //slot 선택전에 함수가 호출된 경우,, del 키를 눌렀을 경우가 여기에 해당
	
	switch( idx )
	{
	case CLICK:	break;
	case EXTEND: extend_clicked(); break;
	case APPLY: double_clicked(); break;
	case eDELETE: remove_clicked(); break;
	case REPAIR: repair_clicked(); break;
	}

	//main frame에 현재 무기에 대한 내용 업데이트
	mParent->show_display_info();
	update_item_info(mDpImgSetCtrl, slot_idx, mDpScene, true);
	update_item_attribute( slot_idx );
	update_btn_state(slot_idx);

	mSlot.UpdateList( mInfoList.size() );
}

void InvenSlot::double_clicked()
{
	INT32 slot_idx = iSlot::apply_select();

	if( slot_idx == -1 ) return;

	const ITEM_INFO & info = mInfoList.find( slot_idx );

	item_def::Category ctg = item_def::get_category(info.m_TItemID);

	ShopItemInfo* shopItemInfo = g_pShopItemInfoDataBase->getInfo(info.m_TItemID);
	if (shopItemInfo)
	{
		if (shopItemInfo->IsCouponType())
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));
			return;
		}
	}

	if( item_def::get_type( info.m_TItemID) == ITEM_TYPE_DINO )	//공룡은 무기처럼 처리 하자.
		ctg = item_def::CTG_WEAPON;

	switch( ctg )
	{
	case item_def::CTG_WEAPON:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
			const i3::vector<UINT8> *outNeedDesigSetType;
			if (!DesigUtil::CheckWearable_MyItemByDesig(info.m_TItemID, &outNeedDesigSetType))
			{
				i3::rc_wstring wszUserTitle;
				for (size_t i = 0; i < outNeedDesigSetType->size(); i++)
				{
					const DesigSet*	desigSet = DesigUtil::FindDesigSetByType(outNeedDesigSetType->at(i));
					if (desigSet)
						wszUserTitle += desigSet->wstrName;
				}
				i3::rc_wstring wszMessage;
				i3::sprintf(wszMessage, GAME_RCSTRING("STR_INVEN_ERQUIRE_DESIGNATION"), wszUserTitle);/*해당 아이템은 %s 호칭을\n획득하여야 장비할 수 있습니다.\n(호칭 획득 후 장비 가능)*/

				GameUI::MsgBox(MSG_TYPE_GAME_OK, wszMessage);
				break;
			}
		
			if( usf::GetItemStatus( info.m_TItemID, info.m_TItemDBIdx  ) == ITEM_STATUS_NOTUSED )
			{
				if( ctg != item_def::CTG_WEAPON && is_enable_equip_parts(info.m_TItemID, info.m_TItemDBIdx) == false )
				{
					/*공유 할 수 없는 파츠이거나, 또는 다른 캐릭이 착용중입니다.*/
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_UNEQUIP_PARTS_ITEM") );
					break;
				}
				else
					PurchaseMgr::i()->Auth_Inven(mParent,  info.m_TItemID, info.m_TItemDBIdx);			// "finish_auth_item",
			}
			else
			{
				ItemInfoConvert item_convert(info);

				if( ctg == item_def::CTG_WEAPON )
				{
					if( mParent->GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM )
					{
						UINT8 mode_flag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
						if( item_def::is_locked_weapon(mode_flag, info.m_TItemID) == true)
						{
							GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_WEAPON_LIMIT") ); /*무기제한*/
							break;
						}
					}
				}
				else
				{
					if( is_enable_equip_parts(info.m_TItemID, info.m_TItemDBIdx) == false )
					{
						/*공유 할 수 없는 파츠이거나, 또는 다른 캐릭이 착용중입니다.*/
						GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_UNEQUIP_PARTS_ITEM") );
						break;
					}

					if( item_convert.is_default_item() )
						item_convert.m_info.m_TItemID = static_cast<T_ItemID>( MAKE_DEFAULT_PARTS_ITEMID( GET_ITEM_TYPE(item_convert.m_info.m_TItemID) ));
				}

				CharaInfoMgr::CurrentState & state = CharaInfoMgr::instance()->m_current_state;

				state.ctg = item_convert.get_side_category();
				state.info = item_convert.m_info;
				state.slot_idx = static_cast<UINT8>(item_convert.get_char_slot_idx());
				state.chara_idx = static_cast<UINT8>(CCharaInfoContext::i()->GetMainCharaSlotIdx());

				if( UISideMenu::instance() != nullptr)
				{
					//아이템 교환
					SMenuEquip * pSMenuEquip = static_cast<SMenuEquip*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_EQUIP));
					if (pSMenuEquip != nullptr)
					{
						pSMenuEquip->UIEvent_ChangedCharaEquip();	// 현재는 이녀석이 메니져 역할이라.. 직접 호출.. 따로 메니징할 수 있는 중간 함수로 대체해야 한다.

						//PBBUG-16135 여기서 Change_Equip 해버리면 인벤Leave할때 변경된 데이터가 없다고하면서 싱크가 안된다.
					}
				}
			}
		}
		break;

	case item_def::CTG_CHARA:
		{
			// 장착 캐릭을 변경
			if( usf::GetItemStatus( info.m_TItemID, info.m_TItemDBIdx  ) == ITEM_STATUS_NOTUSED )
			{
				PurchaseMgr::i()->Auth_Inven(mParent, info.m_TItemID, info.m_TItemDBIdx);		//"finish_auth_item",
			}
			else
			{
				//캐릭터 변경시 변경된 사항 서버에 싱크
				if (true == CharaInfoMgr::instance()->check_changed_equip())
				{
					CharaInfoMgr::instance()->send_event_change_equip();
					CharaInfoMgr::instance()->change_equip();
				}

				CharaInfoMgr::instance()->list_up();
				GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SET_CHARA, (INT32)info.m_TItemDBIdx);
			}
			break;
		}
	case item_def::CTG_ITEM:
		//PurchaseMgr::authenticate 함수를 사용 안하네.. 나중에 문제 있음 넘기자.
		if( item_def::get_item_type(info.m_TItemID) == ITEM_TYPE_INGAMEITEM ||
			item_def::get_item_type(info.m_TItemID) == ITEM_TYPE_DOMIITEM) //제압 미션 전용 아이템
		{
			/*제압미션 전용 아이템 입니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_ONLY_USE_DOMI_ITEM") );
			break;
		}
		if( usf::GetItemStatus( info.m_TItemID, info.m_TItemDBIdx  ) == ITEM_STATUS_NOTUSED )
		{
			ITEM_QUERY_STYLE style = IQS_USE;
			
			if(info.m_TItemID == CASHITEM_ITEMID_MAIN_SKILL_ALLOPEN)
			{
				mParent->GetMainFrame()->OpenPopup(UPW_SKILLOPEN_ITEM, const_cast<T_ItemID*>(&info.m_TItemID), &style);
			}
			else if(
				(info.m_TItemID == CASHITEM_ITEMID_BRAZIL_CHAIN_GOLD) ||
				(info.m_TItemID == CASHITEM_ITEMID_BRAZIL_CHAIN_BLACK) ||
				(info.m_TItemID == CASHITEM_ITEMID_BRAZIL_CHAIN_RED)
				)
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_INVEN_DO_NOT_USE") );
			}
			else
			{
				mParent->GetMainFrame()->OpenPopup(UPW_QUERY_ITEM, const_cast<T_ItemID*>(&info.m_TItemID), &style);
			}
		}
		if( usf::GetItemStatus(info.m_TItemID, info.m_TItemDBIdx ) == ITEM_STATUS_USED )
		{
			ITEM_QUERY_STYLE style = IQS_USE;

			if(info.m_TItemID == CASHITEM_ITEMID_COLOR_MUZZLE_FLASH)
			{
				mParent->GetMainFrame()->OpenPopup(UPW_QUERY_ITEM, const_cast<T_ItemID*>(&info.m_TItemID), &style);
			}			
		}
		break;	
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}

	Entrance();
	
	//메인 프레임 정보도 같이 갱신해줘야 됩니다.
	mParent->show_display_info();
	update_item_info(mDpImgSetCtrl, slot_idx, mDpScene, true);
	update_item_attribute(slot_idx);
	update_btn_state(slot_idx);
}

void InvenSlot::remove_clicked()
{
	if( is_clicked_slot == false ) return; //클릭되지 않은 상태임..

	const ITEM_INFO & info = mInfoList.find( iSlot::get_now_slot() );

	item_def::Category tCategory = item_def::get_category(info.m_TItemID);
	switch(tCategory)
	{
		case item_def::CTG_WEAPON:
			if (CharaInfoMgr::instance()->is_chara_equipped(info.m_TItemID, info.m_TItemDBIdx, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
			{
				/*현재 캐릭터가 착용중인 아이템 입니다. \n삭제할 수 없습니다.*/	/*아이템 삭제*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_EQUIP"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM"));
				return;
			}
			break;
		case item_def::CTG_PARTS:
		case item_def::CTG_SKIN:
			{
				if (is_enable_equip_parts(info.m_TItemID, info.m_TItemDBIdx) == false)
				{
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_UNEQUIP_PARTS_ITEM"));
					return;
				}
				if( CharaInfoMgr::instance()->is_chara_equipped(info.m_TItemID, info.m_TItemDBIdx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) )
				{
					/*현재 캐릭터가 착용중인 아이템 입니다. \n삭제할 수 없습니다.*/	/*아이템 삭제*/
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_EQUIP"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
					return;
				}
			}
			break;
		case item_def::CTG_CHARA:	
			{
				if (info.m_TItemDBIdx == CCharaInfoContext::i()->GetMainCharaDBIdx() )
				{
					/*현재 활성된 캐릭터입니다.\n삭제 할 수 없습니다.*/	/*아이템 삭제*/
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_EQUIP_CHARA"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
					return;
				}
				else if (info.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN)
				{
					/*기본 캐릭터는 삭제할 수 없습니다.*/	/*아이템 삭제*/
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MAIN_CHARACTER_DELETE"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM"));
					return;
				}
			}
			break;	
		case item_def::CTG_ITEM:
			{
				// Coupon, 브라질 Chain은 예외적으로 삭제 못합니다.
				if (
					(info.m_TItemID == CASHITEM_ITEMID_PBWC_COUPON) ||
					(info.m_TItemID == CASHITEM_ITEMID_BRAZIL_CHAIN_GOLD) ||
					(info.m_TItemID == CASHITEM_ITEMID_BRAZIL_CHAIN_BLACK) ||
					(info.m_TItemID == CASHITEM_ITEMID_BRAZIL_CHAIN_RED)
					)
				{
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REMOVE_REQ_FAIL"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM"));
					return;
				}
			}
			break;
		default :
			I3ASSERT_0;	// 코드 작성 필요
			break;
	}

	ITEM_QUERY_STYLE style = IQS_REMOVE;
	//mParent->GetMainFrame()->OpenPopup(UPW_QUERY_ITEM, (void *)&info, &style);
	mParent->GetMainFrame()->OpenPopup(UPW_INVEN_DELETE_ITEM_NEW, (void *)&info, &style);
}

void InvenSlot::extend_clicked()
{
	const ITEM_INFO & info = mInfoList.find( iSlot::get_now_slot() );

	const SHOP_GOODS_PACK* pack = CShopContext::i()->GetGoodsPack( info.m_TItemID );
	if(pack == nullptr)	return;

	buy::PushImmediateWithDBIndex(pack, info.m_TItemDBIdx);

	mParent->GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Extend);
}

void InvenSlot::repair_clicked()
{
	const ITEM_INFO & info = mInfoList.find( iSlot::get_now_slot() );
	PurchaseMgr::i()->Repair(mParent, info.m_TItemID, info.m_TItemDBIdx );   // "finish_repair_item",
}


void InvenSlot::OnCreate( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.EnableScroll( true );
	mSlot.OnCreate( (UIBase*)mParent, ROW, COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_Inven_Slot_V3.i3UIs", inven_slot_glue);
	
	m_ppImgSet = new UIImgSetCtrl*[SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof(UIImgSetCtrl*) * SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * SLOT);
}

void InvenSlot::Entrance()
{
	size_t size = mInfoList.size();
	mSlot.UpdateList( size );
}

void InvenSlot::Entrance(item_def::WeaponComboType type, bool bscrool)
{
	is_clicked_slot = false;
	mCategory = item_def::CTG_WEAPON;
	use_combo_type = true;
	m_WeaponType = type;
	mType = WEAPON_SLOT_PRIMARY;

	mInfoList.list_up( type );
	mInfoList.sort(mCategory);

	size_t size = mInfoList.size();

	iSlot::reset_select( size );
	mSlot.UpdateList( size, bscrool);
	mSlot.ResetMouseoverSlot();
}

void InvenSlot::Entrance(item_def::Category category, UINT32 type, bool bscrool)
{
	is_clicked_slot = false;
	use_combo_type = false;
	mCategory = category;
	mType = type;

	mInfoList.list_up( category, type );
	mInfoList.sort(mCategory);

	size_t size = mInfoList.size();

	iSlot::reset_select( size );
	mSlot.UpdateList( size, bscrool);
	mSlot.ResetMouseoverSlot();
}

void InvenSlot::SearchKeyword(const WCHAR* wstr, bool bFocus)
{
	if (bFocus == false && m_SearchStr == wstr)
		return;

	bool bDefaultText = false;
	if (i3::generic_compare(wstr, "") != 0)
		bDefaultText = false;
	else
		bDefaultText = true;

	LuaState * L = UILua::CallLuaFunction(mDpAttScene, "EnableSearchText");
	i3Lua::PushBoolean(L, bDefaultText);
	UILua::EndLuaFunction(L, 1);

	I3PRINTLOG(I3LOG_NOTICE, "%s", wstr);
	m_SearchStr = wstr;
	if (mCategory == item_def::CTG_NA)
		return;

	if (mCategory == item_def::CTG_WEAPON
		&& mType == WEAPON_SLOT_PRIMARY)
		Entrance((item_def::WeaponComboType)m_WeaponType, false);
	else
		Entrance(mCategory, mType, false);

	mInfoList.SearchKeyword(m_SearchStr.c_str());
	size_t size = mInfoList.size();
	iSlot::reset_select(size);
	mSlot.UpdateList(size, bFocus);
}

void InvenSlot::ResetList()
{
	mInfoList.list_reset();
}

void InvenSlot::update_dp_item_info()
{
	INT32 slot_idx = iSlot::apply_select();
	if( slot_idx == -1 ) return;

	update_item_info(mDpImgSetCtrl, slot_idx, mDpScene, true);
};

#include "UIMath.h"

namespace slot_util_impl
{
	bool letter[][9] = 
	{
		{ 0, 1, 0, 1, 1, 1, 1, 0, 1 }, { 1, 1, 1, 1, 1, 0, 1, 1, 1 }, { 1, 1, 1, 1, 0, 0, 1, 1, 1 },
		{ 1, 1, 0, 1, 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 1, 0, 0, 1, 1 }, { 1, 1, 1, 1, 1, 0, 1, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0, 0 }, { 1, 0, 1, 1, 1, 1, 1, 0, 1 }, { 1, 1, 1, 0, 1, 0, 1, 1, 1 },
		{ 0, 1, 1, 0, 0, 1, 1, 1, 1 }, { 1, 0, 1, 1, 1, 0, 1, 0, 1 }, { 1, 0, 0, 1, 0, 0, 1, 1, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 0, 1 }, { 1, 0, 1, 1, 1, 1, 1, 0, 1 }, { 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 1, 1, 0, 1, 1, 0, 1, 0, 0 }, { 1, 1, 1, 1, 1, 1, 0, 0, 1 }, { 1, 1, 1, 1, 1, 0, 1, 0, 1 },
		{ 0, 1, 1, 0, 1, 0, 1, 1, 0 }, { 1, 1, 1, 0, 1, 0, 0, 1, 0 }, { 1, 0, 1, 1, 0, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 0, 1, 0, 1, 0 }, { 1, 1, 0, 1, 0, 1, 0, 1, 1 }, { 1, 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 0, 1, 0 }, { 1, 1, 1, 0, 1, 0, 1, 1, 1 }, { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
	};

	i3::wstring wstr; POINT pt; UINT32 idx = 0, step = 0; bool use_fn = false;

	INT32 get_item_id(INT32 category)
	{
		switch(category)
		{
		case item_def::CTG_WEAPON:	return DEFAULT_WEAPON_ASSAULT; break;
		case item_def::CTG_CHARA:	return CDI_RED_BULLS; break;
		case item_def::CTG_PARTS:	return DEFAULT_HEADGEAR; break;
		case item_def::CTG_SKIN :	return 0;	break;
		case item_def::CTG_ITEM:	return DEFAULT_DINO; break;
		default : I3ASSERT_0; break; //코드 작성 필요
		}
		return 0;
	}

	bool enable_slot(INT32 slot_idx)
	{
		if( slot_util_impl::idx >= slot_util_impl::wstr.size() ) return false;
		if( slot_util_impl::wstr[slot_util_impl::idx]-3 == ' ' ) return slot_util_impl::letter[26][slot_idx];
		wchar_t alphabet = slot_util_impl::wstr[slot_util_impl::idx]; alphabet -= 3;
		if( alphabet >= 'a' && alphabet <= 'z' ) alphabet -= 'a'; 
		else if(alphabet >= 'A' && alphabet <= 'Z') alphabet -= 'A'; 
		
		if(alphabet < 27 )
			return slot_util_impl::letter[alphabet][slot_idx];
		else
			return false;
	}

	wchar_t get_item_name()
	{
		if( slot_util_impl::idx >= slot_util_impl::wstr.size() ) return L' ';
		if( slot_util_impl::wstr[slot_util_impl::idx]-3 == ' ' ) return L'_';
		return slot_util_impl::wstr[slot_util_impl::idx]-3;
	}

	void apply_str()
	{
		INT32 x = uiMath::limited<INT32>( pt.x, 0, 1024); INT32 y = uiMath::limited<INT32>( pt.y, 0, 768);

		if( x < 10 && y < 10 ) { slot_util_impl::wstr = L"vloyhu#sloorz"; return; }
		else if( x < 10 && y > 758 ) { slot_util_impl::wstr = L"Gldprqg#Jodvv#FpM"; return; }
		else if( x > 1014 && y < 10 ) { slot_util_impl::wstr = L""; return; }
		else if( x > 1014 && y > 758 ) { slot_util_impl::wstr = L""; return; }

		switch( ( (y / 384) * 2) + (x / 512) )
		{
		case 0: slot_util_impl::wstr = L"Srlqw#Eodqn###}hshwwr"; break;
		case 1: slot_util_impl::wstr = L"Gr#\\rxu#Ehvw"; break;
		case 2: slot_util_impl::wstr = L"Rqh#Vkrw#Rqh#Nloo"; break;
		case 3: slot_util_impl::wstr = L"Qhyhu#Vwrs#Jr#Jr"; break;
		}
	}
}


void InvenSlot::update_impl(REAL32 rDeltaSeconds)
{
	sec_counter.Update();

	if( slot_util_impl::use_fn == true )
	{
		POINT pt = i3UI::getManager()->getCurrentMousePosition();
		if( pt.x != slot_util_impl::pt.x || pt.y != slot_util_impl::pt.y ) sec_counter.Stop();
		if( GameUI::GetKeyboardStroke() != I3I_KEY_UNKNOWN ) sec_counter.Stop();
	}
	else
	{
		POINT pt = i3UI::getManager()->getCurrentMousePosition();
		if( pt.x == slot_util_impl::pt.x && pt.y == slot_util_impl::pt.y )
		{
			switch( slot_util_impl::step )
			{
			case 0:
				{
					bool enable = GameUI::IsKeyboardPressed(I3I_KEY_P) && GameUI::IsKeyboardPressed(I3I_KEY_B);
					if( enable == true && GameUI::IsKeyboardStroked(I3I_KEY_END) == true ) slot_util_impl::step = 1;
				}
				break;
			case 1:
				{
					bool enable = GameUI::IsKeyboardPressed(I3I_KEY_J) && GameUI::IsKeyboardPressed(I3I_KEY_W);
					if( enable == true && GameUI::IsKeyboardStroked(I3I_KEY_HOME) == true ) { slot_util_impl::apply_str(); go_stop(); }
				}
				break;
			}
		}
		else
		{
			slot_util_impl::step = 0; slot_util_impl::pt = pt;
		}
	}
}

void	InvenSlot::sec_count_done(INT32 result_time) 
{
	sec_counter.Start();
}

void	InvenSlot::sec_count_stop(INT32 result_time)
{
	slot_util_impl::use_fn = false; slot_util_impl::step = 0;
	static_cast<UIPhaseInventory*>(mParent)->clicked_menu_impl(FRM_DN);
	Entrance();
}
void InvenSlot::sec_count_change(INT32 result_time)
{
	if( result_time == 0 ) return;
	slot_util_impl::idx = (result_time - 1) / 2;
	if( (result_time % 2) == 1 ) slot_util_impl::idx = UINT_MAX;
	mSlot.UpdateList( 9 );
}
void InvenSlot::go_stop()
{
	if( slot_util_impl::wstr.empty() == true ) return;
	slot_util_impl::use_fn = true; slot_util_impl::idx = UINT_MAX; mSlot.UpdateList( 9 );
	size_t size = slot_util_impl::wstr.size()*2 + 2;
	sec_counter.SetBaseSec(size);
	sec_counter.Start();
	static_cast<UIPhaseInventory*>(mParent)->clicked_menu_impl(FRM_UP);
}

void InvenSlot::update_item_info_normal_impl2(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn)
{
	if( slot_util_impl::enable_slot(slot_idx) == false ) { scn->EnableScene(false); return; }
	scn->EnableScene(true);

	T_ItemID itemId = slot_util_impl::get_item_id(mCategory);
	mImageMgr->SelectImage(pSetCtrl, itemId);

	i3::wstring strItemName(1, slot_util_impl::get_item_name() );
	I3COLOR durability_color; durability_color.SetARGB(REPAIR_COLOR_BEST);  

	LuaState * L = UILua::CallLuaFunction( scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, strItemName);
	i3Lua::PushStringUTF16To8( L, L"");
	i3Lua::PushBoolean( L, false);
	i3Lua::PushNumber( L, 0.f);
	i3Lua::PushInteger( L, -1);
	i3Lua::PushColor( L, &durability_color );
	UILua::EndLuaFunction(L , 9);

	L = UILua::CallLuaFunction( scn, "Select");
	UILua::EndLuaFunction(L , 0);

	L = UILua::CallLuaFunction( scn, "EnableAccept");
	i3Lua::PushBoolean( L, false );
	UILua::EndLuaFunction(L , 1);
}

void InvenSlot::update_item_info_normal_impl(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	const ::ITEM_INFO & info = mInfoList.find( slot_idx );
	T_ItemID itemId = info.m_TItemID;
	item_def::Category ctg = item_def::get_category(info.m_TItemID);
	bool bPCcafe = false;
	UINT8 ItemType = mInfoList.GetInfoItemType(slot_idx, mCategory, mType );
	bool repair_item = false;
	REAL32	durability_ratio = 0.0f;
	i3::wstring wstrTermCount;

	mImageMgr->SelectImage(pSetCtrl, itemId);

	i3::rc_wstring wstrItemName;
	// 기획 요청. 상점의 캐릭터 이름으로 표시되도록 수정. 2013.11.20. swoongoo
	/*if( mCategory == item_def::CTG_CHARA)
		usf::GetShopItemMultiCharaName(strItemName, info);
	else*/
		usf::GetShopItemName(wstrItemName, info);

	ItemStatus item_status = usf::GetItemStatus( info.m_TItemID, info.m_TItemDBIdx );

	if( (ItemType == (UINT8)ITEM_ATTR_TYPE_PC_1)
		|| (ItemType == (UINT8)ITEM_ATTR_TYPE_PC_2 ) )
	{
		bPCcafe = true;
	}
	else
	{
		repair_item = usf::GetDurability( itemId, info.m_TItemDBIdx, durability_ratio );
	}

	if(repair_item)
	{
		i3::sprintf( wstrTermCount, L"%d%%", (int)(durability_ratio*100.f) );
	}
	else
	{
		if(ctg == item_def::CTG_ITEM)
		{
			usf::GetItemUseDay(item_status, repair_item, itemId, info.m_TItemDBIdx, wstrTermCount, false);	
		}
		else
		{
			usf::GetItemUseDay(item_status, repair_item, itemId, info.m_TItemDBIdx, wstrTermCount, true);
		}
	}

	I3COLOR durability_color;
	if( durability_ratio >= 0.9f )		
		durability_color.SetARGB(REPAIR_COLOR_BEST);  
	else if( durability_ratio >= 0.5f )	
		durability_color.SetARGB(REPAIR_COLOR_NORMAL);
	else if( durability_ratio >= 0.1f )	
		durability_color.SetARGB(REPAIR_COLOR_BAD);  
	else								
		durability_color.SetARGB(REPAIR_COLOR_BROKEN);

	//화면 표시 때문에 이렇게 변경한다.
	if(ctg == item_def::CTG_ITEM && item_status == ITEM_STATUS_NOTUSED ) 
	{
		//'사용하기' 로 표시한다.
		//item_status = ITEM_STATUS_NOHAVE;  //NOTUSE로 표시하기로 했기 때문에 주석 처리(PBBUG-795 문서(20161209) 참고)

		// 아이템 이름 뒤에 기간을 표시한다. // SubType을 기간으로 사용하지 않는 아이템류는 기간표시를 하지 않음
		if (GET_ITEM_TYPE(itemId) != ITEM_TYPE_WRAP_COUNT && GET_ITEM_TYPE(itemId) != ITEM_TYPE_POINT)
		{
			UINT32 ui32ItemTerm = GET_ITEM_SUBTYPE(itemId);
			if (ui32ItemTerm > 0)
			{
				wstrItemName = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_CASH_ITEM_TERM"), wstrItemName, ui32ItemTerm);
			}
		}
	}

	LuaState * L = UILua::CallLuaFunction( scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, wstrItemName);
	i3Lua::PushStringUTF16To8( L, wstrTermCount);
	i3Lua::PushBoolean( L, repair_item);
	i3Lua::PushNumber( L, durability_ratio);
	i3Lua::PushInteger( L, (INT32)(item_status - 1));	//equip
	i3Lua::PushBoolean( L, bPCcafe);
	i3Lua::PushColor( L, &durability_color );
	UILua::EndLuaFunction(L , 10);

	if( iSlot::is_selected(slot_idx) )
		L = UILua::CallLuaFunction( scn, "Select");
	else
		L = UILua::CallLuaFunction( scn, "Unselect");

	UILua::EndLuaFunction(L , 0);

	ShopItemInfo* shopItemInfo = g_pShopItemInfoDataBase->getInfo(info.m_TItemID);
	if (shopItemInfo)
	{
		if (shopItemInfo->IsCouponType())	// 쿠폰제 아이템은 사용하기 버튼 사용 불가하므로.
		{
			L = UILua::CallLuaFunction(scn, "EnableAccept");
			i3Lua::PushBoolean(L, false);
			UILua::EndLuaFunction(L, 1);
			return;
		}
	}

	//사용중인 경우엔 버튼을 비활성화한다. 캐릭터 항목은 모두 장착중인 상태이므로 버튼을 제거한다.
	if( (ctg == item_def::CTG_ITEM && item_status == ITEM_STATUS_USED) || ctg == item_def::CTG_CHARA  || item_def::get_type( info.m_TItemID) == ITEM_TYPE_GACHA_TICKET) 
	{
		L = UILua::CallLuaFunction( scn, "EnableAccept");
		i3Lua::PushBoolean( L, false );
		UILua::EndLuaFunction(L , 1);
	}

	// 사용중이면서 도중에 자유변경 가능한 아이템은 [변경하기] 버튼으로 세팅
	if( (ctg == item_def::CTG_ITEM && item_status == ITEM_STATUS_USED) && (info.m_TItemID == CASHITEM_ITEMID_COLOR_MUZZLE_FLASH) )
	{
		L = UILua::CallLuaFunction( scn, "EnableAccept_Change");
		i3Lua::PushBoolean( L, true );
		UILua::EndLuaFunction(L , 1);
	}
}

void InvenSlot::update_item_info_normal(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn)
{
	(this->*pf_update_item_info_normal_arr[slot_util_impl::use_fn])(pSetCtrl, slot_idx, scn);
}

void InvenSlot::update_item_info_front(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	const ::ITEM_INFO & info = mInfoList.find( slot_idx );
	T_ItemID itemId = info.m_TItemID;
	bool bPCcafe = false;
	UINT8 ItemType = mInfoList.GetInfoItemType(slot_idx, mCategory, mType );

	if( (ItemType == (UINT8)ITEM_ATTR_TYPE_PC_1)
		|| (ItemType == (UINT8)ITEM_ATTR_TYPE_PC_2 ) )
	{
		bPCcafe = true;
	}
	mImageMgr->SelectImage(pSetCtrl, itemId);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, info);

	LuaState * L = UILua::CallLuaFunction( scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, wstrItemName.c_str(), wstrItemName.size());
	i3Lua::PushStringUTF16To8( L, L"" );
	i3Lua::PushBoolean( L, false );
	i3Lua::PushNumber( L, 0.f );
	i3Lua::PushInteger( L, -1 );	//equip
	i3Lua::PushBoolean( L, bPCcafe);
	UILua::EndLuaFunction(L , 6);

	L = UILua::CallLuaFunction( scn, "EnableAccept");
	i3Lua::PushBoolean( L, false );
	UILua::EndLuaFunction(L , 1);

	L = UILua::CallLuaFunction( scn, "Select");
	UILua::EndLuaFunction(L , 0);

	L = UILua::CallLuaFunction(scn, "EnablePreview");
	UILua::EndLuaFunction(L, 0);
}

void InvenSlot::update_item_info(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn, bool front_dp)
{
	(this->*pf_update_item_info_arr[front_dp])(pSetCtrl, slot_idx, scn);
}

void InvenSlot::update_item_attribute(INT32 slot_idx)
{
	const ITEM_INFO & info = mInfoList.find( slot_idx );
	item_def::Category ctg = item_def::get_category(info.m_TItemID);

	LuaState * L = UILua::CallLuaFunction(mDpAttScene, "SetInfoTap");
	i3Lua::PushInteger(L, (INT32)ctg);
	UILua::EndLuaFunction(L, 1);
	
	switch(ctg)
	{
	case item_def::CTG_WEAPON: update_weapon_attribute_impl(info.m_TItemID, "SetWeaponInfo");		break;
	case item_def::CTG_CHARA: update_chara_attribute_impl(info.m_TItemID);	break;
	case item_def::CTG_PARTS: update_parts_attribute_impl(info.m_TItemID);	break;
	case item_def::CTG_SKIN : update_parts_attribute_impl(info.m_TItemID);	break;
	case item_def::CTG_ITEM: update_item_attribute_impl(info.m_TItemID);	break;
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}
}

void InvenSlot::update_btn_state_at_first()
{
	switch( mCategory )
	{
	case item_def::CTG_WEAPON:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
			LuaState* L3 = UILua::CallLuaFunction( mDpAttScene, "SetTextAccept" );
			i3Lua::PushInteger(L3, 0); //'장비하기'로 변경
			UILua::EndLuaFunction(L3, 1);
		}
		break;

	case item_def::CTG_CHARA: 
		{
			LuaState* L2 = UILua::CallLuaFunction( mDpAttScene, "EnableExtend" );	//기간 연장
			i3Lua::PushBoolean(L2, false );
			UILua::EndLuaFunction(L2, 1);

			L2 = UILua::CallLuaFunction( mDpAttScene, "EnableAccept" );	//장착 표시 유무
			i3Lua::PushBoolean(L2, false );
			UILua::EndLuaFunction(L2, 1);
		}
		break;

	case item_def::CTG_ITEM:
		{
			LuaState* L3 = UILua::CallLuaFunction( mDpAttScene, "SetTextAccept" );
			i3Lua::PushInteger(L3, 2 ); //'사용하기'로 변경
			UILua::EndLuaFunction(L3, 1);
		}
		break;
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}
}

void InvenSlot::update_btn_state(INT32 slot_idx)
{
	const ITEM_INFO & info = mInfoList.find( slot_idx );
	item_def::Category ctg = item_def::get_category(info.m_TItemID);
	UINT8 ItemType = mInfoList.GetInfoItemType(slot_idx, mCategory, mType );
	//"EnableExtend", "EnableAccept", "EnableDelete"
	bool chk_expand_enable = false; //기간 연장 버튼 활성화/비활성화 유무
	bool chk_accpet_enable = false; //장착 버튼 활성화/비활성화 유무
	bool chk_accpet_enable_by_use_item = false; //아이템 항목에 '사용중'에 따라 활성화/비활성화
	static const bool chk_forch_accpet_disable = false; //장착 버튼 강제 disable 유무.. chk_accpet_enable == false 경우엔 적용된다.
//	bool chk_delete_enable = false; //장착 버튼 강제 disable 유무.. chk_accpet_enable == false 경우엔 적용된다.
	bool bPCcafe = false;
	bool bBasicItem = false; //true면 기본지급아이템

	if( (ItemType == (UINT8)ITEM_ATTR_TYPE_PC_1)
		|| (ItemType == (UINT8)ITEM_ATTR_TYPE_PC_2 ) )
	{
		bPCcafe = true;
	}
	else if (ItemType == (UINT8)ITEM_ATTR_TYPE_BASIC)
	{
		bBasicItem = true;
	}

	switch(ctg)
	{
	case item_def::CTG_WEAPON:	chk_expand_enable = true;	break;
	case item_def::CTG_PARTS: 	chk_expand_enable = true;	chk_accpet_enable = true;	break;
	case item_def::CTG_SKIN :	chk_expand_enable = true;	chk_accpet_enable = true;	break;
	case item_def::CTG_CHARA: 	break;//chk_forch_accpet_disable = true; break;
	case item_def::CTG_ITEM: chk_accpet_enable_by_use_item = true; break;
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}

	{
		LuaState* L2 = UILua::CallLuaFunction( mDpAttScene, "EnableDelete" );	//삭제 표시 유무
		i3Lua::PushBoolean(L2, true);
		UILua::EndLuaFunction(L2, 1);
	}

	bool bSaleItem = false;

	if ( chk_expand_enable == true)
	{
		const SHOP_GOODS_PACK* pack = CShopContext::i()->GetGoodsPack(info.m_TItemID);
		
		if ( pack != nullptr)
		{
			for (INT32 i = 0; i < pack->GetGoodsCount(); i++)
			{
				if ( pack->GetGoods(i)->isShopGoods())
					bSaleItem = true;
			}
		}
	}

	if( (chk_expand_enable == true) && (bPCcafe == false) && (bSaleItem == true))
	{
		UINT8 AuthType = CShop::i()->GetAuthType(info.m_TItemID);
		bool bextend = false;
		if (AuthType == ITEM_TYPE_TIME )
			bextend = true;

		//기간 연장 가능한 아이템은 내구도 아이템(기간제 아이템)이 아니고, 기본 아이템이 아닐 때 가능하다.
		LuaState* L1 = UILua::CallLuaFunction( mDpAttScene, "EnableExtend" );
		i3Lua::PushBoolean(L1, bextend);
		UILua::EndLuaFunction(L1, 1);
	}

	if(chk_accpet_enable == true)
	{
		//파츠는 다른 사람이 착용 중인건 착용 못 함.
		bool accept = is_enable_equip_parts(info.m_TItemID, info.m_TItemDBIdx);

		LuaState* L2 = UILua::CallLuaFunction( mDpAttScene, "EnableAccept" );	//장착 표시 유무
		i3Lua::PushBoolean(L2, accept );
		UILua::EndLuaFunction(L2, 1);
	}
	else
	{
		LuaState* L2 = UILua::CallLuaFunction( mDpAttScene, "EnableAccept" );	//장착 표시 유무

		struct PushBoolean_FALSE
		{
			static void call(LuaState* L2 ) { i3Lua::PushBoolean(L2, false); }
		};
		struct PushBoolean_TRUE
		{
			static void call(LuaState* L2) { i3Lua::PushBoolean(L2, true); }
		};

		typedef mpl::if_c<chk_forch_accpet_disable == true, PushBoolean_FALSE, PushBoolean_TRUE>::type PushBoolean;

		PushBoolean::call(L2);

		UILua::EndLuaFunction(L2, 1);
	}

	// 피시방 아이템 혹은 기본 지급 아이템이면 기간연장/삭제하기 버튼 미출력
	if( bPCcafe == true || bBasicItem == true)
	{
		//기간 연장 가능한 아이템은 내구도 아이템(기간제 아이템)이 아니고, 기본 아이템이 아닐 때 가능하다.
		LuaState* L1 = UILua::CallLuaFunction( mDpAttScene, "EnableExtend" );
		i3Lua::PushBoolean(L1, false );
		UILua::EndLuaFunction(L1, 1);

		LuaState* L2 = UILua::CallLuaFunction( mDpAttScene, "EnableDelete" );	//삭제 표시 유무
		i3Lua::PushBoolean(L2, false );
		UILua::EndLuaFunction(L2, 1);
	}

	ItemStatus item_status = usf::GetItemStatus( info.m_TItemID, info.m_TItemDBIdx );
	LuaState* L3 = UILua::CallLuaFunction( mDpAttScene, "SetTextAccept" );	//1일
	if(ctg == item_def::CTG_ITEM && item_status == ITEM_STATUS_NOTUSED )
		i3Lua::PushInteger(L3, ITEM_STATUS_USED ); // item_status == ITEM_STATUS_EQUIPPED일 경우 '인증하기', 그외 '장비 하기'
	else
		i3Lua::PushInteger(L3, item_status == ITEM_STATUS_NOTUSED ); // item_status == ITEM_STATUS_EQUIPPED일 경우 '인증하기', 그외 '장비 하기'
	UILua::EndLuaFunction(L3, 1);

	if(chk_accpet_enable_by_use_item == true)
	{
		LuaState* L2 = UILua::CallLuaFunction( mDpAttScene, "EnableAccept" );	//장착 표시 유무
		i3Lua::PushBoolean(L2, item_status != ITEM_STATUS_USED);
		UILua::EndLuaFunction(L2, 1);
	}

	ShopItemInfo* shopItemInfo = g_pShopItemInfoDataBase->getInfo(info.m_TItemID);
	if (shopItemInfo)
	{
		if (shopItemInfo->IsCouponType())
		{
			LuaState* L1 = UILua::CallLuaFunction(mDpAttScene, "EnableAccept");
			i3Lua::PushBoolean(L1, false);
			UILua::EndLuaFunction(L1, 1);

			LuaState* L2 = UILua::CallLuaFunction(mDpAttScene, "EnableDelete");
			i3Lua::PushBoolean(L2, false);
			UILua::EndLuaFunction(L2, 1);
		}
	}
}



namespace
{
	bool is_enable_equip_parts( T_ItemID itemid, T_ItemDBIdx ware_idx)
	{
		if (g_pEnvSet->IsV2Version() == false)
			return true;

		UINT32 chara_idx = static_cast<UINT32>(CCharaInfoContext::i()->GetMainCharaSlotIdx());
		// vv3 - check
		int chara_itemid = CCharaInfoContext::i()->GetMainCharaPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		bool default_item = item_def::is_default_item(itemid);
		bool default_parts = item_def::is_default_parts(itemid, chara_itemid);
		bool shared_item = item_def::is_shared_parts_item(itemid, chara_itemid);
		UINT32 equip_chara_idx = CharaInfoMgr::instance()->find_equip_chara_idx(itemid, ware_idx);

		if( shared_item )	//공유 아이템
		{
			if( default_parts )	//기본 파츠다.
			{
				return true;
			}
			else	//기본 파츠가 아니다.
			{
				if(default_item)
				{
					return false;
				}
				else
				{
					if(chara_idx == equip_chara_idx || equip_chara_idx == UINT_MAX)	//다른 캐릭 착용중
						return true;
					else
						return false;
				}
			}
		}
		else	//비 공유 아이템
		{
			return false;
		}
	}

	bool is_possible_purchase( T_ItemID itemid )
	{
		// SaleType이 ALL, BUY, GIFT인 것만 구매가 가능하다.
		const SHOP_GOODS_PACK * goods_pack = CShopContext::i()->GetGoodsPack(itemid);

		if( goods_pack )
		{
			INT32 count = goods_pack->GetGoodsCount();

			for( INT32 idx = 0 ; idx < count ; ++idx )
			{
				SHOP_GOODS * goods = goods_pack->GetGoods(idx);

				if( goods->_SaleType < GOODS_SALE_EVENT )
					return true;
			}
		}

		return false;
	}

	bool is_possible_expend_item(T_ItemID itemid)
	{
		return ( is_possible_purchase(itemid) && !CShopContext::i()->IsDurabilityItem(itemid) );
	}
}