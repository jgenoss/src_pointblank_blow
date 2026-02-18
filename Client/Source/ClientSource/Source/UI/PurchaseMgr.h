#pragma once

#include "UISecCount.h"
#include "UIDefine.h"


class UIBase;

namespace buy 
{	
	struct GiftInfo
	{
		i3::rc_wstring wstr_receiver_nick;
		i3::rc_wstring wstr_gift_message;
	};
	
	enum PurchaseRouteType; 
	class DisplayedGoods;
	typedef i3::shared_ptr<DisplayedGoods>			DisplayedGoodsPtr;
	typedef i3::vector< buy::DisplayedGoodsPtr >	DisplayedGoodsList;
}

//예) buy -> SetEvent -> cb_receive_buy_packet -> detail::param_util::call_finish_cb_string
// 위와 같은 예를 꼭 알아야 될 필요가 없다.

struct AllRepairInfo
{
	INT32 total_point_cost;
	INT32 total_cash_cost;
	INT32 repair_item_count;
};

class PurchaseMgr
{

public:
	void Buy(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList);
	void Gift(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList,
		const i3::rc_wstring& wstr_nick, const i3::rc_wstring& wstr_msg);		// 선물하기

	void Auth_Preview(UIBase* pUIBase); //상점 - preview 용
	void Auth_InGame(UIBase* pUIBase, const SHOP_GOODS* goods); // 인게임?? 상점용
	void Auth_Shop(UIBase* pUIBase, const SHOP_GOODS* goods, bool isExtensionPopup); // 상점용
	void Auth_Inven(UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx); //인벤용

	void Remove(UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx); //인벤용
	void Repair(UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx); //인벤용
	void RepairAll(UIBase* pUIBase); //현재 캐릭터 장착 장비에 대해서 모두 수리(캐릭터 뷰용)
	
	bool IsAllRepairPossible(AllRepairInfo* outInfo = NULL);

	const  buy::DisplayedGoodsList&		GetPurchaseGoodsList() const;
	const buy::GiftInfo&				GetGiftInfo() const;
	T_ItemDBIdx							GetTargetItemDBIndex() const;
	const i3::vector<INVEN_BUFFER*>&	GetPurchasedInvenBufferList() const;
	void OnNotify_NewInvenBufferAdded(INVEN_BUFFER* newInvenBuffer);

private:
	typedef void	(PurchaseMgr::*call_func)( INT32 result, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void	RegisterGameEventFunc( GAME_EVENT evt, call_func f);

public:
	PurchaseMgr();
	~PurchaseMgr();

	static PurchaseMgr* i();
	
	void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, i3::user_data& UserData);
	
public: //서버에서 ack 때 호출된다.
	void	ReceiveEvent_Buy( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Buy_Chara( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Auth_Delete( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Delete_Chara( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Item_Auth( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Repair( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Extend( INT32 arg, const i3::user_data &UserData);
	void	ReceiveEvent_Send_Gift( INT32 arg, const i3::user_data &UserData);

public:
	void	OnUpdate( REAL32 rDeltaSeconds);

};

