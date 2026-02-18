#include "pch.h"
#include "UIHudDomi_Flow.h"

#include "../UIShopFunction.h"
#include "../ItemImageMgr.h"
#include "../UIImgSetCtrl.h"
#include "../ItemConsumer.h"
#include "../UIUtil.h"
#include "../UIHUDIngameChat.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClientStageInfoUtil.h"

namespace DominationUI
{
	bool time_event::OnUpdate( REAL32 fElpasedTime)
	{ 
		m_current+= fElpasedTime;
		if( m_current >= m_target)
		{
			(*this)();
			return true;
		}
		return false;
	}


	bool time_event_list::pop_event( time_event_ptr& ptr)
	{
		i3::vector<time_event_ptr>::iterator itr = list.begin();
		while( itr != list.end())
		{
			if( ptr == *itr)
			{
				list.erase(itr);
				return true;
			}
			itr++;
		}
		return false;
	}

	time_event_ptr time_event_list::find( INT32 event_id)
	{
		i3::vector<time_event_ptr>::iterator itr = list.begin();
		while( itr != list.end())
		{
			if( (*itr)->get_id() == event_id)
				return *itr;
			
			itr++;
		}
		return time_event_ptr();
	}

	void time_event_list::OnUpdate( REAL32 fElpasedTime)
	{ 
		i3::vector<time_event_ptr>::iterator itr = list.begin();
		while( itr != list.end())
		{
			if( (*itr)->OnUpdate( fElpasedTime))
			{
				itr = list.erase(itr);
				continue;
			}
			++itr;
		}
	}
};

LuaFuncReg UIHUDDomi_Flow_Glue[] = {
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE( CUIHudDomi_Flow);

CUIHudDomi_Flow::CUIHudDomi_Flow(void) : m_refresh_boss_hp(0.f), m_award_img_ctrl(nullptr), m_bossHowlCanceled(false), m_refresh_boss_howl_canceled(0.0f)
{
	memset( m_frame, 0, sizeof(i3UIFrameWnd*)*dui::flow_control_max);

	refresh[dui::flow_state]	= &CUIHudDomi_Flow::refresh_flow_state;
	refresh[dui::round]			= &CUIHudDomi_Flow::refresh_round;
	refresh[dui::intermission]	= &CUIHudDomi_Flow::refresh_intermission;
	refresh[dui::wave]			= &CUIHudDomi_Flow::refresh_wave;
	refresh[dui::annihilation]	= &CUIHudDomi_Flow::refresh_annihilation;
	refresh[dui::boss_hp]		= &CUIHudDomi_Flow::refresh_boss_hp;
	refresh[dui::revival]		= &CUIHudDomi_Flow::refresh_revival;
	refresh[dui::etc]			= &CUIHudDomi_Flow::refresh_etc;
	refresh[dui::counter]		= &CUIHudDomi_Flow::refresh_counter;
	refresh[dui::award]			= &CUIHudDomi_Flow::refresh_award;
	refresh[dui::grade]			= &CUIHudDomi_Flow::refresh_grade;
	refresh[dui::boss_howling]	= &CUIHudDomi_Flow::refresh_boss_howling;
	refresh[dui::boss_dash]		= &CUIHudDomi_Flow::refresh_boss_dash;
	refresh[dui::boss_howl_hp]	= &CUIHudDomi_Flow::refresh_boss_howl_hp;
	refresh[dui::boss_howl_canceled] = &CUIHudDomi_Flow::refresh_boss_howl_canceled;

	detail[dui::FLOW_D_WAVE]			= &CUIHudDomi_Flow::detail_wave;
	detail[dui::FLOW_D_ROUND]			= &CUIHudDomi_Flow::detail_round;
	detail[dui::FLOW_D_INTERMISSION]	= &CUIHudDomi_Flow::detail_intermission;
	detail[dui::FLOW_D_COUNTER]			= &CUIHudDomi_Flow::detail_counter;
	detail[dui::FLOW_D_BOSS_HP]			= &CUIHudDomi_Flow::detail_boss_hp;
	detail[dui::FLOW_D_AWARD]			= &CUIHudDomi_Flow::detail_award;
	detail[dui::FLOW_D_ETC]				= &CUIHudDomi_Flow::detail_etc;
	detail[dui::FLOW_D_GRADE]			= &CUIHudDomi_Flow::detail_grade;
	detail[dui::FLOW_D_BOSS_HOWLING]	= &CUIHudDomi_Flow::detail_boss_howling;
	detail[dui::FLOW_D_BOSS_DASH]		= &CUIHudDomi_Flow::detail_boss_dash;
	detail[dui::FLOW_D_BOSS_HOWL_HP]	= &CUIHudDomi_Flow::detail_boss_howl_hp;
	detail[dui::FLOW_D_BOSS_HOWL_CANCELED] = &CUIHudDomi_Flow::detail_boss_howl_canceled;
}

CUIHudDomi_Flow::~CUIHudDomi_Flow(void)
{
//	i3::safe_destroy_instance( m_award_img_ctrl);
	I3_SAFE_RELEASE(m_award_img_ctrl);
}

/*virtual*/ void CUIHudDomi_Flow::OnCreate( i3GameNode* pParent)
{
	UIHUDDominationBase::OnCreate( pParent);

	AddScene( "Scene/Domination/PBRe_Domi_Flow.i3UIs", UIHUDDomi_Flow_Glue, true, false);
}

/*virtual*/ void CUIHudDomi_Flow::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDDominationBase::_InitializeAtLoad( pScene);

	for( INT32 i = dui::flow_state; i < dui::flow_control_max; ++i)
	{
		m_frame[i] = (i3UIFrameWnd*)pScene->FindChildByName( dui::g_flow_desc[i]);
	}
}

