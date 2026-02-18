#pragma once

#include "GoodsFileCrawlerImpl.h"

namespace rsc
{
	class GoodsFileCrawlerImpl_V2;

	class GoodsFileCrawlerImpl_V3 : public GoodsFileCrawlerImpl
	{
	public:
		GoodsFileCrawlerImpl_V3(ContentsDataPtr contentsData,
								const Path& workdir,
								const Path& PEFRelPath,
								const std::string& strCountryName);

		virtual ~GoodsFileCrawlerImpl_V3();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUITextures(std::vector<PathPtr>& out) override;

	private:
		using V2ImplPtr = std::unique_ptr<GoodsFileCrawlerImpl_V2>;
		V2ImplPtr m_V2Impl;
	};
}

