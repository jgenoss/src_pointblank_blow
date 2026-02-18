#include "stdafx.h"
#include "PackSync_ShopGoods.h"
#include "PackRule_GuiItem.h"

namespace rsc
{
	void PackSync_ShopGoods::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Gui_Item::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}