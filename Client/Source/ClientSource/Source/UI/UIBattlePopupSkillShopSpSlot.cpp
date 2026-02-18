#include "pch.h"
#include "UIBattlePopupSkillShopSpSlot.h"
#include "../Mode/Domination/Domination_Defines.h"
#include "../Mode/Domination/DomiSkillProperties.h"
#include "../Mode/Domination/DomiUserSkillMgr.h"
#include "BattleSlotContext.h"

namespace
{
	BattleSkillShopSlot* g_this = 0;

	static const INT32 g_total_row_slot = 4;
	static const INT32 g_total_col_slot = 2;
	static const INT32 g_slot_cnt_per_page = 8;

	INT32 GetVectorIndex(INT32 currPage, INT32 currSlotNum)
	{
		INT32 idx = (currPage * g_slot_cnt_per_page) + currSlotNum;
		I3ASSERT(0 <= idx && idx < USER_SKILL_COUNT);
		return idx;
	}
}

extern "C" 
{
	int RequestLearn(LuaState * L)
	{
		g_this->Request();
		return 0;
	}
}

LuaFuncReg battle_skill_shop_sp_slot_glue[] =
{
	{ "RequestLearn", RequestLearn },
	{ nullptr, nullptr}
};


//----------------------------------------------
// class BattleSkillShopSlotSpItem
//----------------------------------------------
BattleSkillShopSlotSpItem::BattleSkillShopSlotSpItem(UIBattlePopupSkillShop* p) : BattleSkillShopSlot(p)
{
	g_this = this;

	m_items.reserve(USER_SKILL_COUNT);
	for (UINT32 i=0; i<USER_SKILL_COUNT; i++)
	{
		DOMI_SKILLSHOP_SP_ITEM* data = new DOMI_SKILLSHOP_SP_ITEM;
		MEMDUMP_NEW( data, sizeof( DOMI_SKILLSHOP_SP_ITEM));

		data->type = i; // enum USER_SKILL

		DomiSpItemInfo* domi_sp_item = g_pShopItemInfoDataBase->find_domi_sp_item_info(i);
		if( domi_sp_item != 0 )
		{
			data->wstr_name = domi_sp_item->GetScriptName();
			data->explain = domi_sp_item->GetDescription();
			data->shape_idx = domi_sp_item->GetShapeIndex();
		}

		m_items.push_back(data);
	}

	m_totalPage = m_items.size() / g_slot_cnt_per_page;
}

BattleSkillShopSlotSpItem::~BattleSkillShopSlotSpItem()
{
	for (size_t i=0; i < m_items.size(); i++)
	{
		I3_SAFE_DELETE( m_items[i]);
	}
	m_items.clear();
}

void BattleSkillShopSlotSpItem::OnCreate(i3GameNode * pParent)
{
	mSlot.EnableScroll(false);
	mSlot.OnCreate((UIBase*)m_parent, g_total_row_slot, g_total_col_slot);
	mSlot.RegisterSlotScene("Scene/Domination/PBRe_Domi_gearskillpop_sp_slot.i3UIs", battle_skill_shop_sp_slot_glue, true);
}

void BattleSkillShopSlotSpItem::ProcessKey()
{
	i3InputKeyboard* kb = g_pFramework->getKeyboard();

	for (size_t i = 0; i < 8; i++)
	{
		if (i < m_items.size())
		{
			if (kb->GetStrokeState(static_cast<I3I_KEY>(i+1)))
			{
				if (m_items[i]->forSale)
					Request(i);
			}
		}
	}
}

// 요청 후 처리순서 : DOMI_PROTOCOL_USER_SKILL_UPGRADE_ACK -> UserSkillMgr(스킬 적용)
void BattleSkillShopSlotSpItem::Request(INT32 slotIdx)
{
	PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ UserSkill;
	
	INT32 vidx = GetVectorIndex(m_currPage, slotIdx);
	UserSkill._i8SkillIdx = static_cast<INT8>(vidx);
	GameEventSender::i()->SetEvent(EVENT_DOMI_SKILL, &UserSkill);
}

void BattleSkillShopSlotSpItem::InitItems()
{
	for (size_t i = 0; i < m_items.size(); i++)
	{
		m_items[i]->type = i; // enum USER_SKILL

		DomiSpItemInfo* domi_sp_item = g_pShopItemInfoDataBase->find_domi_sp_item_info(i);
		if( domi_sp_item != 0 )
		{
			m_items[i]->wstr_name = domi_sp_item->GetScriptName();
			m_items[i]->explain = domi_sp_item->GetDescription();
			m_items[i]->shape_idx = domi_sp_item->GetShapeIndex();
		}
		else
		{
			m_items[i]->wstr_name.clear();
			m_items[i]->explain.clear();
			m_items[i]->shape_idx = -1;
		}
	}
}

