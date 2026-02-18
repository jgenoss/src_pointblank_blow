#include "pch.h"
#include "ItemConsumer.h"

#include "UIDefine.h"
#include "UIItemInfoUtil.h"
#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UIShopFunction.h"

#include "UIMainFrame.h"
#include "UIFloatCharacterView.h"
#include "PurchaseMgr.h"
#include "ClanGameContext.h"
#include "UserInfoContext.h"

#include "UI_V10/UIFloatCharacterView_V10.h"

#include "i3Base/string/ext/format_rcstring.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "UISignal.h"

//- ItemConsumer ----------//
ItemConsumer* ItemConsumer::instance()
{
	static ItemConsumer instance;
	return &instance;
}

ItemConsumer::ItemConsumer() : m_AuthWareDBIndex(0), m_ItemID(0)
{
	m_OpenedUsePopup = UPW_NULL;

	pf_method[USE_METHOD_DEFAULT_AUTH] = &ItemConsumer::auth;
	pf_method[USE_METHOD_DISGUISE_NICK] = &ItemConsumer::open;
	pf_method[USE_METHOD_DISGUISE_RANK] = &ItemConsumer::open;
	pf_method[USE_METHOD_COLOR_NICK] = &ItemConsumer::open;
	pf_method[USE_METHOD_COLOR_CLAN] = &ItemConsumer::open;
	pf_method[USE_METHOD_COLOR_CROSS_HAIR] = &ItemConsumer::open;
	pf_method[USE_METHOD_CHANGE_NICK] = &ItemConsumer::open;
	pf_method[USE_METHOD_CHANGE_CLAN_NAME] = &ItemConsumer::open;
	pf_method[USE_METHOD_CHANGE_CLAN_MARK] = &ItemConsumer::open;
	pf_method[USE_METHOD_BATTLE_FIELD_MOVE] = &ItemConsumer::battle;
	pf_method[USE_METHOD_COLOR_FIRE_MUZZLE] = &ItemConsumer::open;

	m_bClanMaxBy50 = false;
	m_bClanMaxBy10 = false;
}

ItemConsumer::~ItemConsumer()
{
}

