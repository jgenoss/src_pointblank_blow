#include "pch.h"
#include "PurchaseMgr.h"

#include "Shop.h"
#include "UIUtil.h"

#include "UIItemInfoUtil.h"
#include "UIMainFrame.h"
#include "UIPopupItemBase.h"
#include "UIShopFunction.h"
#include "InvenUtil.h"
#include "UICharaInfoMgr.h"
#include "BuyUtil.h"
#include "UIFloatCharacterView.h"
#include "ItemConsumer.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/ext/format_rcstring.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/ColorString.h"
#include "Designation/DesigUtil.h"

#include "UISignal.h"

// detail 네임스페이스가 나쁜 것은 아니었으나, 현재 퍼쳐스매니저의 가독성이 극악으로 나쁜 상황이므로, 최대한 의미있는 영단어로 대체합니다.  어지러운 인터페이스는 최대한 삭제할 예정..
namespace buy
{	
	namespace detail
	{
		struct function_init
		{
			function_init() { i3::function<void(INVEN_BUFFER*)>::allow_global_variable(); }
		} _function_init_;
	}

	struct AuthResultInfo
	{
		i3::vector<bool> result_list;
		size_t			 request_count;
	};
	
	struct SelectedItemInfo
	{
		T_ItemDBIdx			ware_idx;
		const SHOP_GOODS*	goods;
		UINT32				item_id;
	};
	
	struct CartPurchaseResultInfo
	{
		i3::vector<INT32>				resultList;
		i3::vector<T_GoodsID>			GoodsIDList;
	};

	struct GlobalState
	{
		UIBase*					currUI;
		PurchaseRouteType		purchaseRoute;
		DisplayedGoodsList		purchaseGoodsList;
		
		GiftInfo				giftInfo;
		AuthResultInfo			authResultInfo;  // 인증에만 씁니다.. 구매/선물/수리엔 안써요..
		SelectedItemInfo		selectedItemInfo;	// 상점 상품 구매와 많이 무관합니다...인벤/상점에서의 선택된 아이템 정보.(인증/기간 구매/수리 역할 수행) --> 특히 여러개 처리 시 무용지물입니다..뭐여..이거슨 쓰레기입니까.

		CartPurchaseResultInfo  cartPurchaseResultInfo;

		i3::function<void (INVEN_BUFFER*)>	addInvenBuffer;
		i3::vector<INVEN_BUFFER*> purchasedInvenItemList;	// 최근 구매된 목록..(구매 직후 인증처리에만 씁니다.) --> INVEN_BUFFER를 막바로 쓰는건 개인판단하에선 어리석어 보이는데..시간이 없어서 일단 방치합니다.
		
		UISecCount				sec_count;		// 시간재는 유틸..
		bool					working;
		bool					is_point_and_cash_try_purchase;
	};

	static GlobalState g_State;				// 생성자 마련이 되있지 않으므로 PurchaseManager 초기화가 필요할것 같다...

	namespace detail
	{
		struct State_Init
		{
			State_Init() 
			{ 
				g_State.currUI = nullptr;
				g_State.purchaseRoute = PurchaseRoute_Immediate;
				g_State.working = false; 
				g_State.is_point_and_cash_try_purchase = false;
			}
		} _state_init;

	
		
		
		void AddInvenBuffer(INVEN_BUFFER* buffer)
		{
			buy::g_State.purchasedInvenItemList.push_back(buffer);
		}

		enum { ACK_TIME_OUT = 20 };

		void StartEvent()
		{
			buy::g_State.working = true;
			if (buy::g_State.currUI)
			{
				buy::g_State.currUI->SetInputDisable(TRUE);
			}

			buy::g_State.sec_count.SetBaseSec(detail::ACK_TIME_OUT);
			buy::g_State.sec_count.Start();
		}
		
		void EndEvent() 
		{
			if (buy::g_State.currUI)
			{
				//시점 문제로 입력이 막힌 상태를 풀어줘야 하는데. 그 시점에 popup이 활성화 상태라면 막힌 입력을 못 푼다.
				//입력이 막힌 상태이기 때문에 팝업을 활성화 못 할거라 예상되지만, 혹시 활성화 될 경우에 문제가 되기에 주석처리
				//주석 처리한다고 문제가 되지는 않을것 같다.
				//if( g_pFramework->GetUIMainframe()->IsPopupFocused() == false ) //혹시 몰라 코드를 주석처리한다.
				{
					buy::g_State.currUI->SetInputDisable(FALSE);
				}
			}

			buy::g_State.currUI = nullptr;
			buy::g_State.working = false;
			buy::g_State.sec_count.Stop();

			if (buy::g_State.addInvenBuffer.empty() == false)
				buy::g_State.addInvenBuffer.clear();
		}
		
		class SecCountCallback : public	UISecCountCallback
		{
			virtual void OnStart(INT32 result_time) {}
			virtual void OnSecChange(INT32 result_time) {}
			virtual void OnStop(INT32 result_time) {}

			virtual void OnDone(INT32 result_time)
			{
				buy::detail::EndEvent();

				switch (buy::g_State.sec_count.GetBaseSec())
				{
				case ACK_TIME_OUT:
#ifndef _DEBUG
					//	서버로 구입 확인을 보내고 답변을 받을 때까지 기다린다. 
					//	만약 대기 시간이 초과될 경우 서버로 부터 답변을 못받았다고 판단되므로 게임을 강제 종료합니다.

					/*네트워크 문제로 게임이 종료 되었습니다.*/
					GameUI::ExitGamePopup(GAME_RCSTRING("STBL_IDX_EP_BATTLE_TIMEOUT_HOST_C"));
#else
					/*네트워크에 문제로 실패했습니다.\n 다시 시도해 주십시오.*/
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_BY_NET"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
#endif
					break;
				}
			}

		} g_SecCountCB;

	}

}

//- PurchaseMgr ----------//
PurchaseMgr* PurchaseMgr::i()
{
	static PurchaseMgr instance;
	return &instance;
}

PurchaseMgr::PurchaseMgr()
{
	buy::g_State.addInvenBuffer.clear();
	buy::g_State.sec_count.SetReverse(true);
	buy::g_State.sec_count.SetCallback(&buy::detail::g_SecCountCB);

	RegisterGameEventFunc( EVENT_BUY_GOODS,			&PurchaseMgr::ReceiveEvent_Buy);
	RegisterGameEventFunc( EVENT_BUY_EXTEND_GOODS,	&PurchaseMgr::ReceiveEvent_Extend);
	RegisterGameEventFunc( EVENT_ITEM_REPAIR,		&PurchaseMgr::ReceiveEvent_Repair);
	RegisterGameEventFunc( EVENT_AUTH_DELETE,		&PurchaseMgr::ReceiveEvent_Auth_Delete);
	RegisterGameEventFunc( EVENT_CREATE_CHARA,		&PurchaseMgr::ReceiveEvent_Buy_Chara);		// 이것도 이해가 안가고...(서버와 얘기해서 방금 납득함....OK)
	RegisterGameEventFunc( EVENT_SET_CHARA,			&PurchaseMgr::ReceiveEvent_Buy_Chara);		// 아..놔..이게 뭐냐고...뭐 왜 일케 쓰는건데...죄다 엉터리 명명..
	RegisterGameEventFunc( EVENT_DELETE_CHARA,		&PurchaseMgr::ReceiveEvent_Delete_Chara);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,			&PurchaseMgr::ReceiveEvent_Item_Auth);
	RegisterGameEventFunc( EVENT_SEND_GIFT,			&PurchaseMgr::ReceiveEvent_Send_Gift);
}

PurchaseMgr::~PurchaseMgr()
{
	
}

void PurchaseMgr::OnNotify_NewInvenBufferAdded(INVEN_BUFFER* newInvenBuffer)
{
	buy::g_State.addInvenBuffer(newInvenBuffer);
}

void PurchaseMgr::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void PurchaseMgr::ProcessGameEvent( GAME_EVENT evt, INT32 arg, i3::user_data& UserData)
{
	// Update Receive Event
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & f = it->second;
		(this->*f)(arg, UserData);
	}
}

