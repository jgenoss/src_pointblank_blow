#pragma once

#include "IContentsCrawler.h"

namespace rsc
{
	class UIContentsCrawler_V3 : public IContentsCrawler
	{
	public:
		UIContentsCrawler_V3(const Path& workdir, const std::vector<Path>& UIFolderRelPathList);
		virtual ~UIContentsCrawler_V3();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;

		const Path m_workdir;
		std::vector<Path> m_UIFolderRelPathList;
	};
}
