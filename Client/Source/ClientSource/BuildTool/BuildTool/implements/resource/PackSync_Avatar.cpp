#include "stdafx.h"
#include "PackSync_Avatar.h"
#include "PackRule_Avatar.h"
#include "PackRuleCreator_Equipment.h"

namespace rsc
{
	void PackSync_Avatar::_PackingByRule(const std::vector<fs::path>& fileRelPathList,
										 const fs::path& srcWorkdir,
										 const fs::path& dstWorkdir,
										 SyncLoggerPtr logger)
	{
		// 무기 팩 규칙에 따라 입력된 파일들을 대상으로 패킹한다.

		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Avatar>());

		m_ruleModule.Packing(fileRelPathList, srcWorkdir, dstWorkdir, logger);
	}
}