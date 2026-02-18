#include "stdafx.h"
#include "PackSync_Weapon.h"
#include "PackRule_Weapon.h"
#include "PackRule_Sound.h"
#include "PackRule_Chara.h"
#include "PackRule_UIWeapon.h"
#include "PackRule_GuiItem.h"

namespace rsc
{
	void PackSync_Weapon_V1::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Weapon::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Chara::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Sound::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_UIWeapon::CreatorV1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Gui_Item::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void PackSync_Weapon_V2::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Weapon::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Chara::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Sound::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_UIWeapon::CreatorV2, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void PackSync_Weapon_V3::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Weapon::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Chara::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Sound::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_UIWeapon::CreatorV3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}