void BattleSkillShopSlotSpItem::UpdateItems()
{
	if (m_items.empty()) return;

	struct fn
	{
		void operator()(DOMI_SKILLSHOP_SP_ITEM* item)
		{
			INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
			UINT16 sp = domi_state_info::i()->sp.sp[mySlotIdx/2];

			// level 조건 처리
			INT32 currentUserLv = DomiUserSkillMgr::i()->GetLevel(mySlotIdx, static_cast<USER_SKILL>(item->type));
			INT32 skillLvToBuy = currentUserLv + 1; // 구매할 스킬 레벨

			if (skillLvToBuy > domi::SkillProp::i()->GetUserSkill(item->type).level_max->GetVal())
			{
				item->cost = -1;
				item->increase = -1;
				item->round = -1;
				item->forSale = false;
				return;
			}

			// _UserSkill 인덱스 (0,1,2,3,4) -> 적용 스킬 레벨(1,2,3,4,5)
			INT32 percentage = domi::SkillProp::i()->GetUserSkill(item->type).GetIncrPercentage(skillLvToBuy);

			if (item->increase != percentage) 
				item->increase = percentage;

			if (item->lv != skillLvToBuy) 
				item->lv = skillLvToBuy;

			// sp 조건 처리
			INT32 cost = domi::SkillProp::i()->GetUserSkill(item->type).GetCostSp(skillLvToBuy).GetVal();
			if (item->cost != cost) item->cost = cost;

			// round 조건 처리
			INT32 minimumRoundRequired = domi::SkillProp::i()->GetUserSkill(item->type).GetMinRound(skillLvToBuy);
			if (item->round != minimumRoundRequired) 
				item->round = minimumRoundRequired;

			// 판매 여부
			item->forSale = true;
			if ((domi_state_info::i()->round < minimumRoundRequired) || 
				(sp < cost))
			{ 
				item->forSale = false; 
			}
		}
	};

	// 페이지 내 해당 아이템 업데이트
	iterator beg = m_items.begin() + (m_currPage * g_slot_cnt_per_page);

	INT32 pageItemCnt = 0; // 한 페이지의 아이템 개수
	if (m_currPage < m_totalPage)	pageItemCnt = g_slot_cnt_per_page;
	else							pageItemCnt = m_items.size() - (m_currPage * g_slot_cnt_per_page);
	iterator end = beg + pageItemCnt;

	std::for_each(beg, end, fn());

	mSlot.UpdateList(pageItemCnt);
}

void BattleSkillShopSlotSpItem::cb_Enabled_impl(INT32 idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	INT32 vidx = GetVectorIndex(m_currPage, idx);

	I3ASSERT(0 <= m_items[vidx]->type && m_items[vidx]->type < USER_SKILL_COUNT); 

#ifdef DEBUG
	INT32 lvdbg = DomiUserSkillMgr::i()->GetLevel(BattleSlotContext::i()->getMySlotIdx(), static_cast<USER_SKILL>(m_items[vidx]->type));
	INT32 maxLv = domi::SkillProp::i()->GetUserSkill(m_items[vidx]->type).level_max->GetVal();
	I3ASSERT(0 <= lvdbg && lvdbg <= maxLv);
#endif

	INT32 lv = DomiUserSkillMgr::i()->GetLevel(BattleSlotContext::i()->getMySlotIdx(), 
		static_cast<USER_SKILL>(m_items[vidx]->type));

	LuaState* L = UILua::CallLuaFunction(scn, "UpdateItems");

	i3Lua::PushStringUTF16To8(L, m_items[vidx]->wstr_name);
	i3Lua::PushStringUTF16To8(L, m_items[vidx]->explain);
	i3Lua::PushInteger(L, m_items[vidx]->shape_idx);
	i3Lua::PushInteger(L, m_items[vidx]->cost);
	i3Lua::PushInteger(L, domi_state_info::i()->sp.sp[BattleSlotContext::i()->getMySlotIdx()/2]);
	i3Lua::PushInteger(L, abs(m_items[vidx]->increase));
	i3Lua::PushInteger(L, lv);
	i3Lua::PushInteger(L, m_items[vidx]->round);
	i3Lua::PushBoolean(L, m_items[vidx]->forSale);
	i3Lua::PushInteger(L, idx); // 키입력 번호

	UILua::EndLuaFunction(L, 10);
}

void BattleSkillShopSlotSpItem::cb_Disabled_impl(INT32 idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	LuaState* L = UILua::CallLuaFunction(scn, "SetEmptySlot");
	UILua::EndLuaFunction(L, 0);
}

void BattleSkillShopSlotSpItem::cb_Enter_impl(INT32 idx, i3UIScene* scn)
{
	LuaState * L = UILua::CallLuaFunction(scn, "Enter");
	UILua::EndLuaFunction(L , 0);
}

void BattleSkillShopSlotSpItem::cb_Leave_impl(INT32 idx, i3UIScene* scn)
{
	LuaState * L = UILua::CallLuaFunction(scn,  "Leave");
	UILua::EndLuaFunction(L , 0);
}