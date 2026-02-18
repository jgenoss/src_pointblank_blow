#include "stdafx.h"
#include "GoodsFileCrawlerImpl_V3.h"
#include "GoodsFileCrawlerImpl_V2.h"

namespace rsc
{
	GoodsFileCrawlerImpl_V3::GoodsFileCrawlerImpl_V3(ContentsDataPtr contentsData,
													 const Path& workdir,
													 const Path& PEFRelPath,
													 const std::string& strCountryName)
		: GoodsFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName),
		m_V2Impl(std::make_unique<GoodsFileCrawlerImpl_V2>(contentsData, workdir, PEFRelPath, strCountryName))
	{}

	GoodsFileCrawlerImpl_V3::~GoodsFileCrawlerImpl_V3()
	{}

	bool GoodsFileCrawlerImpl_V3::LoadPef()
	{
		return m_V2Impl->LoadPef();
	}

	void GoodsFileCrawlerImpl_V3::GatherPefFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherPefFiles(out);
	}

	void GoodsFileCrawlerImpl_V3::GatherUITextures(std::vector<PathPtr>& out)
	{
		// V3 신규 UI 관련 작업 필요.
	}
}