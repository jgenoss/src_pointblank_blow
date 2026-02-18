#pragma once

#include "IFileCrawler.h"

namespace rsc
{
	class FileCrawlerNull : public IFileCrawler
	{
	public:
		virtual ~FileCrawlerNull() {}

	protected:
		virtual bool _Load() { return false; }
		virtual bool _Gather(std::vector<PathPtr>& out) { return false; }
	};
}