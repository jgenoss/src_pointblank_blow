#pragma once

// 꾸질꾸질한 CBFuction (call_cb / insert_cb) 제거 하기 위해 다음과 같이 처리합니다...
// 입력문자열이 하드코딩임에도 불구하고, 일일이 검색하면서 함수 호출할 필요없다고 판단되며, 임의적인 문자열 키가 필요하다면,
// 그때 외부에서 검색자료구조를 별도로 마련하여 래핑처리한다... 이쪽이 나음...

#include "i3Base/itl/signal/unique_signal.h"
#include "i3Base/itl/signal/unique_signal_reference.h"

namespace buy {typedef i3::shared_ptr <class DisplayedGoods> DisplayedGoodsPtr; }

struct ITEM_INFO;

namespace UISignal
{
	extern i3::unique_signal<void()>	UpdateCartBoard;    // "update_cart_board"
	extern i3::unique_signal<void()>	UpdatePreviewSlot;		// "update_preview_slot"
	extern i3::unique_signal<void(bool, bool)>	UpdateSlot9;	// "update_slot9"

	extern i3::unique_signal<void(bool)>	FinishRepairItem;	// "finish_repair_item"
	extern i3::unique_signal<void(bool)>	FinishRepairAllItem; // "finish_repair_all_item"

	extern i3::unique_signal_reference<void(bool)>		CurrentFinishRepairItem;
		
	extern i3::unique_signal<void(bool, const i3::vector< buy::DisplayedGoodsPtr >&)>  FinishBuyItem;		// "finish_buy_item"  상점구매(캐릭터 포함됩니다..)
	extern i3::unique_signal<void(bool)>	FinishBuyExtendItem;								 // "finish_buy_extend_item"

	extern i3::unique_signal<void(bool)> FinishBuyGift;											// 선물은 분리합니다. (인수가 다릅니다..)
	extern i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishAuthItem;					//	"finish_buy_auth_item", "finish_auth_item"
	extern i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishPreviewAuthItem;			//  "finish_preview_auth_item"
	extern i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishExtensionBuyAuthItem;		//  "finish_extension_buy_auth_item"

	extern i3::unique_signal_reference<void(bool, const SHOP_GOODS*)> CurrentFinishAuthItem;


	extern i3::unique_signal<void(bool, const ITEM_INFO*)>	FinishRemoveItem;					// "finish_remove_item"


	extern i3::unique_signal<void(bool, bool)> FinishBuyPopup;	// "finish_buy_popup" 
																// UBP_BUYITEM, UPW_BUYITEM ( UIPopupBuyItem, UIBattlePopupBuyItem)의 인수로 넘기도록 한다.
	extern i3::unique_signal<void(bool, bool)> 	FinishExtensionBuyItem;	// "finish_extension_buy_item"
																// UBP_BUYITEM, UPW_BUYITEM ( UIPopupBuyItem, UIBattlePopupBuyItem)의 인수로 넘기도록 한다.

	extern i3::unique_signal<void()> SMenuEquipSlotClear;		//"smenu_equip_slot_clear"
	extern i3::unique_signal<void(bool)> SMenuEquipClickClose; 	// "smenu_equip_click_close"

	extern i3::unique_signal<void()> ClanLobbyCreateTeam;		//"clan_lobby_create_team"
	extern i3::unique_signal<void(INT32)> ClanLobbyJoinTeam;	// "clan_lobby_join_team"
	extern i3::unique_signal<void()> ClanLobbyLeaveTeam;		//  "clan_lobby_leave_team"
	extern i3::unique_signal<void()> ClanLobbyQuickMatch;		// "clan_lobby_quick_match"
	extern i3::unique_signal<void()> ClanLobbyQuickMatchCancel;		// "clan_lobby_quick_match_cancel"
	extern i3::unique_signal<void()> ClanLobbyEnterRoomList;	// "clan_lobby_enter_room_list"
	extern i3::unique_signal<void(INT32)> ClanLobbyScrollTeamList;  // "clan_lobby_scroll_team_list"
	extern i3::unique_signal<void(INT32)> ClanLobbyChangePerson;    // "clan_lobby_change_person"
	extern i3::unique_signal<void(const i3::literal_range&)> ClanLobbyChangeOperation;	// "clan_lobby_change_operation"

	extern i3::unique_signal<void(bool)> EnableCompareDisplayAttr;  // "enable_compare_display_attr"
	extern i3::unique_signal<void(bool)> SetCompareSkipButton;		// "set_compare_skip_button"
	extern i3::unique_signal<void()>	 SMenuAbilityClose;			// "smenu_ability_close"

	extern i3::unique_signal<void(bool&)>	IsIgnoreShared;			// "is_ingore_shared"   고어물이 공유됬나...철자 오류.. 또한 네이밍이 몹시 좋지 않으나..
																	// 일단 방치하겠습니다...


}