namespace
{
	static void BuyResult_Sucess_Imp() // 일단 1개짜리,장바구니 모두에서 쓰이도록 합니다.. (기존 코드대로입니다. 아주 재수없게 더럽네요.  언젠가 또 고치면 좋겠다..)
	{

#if defined(USE_SALE_COUPON)
		// 사용된 쿠폰을 인벤에서 제거
		CInvenList::i()->DeleteOnlyUsedCouponBuffer();
#endif 

		if (buy::g_State.purchaseRoute == buy::PurchaseRoute_Preview)
			return; //미리보기 구매

#if !defined(AUTH_FROM_BUY_GOODS)
					//	구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
		return;
#else // defined(AUTH_FROM_BUY_GOODS)

		if (buy::g_State.purchaseRoute == buy::PurchaseRoute_Cart)	//장바구니 구매
		{
			/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
			return;
		}

		const buy::DisplayedGoodsPtr& displayedGoods = buy::g_State.purchaseGoodsList[0];

		const SHOP_GOODS* buy_goods = displayedGoods->GetCurrentGoods();

		//즉시 구매 루틴
		if (CInvenList::i()->HaveItemByItemID(buy_goods->_ItemID))
		{
			CShop* shop = CShop::i();

			//Set 아이템은 인증 없고, 인벤에 들어간다.
			if (shop->IsPackageGoods(buy_goods->_GoodsID))
			{
#if !defined(TEMP_ERASE_CODE)
				UINT32 point = 0;

				SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(buy_goods->_GoodsID);
				for (INT32 i = 0; i < pGoodsList->_ui8ItemCount; ++i)
				{
					//포인트 상품일 경우
					if (ITEM_TYPE_POINT == GET_ITEM_TYPE(pGoodsList->_ppItemData[i]->_ui32ItemID))
					{
						switch (GET_ITEM_NUMBER(itemId))
						{
						case 1: point += 1000;		break;
						case 2: point += 5000;		break;
						case 3: point += 10000;		break;
						case 4: point += 25000;		break;
						case 5: point += 50000;		break;
						case 6: point += 100000;	break;
						case 7: point += 20000;		break;
						}
					}
				}

				if (0 < point)
				{
					//	"%d 포인트가 지급되었으며\n \n구입하신 아이템은 인벤토리에서\n확인하실 수 있습니다."
					i3::rc_wstring wstr_buyMessage;
					i3::sprintf(wstr_buyMessage, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_PRICE_SUCCESS"), point);
					GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_buyMessage, &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				}
				else
#endif
				{
					if (item_def::get_category_goodsID(buy_goods->_GoodsID) != item_def::CTG_ITEM)
					{
						/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
						GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
					}
				}
			}
		}

#endif // defined(AUTH_FROM_BUY_GOODS)
	}
		
	static void BuyResult_Fail_Imp(INT32 Arg) // 일단 1개짜리에서만 쓰이도록 합니다.. (기존 코드대로입니다. 다소 더럽네요. 위에 있는 BuyResult_Sucess_Imp보다는 좀 낫습니다...)
	{
		i3::rc_wstring wstrErrorMsg;

		switch (Arg)
		{
		case EVENT_ERROR_INVALID_COUPON:
			/* 존재하지 않는 쿠폰 입니다.*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_ERROR_INVALID_COUPON"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			break;

		case EVENT_ERROR_EXPIRY_DATE_COUPON:
			/*사용 기간이 올바르지 않습니다.*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_LIMIT_TIME"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			break;

		case EVENT_ERROR_COUPON_DATA:
			/* 잘못된 쿠폰 데이터이거나 쿠폰 사용 조건에 맞지 않습니다. */
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_ERROR_COUPON_DATA"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			break;

		case 0X83000045:
			// 내구도로 바뀌면서 이미 가지고 있는 아이템이기 때문에 구매 실패 -- 영권작업 필요 -- (이 닝겐은 누군가요)
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
			break;

		default:
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
			break;
		}
	}

	// bool detail::param_util::result_buy_ack_and_msg()  원래 이런 함수인데....그냥 봐선 그럴듯해보여도, 함수 이름으로 기능 유추하기가 힘듭니다.. 
	// 적당이 수정했으며 만족할만한 수준은 못됩니다..

	static bool CheckResultBuyAck_Imp( const i3::vector<INT32>& resultList, const i3::vector<T_GoodsID>& resGoodsIDList)
	{
		i3::vector<INT32>& purchaseResultList =	buy::g_State.cartPurchaseResultInfo.resultList;
		i3::vector<T_GoodsID>& purchaseResultGoodsIDList = buy::g_State.cartPurchaseResultInfo.GoodsIDList;
		
		purchaseResultList.insert(purchaseResultList.end(), resultList.begin(), resultList.end());
		purchaseResultGoodsIDList.insert(purchaseResultGoodsIDList.end(), resGoodsIDList.begin(), resGoodsIDList.end());

		if (buy::g_State.is_point_and_cash_try_purchase)   // RECV를 한번더 받아야 한다고 하네요...
			return false;
		
		bool is_success = true;
		bool inven_full_error = false;

		i3::stack_wstring wstr_fail_item_name;

		const i3::vector< buy::DisplayedGoodsPtr > & goodsList = buy::g_State.purchaseGoodsList;

		const size_t num_good_to_buy = goodsList.size();				// 이것이 루프 기준입니다..
			
		for (size_t i = 0; i < num_good_to_buy; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = goodsList[i].get();
			const SHOP_GOODS* pCurrGoods = pDisplayedGoods->GetCurrentGoods();

			const size_t num_result = purchaseResultGoodsIDList.size();

			bool bSucessPurchaseEach = false;

			for (size_t j = 0; j < num_result; ++j)
			{
				T_GoodsID resGoodsID = purchaseResultGoodsIDList[j];

				if (pCurrGoods->_GoodsID == resGoodsID)
				{
					INT32 resultValue = purchaseResultList[j];

					if (EV_FAILED(resultValue) == false)		// 성공한 경우..
					{
						bSucessPurchaseEach = true;
					}
					else
					{
						if (resultValue == EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP)
							inven_full_error = true;	//	인벤토리가 가득차 구입 실패.)

						i3::stack_string errString;
						i3::snprintf(errString, MAX_PATH, "BF[%d] : %x ", resGoodsID, resultValue);
						I3PRINTLOG(I3LOG_QA, errString);

					}
					
					i3::vu::erase_index(purchaseResultGoodsIDList, j);
					i3::vu::erase_index(purchaseResultList, j);
					break;			// 반드시 들어가야함..
				}
			}

			if ( bSucessPurchaseEach == false)
			{
				is_success = false;
				i3::rc_wstring wstr_name;
				usf::GetShopItemName(wstr_name, (SHOP_GOODS_PACK*)goodsList[i]->GetGoodsPack());
				wstr_fail_item_name += wstr_name;
				wstr_fail_item_name += L',';
			}

		}
		
		/*
		const size_t num_result = resultList.size();					// 이것은 유효값 범위로 내부에서 조건검사로 걸릅니다.
		
		for (size_t i = 0; i < num_good_to_buy; ++i)
		{
			if ( i < num_result )
			{
				INT32 resultValue = resultList[i];
				// 실패 로그 기록합니다.

				if ( EV_FAILED(resultValue) )
				{
					i3::stack_string errString;
					i3::snprintf(errString, MAX_PATH, "BF[%d] : %x ", i, resultValue);
					I3PRINTLOG(I3LOG_QA, errString);

					if (resultValue == EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP)	
						inven_full_error = true;	//	인벤토리가 가득차 구입 실패.
				
					i3::rc_wstring wstr_name;
					usf::GetShopItemName(wstr_name, (SHOP_GOODS_PACK*)goodsList[i]->GetGoodsPack());
					wstr_fail_item_name += wstr_name;
					wstr_fail_item_name += L',';

					is_success = false;
				}
			}
			else  // 이쪽은 모조리 실패입니다..
			{
				i3::rc_wstring wstr_name;
				usf::GetShopItemName(wstr_name, (SHOP_GOODS_PACK*)goodsList[i]->GetGoodsPack());
				wstr_fail_item_name += wstr_name;  
				wstr_fail_item_name += L',';
				is_success = false;
			}
		}
		*/
		
		if (is_success == false)
		{
			i3::stack_wstring fail_msg = GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_E");
			fail_msg += L'\n';

			wstr_fail_item_name.erase(wstr_fail_item_name.size() - 1); //맨뒤 "," 제거
			fail_msg.append(wstr_fail_item_name.begin(), wstr_fail_item_name.end());

			/*구입실패*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, fail_msg, &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
		}

		if (inven_full_error)
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_MESSAGE_INVENTORY_FULL_BUY_FAIL"));
		
		purchaseResultList.clear(); 
		purchaseResultGoodsIDList.clear();
		
		return is_success;
	}
	
}


// 서버는 리시브 패킷을 한번만 보내지만, 클라쪽이 상품단위로 쪼개서 리시브 처리를 하던걸 처리 1회로 고쳤습니다....( 이게 웬 개떡같은 일이냐 이거지...)
// 캐릭터는 이리로 오면 안됩니다..

void PurchaseMgr::ReceiveEvent_Buy( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;				// 가차에서 이리로 오는 이상한 케이스는 이것으로 걸러내질것으로 보여진다..(좋지는 않네)
	
	bool ack_result;

	if (buy::g_State.purchaseRoute == buy::PurchaseRoute_Immediate)
	{
		ack_result = (EV_SUCCESSED(arg)); // 0번 인덱스.. 
		if (ack_result == true)
			BuyResult_Sucess_Imp();
		else
			BuyResult_Fail_Imp(arg);
	}
	else  // 장바구니 및 미리보기로 예상됩니다..
	{
		const RecvData_EVENT_BUY_GOODS& data = i3::value_cast<RecvData_EVENT_BUY_GOODS>(UserData);
		const i3::vector<INT32>& resList = data.resultList;
		const i3::vector<T_GoodsID>& resGoodsIDList = data.GoodsIDList;

		ack_result = CheckResultBuyAck_Imp(resList, resGoodsIDList);

		if (ack_result == true)
			BuyResult_Sucess_Imp();
		else
		if (buy::g_State.is_point_and_cash_try_purchase)
		{
			buy::g_State.is_point_and_cash_try_purchase = false;   // 나머지 한번 더 받기 위해 리셋..
			return;
		}
	}
	
	buy::detail::EndEvent();
	
	UISignal::FinishBuyItem(ack_result, buy::g_State.purchaseGoodsList);			// 상점에서만 쓴다.  선물상점은 별도 처리..(그쪽은 인수가 아예 달라버림)
																					// 야전상점은 통합 (둘째인수가 실제 쓰이지 않지만 그대로 방치)
	I3TRACE("PurchaseMgr::ReceiveEvent_Buy \n");				
}

// 캐릭터 구매의 경우, 즉시 구매에 한합니다.. 다른 경로로 오는 것은 모두 이상한 겁니다...

void PurchaseMgr::ReceiveEvent_Buy_Chara( INT32 arg, const i3::user_data &UserData)
{
	if(buy::g_State.working == false ) return;

	if (buy::g_State.purchaseGoodsList.size() > 1)			// 장바구니, 미리보기구매 등 2개이상 아이템 동시 구매시 이쪽으로 돌아온다면 몽땅 튕겨냄..
		return;
	
//	I3ASSERT_RETURN(buy::g_State.purchaseRoute == buy::PurchaseRoute_Immediate);

	bool ack_result = ( EV_SUCCESSED( arg ) );

	if( ack_result == true )
		BuyResult_Sucess_Imp();
	else
		BuyResult_Fail_Imp( arg );
		
	buy::detail::EndEvent();

	UISignal::FinishBuyItem(ack_result, buy::g_State.purchaseGoodsList);			// 상점에서만 쓴다.  선물상점은 별도 처리..(그쪽은 인수가 아예 달라버림)
																					// 야전상점은 통합 (둘째인수가 실제 쓰이지 않지만 그대로 방치)

	I3TRACE( "PurchaseMgr::ReceiveEvent_Buy_Chara \n");				// 현재로선 알맞지 않은 트레이스. 함수명에 큰 문제가 있으나 나중에 수정..
}

// 캐릭터는 이리로 오면 안됩니다..
void PurchaseMgr::ReceiveEvent_Auth_Delete( INT32 arg, const i3::user_data &UserData)
{
	if(buy::g_State.working == false ) return;

	ReceiveEvent_Delete_Chara( arg, UserData);				// 이런 미친...네이밍...
}

void PurchaseMgr::ReceiveEvent_Delete_Chara( INT32 arg, const i3::user_data &UserData)
{
	if(buy::g_State.working == false ) return;

	bool ack_result = ( EV_SUCCESSED( arg ) ) ? true : false;
	if( ack_result )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_INVEN_DELETE_ITEM_SUCCESS"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

		ITEM_INFO info;
		info.m_TItemID = buy::g_State.selectedItemInfo.item_id;			
		info.m_TItemDBIdx = buy::g_State.selectedItemInfo.ware_idx;		

		buy::detail::EndEvent();

		UISignal::FinishRemoveItem(true, &info);
	//	detail::param_util::call_finish_cb_string(true, &info);
			
	}
	else
	{
		const RecvData_EVENT_DELETE_CHARA &data = i3::value_cast<RecvData_EVENT_DELETE_CHARA>(UserData);
		CCharaInfoContext::i()->SetMainCharaBySlotIdx( data._DeleteSlotIdx );
		UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
		if( pCharaViewWin != nullptr)
			pCharaViewWin->SetChangeChara(0, true);

		/*대표 캐릭터 설정이 잘 못 되었습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INCORRECT_MAIN_CHARACTER"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

		buy::detail::EndEvent();

		UISignal::FinishRemoveItem(false, nullptr);
	//	detail::param_util::call_finish_cb_string(false);

	
	}
}

namespace
{
	// 여러번 패킷을 보내는 역할을 하게 됨...( 상점구매는 일괄 구매 구조로 되어 있는데, 인증은 그냥 여러번 보내는걸로 하는 모냥구..)

	static void SendAuthPacket_Imp(INT32 currRequestIndex)
	{
		const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = buy::g_State.purchasedInvenItemList;
		
		if (currRequestIndex >= (INT32)catched_inven_buffer_list.size()) return;

		INVEN_BUFFER* buffer = catched_inven_buffer_list[currRequestIndex];

		buy::g_State.selectedItemInfo.ware_idx = buffer->_TItemWareDBIdx;
		buy::g_State.selectedItemInfo.item_id = buffer->_ui32ItemID;

		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &buy::g_State.selectedItemInfo.ware_idx, 
														&buy::g_State.selectedItemInfo.item_id);

	}

	// 종료되면 false인 모양입니다... 리턴값은 함수명으로 유추가 안된다. (다소 안좋다는 겁니다..)...

	static bool TrySendAuth_Imp(bool ack_result)
	{
		buy::g_State.authResultInfo.result_list.push_back(ack_result);

		size_t ack_size = buy::g_State.authResultInfo.result_list.size();

		if (buy::g_State.authResultInfo.request_count <= ack_size)		// 충분이 받으면 종료시킬 모양..아니라면 계속 반복...
			return false;

		SendAuthPacket_Imp(ack_size);					// 다음번 요청 패킷 번호 진행입니다..
		return true;
	}



}

namespace
{
	
	void PrintMsgIfAuthResultError_Imp()
	{
		bool is_success = true;

		i3::stack_wstring wstr_fail_item_name;

		const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = buy::g_State.purchasedInvenItemList;
			
		size_t size = buy::g_State.authResultInfo.result_list.size();

		for (size_t i = 0; i < size; ++i)
		{
			if (buy::g_State.authResultInfo.result_list[i] == false)
			{
				is_success = false;

				i3::rc_wstring wstr_name;
				usf::GetShopItemName(wstr_name, catched_inven_buffer_list[i]->_ui32ItemID);
				wstr_fail_item_name.append(wstr_name.begin(), wstr_name.end());
				wstr_fail_item_name += L',';
			}
		}

		if (is_success == false)
		{
			i3::stack_wstring fail_msg = GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_AUTH");
			fail_msg += L'\n';
			wstr_fail_item_name.erase(wstr_fail_item_name.size() - 1); //맨뒤 "," 제거
			fail_msg.append(wstr_fail_item_name.begin(), wstr_fail_item_name.end());

			/*아이템 사용 실패*/ /*아이템 사용 실패*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, fail_msg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		}
	}
}

void PurchaseMgr::ReceiveEvent_Item_Auth( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;

	bool ack_result = ( EV_SUCCESSED( arg ) ) ? true : false;
	bool is_auth_send = TrySendAuth_Imp(ack_result);

	if(is_auth_send == true) 
		return;			// 계속 요청...

	PrintMsgIfAuthResultError_Imp();

	buy::detail::EndEvent();

	UISignal::CurrentFinishAuthItem(ack_result, buy::g_State.selectedItemInfo.goods); 	// 프리뷰도 있기 때문에 신경을 좀 써야한다..프리뷰의 경우 이 인수가 쓸모가 없어보인다..

}

namespace
{

	// 뭐...목록은 들고 오는데...딱히 하는 일은 별거 읍네...

	static bool  CheckRepairAck_Imp( const i3::vector<INT32>& resultList)
	{
		bool is_success = true;

		const size_t size = resultList.size();		// 요청한 갯수와 실제로 안맞을수도 있겟습니다...최소한 한개의 에러메세지는 들어있기 때문에 일단 이렇게 방치합니다..

		for (size_t i = 0; i < size; ++i)
		{
			if (EV_FAILED(resultList[i] ) )
			{
				i3::stack_string strErrorLog;
				i3::snprintf(strErrorLog, MAX_PATH, "RF[%d] : %x ", i, resultList[i]);
				I3PRINTLOG(I3LOG_QA, strErrorLog);
				is_success = false; 
			}
		}

		i3::rc_wstring wstrMsg;

		if ( buy::g_State.selectedItemInfo.ware_idx != 0 )			// 단일 품목 수리.. 전체수리는 wareDBIdx값을 0으로 셋팅했습니다..
		{
			// 아래 메세지는 단일 수리만 해당할 것입니다.  buy::g_State.selectedItemInfo는 현재 1개품목만 해당되는 상황인지라...현재 캐릭터의 전체수리와 전혀 맞지 않습니다...
			if (item_def::is_dino_category(buy::g_State.selectedItemInfo.item_id) == false)
			{
				wstrMsg = is_success ? GAME_RCSTRING("STR_POPUP_REPAIR_SUCCESS") : GAME_RCSTRING("STR_POPUP_REPAIR_FAIL");
			}
			else
			{
				wstrMsg = is_success ? GAME_RCSTRING("STR_POPUP_RECOVER_SUCCESS") : GAME_RCSTRING("STR_POPUP_RECOVER_FAIL");
			}
		}
		else 
		{
			wstrMsg = is_success ? GAME_RCSTRING("STR_POPUP_REPAIR_SUCCESS") : GAME_RCSTRING("STR_POPUP_REPAIR_FAIL");
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrMsg, &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

		return is_success;
	}

}


void PurchaseMgr::ReceiveEvent_Repair( INT32 arg, const i3::user_data &UserData)
{
	if( EV_SUCCESSED( arg))
	{
		if(buy::g_State.working == false ) return;

		static const bool ack_result_sucess = true;

		const RecvData_EVENT_ITEM_REPAIR& resData = i3::value_cast<RecvData_EVENT_ITEM_REPAIR>(UserData);
				
		bool  ack_result = CheckRepairAck_Imp(resData.resultList);

		buy::detail::EndEvent();

		UISignal::CurrentFinishRepairItem(ack_result);
	}
}

// 기간 연장 구매는 1개뿐이랩니다...나중에 여러개로 바뀌면 이것저것 고치야 되는거냐...

void PurchaseMgr::ReceiveEvent_Extend( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;

	bool ack_result = ( EV_SUCCESSED( arg ) ) ? true : false;

	const i3::rc_wstring& wstr_msg = ack_result ? GAME_RCSTRING("STR_POPUP_EXTEND_SUCCESS") : GAME_RCSTRING("STR_POPUP_EXTEND_FAIL");
	GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_msg, &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

	buy::detail::EndEvent();
	UISignal::FinishBuyExtendItem(ack_result);

}

// 선물구매는 다른 사람한테 보내는거라..리시브로 물건 받는게 없다..
namespace
{
	static bool CheckGiftAck_Imp( const i3::vector<INT32>& resultList )
	{
		bool ack_result = true;

		const size_t numResult = resultList.size();

		for (size_t i = 0; i < numResult; ++i)
		{
			INT32 res = resultList[i];
			if (EV_FAILED(res))
			{
				// 실패 로그 기록합니다.
				/*EVENT_ERROR_SUCCESSED
				EVENT_ERROR_EVENT_GIFT_GOODS_NOT_ENOUGH_POINT	포인트 부족
				EVENT_ERROR_EVENT_GIFT_GOODS_WRONG_RECEIVE_USER 없는 수신자
				EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_GOODS	네트워크 에러
				EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_USER	네트워크 에러
				EVENT_ERROR_EVENT_CHEKC_GIFT_PARAMETER_GOODS	없는 상품
				EVENT_ERROR_FAIL	네트워크 에러*/

				i3::stack_string strErrorLog;
				i3::snprintf(strErrorLog, MAX_PATH, "AGF[%d] : %x ", i, res);
				I3PRINTLOG(I3LOG_QA, strErrorLog);
				ack_result = false;
			}
		}

		i3::rc_wstring wstr_result;
		i3::rc_wstring wstr_caption;

		if (ack_result)
		{
			i3::sprintf(wstr_result, L"[%s]%s", buy::g_State.giftInfo.wstr_receiver_nick, GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_SUCCESS"));
			wstr_caption = GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_SUCCESS_CAPTION");
		}
		else
		{
			wstr_result = GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_FAIL_E");
			wstr_caption = GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_FAIL_CAPTION");
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_result, &wstr_caption);

		return ack_result;
	}
}

void PurchaseMgr::ReceiveEvent_Send_Gift( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;

	const RecvData_EVENT_SEND_GIFT& data = i3::value_cast<RecvData_EVENT_SEND_GIFT>(UserData);
	
	bool ack_result = CheckGiftAck_Imp(data.resultList);

	buy::detail::EndEvent();

	UISignal::FinishBuyGift(ack_result);
	
}

void	PurchaseMgr::OnUpdate( REAL32 rDeltaSeconds)
{
	buy::g_State.sec_count.Update();
}


namespace
{
	// 이런 함수는 그냥 헤더에 일반함수로 노출시켜도 될듯..?