/*virtual*/ void CUIHudDomi_Flow::OnLoadAllScenes()
{
	UIHUDDominationBase::OnLoadAllScenes();

	i3UIScene * pScene = GetScene( 0);
	if( pScene)
	{
		LuaState* L = UILua::CallLuaFunction( pScene, "get_award_img_ctrl");
		i3UIButtonImageSet* btnset = reinterpret_cast< i3UIButtonImageSet* >(UILua::ReturnLuaPointer(L, 0));
		m_award_img_ctrl = GetItemImageMgr()->CreateImgSetCtrl( btnset);
	}
}

/*virtual*/ void CUIHudDomi_Flow::OnUnloadAllScenes()
{
	UIHUDDominationBase::OnUnloadAllScenes();

	//i3::safe_destroy_instance( m_award_img_ctrl);
}

/*virtual*/ bool CUIHudDomi_Flow::OnEntranceStart( void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UIHUDDominationBase::OnEntranceStart( pArg1, pArg2) == false) return false;

	for( INT32 i = dui::flow_state; i < dui::flow_control_max; ++i)
	{
		if( i == dui::flow_state) continue;

		m_frame[i]->EnableCtrl( false);
	}

	return false;
}

/*virtual*/ void CUIHudDomi_Flow::OnEntranceEnd()
{
	UIHUDDominationBase::OnEntranceEnd();

	SetInputDisable( true);	//input을 막습니다..
}

