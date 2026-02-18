#pragma once

#include "GoodsFileCrawlerImpl.h"

namespace rsc
{
	class GoodsFileCrawlerImpl_V2 : public GoodsFileCrawlerImpl
	{
	public:
		GoodsFileCrawlerImpl_V2(ContentsDataPtr contentsData,
								const Path& workdir,
								const Path& PEFRelPath,
								const std::string& strCountryName);

		virtual ~GoodsFileCrawlerImpl_V2();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUITextures(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;
	};
}
