#pragma once

#include "WorldFileCrawlerImpl.h"

namespace rsc
{
	class WorldFileCrawlerImpl_V2;

	class WorldFileCrawlerImpl_V3 : public WorldFileCrawlerImpl
	{
	public:
		WorldFileCrawlerImpl_V3(ContentsDataPtr contentsData,
								const Path& workdir,
								const Path& PEFRelpath,
								const std::string& strCountryName);

		virtual ~WorldFileCrawlerImpl_V3();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) override;
		virtual void Gatheri3INS(std::vector<PathPtr>& out) override;
		virtual void GatherMinimapFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;

	private:
		using V2ImplPtr = std::unique_ptr<WorldFileCrawlerImpl_V2>;
		V2ImplPtr m_V2Impl;
	};
}
