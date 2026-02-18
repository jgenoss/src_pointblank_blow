#include "pch.h"
#include "UIBattlePopupSkillShopCashSlot.h"
#include "../Mode/Domination/Domination_Defines.h"
#include "HUD/UIHUD_Domination_Manager.h"
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
	int RequestUse(LuaState * L)
	{
		g_this->Request();
		return 0;
	}

	int Purchase(LuaState* L)
	{
		static_cast<BattleSkillShopSlotCashItem*>(g_this)->OpenBattleShop();
		return 0;
	}
}

LuaFuncReg battle_skill_shop_cash_slot_glue[] =
{
	{ "RequestUse", RequestUse	},
	{ "Purchase",	Purchase	},
	{ nullptr, nullptr}
};

//----------------------------------------------
// class BattleSkillShopSlotCashItem
//----------------------------------------------
namespace
{
	inline bool IsSPSufficient(const INT32& itemType, INT32 min_remain)
	{
		CUIHudDomi_Skill* hud = static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL));

		UINT16 curSP = domi_state_info::i()->sp.sp[BattleSlotContext::i()->getMySlotIdx()/2];
		I3ASSERT( hud->GetSPMax() < 32768);
		UINT16 maxSP = (INT16) hud->GetSPMax();
		UINT16 remain = maxSP-curSP;

		return remain > min_remain ? false : true;
	}
}

BattleSkillShopSlotCashItem::BattleSkillShopSlotCashItem(UIBattlePopupSkillShop* p) : BattleSkillShopSlot(p)
{
	g_this = this;

	const i3::vector<DomiCashItemInfo*> & domi_cash_item_list = g_pShopItemInfoDataBase->get_insg_domi_cash_item_info_list();

	UINT32 size = domi_cash_item_list.size();
	for (UINT32 i = 0; i < size; i++)
	{
		DOMI_SKILLSHOP_CASH_ITEM* data = new DOMI_SKILLSHOP_CASH_ITEM;
		MEMDUMP_NEW( data, sizeof( DOMI_SKILLSHOP_CASH_ITEM));

		data->type = i;
		data->wstr_name = domi_cash_item_list[i]->GetScriptName();
		data->explain = domi_cash_item_list[i]->GetDescription();
		data->increase = domi_cash_item_list[i]->GetSpIncrease();
		data->group = domi_cash_item_list[i]->GetGroupIndex();
		data->shape_idx = domi_cash_item_list[i]->GetShapeIndex();
		data->min_remain = domi_cash_item_list[i]->GetMinRemain();
		data->possessionNum = 0;

		m_items.push_back(data);
	}

	m_totalPage = m_items.size() / g_slot_cnt_per_page;
}

BattleSkillShopSlotCashItem::~BattleSkillShopSlotCashItem()
{
	for (size_t i=0; i < m_items.size(); i++)
	{
		I3_SAFE_DELETE( m_items[i]);
	}
	m_items.clear();
}

void BattleSkillShopSlotCashItem::OnCreate(i3GameNode * pParent)
{
	mSlot.EnableScroll(false);
	mSlot.OnCreate((UIBase*)m_parent, g_total_row_slot, g_total_col_slot);
	mSlot.RegisterSlotScene("Scene/Domination/PBRe_Domi_gearskillpop_cash_slot.i3UIs", battle_skill_shop_cash_slot_glue, true);
}

void BattleSkillShopSlotCashItem::OnUpdate(REAL32 tm)
{
	BattleSkillShopSlot::OnUpdate(tm);
	UpdateItems();
}

void BattleSkillShopSlotCashItem::ProcessKey()
{
	i3InputKeyboard* kb = g_pFramework->getKeyboard();

	for (size_t i = 0; i < 8; i++)
	{
		if (i < m_items.size())
		{
			if (kb->GetStrokeState(static_cast<I3I_KEY>(i+1)))
			{
				if ((m_items[i]->possessionNum > 0) && (IsSPSufficient(m_items[i]->type, m_items[i]->min_remain) == false))
					Request(i);
				else if (m_items[i]->possessionNum == 0 )
					OpenBattleShop(i);
			}
		}
	}
}

void BattleSkillShopSlotCashItem::Request(INT32 slotIdx)
{
	INT32 vidx = GetVectorIndex(m_currPage, slotIdx);
	if (m_items[vidx]->possessionNum <= 0) return;

	INVEN_BUFFER* ib = nullptr;//CInvenList::i()->FindCashItem( (CASHITEM_GROUP_TYPE)m_items[vidx]->group );
	if( ib == 0 ) return;

	PACKET_DOMINATION_USE_ITEM_REQ i;
	i._TItemWareIdx = ib->_TItemWareDBIdx;
	GameEventSender::i()->SetEvent(EVENT_DOMI_USE_ITEM, reinterpret_cast<void*>(&i));
}

// 보유 개수가 0인 경우 야전상점 오픈하여 구매할 수 있도록.
void BattleSkillShopSlotCashItem::OpenBattleShop()
{
	//INT32 slotIdx = iSlot::apply_select();

	//INT32 vidx = GetVectorIndex(m_currPage, slotIdx);
	//domi_util::buy_domi_cash_item( (CASHITEM_GROUP_TYPE)m_items[vidx]->group );
}