	static bool IsPeriodGoods_Imp(const SHOP_GOODS* goods)
	{
		SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData(goods->_GoodsID);
		if (!pGoodsList) return false;

		UINT8 Type = 0; UINT32 Arg = 0;
		INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(goods->_ItemID);
		if (CInvenList::i()->GetItemTypeByItemID(dbType, goods->_ItemID, &Type, &Arg) == false) return false;

		if (pGoodsList->_ui8ItemCount > 1) return false;	//세트 아이템이다.

		if (pGoodsList->_ui8ItemCount == 1 && pGoodsList->_ppItemData[0]->_ui8AuthType == ITEM_TYPE_TIME)
			return true;

		return false;
	}

	static bool CheckPeriod_Imp()
	{
		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (IsPeriodGoods_Imp(pGoods))
			{
				return true;
			}
		}
		return false;
	}

	static bool IsDuplicablePurchaseGoods_Imp(const SHOP_GOODS* goods)
	{
		UINT8 Type = 0; UINT32 Arg = 0;
		INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(goods->_ItemID);

		if (item_def::get_category_goodsID(goods->_GoodsID) == item_def::CTG_PARTS)	//parts는 중복 구매 가능
		{
			if (goods->_AuthType < 1 || goods->_AuthType > 2)			// 파츠아이템의 경우 _AuthType이 기간제1, 개수제2 일경우만 중복구매가능
				return false;
		}

		if (g_pEnvSet->IsV2Version())
		{
			if (item_def::get_category_goodsID(goods->_GoodsID) == item_def::CTG_CHARA && //캐릭터 중복 구매 가능
				item_def::get_type(goods->_ItemID) != ITEM_TYPE_DINO)
			{
				if (goods->_AuthType < 1 || goods->_AuthType > 2)			// 캐릭터아이템의 경우 _AuthType이 기간제1, 개수제2 일경우만 중복구매가능
					return false;
			}
		}

		return CInvenList::i()->GetItemTypeByItemID(dbType, goods->_ItemID, &Type, &Arg);	//보유중이면 True
	}

