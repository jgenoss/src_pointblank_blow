#include "pch.h"
#include "UIBattlePopupSkillShop.h"
#include "UIBattlePopupSkillShopSlot.h"
#include "UIBattlePopupSkillShopSpSlot.h"
#include "UIBattlePopupSkillShopCashSlot.h"
#include "UIBattleFrame.h"
#include "UIUtil.h"

#include "../Mode/Domination/Domination_Defines.h"
#include "BattleSlotContext.h"

// 기획 문서 참조 : http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId=3083

extern "C" 
{
	int Close(LuaState * L)
	{
		if( UIBattleFrame::i() != nullptr)
			UIBattleFrame::i()->ClosePopup(UBP_SKILL_SHOP);
		return 0;
	}

	int GoPrePage(LuaState* L)
	{
		UIBattlePopupSkillShop * p = static_cast<UIBattlePopupSkillShop*>(GameUI::GetBattlePopup(UBP_SKILL_SHOP));
		if( p != nullptr)
			p->GoPrePage();
		return 0;
	}

	int GoNextPage(LuaState* L)
	{
		UIBattlePopupSkillShop * p = static_cast<UIBattlePopupSkillShop*>(GameUI::GetBattlePopup(UBP_SKILL_SHOP));
		if( p != nullptr)
			p->GoNextPage();
		return 0;
	}

	int GoSkillTab(LuaState* L)
	{
		UIBattlePopupSkillShop * p = static_cast<UIBattlePopupSkillShop*>(GameUI::GetBattlePopup(UBP_SKILL_SHOP));
		if( p != nullptr)
			p->GoSkillTab();
		return 0;
	}

	int GoItemTab(LuaState* L)
	{
		UIBattlePopupSkillShop * p = static_cast<UIBattlePopupSkillShop*>(GameUI::GetBattlePopup(UBP_SKILL_SHOP));
		if( p != nullptr)
			p->GoItemTab();
		return 0;
	}
}

LuaFuncReg SkillShopSlot_Glue[] =
{
	{ "Close",		Close		},
	{ "GoPrePage",	GoPrePage	},
	{ "GoNextPage", GoNextPage	},
	{ "GoSkillTab", GoSkillTab	},
	{ "GoItemTab",  GoItemTab	},
	{ nullptr, nullptr }
};


I3_CLASS_INSTANCE(UIBattlePopupSkillShop);

UIBattlePopupSkillShop::UIBattlePopupSkillShop() 
{
	m_slot[SKILLSHOP_SLOT_SP] = new BattleSkillShopSlotSpItem(this);
	MEMDUMP_NEW( m_slot[SKILLSHOP_SLOT_SP], sizeof( BattleSkillShopSlotSpItem));

	m_slot[SKILLSHOP_SLOT_CASH] = new BattleSkillShopSlotCashItem(this);
	MEMDUMP_NEW( m_slot[SKILLSHOP_SLOT_CASH], sizeof( BattleSkillShopSlotCashItem));
}

UIBattlePopupSkillShop::~UIBattlePopupSkillShop() 
{
	for (INT32 i = 0; i < SKILLSHOP_SLOT_MAX; i++)
	{
		I3_SAFE_DELETE( m_slot[i]);
	}
}

void UIBattlePopupSkillShop::OnCreate(i3GameNode* parent)
{
	UIBattlePopupBase::OnCreate(parent);

	AddScene("Scene/Domination/PBRe_Domi_gearskillpop.i3UIs", SkillShopSlot_Glue, true, false);

	for (INT32 i = 0; i < SKILLSHOP_SLOT_MAX; i++)
		m_slot[i]->OnCreate(parent);
}

void UIBattlePopupSkillShop::_InitializeAtLoad(i3UIScene* scn)
{
	UIBattlePopupBase::_InitializeAtLoad(scn);
}

bool UIBattlePopupSkillShop::OnEntranceStart(void* arg1, void* arg2)
{
	if (UIBattlePopupBase::OnEntranceStart(arg1, arg2) == false) return false;

	GoSkillTab();
	RefreshPage();

	return true;
}

void UIBattlePopupSkillShop::OnExitEnd()
{
	GoSkillTab();

	for (INT32 i=0; i<SKILLSHOP_SLOT_MAX; i++)
		m_slot[i]->m_currPage = 0;

	return UIBattlePopupBase::OnExitEnd();
}

void UIBattlePopupSkillShop::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();

	for (INT32 i = 0; i < SKILLSHOP_SLOT_MAX; i++)
		m_slot[i]->OnLoadAllScenes();
}

bool UIBattlePopupSkillShop::OnKeyInput(i3InputDeviceManager* input)
{
	if (UIBattlePopupBase::OnKeyInput(input) == true) return true;

	m_currSlot->ProcessKey();

	return true;
}

void UIBattlePopupSkillShop::OnUpdate(REAL32 tm)
{
	UIBattlePopupBase::OnUpdate(tm);

	m_currSlot->OnUpdate(tm);
}


void UIBattlePopupSkillShop::GetLeftTopPos(INT32& outLeft, INT32& outTop)
{
	i3UIFrameWnd* main_frame = (i3UIFrameWnd*)GetScene(0)->FindChildByName("frame_main");
	VEC3D* pos = main_frame->getAccumulatedPos();
	outLeft = (INT32)pos->x; outTop = (INT32)pos->y;
}

void UIBattlePopupSkillShop::RefreshPage()
{
	LuaState* L = _CallLuaFunction("SetCurPage");
	i3Lua::PushInteger(L, m_currSlot->m_currPage+1);
	i3Lua::PushInteger(L, m_currSlot->m_totalPage+1);
	_EndLuaFunction(L, 2);

	L = _CallLuaFunction("SetCurrSP");
	i3Lua::PushInteger(L, domi_state_info::i()->sp.sp[BattleSlotContext::i()->getMySlotIdx()/2]);
	_EndLuaFunction(L, 1);

	m_currSlot->InitItems();
	m_currSlot->UpdateItems();
}

void UIBattlePopupSkillShop::GoPrePage()
{
	if (m_currSlot->m_currPage <= 0) return;

	m_currSlot->m_currPage -= 1;

	RefreshPage();
}

void UIBattlePopupSkillShop::GoNextPage()
{
	if (m_currSlot->m_currPage >= m_currSlot->m_totalPage) return;

	m_currSlot->m_currPage += 1;

	RefreshPage();
}

void UIBattlePopupSkillShop::GoSkillTab()
{
	m_currSlot = m_slot[SKILLSHOP_SLOT_SP];
	m_currSlot->SetVisible(true);
	m_currSlot->SetEnableMouseOver(true);

	m_slot[SKILLSHOP_SLOT_CASH]->SetVisible(false);
	m_slot[SKILLSHOP_SLOT_CASH]->SetEnableMouseOver(false);

	RefreshPage();
}

void UIBattlePopupSkillShop::GoItemTab()
{
	m_currSlot = m_slot[SKILLSHOP_SLOT_CASH];
	m_currSlot->SetVisible(true);
	m_currSlot->SetEnableMouseOver(true);

	m_slot[SKILLSHOP_SLOT_SP]->SetVisible(false);
	m_slot[SKILLSHOP_SLOT_SP]->SetEnableMouseOver(false);

	RefreshPage();
}



