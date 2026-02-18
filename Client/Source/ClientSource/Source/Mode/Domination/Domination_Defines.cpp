#include "pch.h"
#include "Domination_Defines.h"

#include "../../ui/UIBattleFrame.h"
#include "../../ui/UIBattlePopupShop.h"
#include "../../UI/UIUtil.h"

namespace DomiData
{
	
}

void domi_state_info::init()
{	
	is_wave_end = false;

	round = 0;			//라운드
	max_wave = 0;		//max 웨이브

	wave_count = 1;		//current 웨이브
	wave_pre_counter = 0.f;

	max_dino = 0;
	dino_count = 0;

	memset( total_score, 0, sizeof(UINT32)*4);
	play_time = 0.f;

	intermission_time = 0.f;
	inIntermission = false;

	single_respawn_count = 0;
	everyone_respawn_count = 0;

	memset( intermission_ready, 0, sizeof(bool)*4);
	memset( &sp, 0, sizeof(DOMI_SP));

	boss_list.clear();

	award_item = 0;
	award_item_arg = 0;
	award_point = 0;

	etc_time = 0.f;
	etc_count = 0;

	grade_state = DOMI_REWARD_GRADE_STATE_NONE;
	grade_user_nick.clear();


	boss_howling_time = 6;
	boos_dash_target_type = DOMI_TYPE_NO;
	boss_dash_target_idx = 0;
	boss_howl_cancel_threshold = 0.0f;
	boss_cur_hp_at_howl_start = 0.0f;

	init_shop_item();
}

void domi_state_info::init_wave()
{
	dino_count = 0;
	memset( intermission_ready, 0, sizeof(bool)*4);

	award_item = 0;
	award_item_arg = 0;
	award_point = 0;
}

void domi_state_info::init_shop_item()
{
	//single_respawn_count = domi_util::get_cash_item_count(CASHITEM_GROUP_INDIVIDUAL_REVIVAL);
	//everyone_respawn_count = domi_util::get_cash_item_count(CASHITEM_GROUP_WHOLE_REVIVAL);
}

////-----------------------------------////-----------------------------------//
////domi_util
//void domi_util::buy_domi_item(CASHITEM_GROUP_TYPE group_type) //아이템을 구매한다.
//{
//	UIBattlePopupShop* igs = static_cast<UIBattlePopupShop*>(GameUI::GetBattlePopup( UBP_INGAME_SHOP ));
//	igs->buy_domi_item( group_type );
//}
//
//void domi_util::buy_domi_cash_item(CASHITEM_GROUP_TYPE group_type) //아이템을 구매한다.
//{
//	UIBattlePopupShop* igs = static_cast<UIBattlePopupShop*>(GameUI::GetBattlePopup( UBP_INGAME_SHOP ));
//	igs->buy_domi_cash_item( group_type );
//}
//
//INT32 domi_util::get_cash_item_count(CASHITEM_GROUP_TYPE group_type) //없는 경우 0 리턴
//{
//	INVEN_BUFFER* ib = CInvenList::i()->FindCashItem(group_type);
//	if( ib == 0 ) return 0;
//
//	return ib->_ui32ItemArg;
//}
//
//bool domi_util::is_buyable_domi_item(CASHITEM_GROUP_TYPE group_type) //서버 상점 목록에 등록 여부를 알려준다.
//{
//	UIBattlePopupShop* igs = static_cast<UIBattlePopupShop*>(GameUI::GetBattlePopup( UBP_INGAME_SHOP ));
//	if( igs != nullptr)
//		return igs->domi_shop_list.find_domi_item_pack(group_type) != 0;
//	return false;
//}
//bool domi_util::is_buyable_domi_cash_item(CASHITEM_GROUP_TYPE group_type) //서버 상점 목록에 등록 여부를 알려준다.
//{
//	UIBattlePopupShop* igs = static_cast<UIBattlePopupShop*>(GameUI::GetBattlePopup( UBP_INGAME_SHOP ));
//	if( igs != nullptr)
//		return igs->domi_shop_list.find_domi_cash_item_pack(group_type) != 0;
//	return false;
//}