	static bool CheckDuplicablePurchase_Imp()
	{
		size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (IsDuplicablePurchaseGoods_Imp(pGoods))
				return true;
		}
		return false;
	}
	
	static bool CheckMoney_Imp()
	{
		UINT32 tot_point = 0, tot_cash = 0;
		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			GOODS_PRICE_INFO info;
			usf::GetShopGoodInfo(pGoods, info);

			if (pDisplayedGoods->GetCurrentPriceType() == GOODS_PRICE_CASH)
				tot_cash += info._Cash;
			else
				tot_point += info._Point;
		}

		UINT32 nHaveCash = UserInfoContext::i()->GetMyCash();
		UINT32 nHavePoint = UserInfoContext::i()->GetMyPoint();

		if (tot_point == 0) return (INT32(nHaveCash) - INT32(tot_cash)) >= 0 ? true : false;
		if (tot_cash == 0) return (INT32(nHavePoint) - INT32(tot_point)) >= 0 ? true : false;

		return ((INT32(nHavePoint) - INT32(tot_point)) >= 0 && (INT32(nHaveCash) - INT32(tot_cash)) >= 0);
	}

	static bool CheckClanMaxPerson_Imp()
	{
		const UINT8 PLUS_10 = 10; const UINT8 PLUS_50 = 50;
		UINT8 plus_person = 0;

		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (item_def::get_category_goodsID(pGoods->_GoodsID) == item_def::CTG_ITEM)
			{
				if (pGoods->_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10 || pGoods->_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
				{
					CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();

					if (pGoods->_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10) plus_person = PLUS_10;
					else plus_person = PLUS_50;

					//	클랜원증가아이템은최대회원수를넘을시사용불가
					if (pClanInfo->m_maxPerson + plus_person > MAX_MEMBER_COUNT)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	static bool CheckAnnounceUseOnlyClanMaster_Imp()
	{
		size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (item_def::get_category_goodsID(pGoods->_GoodsID) == item_def::CTG_ITEM)
			{
				if (pGoods->_ItemID == CASHITEM_ITEMID_CLR_CLAN_HISTORY)
					return true;
			}
		}
		return false;
	}

	static void 	UpdatePointAndCashTryPurchase()
	{
		buy::g_State.cartPurchaseResultInfo.resultList.clear();
		buy::g_State.cartPurchaseResultInfo.GoodsIDList.clear();
		

		bool isPointPurchase = false;
		bool isCashPurchase = false;
		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			GOODS_PRICE_TYPE priceType = pDisplayedGoods->GetCurrentPriceType();

			if (priceType == GOODS_PRICE_POINT)
				isPointPurchase = true;
			else
			if (priceType == GOODS_PRICE_CASH)
				isCashPurchase = true;
		}

		if (isPointPurchase && isCashPurchase)
		{
			buy::g_State.is_point_and_cash_try_purchase = true;
		}
	}


}

namespace MsgBoxCB
{

	static void BuyMoreCB(void*, INT32 param)
	{
		if (param == MBR_OK)
		{
			buy::detail::StartEvent();
			buy::g_State.purchasedInvenItemList.clear();
			buy::g_State.addInvenBuffer = buy::detail::AddInvenBuffer;

			// Set 아이템인지 체크합니다.
			const buy::DisplayedGoodsPtr& displayedGoods = buy::g_State.purchaseGoodsList[0];

			const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();

			if (GET_GOODS_TYPE(pGoods->_GoodsID) == ITEM_TYPE_SET)
			{
				// Set Item일 경우에는 Buy Goods로만 보냅니다.

				UpdatePointAndCashTryPurchase();

				GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
			}
			else
			{
				T_ItemID item_id = pGoods->_ItemID;

				if (item_def::is_chara_category(item_id) == true)
				{
					GameEventSender::i()->SetEvent(EVENT_BUY_CHAR_GOODS);
				}
				else
				{
					UpdatePointAndCashTryPurchase();
					GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
				}
			}
		}
	}

}

namespace
{

	static void BuyItemPeriodExtension_Imp(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList)
	{
		buy::g_State.currUI = pUIBase;
		buy::g_State.purchaseRoute = routeType;
		buy::g_State.purchaseGoodsList = goodsList;
		buy::g_State.is_point_and_cash_try_purchase = false;

		buy::detail::StartEvent();
		buy::g_State.purchasedInvenItemList.clear();
		buy::g_State.addInvenBuffer = buy::detail::AddInvenBuffer;

		GameEventSender::i()->SetEvent(EVENT_BUY_EXTEND_GOODS);
	}

	static void Buy_Imp(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList)
	{
		buy::g_State.currUI = pUIBase;
		buy::g_State.purchaseRoute = routeType;
		buy::g_State.purchaseGoodsList = goodsList;
		buy::g_State.is_point_and_cash_try_purchase = false;

		if (CheckMoney_Imp() == false)
		{
			/*잔액이 부족하여 아이템을 구입할 수 없습니다.*/
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_BASE_NO_POINT_TO_GET_ITEM"));
			return;
		}

		// 클랜 제한 인원이 넘을 시 구매 불가능하도록
		if (CheckClanMaxPerson_Imp())
		{
			/*아이템을 사용할 수 없습니다.\n \n 클랜 최대 인원수는\n {col:255,153,51,255}%d{/col}명으로 제한되어 있습니다.*/
			i3::rc_wstring wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			return;
		}

		if (CheckPeriod_Imp())	//기간 연장
		{
			/*보유중인 아이템은 구입 시\n아이템의 기간 및 수량이 증가합니다.\n \n구입 하시겠습니까?*//*구입확인*/
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,
				GAME_RCSTRING("STR_POPUP_Q_MESSAGE_BUY_SET_GOODS"),
				&GAME_RCSTRING("STR_POPUP_CAPTION_CONFIRM_BUY"),
				MAKE_CALLBACK(MsgBoxCB::BuyMoreCB), PurchaseMgr::i() );
			return;
		}

		if (CheckDuplicablePurchase_Imp())		//중복 구매
		{
			/*보유중인 아이템은 구입 시\n아이템의 기간 및 수량이 증가합니다.\n \n구입 하시겠습니까?*//*구입확인*/
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,
				GAME_RCSTRING("STR2_SHOP_DUPLICATED_BUYING"),
				&GAME_RCSTRING("STR_POPUP_CAPTION_CONFIRM_BUY"),
				MAKE_CALLBACK(MsgBoxCB::BuyMoreCB), PurchaseMgr::i() );
			return;
		}

		if (CheckAnnounceUseOnlyClanMaster_Imp())	//클랜 마스터만 사용할 수 있는 아이템 알림
		{
			/*클랜 마스터만 사용이 가능한 아이템입니다\n 구매 하시겠습니까?*//*구입확인*/
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,
				GAME_RCSTRING("STR_SHOP_ITEM_BUYING_CLANMASTER"),
				&GAME_RCSTRING("STR_POPUP_CAPTION_CONFIRM_BUY"),
				MAKE_CALLBACK(MsgBoxCB::BuyMoreCB), PurchaseMgr::i());
			return;
		}

		buy::detail::StartEvent();
		buy::g_State.purchasedInvenItemList.clear();
		buy::g_State.addInvenBuffer = buy::detail::AddInvenBuffer;

		// Set 아이템인지 체크합니다.
		const buy::DisplayedGoodsPtr& displayedGoods = buy::g_State.purchaseGoodsList[0];

		const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();

		if (GET_GOODS_TYPE(pGoods->_GoodsID) == ITEM_TYPE_SET)
		{
			// Set Item일 경우에는 Buy Goods로만 보냅니다.
			UpdatePointAndCashTryPurchase();

			GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
		}
		else
		{
			T_ItemID item_id = pGoods->_ItemID;

			if (item_def::is_chara_category(item_id) == true)
			{
				GameEventSender::i()->SetEvent(EVENT_BUY_CHAR_GOODS);
			}
			else
			{
				// 구매 패킷을 보내기 전에 복수 구매시 포인트와 캐시를 동시에 써야하는지 알아야 한다..
				UpdatePointAndCashTryPurchase();

				GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
			}
		}
	}
	


}


