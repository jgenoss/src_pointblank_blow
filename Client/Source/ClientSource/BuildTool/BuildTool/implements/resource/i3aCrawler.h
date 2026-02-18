#pragma once

#include "IFileCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class i3aCrawler : public IFileCrawler, public IFilePathable
	{
	public:
		i3aCrawler() = default;
		i3aCrawler(const Path& workdir, const Path& i3aRelPath);
		virtual ~i3aCrawler();

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;
	};
}