/*virtual*/ void CUIHudDomi_Flow::OnUpdate( REAL32 rDeltaSec)
{
	UIHUDDominationBase::OnUpdate( rDeltaSec);
	
	m_time_event_list.OnUpdate( rDeltaSec);

	//intermission refresh
	domi_state_info* info = domi_state_info::i();
	if( info && info->intermission_time > 0.f)
	{
		REAL32 temp = info->intermission_time - rDeltaSec;
		if( static_cast<INT32>(info->intermission_time) != static_cast<INT32>(temp))
			SetHudEvent( dui::FLOW_E_REFRESH, dui::intermission);

		info->intermission_time = temp;			
	}
	
	//wave pre counter
	if( info && info->wave_pre_counter > 0.f)
	{
		REAL32 temp = info->wave_pre_counter - rDeltaSec;
		if( static_cast<INT32>(info->wave_pre_counter) != static_cast<INT32>(temp))
			SetHudEvent( dui::FLOW_E_REFRESH, dui::counter);

		info->wave_pre_counter = temp;
	}

	//etc time
	if( info && info->etc_time > 0.f)
	{
		REAL32 temp = info->etc_time - rDeltaSec;
		if( static_cast<INT32>(info->etc_time) != static_cast<INT32>(temp))
			SetHudEvent( dui::FLOW_E_REFRESH, dui::etc);

		info->etc_time = temp;
		if( info->etc_time <= 0)
		{
			m_frame[dui::etc]->EnableCtrl( false);
			info->etc_time = 0.f;
		}
	}

	//play time
	if( info)
	{
		REAL32 temp = info->play_time;
		info->play_time += rDeltaSec;
		if( static_cast<INT32>(info->play_time) != static_cast<INT32>(temp))
			update_play_time();
	}

	//boss hp
	if( SetHudEvent( dui::FLOW_E_CHK_VISIBLE, dui::boss_hp))
	{
		m_refresh_boss_hp += rDeltaSec;
		if( m_refresh_boss_hp >= 1.f)
		{
			SetHudEvent( dui::FLOW_E_REFRESH, dui::boss_hp);
			m_refresh_boss_hp = 0.f;
		}
	}

	//boss howl cancel
	if ( m_bossHowlCanceled)
	{
		m_refresh_boss_howl_canceled += rDeltaSec;
		if (m_refresh_boss_howl_canceled > 2.0f)
		{
			m_frame[dui::boss_howling]->EnableCtrl( false); // boss_howling 프레임의 boss_howling_msg 텍스트를 사용하기 위해.
			
			m_bossHowlCanceled = false;
			m_refresh_boss_howl_canceled = 0.0f;
		}
	}
}

/*virtual*/ bool CUIHudDomi_Flow::SetHudEvent( INT32 nEventNum, INT32 arg /* = nullptr */)
{
	if( nEventNum < 0 || nEventNum >= dui::FLOW_E_MAX) return false;

	switch( nEventNum)
	{
	case dui::FLOW_E_CHK_VISIBLE:
		I3_BOUNDCHK( arg, dui::flow_control_max);
		return (m_frame[arg]->isEnable() == true);

	case dui::FLOW_E_VISIBLE:
		I3_BOUNDCHK( arg, dui::flow_control_max);
		m_frame[arg]->EnableCtrl( true);
		break;

	case dui::FLOW_E_INVISIBLE:
		I3_BOUNDCHK( arg, dui::flow_control_max);
		m_frame[arg]->EnableCtrl( false);
		break;

	case dui::FLOW_E_REFRESH:
		I3_BOUNDCHK( arg, dui::flow_control_max);
		(this->*refresh[arg])();
		break;

	case dui::FLOW_E_DETAIL:
		I3_BOUNDCHK( arg, dui::FLOW_D_MAX);
		(this->*detail[arg])();
		break;

	case dui::FLOW_E_INPUT:
		process_input( arg);
		break;
	}

	return false;
}

void CUIHudDomi_Flow::refresh_round()
{
	LuaState* L = _CallLuaFunction("refresh_round");
	if( L != nullptr)
	{
		domi_state_info* info = domi_state_info::i();
		
		i3::stack_wstring wstr_out;
		wstr_out = CStageInfoUtil::GetMapData((UINT8)MyRoomInfoContext::i()->getStage()->GetStageUID())->getDisplayName();

		i3Lua::PushInteger( L, (info->round-1));
		i3Lua::PushStringUTF16To8( L, wstr_out);
		_EndLuaFunction(L, 2);
	}
}

void CUIHudDomi_Flow::refresh_flow_state()
{
	LuaState* L = _CallLuaFunction("refresh_state");
	if( L != nullptr)
	{
		domi_state_info* info = domi_state_info::i();

		i3Lua::PushInteger( L, info->round);
		i3Lua::PushInteger( L, info->max_wave);
		i3Lua::PushInteger( L, info->wave_count);
		i3Lua::PushInteger( L, info->total_score[BattleSlotContext::i()->getMySlotIdx()/2]);
		i3Lua::PushInteger( L, info->max_dino);
		i3Lua::PushInteger( L, info->dino_count);

		_EndLuaFunction(L, 6);
	}
}