void	PurchaseMgr::Buy(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList)
{
	if (buy::g_State.working == true) return;

	if (routeType == buy::PurchaseRoute_Extend)
		BuyItemPeriodExtension_Imp(pUIBase, routeType, goodsList);
	else
		Buy_Imp(pUIBase, routeType, goodsList);
}


void PurchaseMgr::Gift(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList,
	const i3::rc_wstring& wstr_nick, const i3::rc_wstring& wstr_msg)
{
	if(buy::g_State.working == true ) return;

	buy::g_State.currUI = pUIBase;
	buy::g_State.purchaseRoute = routeType;
	buy::g_State.purchaseGoodsList = goodsList;
	buy::g_State.giftInfo.wstr_receiver_nick = wstr_nick;
	buy::g_State.giftInfo.wstr_gift_message = wstr_msg;
	buy::g_State.is_point_and_cash_try_purchase = false;

	if(CheckMoney_Imp() == false )
	{
		/*잔액이 부족하여 물품을 선물할 수 없습니다.*/
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_GIFT") );
		return;
	}

	buy::detail::StartEvent();
	GameEventSender::i()->SetEvent(EVENT_SEND_GIFT);
}

namespace MsgBoxCB
{
	static void AuthCommonCB_Imp(void* arg, INT32 nParam)
	{
		ItemConsumer::instance()->m_ItemID = 0; //ItemConsumer은 process_game_event 함수로 처리하기 때문에 Item(category)이 아닐경우를 판단하기 위해

		if (nParam == MBR_OK)
		{
			buy::detail::StartEvent();
			GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &buy::g_State.selectedItemInfo.ware_idx, &buy::g_State.selectedItemInfo.item_id);
		}
	}

	static void AuthInvenCB(void* arg, INT32 nParam)
	{
		AuthCommonCB_Imp(arg, nParam);
	}

}

