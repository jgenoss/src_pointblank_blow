#include "pch.h"
#include "UIHudDomi_UserState.h"

#include "../StageBattle/UserContext.h"
#include "BattleSlotContext.h"

LuaFuncReg UIHUDDomi_UserState_Glue[] = {
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE( CUIHudDomi_UserState);

CUIHudDomi_UserState::CUIHudDomi_UserState()
{

}
CUIHudDomi_UserState::~CUIHudDomi_UserState()
{

}

/*virtual*/ void CUIHudDomi_UserState::OnCreate( i3GameNode* pParent)
{
	UIHUDDominationBase::OnCreate( pParent); 

	AddScene( "Scene/Domination/PBRe_Domi_playrank.i3UIs", UIHUDDomi_UserState_Glue, true, false);
}

/*virtual*/ void CUIHudDomi_UserState::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDDominationBase::_InitializeAtLoad( pScene);

	if( pScene)
	{
		for( INT32 i = 0; i < 4; ++i)
		{
			char buf[64] = {0,};
			StringCchPrintf( buf, 64, "hp%d", i);

			m_ProgressBar[i].SetControl(pScene, buf);

			m_ProgressBar[i].AddMaxColorSpot(142, 183,  64); //연녹색
			m_ProgressBar[i].AddColorSpot(80, 202, 176,  54); //노란
			m_ProgressBar[i].AddColorSpot(60, 202, 134,  54); //살구
			m_ProgressBar[i].AddColorSpot(40, 211,  92,  34); //갈색
			m_ProgressBar[i].AddColorSpot(20, 181,  35,   0); //빨간
		}
	}
}

/*virtual*/ bool CUIHudDomi_UserState::OnEntranceStart( void* pArg1, void* pArg2)
{
	if( UIHUDDominationBase::OnEntranceStart( pArg1, pArg2) == false) 
		return false;

	//
	for( INT32 i = 0; i < 4; ++i)
	{
		dus::slot_info _slot_info;
		_set_slot_info( i, _slot_info);		//empty info.
	}
	return true;
}

bool CUIHudDomi_UserState::SetHudEvent( INT32 nEventNum, INT32 arg)
{
	switch( nEventNum)
	{
	case dus::USER_STATE_E_INIT:
		//..
		break;

	case dus::USER_STATE_E_SYNC:
		dus::slot_info _slot_info[4];
		for( INT32 i = 0; i < 4; ++i)
		{
			const SLOT_INFO* _slot = BattleSlotContext::i()->getSlotInfo( i*2);
			if( _slot && 
				_slot->_State >= SLOT_STATE_LOAD && 
				_slot->_State <= SLOT_STATE_BATTLE && 
				UserContext::i()->IsPlayingSlot( i*2))
			{
				_get_user_info( i*2, _slot_info[i]);
			}
		}

		struct cmp
		{
			bool operator()( dus::slot_info& o1, dus::slot_info& o2)
			{
				return o1._score > o2._score;
			}
		};
		i3::sort( _slot_info, _slot_info+4/*count*/, cmp());

		for( INT32 i = 0; i < 4; ++i)
		{
			_set_slot_info( i, _slot_info[i]);
		}
		break;
	}

	return false;
}




void CUIHudDomi_UserState::_set_slot_info( INT32 slot_idx, dus::slot_info& in)
{
	LuaState* L = _CallLuaFunction("set_slot_info");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot_idx);
		i3Lua::PushStringUTF16To8( L, in._wstr_name);
		i3Lua::PushInteger( L, in._score);
		i3Lua::PushInteger( L, in._max_hp);
		i3Lua::PushInteger( L, in._cur_hp);
		i3Lua::PushInteger( L, in._state);

		_EndLuaFunction(L, 6);

		m_ProgressBar[slot_idx].SetNumber( in._cur_hp, 0, in._max_hp);
	}
}

void CUIHudDomi_UserState::_get_user_info( INT32 user_slot_idx, dus::slot_info& out)
{
	//
	out._wstr_name			= BattleSlotContext::i()->getNickForSlot( user_slot_idx);
	out._score			= tinst<domi_state_info>()->total_score[user_slot_idx/2];
	out._state			= (user_slot_idx == BattleSlotContext::i()->getMySlotIdx()) ? dus::slot_info::ME : dus::slot_info::OTHER;
	
	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx( user_slot_idx);
	if( chara)
	{
		const GLOBAL_CHARA_INFO* chara_info = chara->getCharaInfo();
		out._cur_hp			= (INT32)chara_info->GetHP();
		out._max_hp			= (chara->getFullHP() >= out._cur_hp) ? chara->getFullHP() : out._cur_hp;
	}
}