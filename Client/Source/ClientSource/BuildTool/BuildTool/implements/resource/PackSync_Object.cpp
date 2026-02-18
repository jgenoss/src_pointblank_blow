#include "stdafx.h"
#include "PackSync_Object.h"
#include "PackRule_Object.h"

namespace rsc
{
	void PackSync_Object::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_Object::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}