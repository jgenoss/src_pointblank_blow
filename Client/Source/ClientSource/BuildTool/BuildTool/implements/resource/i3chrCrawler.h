#pragma once

#include "IFileCrawler.h"
#include "IFilePathable.h"
#include "RscTool/i3RscPartialFile.h"

namespace rsc
{
	class i3chrCrawler : public IFileCrawler, public IFilePathable
	{
	public:
		i3chrCrawler() = default;
		i3chrCrawler::i3chrCrawler(const Path& workdir, const Path& i3chrRelPath);
		virtual ~i3chrCrawler();

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;
	};
}