void CUIHudDomi_Flow::refresh_intermission()
{
	domi_state_info* state_info = domi_state_info::i();
	INT32 time = static_cast<INT32>(state_info->intermission_time);

	LuaState* L = _CallLuaFunction("refresh_intermission");
	if( L != nullptr)
	{		
		INT32 minute = time / 60;
		INT32 second = time - minute;
		
		i3Lua::PushInteger( L, minute);
		i3Lua::PushInteger( L, second);

		_EndLuaFunction(L, 2);
	}

	INT32 slot_idx = 0;
	INT32 lua_idx = 1;
	for( int i = 0; i < 4; ++i)
	{
		slot_idx = i*2; //0, 2, 4, 6
		if( BattleSlotContext::i()->getMySlotIdx() == slot_idx)
		{
			L = _CallLuaFunction("refresh_intermission_me");
			if( L != nullptr)
			{
				i3Lua::PushBoolean( L, domi_state_info::i()->intermission_ready[i]);
				_EndLuaFunction(L, 1);
			}
		}
		else
		{
			L = _CallLuaFunction("refresh_intermission_other");
			if( L != nullptr)
			{
				const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo(slot_idx);

				bool enable = (info->_State == SLOT_STATE_BATTLE);
				UINT8 rank = info->_Rank;
				i3::rc_wstring wstr_nick = BattleSlotContext::i()->getNickForSlot(slot_idx);

				i3Lua::PushInteger( L, lua_idx++);
				i3Lua::PushBoolean( L, enable);
				i3Lua::PushInteger( L, rank);
				i3Lua::PushStringUTF16To8( L, wstr_nick);
				i3Lua::PushBoolean( L, domi_state_info::i()->intermission_ready[i]);

				_EndLuaFunction(L, 5);
			}
		}
	}
}

void CUIHudDomi_Flow::refresh_wave()
{
	LuaState* L = _CallLuaFunction("refresh_wave");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, domi_state_info::i()->wave_count);
		_EndLuaFunction(L, 1);
	} 
}

void CUIHudDomi_Flow::refresh_counter()
{
	domi_state_info* info = domi_state_info::i();
	INT32 time = static_cast<INT32>(info->wave_pre_counter);

	LuaState* L = _CallLuaFunction("refresh_counter");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, (time-1));
		_EndLuaFunction(L, 1);

		//I3TRACE("################ domi count time %d \n", time);
	}
}

void CUIHudDomi_Flow::update_play_time()
{
	domi_state_info* info = domi_state_info::i();
	INT32 time = static_cast<INT32>(info->play_time);

	LuaState* L = _CallLuaFunction("refresh_state_time");
	if( L != nullptr)
	{
		INT32 minute = time / 60;
		INT32 second = time - (minute*60);

		i3Lua::PushInteger( L, minute);
		i3Lua::PushInteger( L, second);

		_EndLuaFunction(L, 2);
	}
}

void CUIHudDomi_Flow::refresh_annihilation()
{
	domi_state_info* info = domi_state_info::i();
	info->init_shop_item();

	// 매개인자에 따른 행동 : -1(UI 사라짐), 0(아이템 구매), 1이상(아이템 사용)
	LuaState* L = _CallLuaFunction("refresh_annihilation");

	if( info->single_respawn_count != 0 )
	{
		i3Lua::PushInteger( L, info->single_respawn_count);
	}
	else
	{
		//if( domi_util::is_buyable_domi_item(CASHITEM_GROUP_INDIVIDUAL_REVIVAL) == true )
		//	i3Lua::PushInteger( L, 0);
		//else
			i3Lua::PushInteger( L, -1);
	}

	if( info->everyone_respawn_count != 0 )
	{
		i3Lua::PushInteger( L, info->everyone_respawn_count);
	}
	else
	{
		//if( domi_util::is_buyable_domi_item(CASHITEM_GROUP_WHOLE_REVIVAL) == true )
		//	i3Lua::PushInteger( L, 0);
		//else
			i3Lua::PushInteger( L, -1);
	}

	_EndLuaFunction(L, 2);
}

