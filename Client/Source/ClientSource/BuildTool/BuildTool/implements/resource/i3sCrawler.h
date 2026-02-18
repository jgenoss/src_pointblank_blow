#pragma once

#include "IFileCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class i3sCrawler : public IFileCrawler, public IFilePathable
	{
	public:
		i3sCrawler() = default;
		i3sCrawler(const Path& workdir, const Path& i3sRelPath);
		virtual ~i3sCrawler();

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;
	};
}