//--------------------////--------------------//
bool ItemConsumer::use(UIMainFrame* frm, T_ItemID item_id)
{
	m_MainFrame = frm;
	m_ItemID = item_id;

	CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
	const UINT8 PLUS_10 = 10; const UINT8 PLUS_50 = 50;

	// 클랜 수용가능 인원이 최대치에 도달한 상태에서 클랜원 증가권을 사용할 수 없음. 
	if(m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
	{
		if( pClanInfo->m_maxPerson + PLUS_50 > MAX_MEMBER_COUNT){
			
			i3::rc_wstring wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			return false;	// 사용 시도 실패시 패킷 보내지 않습니다 ()
		}
	}

	if( m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10)
	{
		if( pClanInfo->m_maxPerson + PLUS_10 > MAX_MEMBER_COUNT){
			
			i3::rc_wstring wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			return false;
		}
	}

	m_AuthWareDBIndex = CInvenList::i()->FindWareDBIdx_ByItemID( m_ItemID );
	if( m_AuthWareDBIndex == INVALID_WAREDB_INDEX)
	{
		/*사용할 수 없는 아이템입니다.*/ /*아이템 사용 실패*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		return false;
	}

	//중복 사용 가능한 아이템인지 예외 검사.
	usf::ValidUseType result = usf::IsUseableItem(m_ItemID);
	if( result != usf::vut_Ok )
	{
		usf::Message_UseResert(result);
		return false;
	}

	//ItemConsumer::use 함수에서 사용하는 아이템인지 판단한다.
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(item_id);

	if ( pInfo && pInfo->GetUsePosition() != USE_POS_ANYWHERE )
	{
		const i3::rc_wstring& wstrMsg = pInfo->GetUsePositionDesc();				// 사용할 수 있는 위치 메시지
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg, &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));	//아이템 사용 불가
		return false;
	}

	if (false == chk_item_period::_CheckUseItemPeriod(m_ItemID))
	{
		// 최대 기간/갯수 제한팝업, 하나라도 제한에 걸리는게 있다면 리턴(구매X)
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INVEN_PERIOD_LIMIT_POPUP1"), &GAME_RCSTRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"));
		return false;
	}

	ITEM_USE_METHOD	method = use_item::UseMethodStringToEnum(m_ItemID);
	if(method != USE_METHOD_UNKNOWN)
		(this->*pf_method[method])(method);

	return true;
}

//bool ItemConsumer::use_in_domi(CASHITEM_GROUP_TYPE type)
//{
//	T_ItemID ItemID = MAKE_CASH_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DOMIITEM, type, CASHITEM_DAY_NO );
//	T_ItemDBIdx db_idx = CInvenList::i()->FindWareDBIdx_ByItemID(ItemID);
//
//	if(db_idx == INVALID_WAREDB_INDEX) return false;
//
//	PACKET_DOMINATION_USE_ITEM_REQ i;
//	i._TItemWareIdx = db_idx;
//
//	GameEventSender::i()->SetEvent( EVENT_DOMI_USE_ITEM, reinterpret_cast<void*>(&i));
//
//	return true;
//}


bool ItemConsumer::remove(UIMainFrame* frm, T_ItemID item_id, T_ItemDBIdx item_db_idx)
{
	m_MainFrame = frm;
	m_ItemID = item_id;
	m_AuthWareDBIndex = item_db_idx;

	if( item_db_idx == INVALID_WAREDB_INDEX)
	{
		/*사용할 수 없는 아이템입니다.*/ /*아이템 사용 실패*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		return false;
	}

	PurchaseMgr::i()->Remove(0, m_ItemID, item_db_idx );   // "finish_remove_item",

	return true;
}

void	 ItemConsumer::process_game_event_in_query( INT32 event, INT32 arg, const i3::user_data& UserData)
{
	if( m_ItemID <= 0) return; //아이템 사용중이 아니므로 무시한다. (이 코드에가 필요한 이유를 모르겠음.. 

	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{
	case EVENT_ITEM_AUTH_DATA_CHAGNE:
	case EVENT_ITEM_AUTH:	// 갯수제 아이템 인증
	case EVENT_CLAN_PERSON_PLUS:	// 클랜원 증가 캐시 아이템 (CASHITEM_GROUP_CLAN_PERSON_PLUS_50) 사용 결과
		{
			if (EV_SUCCESSED(arg) )
			{
				// 클랜원 증가 캐시 아이템은 EVENT_ITEM_AUTH 이벤트일때는 클랜원의 실제 증가된 수치가 서버와 싱크되지 않을수 있으므로
				// 정확하게 서버에서 업데이트되는 EVENT_CLAN_PERSON_PLUS 이벤트를 받을때만 사용 결과 창을 출력한다.
				// 14. 4. 30 Gyunjgu.kim 추가) 사용 실패 경우에 대해서는 다른 곳(use)에서도 결과 창을 출력합니다.
				if( event == EVENT_ITEM_AUTH && m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10)
					break;

				if( event == EVENT_ITEM_AUTH && m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
					break;

				m_MainFrame->ClosePopup(UPW_QUERY_ITEM);
				if( m_OpenedUsePopup != -1 )
					m_MainFrame->ClosePopup(m_OpenedUsePopup);
				
				if( m_ItemID != CASHITEM_ITEMID_MEGAPHONE)
					open_popup(USE_ITEM_COMPLETE);
				if(g_pEnvSet->m_nUIVersion == UFV_1_5)
				{
					UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));

					if( pCharaViewWin != nullptr)
						pCharaViewWin->DoUpdateCharacter(); //캐릭터 상태 변경
				}
				else
				{
					UIFloatCharacterView_V10 * pCharaViewWin = static_cast<UIFloatCharacterView_V10*>(GameUI::GetFloating(UFW_CHARACTER));

					if( pCharaViewWin != nullptr)
						pCharaViewWin->DoUpdateCharacter(); //캐릭터 상태 변경
				}
				
			//	UI::call_cb( "update_slot9", (void*) true, (void*) true); //상점/인벤 슬롯 상태 변경
				UISignal::UpdateSlot9(true, true);		//상점/인벤 슬롯 상태 변경
			}
			else
			{
				i3::rc_wstring wstrErrorMsg;

				switch(arg)
				{
				case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
					//{
					//	// 클랜을 찾을수 없습니다.
					//	wstrErrorMsg = GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_NO_FINDING");
					//}
					//break;
				case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
					{
						// 클랜 마스터만\n 사용할 수 있는 아이템입니다.
						wstrErrorMsg = GAME_RCSTRING("STR_INVEN_ONLY_MASTER");
					}
					break;
				case EVENT_ERROR_CLAN_INCREASE_MAXPERSON_FAIL:
					{						
						if(m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
							m_bClanMaxBy50 = true;
						else if(m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10)
							m_bClanMaxBy10 = true;

						// 아이템을 사용할 수 없습니다.\n \n 클랜 최대 인원수는\n {col:255,153,51,255}%d{/col}명으로 제한되어 있습니다.
						// note. 클랜 최대 인원수는 MAX_MEMBER_COUNT로 제한되있습니다,
						wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
					}
					break;
				default:
					{
						// 인증에 실패하였습니다. 잠시 후 다시 시도해주세요.
						wstrErrorMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_AUTH");
					}
					break;
				}

				/*아이템 사용 실패*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			}
		}
		break;
	}
}

//--------------------////--------------------//
void ItemConsumer::auth(ITEM_USE_METHOD method)
{
	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &m_AuthWareDBIndex);
}

void ItemConsumer::open(ITEM_USE_METHOD method)
{
	switch(method)
	{
	case USE_METHOD_DISGUISE_NICK: open_popup( USE_ITEM_DISGUISE_NICK); break;
	case USE_METHOD_DISGUISE_RANK: open_popup( USE_ITEM_DISGUISE_RANK); break;
	case USE_METHOD_COLOR_NICK:open_popup( USE_ITEM_COLOR_NICK); break;
	case USE_METHOD_COLOR_CLAN: open_popup( USE_ITEM_COLOR_CLAN_NAME); break;
	case USE_METHOD_COLOR_CROSS_HAIR: open_popup( USE_ITEM_COLOR_CROSSHAIR); break;
	case USE_METHOD_CHANGE_NICK: open_popup( USE_ITEM_CHANGE_NICK); break;
	case USE_METHOD_CHANGE_CLAN_NAME: open_popup( USE_ITEM_CHANGE_CLAN_NAME); break;
	case USE_METHOD_CHANGE_CLAN_MARK: open_popup( USE_ITEM_CHANGE_CLAN_MARK); break;
	case USE_METHOD_COLOR_FIRE_MUZZLE: open_popup( USE_ITEM_COLOR_FIRE_MUZZLE); break;
	}
}

void ItemConsumer::error(ITEM_USE_METHOD method)
{
	/*사용할 수 없는 아이템입니다.*/ /*아이템 사용 실패*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
}

void ItemConsumer::battle(ITEM_USE_METHOD method)
{
	/*게임중에만 사용할 수 있습니다.*/ /*아이템 사용 실패*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_USE_IN_GAME"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
}

//--------------------////--------------------//
void ItemConsumer::open_popup(USE_ITEM_POPUP_TYPE type)
{
	UINT8 Type = 0;	UINT32 Arg = 0;
	if (CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM,  m_ItemID, &Type, &Arg))
	{		
		//	현재 사용중인 아이템
		if ((ITEM_ATTR_TYPE_AUTH == Type) &&
			(m_ItemID != CASHITEM_ITEMID_COLOR_MUZZLE_FLASH)) // 컬러총구화염 아이템은 사용중 내용변경이 가능합니다.
		{
			/*아이템 사용*//*현재 사용중인 아이템입니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CURRENT_USE_ITEM"), &GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"));
			return;
		}
	}

	UIPOPUPWIN popup = UPW_NULL;
	void* arg1 = &m_ItemID, *arg2 = &type;

	switch( type)
	{
	case USE_ITEM_COMPLETE: popup = UPW_RESULT_USEITEM; break;
	
	case USE_ITEM_CHANGE_NICK: popup = UPW_CHANGENICK; break;
	case USE_ITEM_DISGUISE_NICK: popup = UPW_CHANGENICK; break;
	case USE_ITEM_CHANGE_CLAN_NAME: popup = UPW_CHANGENICK; break;

	case USE_ITEM_COLOR_NICK: popup = UPW_CHANGE_COLOR_NICK; break;
	case USE_ITEM_COLOR_CLAN_NAME: popup = UPW_CHANGE_COLOR_NICK; break;
	case USE_ITEM_COLOR_CROSSHAIR: popup = UPW_CHANGE_CROSSHAIR_COLOR; break;

	case USE_ITEM_DISGUISE_RANK: popup = UPW_DISGUISE_RANK; break;
	case USE_ITEM_COLOR_FIRE_MUZZLE: popup = UPW_COLOR_FIRE_MUZZLE_EFFECT; break;	
	case USE_ITEM_CHANGE_CLAN_MARK: 
		popup = UPW_CHANGE_CLAN_MARK; 
		CLAN_DETAIL_INFO * pClanInfo = g_pFramework->GetClanContext()->GetMyClanDetailInfo();
		arg2 = &pClanInfo->m_mark;
		break;
	}

	if(popup == UPW_NULL) return; //등록이 안 된 아이템
	if( m_MainFrame->IsOpenedPopup(popup) == true ) return; //아이템 사용창이 열린 상태

	m_MainFrame->OpenPopup(popup, arg1, arg2);

	if( popup != UPW_RESULT_USEITEM )
		m_OpenedUsePopup = popup;
}


//chk_item_period (namespace)
// 사용시 아이템 만료기간체크
bool chk_item_period::_CheckUseItemPeriod(T_ItemID itemID)
{
	INT32 ItemID = itemID;
	INT32 CheckItemID = 0;

	CheckItemID = GetCashItemByGroup(ItemID);

	UINT8 ui8Type = 0;
	UINT32 ui32Arg = 0;

	UINT32 Inven_Itemterm = 0;
	UINT32 CheckItemTerm = 0;
	UINT32 Max_Term = 0;

	CheckItemTerm = GET_ITEM_TIME(ItemID);

	bool isItem = false;

	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(ItemID);
	isItem = CInvenList::i()->GetItemTypeByItemID(dbType, CheckItemID, &ui8Type, &ui32Arg);

	// 인벤에 해당아이템이 없으면 return true
	if (false == isItem)
	{
		return true;
	}

	CASHITEM_USAGE_TYPE usageType = CashItem::GetUsageFromItemID(CheckItemID);
	// 기간제 아이템이면.
	switch (usageType)
	{
	case CASHITEM_USAGE_MAINTENANCE:
	{
		Max_Term		=	UserInfoContext::i()->GetInventorySecTime(LIMIT_ITEM_DATE);
		Inven_Itemterm	=	UserInfoContext::i()->GetInventorySecTime(ui32Arg);
		Max_Term = ConvertSecToUseDay(Max_Term);
		Inven_Itemterm = ConvertSecToUseDay(Inven_Itemterm);

		if (CheckItemTerm + Inven_Itemterm > Max_Term)
			return false;
	}
	break;
	case CASHITEM_USAGE_EXPENDABLES:
	{
	}
	break;
	}

	return true;
}