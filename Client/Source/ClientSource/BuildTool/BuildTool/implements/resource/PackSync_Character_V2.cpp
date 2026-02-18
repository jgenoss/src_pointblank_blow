#include "stdafx.h"
#include "PackSync_Character_V2.h"
#include "PackRuleCreator_Chara.h"
#include "PackRuleCreator_Equipment.h"

namespace rsc
{
	void PackSync_Character_V2::_PackingByRule(const std::vector<fs::path>& fileRelPathList,
											   const fs::path& srcWorkdir,
											   const fs::path& dstWorkdir,
											   SyncLoggerPtr loggerPtr)
	{
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Chara>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Avatar>());

		m_ruleModule.Packing(fileRelPathList, srcWorkdir, dstWorkdir, loggerPtr);
	}
}