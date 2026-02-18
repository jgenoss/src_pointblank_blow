#pragma once

#include "IPackRule.h"

namespace rsc
{
	class PackRule_Avatar : public IPackRule
	{
	public:
		PackRule_Avatar(const std::string& strAvatarFolderRelPath, 
						const std::vector<PathPtr>& srcList, 
						const Path& srcWorkdir, 
						const Path& dstWorkdir);

		virtual ~PackRule_Avatar();

		static void Creator(std::vector<PathPtr>& inout,
							const Path& srcWorkdir, 
							const Path& dstWorkdir,
							std::vector<PackRulePtr>& out);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) override;

	private:
		const std::string m_strAvatarFolderRelPath;
	};
}