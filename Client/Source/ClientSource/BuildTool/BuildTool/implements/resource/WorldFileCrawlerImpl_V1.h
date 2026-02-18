#pragma once

#include "WorldFileCrawlerImpl.h"

namespace rsc
{
	class WorldFileCrawlerImpl_V1 : public WorldFileCrawlerImpl
	{
	public:
		WorldFileCrawlerImpl_V1(ContentsDataPtr contentsData,
								const Path& workdir,
								const Path& PEFRelpath,
								const std::string& countryName);

		virtual ~WorldFileCrawlerImpl_V1();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) override;
		virtual void Gatheri3INS(std::vector<PathPtr>& out) override;
		virtual void GatherMinimapFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;
	};
}