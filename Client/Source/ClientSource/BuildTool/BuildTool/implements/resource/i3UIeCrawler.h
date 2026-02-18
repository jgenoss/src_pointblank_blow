#pragma once

#include "IFileCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class i3UIeCrawler : public IFileCrawler, public IFilePathable
	{
	public:
		i3UIeCrawler() = default;
		i3UIeCrawler(const Path& workdir, const Path& i3UIeRelPath);
		virtual ~i3UIeCrawler();

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;
	};
}