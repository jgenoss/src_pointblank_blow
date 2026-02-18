#include "stdafx.h"
#include "GoodsFileCrawlerImpl_V2.h"

namespace rsc
{
	GoodsFileCrawlerImpl_V2::GoodsFileCrawlerImpl_V2(ContentsDataPtr contentsData,
													 const Path& workdir,
													 const Path& PEFRelPath,
													 const std::string& strCountryName)
		: GoodsFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{}

	GoodsFileCrawlerImpl_V2::~GoodsFileCrawlerImpl_V2()
	{}

	bool GoodsFileCrawlerImpl_V2::LoadPef()
	{
		return GoodsFileCrawlerImpl::LoadPef();
	}

	void GoodsFileCrawlerImpl_V2::GatherPefFiles(std::vector<PathPtr>& out)
	{
		GoodsFileCrawlerImpl::GatherPefFiles(out);
	}

	void GoodsFileCrawlerImpl_V2::GatherUITextures(std::vector<PathPtr>& out)
	{
		// 신규 UI 관련 작업 필요.
	}

	void GoodsFileCrawlerImpl_V2::GatherMiscs(std::vector<PathPtr>& out)
	{}
}