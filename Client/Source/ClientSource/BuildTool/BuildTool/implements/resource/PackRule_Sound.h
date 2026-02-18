#pragma once

#include "IPackRule.h"

namespace rsc
{
	class PackRule_Sound : public IPackRule
	{
	public:
		PackRule_Sound(const std::string& strSoundFolderRelPath,
					   const std::vector<PathPtr>& srcList, 
					   const Path& srcWorkdir, 
					   const Path& dstWorkdir);

		virtual ~PackRule_Sound();

		static void Creator(std::vector<PathPtr>& inout,
							const Path& srcWorkdir,
							const Path& dstWorkdir,
							std::vector<PackRulePtr>& out);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) override;

	private:
		std::string m_strSoundFolderRelPath;
	};
}