//인벤 인증
void PurchaseMgr::Auth_Inven(UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx) //인벤용
{
	if( buy::g_State.working == true ) return;
		
	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = 0;					// 상점 제외하면 항상 널값이구만...
	buy::g_State.selectedItemInfo.item_id = item_id;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;
		
	UISignal::CurrentFinishAuthItem = UISignal::FinishAuthItem; //	SelectFinishAuthItem_Normal();

	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = 1;		//1개 인증이다.
	
	item_def::Category ctg = item_def::get_category( item_id );
	if( item_def::get_type(item_id) == ITEM_TYPE_DINO )
		ctg = item_def::CTG_WEAPON;
	
	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
			// vv3 - check
			bool ignore_authenticate = true;
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(static_cast<TEAM_TYPE>(idx)); 
				int chara_itemid = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
				ignore_authenticate &= (item_def::is_shared_parts_item(item_id, chara_itemid) == false);
			}

			if( ignore_authenticate )
				return;
		}		
	case item_def::CTG_WEAPON:
	case item_def::CTG_CHARA:
		{
			if( CharaInfoMgr::instance()->is_chara_equipped(item_id, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) == false) //장비중이 아니면
			{
				/*사용을 시작하지 않은 아이템입니다.\n장비 시부터 남은 기간만큼 사용이 가능합니다.\n장비하시겠습니까?*/
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_INVEN_STARTING_AUTH"), nullptr, MAKE_CALLBACK(MsgBoxCB::AuthInvenCB), this);
				return;
			}
		}
		break;
	case item_def::CTG_ITEM:
		{
			bool bMaster = g_pFramework->GetClanContext()->IsMaster() ? true:false;

			// 사용여부
			UINT8 itemAttr = 0; UINT32 itemArg = 0;
			if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM,  item_id, &itemAttr, &itemArg ) )
			{
				if( ITEM_ATTR_TYPE_BUY == itemAttr ||
					ITEM_ATTR_TYPE_AUTH < itemAttr)	//이벤트 아이템일 경우에는 ItemID를 Shop에서 얻을 수 없기 때문에 바로 장비버튼을 활성화 처리합니다.
				{
					ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(item_id);
					if( pInfo == nullptr) return;	//	Goods.pef에 등록되지 않은 아이템

					if ( pInfo->GetPurchaseAuthority() != PURCHASE_AUTH_ANYONE )
					{
						if ( pInfo->GetPurchaseAuthority() == PURCHASE_AUTH_CLAN_MASTER && !bMaster )
						{
							// 사용할 수 없는 메시지
							GameUI::MsgBox( MSG_TYPE_GAME_OK, pInfo->GetCertificationAuthorityDesc(), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));	//아이템 사용 불가
							return;
						}
					}

					if ( pInfo->GetUsePosition() != USE_POS_ANYWHERE )
					{
							// 사용할 수 있는 위치 메시지
						
						GameUI::MsgBox( MSG_TYPE_GAME_OK, pInfo->GetUsePositionDesc(), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));	//아이템 사용 불가
						return;
					}

					//	아이템 별로 사용시 예외z처리
					if( GET_ITEM_TYPE( item_id ) == ITEM_TYPE_WRAP_COUNT)		//	소모성
					{
						CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
						switch( item_id )
						{
						case CASHITEM_ITEMID_CLAN_PERSON_PLUS_10:
						case CASHITEM_ITEMID_CLAN_PERSON_PLUS_50:
							{
								//	클랜원 증가 아이템은 최대 회원수를 넘을시 사용 불가
								if( pClanInfo->m_maxPerson >= MAX_MEMBER_COUNT)
								{
									/*아이템을 사용할 수 없습니다.\n \n 클랜 최대 인원수는\n {col:255,153,51,255}%d{/col}명으로 제한되어 있습니다.*/
									GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));/*아이템 사용 불가*/
									return;
								}
							}
							break;
						case CASHITEM_ITEMID_CHANGE_NICK:		//	닉네임 변경
							{
								if(CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_DISGUISE_NICK) == true)
								{
									/*임시 닉네임 적용 중에는 닉네임 변경권을 사용할 수 없습니다.\n임시 닉네임 아이템 삭제 후 닉네임을 변경하시기 바랍니다.*/
									GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_MODIFY_NICK_WITHOUT_DISUISE_NICK"), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));
									return;
								}
							}
							break;
						}
					}
				}
			}
		}
		break;
	}
}

namespace MsgBoxCB
{
	
	static void AuthShopCB_ShopItemCategory()
	{
		CShop* shop = CShop::i();
		if (shop->IsPackageGoods(buy::g_State.selectedItemInfo.goods->_GoodsID))
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/

			return;
		}

		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(buy::g_State.selectedItemInfo.item_id);
		if (pInfo != NULL && pInfo->GetUsePosition() == USE_POS_ANYWHERE)
		{
			//아이템을 사용할지 질문
			ITEM_QUERY_STYLE style = IQS_USE;

			UIBase* ui_base = buy::g_State.currUI;

			UIFramework * pParent = i3::same_cast<UIFramework*>(ui_base->getParent());
			I3ASSERT(pParent != NULL);

			if (buy::g_State.selectedItemInfo.item_id == CASHITEM_ITEMID_MAIN_SKILL_ALLOPEN)
				pParent->GetUIMainframe()->OpenPopup(UPW_SKILLOPEN_ITEM, &buy::g_State.selectedItemInfo.item_id, &style);
			else
				pParent->GetUIMainframe()->OpenPopup(UPW_QUERY_ITEM, &buy::g_State.selectedItemInfo.item_id, &style);
		}
		else
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
		}

	}

	static void AuthShopCB(void* arg, INT32 nParam)
	{
		ItemConsumer::instance()->m_ItemID = 0; //ItemConsumer은 process_game_event 함수로 처리하기 때문에 Item(category)이 아닐경우를 판단하기 위해

		if (nParam == MBR_OK)
		{
			switch (item_def::get_category_goodsID(buy::g_State.selectedItemInfo.goods->_GoodsID))
			{
			case item_def::CTG_CHARA:
			case item_def::CTG_WEAPON:
			case item_def::CTG_PARTS:
			case item_def::CTG_SKIN:
				MsgBoxCB::AuthCommonCB_Imp(0, MBR_OK); //auth_inven 사용해도 무방함 ( 이렇게 네이밍구성해서 처리하지 말것...무방한게 아니라..가독성이 떨어진다..)
				break;

			case item_def::CTG_ITEM:
				MsgBoxCB::AuthShopCB_ShopItemCategory();
				break;
			}
		}
		else
		{
			switch (item_def::get_category_goodsID(buy::g_State.selectedItemInfo.goods->_GoodsID))
			{
			case item_def::CTG_WEAPON:
			case item_def::CTG_PARTS:
				/*구입하신 아이템은 인벤토리에서\n확인하실 수 있으며 구입후 한번도\n장비하지 않은 미사용 무기는\n게임 도중에 교체할 수 없습니다.*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_ITEM_BUY_COMPLETE_1"),
					&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				break;

			case item_def::CTG_CHARA:
			case item_def::CTG_ITEM:
			case item_def::CTG_SKIN:
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
					&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				break;
			}
		}
	}


	static void EquipShopCB(void* arg, INT32 nParam)
	{
		//	아이템을 인벤에 장비
		if (nParam != MBR_OK)
		{
			/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
		}

		buy::detail::EndEvent();

		if (nParam == MBR_OK)
		{
			UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
		}
	}
	
}

T_ItemDBIdx PurchaseMgr::GetTargetItemDBIndex() const
{
	return buy::g_State.selectedItemInfo.ware_idx;
}

const i3::vector<INVEN_BUFFER*>&	PurchaseMgr::GetPurchasedInvenBufferList() const
{
	return buy::g_State.purchasedInvenItemList;
}

const buy::DisplayedGoodsList&		PurchaseMgr::GetPurchaseGoodsList() const
{
	return buy::g_State.purchaseGoodsList;
}

const buy::GiftInfo&		PurchaseMgr::GetGiftInfo() const
{
	return buy::g_State.giftInfo;
}

//상점 인증
void PurchaseMgr::Auth_Shop( UIBase* pUIBase, const SHOP_GOODS* goods, bool isExtensionPopup)
{
	if( buy::g_State.working == true ) return;

	INT32	itemid	= goods->_ItemID;

	T_ItemDBIdx ware_idx = INVALID_WAREDB_INDEX;
	if (buy::g_State.purchasedInvenItemList.empty() == false)
		ware_idx = buy::g_State.purchasedInvenItemList.back()->_TItemWareDBIdx;
	
	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = goods;
	buy::g_State.selectedItemInfo.item_id = itemid;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;
	
	UISignal::CurrentFinishAuthItem = (isExtensionPopup) ? UISignal::FinishExtensionBuyAuthItem : UISignal::FinishAuthItem;

	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = 1;		//1개 인증이다.


	item_def::Category ctg = item_def::get_category_goodsID( goods->_GoodsID );
	if( item_def::get_type(itemid) == ITEM_TYPE_DINO )	//공룡 인증 방식을 무기 처럼 하면 될 듯
		ctg = item_def::CTG_WEAPON;

	

	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			// vv3 - check
			bool ignore_authenticate = true;
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(static_cast<TEAM_TYPE>(idx)); 
				int chara_itemid = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);				
				ignore_authenticate &= (item_def::is_shared_parts_item(itemid, chara_itemid) == false);
			}

			if( ignore_authenticate )
				break;
		}						// 여기에 break없이 구현된 코드는 어거지이다...주석으로 주의하는 것만 갖고는 안되고...이런 코드를 짜면 안될듯하다..
	case item_def::CTG_CHARA:
	case item_def::CTG_WEAPON: 
		{
			bool bWearable = true;

			bWearable = DesigUtil::CheckWearable_MyItemByDesig(itemid);

			if(bWearable && CharaInfoMgr::instance()->is_chara_equipped(itemid, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) == false) //장비중이 아니면
			{
				const i3::rc_wstring& wstrMessage = GAME_RCSTRING("STR_POPUP_Q_MESSAGE_EQUIP");	/*바로 장비 하시겠습니까?*/
				SHOP_ITEM_DATA* pItem = CShop::i()->GetItemData( itemid );
				switch( pItem->_ui8AuthType )
				{
				case ITEM_TYPE_COUNT: case ITEM_TYPE_ETERNITY:	// 개수제, 영구제
					GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(MsgBoxCB::EquipShopCB), this);
					return;
					break;
				case ITEM_TYPE_TIME:	// 기간제
					{
						UINT8 itemAttr = 0;
						UINT32 itemArg = 0;
						inven::GetItemTypeArg(itemid, ware_idx, &itemAttr, &itemArg);
						if(ITEM_ATTR_TYPE_BUY == itemAttr)
						{
							if(	buy::g_State.selectedItemInfo.ware_idx != INVALID_WAREDB_INDEX )
							{
								GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(MsgBoxCB::AuthShopCB), this);
								return;
							}
						}
						else if( ctg == item_def::CTG_WEAPON )
						{
							GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(MsgBoxCB::EquipShopCB), this);
							return;
						}
						else if( ctg == item_def::CTG_CHARA )
						{
							const i3::rc_wstring& wstrMessage2 = GAME_RCSTRING("STR2_Q_ENABLE_MAIN_CHARA");	/*메인 캐릭터로 변경하시겠습니까?*/
							GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage2, nullptr, MAKE_CALLBACK(MsgBoxCB::EquipShopCB), this);
							return;
						}
					}
					break;
				}
			}
		} 
		break; 
	case item_def::CTG_ITEM:
		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemid);

			if( pInfo != nullptr && pInfo->GetUsePosition() != USE_POS_ANYWHERE ) break; //특정 위치에서 사용 가능한 아이템이면 인증하지 않는다.
			if( item_def::is_direct_use_item(itemid) == false ) break;

			INVEN::DB_TYPE InvenItemType = CInvenList::i()->FindInvenDBTypeByDBIdx(ware_idx);
			if( InvenItemType == INVEN::DB_CASHITEM)
			{
				MsgBoxCB::AuthShopCB( this, MBR_OK);	//	바로 사용여부를 물어본다.
				return;
			}
		}
		break;
	}

	//구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
		&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
}

