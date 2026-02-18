#pragma once

#include "IPackRule.h"

namespace rsc
{
	class PackRule_World_Col_Hero : public IPackRule
	{
	public:
		PackRule_World_Col_Hero(const std::string& strWorldName,
								const std::vector<PathPtr>& srcList,
								const Path& srcWorkdir, 
								const Path& dstWorkdir);

		virtual ~PackRule_World_Col_Hero();

		static void Creator(std::vector<PathPtr>& inout,
							const Path& srcWorkdir,
							const Path& dstWorkdir,
							std::vector<PackRulePtr>& out);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) override;

	private:
		const std::string m_strWorldName;
	};
}