#pragma once

#include "IPackRule.h"

namespace rsc
{
	class PackRule_Chara : public IPackRule
	{
	public:
		PackRule_Chara(const std::string& strCharaName, 
					   const std::vector<PathPtr>& srcList, 
					   const Path& srcWorkdir, 
					   const Path& dstWorkdir);

		virtual ~PackRule_Chara();

		static void Creator(std::vector<PathPtr>& inout,
							const Path& srcWorkdir,
							const Path& dstWorkdir,
							std::vector<PackRulePtr>& out);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) override;

	private:
		const std::string m_strCharaName;
	};
}