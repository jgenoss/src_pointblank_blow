#pragma once

#include "IContentsCrawler.h"
#include "RscTool/i3RscPartialFile.h"
#include "IPefable.h"

namespace rsc
{
	class CharacterContentsCrawler_V1 final : public IContentsCrawler, public IPefable
	{
	public:
		CharacterContentsCrawler_V1(const Path& pefWorkdir, const Path& pefRelPath);
		virtual ~CharacterContentsCrawler_V1();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}
