#include "stdafx.h"
#include "PackSync_World_V2.h"
#include "PackRuleCreator_World.h"
#include "PackRuleCreator_Sound.h"
#include "PackRuleCreator_Object.h"

namespace rsc
{
	void PackSync_World_V2::_PackingByRule(const std::vector<fs::path>& fileRelPathList,
										   const fs::path& srcWorkdir,
										   const fs::path& dstWorkdir,
										   SyncLoggerPtr logger)
	{
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World_Col>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World_Col_Hero>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World_Scene>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World_SubData>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World_TabMinimap>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_World_Textures>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Sound>());
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Object>());

		m_ruleModule.Packing(fileRelPathList, srcWorkdir, dstWorkdir, logger);
	}
}