//인게임 상점용 - 상점용과 비슷하지만, 사용자에게 사용 여부를 묻지 않고, 서버와 통신한다.
void PurchaseMgr::Auth_InGame( UIBase* pUIBase, const SHOP_GOODS* goods)
{
	if( buy::g_State.working == true ) return;

	INT32	itemid	= goods->_ItemID;
	T_ItemDBIdx ware_idx = INVALID_WAREDB_INDEX;
	if (buy::g_State.purchasedInvenItemList.empty() == false)
		ware_idx = buy::g_State.purchasedInvenItemList.back()->_TItemWareDBIdx;
	
	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = goods;
	buy::g_State.selectedItemInfo.item_id = itemid;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;
		
	UISignal::CurrentFinishAuthItem = UISignal::FinishAuthItem;

	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = 1;		//1개 인증이다.


	item_def::Category ctg = item_def::get_category( itemid );
	if( item_def::get_type(itemid) == ITEM_TYPE_DINO )	//공룡 인증 방식을 무기 처럼 하면 될 듯
		ctg = item_def::CTG_WEAPON;

	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			// vv3 - check
			bool ignore_authenticate = true;
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(static_cast<TEAM_TYPE>(idx)); 
				int chara_itemid = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);				
				ignore_authenticate &= (item_def::is_shared_parts_item(itemid, chara_itemid) == false);
			}

			if( ignore_authenticate )
				break;
		}
	case item_def::CTG_WEAPON: 
		{
			if( CharaInfoMgr::instance()->is_chara_equipped(itemid, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) == false) //장비중이 아니면
			{
				SHOP_ITEM_DATA* pItem = CShop::i()->GetItemData( itemid );
				switch( pItem->_ui8AuthType )
				{
				case ITEM_TYPE_COUNT:	// 개수제
					{
						//인증이 필요없는 아이템이다.
						buy::detail::EndEvent();
						UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
					}
					return;
					break;
				case ITEM_TYPE_TIME:	// 기간제
					{
						UINT8 itemAttr = 0; UINT32 itemArg = 0;
						inven::GetItemTypeArg(itemid, ware_idx, &itemAttr, &itemArg);
						if(ITEM_ATTR_TYPE_BUY == itemAttr)
						{
							if(buy::g_State.selectedItemInfo.ware_idx != INVALID_WAREDB_INDEX )
							{
								//인증이 필요한 아이템이다.
								ItemConsumer::instance()->m_ItemID = 0;
								buy::detail::StartEvent();
								GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, 
									&buy::g_State.selectedItemInfo.ware_idx, &buy::g_State.selectedItemInfo.item_id);
								return;
							}
						}
						else
						{
							buy::detail::EndEvent();
							UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
							return;
						}
					}
					break;
				}
			}
		} 
		break; 
	case item_def::CTG_CHARA:
		{
			//인증이 필요없는 아이템이다.
			buy::detail::EndEvent();
			UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
			return;
		}
		break; 

	case item_def::CTG_ITEM:
		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemid);

			if( pInfo != nullptr && pInfo->GetUsePosition() != USE_POS_ANYWHERE ) break; //특정 위치에서 사용 가능한 아이템이면 인증하지 않는다.
			if( item_def::is_direct_use_item(itemid) == false ) break;

			INVEN::DB_TYPE InvenItemType = CInvenList::i()->FindInvenDBTypeByDBIdx(ware_idx);
			if( InvenItemType == INVEN::DB_CASHITEM)
			{
				//인증이 필요한 아이템이다.
				buy::detail::StartEvent();
				GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &buy::g_State.selectedItemInfo.ware_idx, &buy::g_State.selectedItemInfo.item_id);
				return;
			}
		}
		break;
	}

	g_pFramework->PutSystemChatting( GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS") ); /*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
}

//상점 - preview 용

namespace MsgBoxCB
{
	static void AuthShopPreviewCB(void* arg, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			buy::detail::StartEvent();
			SendAuthPacket_Imp(0);				// 시작..
		}
		else
		{
			//구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
		}
	}
}

void PurchaseMgr::Auth_Preview( UIBase* pUIBase)
{
	if( buy::g_State.working == true ) return;

	const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = buy::g_State.purchasedInvenItemList;
	
	buy::g_State.currUI = pUIBase;
	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = catched_inven_buffer_list.size();		//여러 개 인증이다.
	
	UISignal::CurrentFinishAuthItem = UISignal::FinishPreviewAuthItem;

	/*바로 장비 하시겠습니까?*/
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_EQUIP"), 
		nullptr, MAKE_CALLBACK(&MsgBoxCB::AuthShopPreviewCB), this);
}


namespace MsgBoxCB
{