void CUIHudDomi_Flow::refresh_boss_hp()
{
	domi_state_info* info = domi_state_info::i();

	if( info->boss_list.size() <= 0)
	{
		m_frame[dui::boss_hp]->EnableCtrl( false);


		//켜기.
		LuaState* L = _CallLuaFunction("enable_dino_count");
		if( L != nullptr)
		{
			i3Lua::PushBoolean( L, true);
			_EndLuaFunction(L, 1);
		}


		return;
	}


	REAL32 hp = 0.f;
	REAL32 ratio = 0.f;
	i3::rc_wstring wstr_nick;

	// 기획에서 보스 1개로 제한해서.. 지우근. 2013.06.18
	if( info->boss_list.size() > 0)
	{
		REAL32 max = info->boss_list[0].boss_max_hp;
		CGameCharaBase* boss = info->boss_list[0].boss;

		if( boss && boss->getCharaInfo())
		{
			hp = boss->getCharaInfo()->GetHP(); //..
			ratio = (max > 0) ? (hp/max) : (0.f);
			wstr_nick = BattleSlotContext::i()->getNickForSlot( boss->getCharaNetIndex(), boss->getBufferType());
		}
	}

	LuaState* L = _CallLuaFunction("refresh_boss_hp");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstr_nick);
		i3Lua::PushNumber( L, ratio);
		_EndLuaFunction( L, 2);
	}

}

void CUIHudDomi_Flow::refresh_revival()
{
	domi_state_info* info = domi_state_info::i();

	LuaState* L = _CallLuaFunction("refresh_revival");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, info->single_respawn_count);
		_EndLuaFunction(L, 1);
	}
}

void CUIHudDomi_Flow::refresh_etc()
{
	domi_state_info* info = domi_state_info::i();
	INT32 time = (INT32)info->etc_time;

	LuaState* L = _CallLuaFunction("refresh_etc");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, GAME_RCSTRING( "STR_DOMI_BATTLE_1"));
		i3Lua::PushInteger( L, time - 1);
		_EndLuaFunction(L, 2);
	}
}

void CUIHudDomi_Flow::refresh_boss_howling()
{	
	LuaState* L = _CallLuaFunction("refresh_boss_howling");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, domi_state_info::i()->boss_howling_time);
		_EndLuaFunction(L, 1);
	}
}

void CUIHudDomi_Flow::refresh_boss_dash()
{
	LuaState* L = _CallLuaFunction("refresh_boss_dash");
	if( L != nullptr)
	{
		i3::wstring wstr = BattleSlotContext::i()->getNickForSlot( domi_state_info::i()->boss_dash_target_idx, CHARA::BUFFER_USER);	

		switch( domi_state_info::i()->boos_dash_target_type)
		{
		case DOMI_TYPE_USER:
			//
			break;

		case DOMI_TYPE_SENTRYGUN:
			wstr += L"'s SentryGun";
			break;

		case DOMI_TYPE_DUMMY:
			wstr += L"'s Dummy";
			break;
		
		default:
			wstr += L"???_type";
			break;
		}
		i3Lua::PushStringUTF16To8( L, wstr);
		_EndLuaFunction(L, 1);
	}
}

