#pragma once

#include "Useable_Item_Def.h"

enum UIPOPUPWIN;

namespace chk_item_period
{
	bool _CheckUseItemPeriod(T_ItemID itemID);
}

#pragma pack(push,8)
//구매시에 서버 패킷과 관련된 일련에 과정을 다루는 클래스
//예) buy -> SetEvent -> cb_receive_buy_packet -> detail::param_util::call_finish_cb_string
class ItemConsumer
{
public:
	ItemConsumer();
	~ItemConsumer();

	static ItemConsumer* instance();

public:
	bool use(UIMainFrame* frm, T_ItemID item_id);
	//bool use_in_domi(CASHITEM_GROUP_TYPE type);
	
	bool m_bClanMaxBy50;		// 50명 증가권을 사용하여 클랜원 최대에 도달했는지 여부. 도달시 true
	bool m_bClanMaxBy10;		// 10명 증가권을 사용하여 클랜원 최대에 도달했는지 여부. 도달시 true

	bool remove(UIMainFrame* frm, T_ItemID item_id, T_ItemDBIdx item_db_idx);
	void process_game_event_in_query( INT32 event, INT32 arg, const i3::user_data& UserData);

	void InitItemConsumer()
	{
		//상점이나 인벤을 나가면 초기화 시켜주기로 하자.
		m_ItemID = 0;
		m_AuthWareDBIndex = 0;
		m_nSelected = 0;
	}

private:
	void auth(ITEM_USE_METHOD method);
	void open(ITEM_USE_METHOD method);
	void error(ITEM_USE_METHOD method);
	void battle(ITEM_USE_METHOD method);
	void (ItemConsumer::*pf_method[MAX_USE_METHOD])(ITEM_USE_METHOD method);

private:
	void open_popup(USE_ITEM_POPUP_TYPE type);

	//2014.4.14 박기성 
	//랜덤박스 테스트 커맨드 관련 함수
	//추후 클래스로 빼내거나, 다른 테스트 커맨드와 같이 묶어놓을 수 있을 것 같다.
private:
	bool isTestEnable;
	
public:
	void SetRandomBoxTestEnable(bool isEnable)	{isTestEnable = isEnable;}
	bool IsRandomBoxTestEnable()				{return isTestEnable;}


public:
	T_ItemDBIdx m_AuthWareDBIndex;
	
	T_ItemID m_ItemID;
	INT32 m_nSelected;

	UIMainFrame* m_MainFrame;

	UIPOPUPWIN m_OpenedUsePopup;
};

#pragma pack(pop)

