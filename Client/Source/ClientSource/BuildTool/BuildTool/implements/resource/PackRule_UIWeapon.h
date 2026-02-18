#pragma once

#include "IPackRule.h"

namespace rsc
{
	class PackRule_UIWeapon : public IPackRule
	{
	public:
		PackRule_UIWeapon(const Path& representPath,
						  const std::vector<PathPtr>& srcList, 
						  const Path& srcWorkdir, 
						  const Path& dstWorkdir);

		virtual ~PackRule_UIWeapon();

		static void CreatorV1(std::vector<PathPtr>& inout,
							  const Path& srcWorkdir,
							  const Path& dstWorkdir,
							  std::vector<PackRulePtr>& out);

		static void CreatorV2(std::vector<PathPtr>& inout,
							  const Path& srcWorkdir,
							  const Path& dstWorkdir,
							  std::vector<PackRulePtr>& out);

		static void CreatorV3(std::vector<PathPtr>& inout,
							  const Path& srcWorkdir,
							  const Path& dstWorkdir,
							  std::vector<PackRulePtr>& out);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) override;

	private:
		const Path m_representPath;	// 대표 경로. 패킹 규칙을 추출할 수 있는. 
	};
}