#pragma once

#include "IContentsCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class UIContentsCrawler_V2_UIRe : public IContentsCrawler, public IFilePathable
	{
	public:
		UIContentsCrawler_V2_UIRe(const Path& workdir, const Path& UIReFolderTgtRelPath);
		virtual ~UIContentsCrawler_V2_UIRe();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}