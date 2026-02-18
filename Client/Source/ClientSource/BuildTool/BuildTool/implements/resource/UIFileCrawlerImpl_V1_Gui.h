#pragma once

#include "UIFileCrawlerImpl.h"

namespace rsc
{
	class UIFileCrawlerImpl_V1_Gui : public UIFileCrawlerImpl
	{
	public:
		UIFileCrawlerImpl_V1_Gui(ContentsDataPtr contentsData,
								 const Path& workdir,
								 const Path& folderRelPath,
								 const std::string& strCountryName);

		virtual ~UIFileCrawlerImpl_V1_Gui();
		
		virtual bool Load() override;
		virtual void GatherSceneFiles(std::vector<PathPtr>& out) override;
		virtual void GatherSceneAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;
	};
}