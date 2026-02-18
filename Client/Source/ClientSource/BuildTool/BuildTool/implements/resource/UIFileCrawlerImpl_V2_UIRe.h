#pragma once

#include "UIFileCrawlerImpl.h"

class i3UIScene;

namespace rsc
{
	class UIFileCrawlerImpl_V2_UIRe : public UIFileCrawlerImpl
	{
	public:
		UIFileCrawlerImpl_V2_UIRe(ContentsDataPtr contentsData,
								  const Path& workdir,
								  const Path& folderRelPath,
								  const std::string& strCountryName);

		virtual ~UIFileCrawlerImpl_V2_UIRe();

		virtual bool Load() override;
		virtual void GatherSceneFiles(std::vector<PathPtr>& out) override;
		virtual void GatherSceneAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;

	private:
		Path m_i3UIeRelPath;
	};
}