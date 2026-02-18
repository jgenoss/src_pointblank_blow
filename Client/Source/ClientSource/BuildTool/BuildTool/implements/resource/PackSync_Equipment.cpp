#include "stdafx.h"
#include "PackSync_Equipment.h"
#include "PackRule_EquipMask.h"
#include "PackRule_Avatar.h"

namespace rsc
{
	void PackSync_Equipment::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_EquipMask::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void PackSync_Avatar::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Avatar::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}