void CUIHudDomi_Flow::refresh_boss_howl_hp()
{
	domi_state_info* info = domi_state_info::i();

	REAL32 ratio = 0.0f;

	if( info->boss_list.size() > 0)
	{
		CGameCharaBase* boss = info->boss_list[0].boss;

		if( boss && boss->getCharaInfo())
		{
			REAL32 max_howl_hp = info->boss_cur_hp_at_howl_start - info->boss_howl_cancel_threshold;

			if (0 <= max_howl_hp)
			{
				REAL32 cur_howl_hp = boss->getCharaInfo()->GetHP() - info->boss_howl_cancel_threshold;

				ratio = (max_howl_hp > 0) ? (cur_howl_hp/max_howl_hp) : (0.f);
				i3::rc_wstring wstr_nick = BattleSlotContext::i()->getNickForSlot( boss->getCharaNetIndex(), boss->getBufferType());

				//I3TRACE("max_howl_hp[%f] = boss_cur_hp_at_howl_start[%f]-boss_howl_cancel_threshold[%f], \
				//		cur_howl_hp[%f] = boss->getCharaInfo()->GetHP()[%f]-boss_howl_cancel_threshold[%f], \
				//		ratio[%f]\n", max_howl_hp, info->boss_cur_hp_at_howl_start, info->boss_howl_cancel_threshold, 
				//		cur_howl_hp, boss->getCharaInfo()->GetHP(), info->boss_howl_cancel_threshold, ratio);

				LuaState* L = _CallLuaFunction("refresh_boss_howl_hp");
				if( L != nullptr)
				{
					i3Lua::PushStringUTF16To8( L, wstr_nick);
					i3Lua::PushNumber( L, ratio);
					_EndLuaFunction( L, 2);
				}
			}
		}
	}	
}

void CUIHudDomi_Flow::refresh_boss_howl_canceled()
{
	LuaState* L = _CallLuaFunction("refresh_boss_howl_canceled");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
}

void CUIHudDomi_Flow::refresh_award()
{
	domi_state_info* info = domi_state_info::i();

	LuaState* L = _CallLuaFunction("refresh_award");
	if( L != nullptr)
	{
		i3::rc_wstring wstr_buf_name;

		ITEM_TYPE ItemClass = (ITEM_TYPE) GET_ITEM_TYPE(info->award_item);
		if( ItemClass == ITEM_TYPE_POINT)
		{
			ShopItemInfo* item_info = g_pShopItemInfoDataBase->getInfo( info->award_item);
			i3::sprintf( wstr_buf_name, item_info->GetScriptName(), GAME_RCSTRING("STBL_IDX_SHOP_MONEY"), info->award_point);
		}
		else
		{
			usf::GetShopItemName( wstr_buf_name, info->award_item);
		}

		i3Lua::PushStringUTF16To8( L, wstr_buf_name.c_str(), wstr_buf_name.size() );

		_EndLuaFunction(L, 1);
	}

	if( m_award_img_ctrl)
		GetItemImageMgr()->SelectImage( m_award_img_ctrl, (INT32)info->award_item);
}

void CUIHudDomi_Flow::refresh_grade()
{
	domi_state_info* info = domi_state_info::i();
	i3::wstring wstr_buf = L"..";

	switch( info->grade_state)
	{
	case DOMI_REWARD_GRADE_STATE_FIRST_WAVE:
		domi_util::to_string_reward_grade_start_battle( wstr_buf);
		break;

	case DOMI_REWARD_GRADE_STATE_USER_ENGER:
		domi_util::to_string_reward_grade_in_user( wstr_buf, info->grade_user_nick);
		break;

	case DOMI_REWARD_GRADE_STATE_USER_LEAVE:
		domi_util::to_string_reward_grade_out_user( wstr_buf, info->grade_user_nick);
		break;
	}
	

	LuaState* L = _CallLuaFunction("refresh_grade");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstr_buf);
		_EndLuaFunction(L, 1);
	}
}


void CUIHudDomi_Flow::detail_round()
{
	m_frame[dui::round]->EnableCtrl( true);

	SetHudEvent( dui::FLOW_E_REFRESH, dui::round);
	dui::time_disable * pTimeDisable = new dui::time_disable( m_frame[dui::round], 2.5f);
	MEMDUMP_NEW( pTimeDisable, sizeof(dui::time_disable));
	m_time_event_list.push_event( dui::time_event_ptr( pTimeDisable));
}

void CUIHudDomi_Flow::detail_intermission()
{
	m_frame[dui::intermission]->EnableCtrl( true);

	i3UIControl* p = m_frame[dui::intermission]->FindCtrlByName( "intermission_frame");
	I3ASSERT( p != nullptr);
	p->EnableCtrl( true);

	SetHudEvent( dui::FLOW_E_REFRESH, dui::intermission);
	dui::time_disable * pTimeDisable = new dui::time_disable( p, 3.f);
	MEMDUMP_NEW( pTimeDisable, sizeof(dui::time_disable));
	m_time_event_list.push_event( dui::time_event_ptr( pTimeDisable));
}

