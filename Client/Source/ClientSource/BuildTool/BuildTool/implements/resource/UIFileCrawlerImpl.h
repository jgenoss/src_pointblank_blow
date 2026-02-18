#pragma once

#include "ContentsDefines.h"
#include "IFilePathable.h"

namespace rsc
{
	class UIFileCrawlerImpl : public IFilePathable
	{
	public:
		UIFileCrawlerImpl(ContentsDataPtr contentsData,
						  const Path& workdir,
						  const Path& folderRelPath,
						  const std::string& strCountryName);

		virtual ~UIFileCrawlerImpl() = 0;

		virtual bool Load() = 0;
		virtual void GatherSceneFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherSceneAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMiscs(std::vector<PathPtr>& out) {}

	protected:
		ContentsDataPtr m_contentsData = nullptr;
	};
}