	static void RemoveCB(void* arg, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			bool chara_category = item_def::is_chara_category(buy::g_State.selectedItemInfo.item_id);

			buy::detail::StartEvent();
			if (chara_category == true)
			{
				//INT32 main_chara_slot_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
				T_ItemID itemid_for_searching = buy::g_State.selectedItemInfo.item_id;
				INT32 idx = CCharaInfoContext::i()->FindCharaInfoIndexByItemID(itemid_for_searching);
				if (idx != -1)
				{
					const CHARA_INFO* info = CCharaInfoContext::i()->GetCharaInfo(idx);
					GameEventSender::i()->SetEvent(EVENT_DELETE_CHARA, &buy::g_State.selectedItemInfo.ware_idx, &info->m_CharaBasicInfo.m_ui8CharaSlotIdx);
				}
			}
			else
			{
				GameEventSender::i()->SetEvent(EVENT_AUTH_DELETE, &buy::g_State.selectedItemInfo.ware_idx);
			}
		}
	}

}

void PurchaseMgr::Remove( UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx) //인벤용
{
	if( buy::g_State.working == true ) return;

	if( INVALID_WAREDB_INDEX == ware_idx )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REMOVE_REQ_FAIL"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = 0;
	buy::g_State.selectedItemInfo.item_id = item_id;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;

	ItemInfoConvert item_info_convert(item_id);

	//기본 무기/파츠인지 판단한다.
	switch( item_info_convert.get_category() )
	{
	case item_def::CTG_WEAPON:	
		if( item_info_convert.is_default_item() )
		{
			/*기본 지급 무기 입니다.\n삭제할 수 없습니다.*/	/*아이템 삭제*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_DEFAULT_WEAPON"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
			return;
		}
		break;

	case item_def::CTG_PARTS:
		if( item_info_convert.is_default_item() )
		{
			/*기본 지급 파츠 입니다.\n삭제할 수 없습니다.*/	/*아이템 삭제*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_DEFAULT_PARTS"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
			return;
		}
		break;
	
	case item_def::CTG_CHARA:
		//캐릭터는 1개 이하로 삭제 할 수 없음
		if( item_def::is_chara_category(item_id) )
		{
			if( CCharaInfoContext::i()->GetHasCharacterCount() <= 1 )
			{
				/*더 이상 캐릭터를 삭제할 수 없습니다.*/	/*아이템 삭제*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_CHARA"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
				return;
			}
		}
		else
		{
			//2014-10-07 버그 10213 해결을 위하여 공룡 부분만 is_default_item_to_DBidx 함수로 대체 합니다. ENG_김대영
			if( item_info_convert.is_default_item_to_DBidx(ware_idx) )
			{
				/*기본 지급 공룡 입니다.\n삭제할 수 없습니다.*/	/*아이템 삭제*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_DEFAULT_DINO"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
				return;
			}
		}
		break;

	case item_def::CTG_ITEM:
		if( usf::GetItemStatus( item_id, ware_idx  ) == ITEM_STATUS_USED )

		{
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_INVEN_MESSAGE_DELETE_ITEM"), nullptr, MAKE_CALLBACK(MsgBoxCB::RemoveCB), this);
			return;
		}
		break;

	case item_def::CTG_SKIN :
		break;
	}

	MsgBoxCB::RemoveCB(0, MBR_OK); //바로 삭제한다.
}


namespace MsgBoxCB
{
	static void RepairCB(void* arg, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			buy::g_State.authResultInfo.result_list.clear();
			buy::g_State.authResultInfo.request_count = 1;			

			UISignal::CurrentFinishRepairItem = UISignal::FinishRepairItem;

			buy::detail::StartEvent();
			GameEventSender::i()->SetEvent(EVENT_ITEM_REPAIR, &buy::g_State.selectedItemInfo.ware_idx);
		}

	}
}

void PurchaseMgr::Repair( UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx)
{
	if( buy::g_State.working == true ) return;

	if( INVALID_WAREDB_INDEX == buy::g_State.selectedItemInfo.ware_idx )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_REQ_FAIL"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = 0;
	buy::g_State.selectedItemInfo.item_id = item_id;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;

	// 수리할 수 있는지 확인한다.
	REPAIR_TYPE	Rv = CShopContext::i()->CheckCanRepair(item_id);
	if ( Rv == REPAIR_TYPE_UNKOWN || Rv == REPAIR_TYPE_CANNOT )
	{
		/*수리를 할 수 없습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_REQ_FAIL"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	if ( Rv == REPAIR_TYPE_CAN_SOME )
	{
		/*소지 금액이 부족하여 수리할 수 없습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_3"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	UINT32 Price = 0;
	GOODS_PRICE_TYPE	PriceType	= GOODS_PRICE_UNKNOWN;
	CShopContext::i()->GetRepairPrice(item_id, Price, PriceType);

	i3::rc_wstring wstr;

	if( item_def::is_dino_category(item_id) == false )
	{
		//수리 비용 %d(%s)\n수리하시겠습니까?
		i3::sprintf(wstr, L"%s %d(%s)\n%s", GAME_RCSTRING("STR_POPUP_REPAIR_PRICE"), Price,
			(PriceType == GOODS_PRICE_CASH) ? GAME_RCSTRING("STR_SHOP_INFO_CASH") : GAME_RCSTRING("STR_SHOP_INFO_POINT"),
			GAME_RCSTRING("STR_POPUP_REPAIR_1") );
	}
	else 
	{
		//체력 획복 비용 %d(%s)\n회복하시겠습니까?
		i3::sprintf(wstr, L"%s %d(%s)\n%s", 	GAME_RCSTRING("STR_POPUP_RECOVER_PRICE"), Price,
			(PriceType == GOODS_PRICE_CASH) ? GAME_RCSTRING("STR_SHOP_INFO_CASH") : GAME_RCSTRING("STR_SHOP_INFO_POINT"),
			GAME_RCSTRING("STR_POPUP_REPAIR_5") );
	}

	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstr, &GAME_RCSTRING("STR_POPUP_REPAIR_CAPTION"), MAKE_CALLBACK(MsgBoxCB::RepairCB), this);
}

// 두번째 인수는 원래 필요로 하지 않아야 한다..
void PurchaseMgr::RepairAll( UIBase* pUIBase) //현재 캐릭터 장착 장비에 대해서 모두 수리(캐릭터 뷰용)
{
	if( buy::g_State.working == true ) return;

	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = nullptr;
	buy::g_State.selectedItemInfo.item_id = 0;
	buy::g_State.selectedItemInfo.ware_idx = 0;				// 초기화를 통해서 1개 수리와 구별하도록 일단 처리해둡니다..  (좋은 방법이 아닐텐데...)
		
	UISignal::CurrentFinishRepairItem = UISignal::FinishRepairAllItem;   // 이걸로 구별해도 되긴 되는데...

	buy::detail::StartEvent();

	// RepairAll에 대한 패킷 보내기 처리는 이곳에 있는게 일관성 있으므로 UIFloatCharacterView에서 이쪽으로 옮깁니다...
	UINT32 chara_idx = static_cast<UINT32>(CCharaInfoContext::i()->GetMainCharaSlotIdx());
	GameEventSender::i()->SetEvent(EVENT_REPAIR_EQUIPMENT, &chara_idx);
	
}

bool PurchaseMgr::IsAllRepairPossible(AllRepairInfo* outInfo)
{
	UINT32 total_point_cost = 0;
	UINT32 total_cash_cost = 0;
	INT32 repair_item_count = 0;

	GOODS_PRICE_TYPE	PriceType = GOODS_PRICE_UNKNOWN;
	UINT32 Price;

	UINT32 chara_idx = (UINT32)CCharaInfoContext::i()->GetMainCharaSlotIdx();
	for (INT32 i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++)
	{
		ITEM_INFO info = CharaInfoMgr::instance()->find(chara_idx, item_def::SCTG_WEAPON, i);
		if (info.m_TItemID == 0) continue;

		REAL32	durability_ratio = 0.0f;
		if (usf::GetDurability(info.m_TItemID, info.m_TItemDBIdx, durability_ratio) == false) continue; //내구도 아이템이 아니다.
		if (durability_ratio == 1.f) continue; //수리 할 필요가 없다.

		if (CShopContext::i()->GetRepairPrice(info.m_TItemID, Price, PriceType) == false) continue;

		if (PriceType == GOODS_PRICE_POINT)
			total_point_cost += Price;
		else if (PriceType == GOODS_PRICE_CASH)
			total_cash_cost += Price;

		++repair_item_count;
	}

	if (outInfo != NULL)
	{
		outInfo->total_point_cost = total_point_cost;
		outInfo->total_cash_cost = total_cash_cost;
		outInfo->repair_item_count = repair_item_count;
	}

	if (total_point_cost > UserInfoContext::i()->GetMyPoint()) return false; //돈이 없다.
	if (total_cash_cost > UserInfoContext::i()->GetMyCash()) return false; //돈이 없다.

	return true;
}