void CUIHudDomi_Flow::detail_wave()
{
	m_frame[dui::intermission]->EnableCtrl( false);
	m_frame[dui::counter]->EnableCtrl( false);

	m_frame[dui::wave]->EnableCtrl( true);
	
	SetHudEvent( dui::FLOW_E_REFRESH, dui::wave);
	dui::time_disable * pTimeDisable = new dui::time_disable( m_frame[dui::wave], 3.f);
	MEMDUMP_NEW( pTimeDisable, sizeof(dui::time_disable));
	m_time_event_list.push_event( dui::time_event_ptr( pTimeDisable));

	domi_state_info::i()->init_wave();
	SetHudEvent( dui::FLOW_E_REFRESH, dui::flow_state);

	//1wave 일 경우
	if( domi_state_info::i()->wave_count == 1)
	{
		domi_state_info::i()->grade_state = DOMI_REWARD_GRADE_STATE_FIRST_WAVE;

		//grade msg 예약.
		dui::member_call_back<CUIHudDomi_Flow> * pCb = new dui::member_call_back<CUIHudDomi_Flow>( this, &CUIHudDomi_Flow::cb_detail_grade, nullptr);
		MEMDUMP_NEW( pCb, sizeof( dui::member_call_back<CUIHudDomi_Flow>));
		dui::call_back_ptr cb_detail( pCb);

		dui::time_event_detail * pEvt = new dui::time_event_detail( m_frame[dui::grade], 3.f, cb_detail);
		MEMDUMP_NEW( pEvt, sizeof( dui::time_event_detail));
		dui::time_event_ptr event_detail( pEvt);
		m_time_event_list.push_event( event_detail);
	}
}

void CUIHudDomi_Flow::detail_counter()
{
	m_frame[dui::intermission]->EnableCtrl( false);
	m_frame[dui::counter]->EnableCtrl( true);

	domi_state_info* info = domi_state_info::i();
	info->wave_pre_counter = COUNTDOWN_WAIT_SECOND;

	dui::member_call_back<CUIHudDomi_Flow> * pCb = new dui::member_call_back<CUIHudDomi_Flow>( this, &CUIHudDomi_Flow::cb_detail_wave, nullptr);
	MEMDUMP_NEW( pCb, sizeof(dui::member_call_back<CUIHudDomi_Flow>));

	dui::time_event_detail * pEvt = new dui::time_event_detail( m_frame[dui::wave], info->wave_pre_counter, dui::call_back_ptr( pCb));
	MEMDUMP_NEW( pEvt, sizeof( dui::time_event_detail));

	m_time_event_list.push_event( dui::time_event_ptr( pEvt));
}

void CUIHudDomi_Flow::detail_boss_hp()
{
	m_frame[dui::boss_hp]->EnableCtrl( true);
	m_refresh_boss_hp = 0.f;

	SetHudEvent( dui::FLOW_E_REFRESH, dui::boss_hp);

	//끄기.
	LuaState* L = _CallLuaFunction("enable_dino_count");
	if( L != nullptr)
	{
		i3Lua::PushBoolean( L, false);
		_EndLuaFunction(L, 1);
	}
}

void CUIHudDomi_Flow::detail_award()
{
	m_frame[dui::award]->EnableCtrl( true);
	SetHudEvent( dui::FLOW_E_REFRESH, dui::award);

	dui::time_disable * pTimeDisable = new dui::time_disable( m_frame[dui::award], 3.f);
	MEMDUMP_NEW( pTimeDisable, sizeof( dui::time_disable));
	m_time_event_list.push_event( dui::time_event_ptr( pTimeDisable));

	i3UIScene * pScene = GetScene( 0);
	if( pScene)
	{//제일 위에 나오게 하기 위해서..
		pScene->Attach( false);
	}
}

void CUIHudDomi_Flow::detail_etc()
{
	m_frame[dui::etc]->EnableCtrl( true);
	SetHudEvent( dui::FLOW_E_REFRESH, dui::etc);
}