// 보유 개수가 0인 경우 야전상점 오픈하여 구매할 수 있도록.
void BattleSkillShopSlotCashItem::OpenBattleShop(INT32 slotIdx)
{
	//INT32 vidx = GetVectorIndex(m_currPage, slotIdx);
	//domi_util::buy_domi_cash_item( (CASHITEM_GROUP_TYPE)m_items[vidx]->group );
}

void BattleSkillShopSlotCashItem::DecreaseItemPossessionNum(INT32 group)
{
	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (m_items[i]->group == group)
		{
			//m_items[i]->possessionNum = domi_util::get_cash_item_count( (CASHITEM_GROUP_TYPE)m_items[i]->group );
			break;
		}
	}
}

void BattleSkillShopSlotCashItem::InitItems()
{
	for (size_t i = 0; i < m_items.size(); i++)
	{
		m_items[i]->type = i;

		DomiCashItemInfo* domi_cash_item = g_pShopItemInfoDataBase->find_domi_cash_item_info( i );
		if( domi_cash_item != 0 )
		{
			m_items[i]->wstr_name = domi_cash_item->GetScriptName();
			m_items[i]->explain = domi_cash_item->GetDescription();
			m_items[i]->increase = domi_cash_item->GetSpIncrease();
			m_items[i]->group = domi_cash_item->GetGroupIndex();
			m_items[i]->shape_idx = domi_cash_item->GetShapeIndex();
			m_items[i]->min_remain = domi_cash_item->GetMinRemain();
			m_items[i]->possessionNum = 0;// domi_util::get_cash_item_count((CASHITEM_GROUP_TYPE)m_items[i]->group);
		}
		else
		{
			m_items[i]->wstr_name = L"";
			m_items[i]->explain.clear();
			m_items[i]->increase = 0;
			m_items[i]->group = 0;
			m_items[i]->shape_idx = -1;
			m_items[i]->min_remain = 0;
			m_items[i]->possessionNum = 0;
		}
	}

	//보유 아이템이 없고, 상점 구입 못 하면 삭제 처리한다.
	i3::vector<DOMI_SKILLSHOP_CASH_ITEM*>::iterator it = m_items.begin();
	while( it != m_items.end() )
	{
		DOMI_SKILLSHOP_CASH_ITEM* cash_item = *it;

		if( cash_item->possessionNum == 0 ) 
		{
			if( false ) //domi_util::is_buyable_domi_cash_item( (CASHITEM_GROUP_TYPE)cash_item->group ) == false )
			{
				I3_SAFE_DELETE( cash_item);
				it = m_items.erase(it);
				continue;
			}
		}

		++it;
	}

	m_totalPage = m_items.size() / g_slot_cnt_per_page;
}

void BattleSkillShopSlotCashItem::UpdateItems()
{
	if (m_items.empty()) return;

	INT32 pageItemCnt = 0; // 한 페이지의 아이템 개수
	if (m_currPage < m_totalPage)	pageItemCnt = g_slot_cnt_per_page;
	else							pageItemCnt = m_items.size() - (m_currPage * g_slot_cnt_per_page);

	mSlot.UpdateList(pageItemCnt);
}

void BattleSkillShopSlotCashItem::cb_Enabled_impl(INT32 idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	LuaState* L = UILua::CallLuaFunction(scn, "UpdateItems");

	INT32 vidx = GetVectorIndex(m_currPage, idx);

	i3Lua::PushBoolean(L, ((m_items[vidx]->possessionNum > 0) && (IsSPSufficient(m_items[vidx]->type, m_items[vidx]->min_remain) == false)));
	i3Lua::PushInteger(L, m_items[vidx]->type);
	i3Lua::PushStringUTF16To8(L, m_items[vidx]->wstr_name);
	i3Lua::PushStringUTF16To8(L, m_items[vidx]->explain);
	i3Lua::PushInteger(L, m_items[vidx]->possessionNum);
	i3Lua::PushInteger(L, abs(m_items[vidx]->increase));
	i3Lua::PushInteger(L, m_items[vidx]->shape_idx);
	i3Lua::PushInteger(L, idx); // 키입력 번호

	UILua::EndLuaFunction(L, 8);
}

void BattleSkillShopSlotCashItem::cb_Disabled_impl(INT32 idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	LuaState* L = UILua::CallLuaFunction(scn, "SetEmptySlot");
	UILua::EndLuaFunction(L, 0);
}

void BattleSkillShopSlotCashItem::cb_Enter_impl(INT32 idx, i3UIScene* scn)
{
	LuaState * L = UILua::CallLuaFunction(scn, "Enter");
	UILua::EndLuaFunction(L , 0);
}

void BattleSkillShopSlotCashItem::cb_Leave_impl(INT32 idx, i3UIScene* scn)
{
	LuaState * L = UILua::CallLuaFunction(scn,  "Leave");
	UILua::EndLuaFunction(L , 0);
}