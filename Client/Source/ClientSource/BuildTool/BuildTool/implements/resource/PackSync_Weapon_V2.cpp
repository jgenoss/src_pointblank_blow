#include "stdafx.h"
#include "PackSync_Weapon_V2.h"
#include "PackRuleCreator_Weapon.h"
#include "PackRuleCreator_Sound.h"
#include "PackRuleCreator_Chara.h"

namespace rsc
{
	void PackSync_Weapon_V2::_PackingByRule(const std::vector<fs::path>& fileRelPathList,
											const fs::path& srcWorkdir, 
											const fs::path& dstWorkdir,
											SyncLoggerPtr logger)
	{
		// 무기 팩 규칙에 따라 입력된 파일들을 대상으로 패킹한다.

		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Weapon>()); // 'Weapon/' 경로에 있는 무기 고유 리소스.
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Chara>()); // 캐릭터 i3AnimPack.
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_Sound>()); // 사운드.
		m_ruleModule.RegisterRuleCreator(std::make_unique<PackRuleCreator_UIWeapon_V2>());
		
		m_ruleModule.Packing(fileRelPathList, srcWorkdir, dstWorkdir, logger);
	}
}