void CUIHudDomi_Flow::detail_grade()
{
	m_frame[dui::grade]->EnableCtrl( true);

	SetHudEvent( dui::FLOW_E_REFRESH, dui::grade);

	if( m_time_event_list.find( IDE_GRADE_DISABLE).get() == nullptr)
	{
		dui::time_disable * pTimeDisable = new dui::time_disable( m_frame[dui::grade], 5.f, IDE_GRADE_DISABLE);
		MEMDUMP_NEW( pTimeDisable, sizeof( dui::time_disable));
		m_time_event_list.push_event( dui::time_event_ptr( pTimeDisable));
	}
	else
	{
		m_time_event_list.find( IDE_GRADE_DISABLE).get()->m_current = 0.f;
	}
}

void CUIHudDomi_Flow::detail_boss_howling()
{
	m_frame[dui::boss_howling]->EnableCtrl( true);
	SetHudEvent( dui::FLOW_E_REFRESH, dui::boss_howling);
}

void CUIHudDomi_Flow::detail_boss_dash()
{
	m_frame[dui::boss_dash]->EnableCtrl( true);
	SetHudEvent( dui::FLOW_E_REFRESH, dui::boss_dash);
	
	if( m_time_event_list.find( IDE_DASH_DISABLE).get() == nullptr)
	{
		dui::time_disable * pTimeDisable = new dui::time_disable( m_frame[dui::boss_dash], 3.f, IDE_DASH_DISABLE);
		MEMDUMP_NEW( pTimeDisable, sizeof(dui::time_disable));
		m_time_event_list.push_event( dui::time_event_ptr( pTimeDisable));
	}
	else
	{
		m_time_event_list.find( IDE_DASH_DISABLE).get()->m_current = 0.f;
	}
}

void CUIHudDomi_Flow::detail_boss_howl_hp()
{
	m_frame[dui::boss_howl_hp]->EnableCtrl( true);
	SetHudEvent( dui::FLOW_E_REFRESH, dui::boss_howl_hp);
}

void CUIHudDomi_Flow::detail_boss_howl_canceled()
{
	m_frame[dui::boss_howling]->EnableCtrl( true); // boss_howling 프레임의 boss_howling_msg 텍스트를 사용하기 위해.
	SetHudEvent( dui::FLOW_E_REFRESH, dui::boss_howl_canceled);
	m_bossHowlCanceled = true;
	m_refresh_boss_howl_canceled = 0.0f;
}

void CUIHudDomi_Flow::process_input( INT32 key)
{
	if( UIHUDIngameChat::instance()->GetEnableChat()) return;

	if( key == I3I_KEY_F5)
	{
		if( SetHudEvent( dui::FLOW_E_CHK_VISIBLE, dui::intermission))
		{
//			INT16 size = 0;

#ifdef DOMI_NEW_FLOW
			if( domi_state_info::i()->intermission_ready[BattleSlotContext::i()->getMySlotIdx()/2] == false)
			{
				GameEventSender::i()->SetEvent( EVENT_DOMI_INTERMISSION_READY );
			}
			else
			{
				GameEventSender::i()->SetEvent( EVENT_DOMI_INTERMISSION_READY_CANCEL );
			}
#endif
		}
	}
	else if( key == I3I_KEY_U /*&& !g_pFramework->getChatBox()->isEnable()*/)
	{//개인 부활
		if( SetHudEvent( dui::FLOW_E_CHK_VISIBLE, dui::revival) || SetHudEvent( dui::FLOW_E_CHK_VISIBLE, dui::annihilation))
		{	
			//ItemConsumer::instance()->use_in_domi( CASHITEM_GROUP_INDIVIDUAL_REVIVAL);
		}
	}
	else if( key == I3I_KEY_I /*&& !g_pFramework->getChatBox()->isEnable()*/)
	{//전체 부활
		if( SetHudEvent( dui::FLOW_E_CHK_VISIBLE, dui::annihilation))
		{
			//ItemConsumer::instance()->use_in_domi( CASHITEM_GROUP_WHOLE_REVIVAL);
		}
	}
}