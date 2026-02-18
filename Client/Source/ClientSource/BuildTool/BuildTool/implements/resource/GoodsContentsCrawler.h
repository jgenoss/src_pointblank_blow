#pragma once

#include "IContentsCrawler.h"
#include "RscTool/i3RscPartialFile.h"
#include "IPefable.h"

namespace rsc
{
	class GoodsContentsCrawler final : public IContentsCrawler, public IPefable
	{
	public:
		GoodsContentsCrawler(const Path& workdir, const Path& PEFRelPath);
		virtual ~GoodsContentsCrawler();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}