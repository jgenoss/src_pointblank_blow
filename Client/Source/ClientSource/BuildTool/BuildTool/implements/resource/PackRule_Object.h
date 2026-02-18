#pragma once

#include "IPackRule.h"

namespace rsc
{
	class PackRule_Object : public IPackRule
	{
	public:
		PackRule_Object(const std::string& strObjFolderRelPath,
						const std::vector<PathPtr>& srcList, 
						const Path& srcWorkdir, 
						const Path& dstWorkdir);

		virtual ~PackRule_Object();

		static void Creator(std::vector<PathPtr>& inout,
							const Path& srcWorkdir,
							const Path& dstWorkdir,
							std::vector<PackRulePtr>& out);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) override;

	private:
		std::string m_strObjFolderRelPath;
	};
}
