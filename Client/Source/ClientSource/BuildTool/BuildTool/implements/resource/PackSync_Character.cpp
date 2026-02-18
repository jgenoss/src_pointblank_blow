#include "stdafx.h"
#include "PackSync_Character.h"
#include "PackRule_Avatar.h"
#include "PackRule_Chara.h"
#include "PackRule_EquipMask.h"

namespace rsc
{
	void PackSync_Character_V1::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Chara::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_EquipMask::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void PackSync_Character_V2::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Chara::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Avatar::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}