#include "stdafx.h"
#include "GoodsFileCrawler.h"
#include "GoodsFileCrawlerImpl.h"
#include "GoodsFileCrawlerImpl_V1.h"
#include "GoodsFileCrawlerImpl_V2.h"
#include "GoodsFileCrawlerImpl_V3.h"

namespace rsc
{
	GoodsFileCrawler::GoodsFileCrawler(GoodsFileCrawlerImpl* impl)
		: m_impl(impl)
	{}

	GoodsFileCrawler::~GoodsFileCrawler()
	{}

	bool GoodsFileCrawler::_Load()
	{
		return m_impl->LoadPef();
	}

	bool GoodsFileCrawler::_Gather(std::vector<PathPtr>& out)
	{
		m_impl->GatherPefFiles(out);
		m_impl->GatherUITextures(out);
		m_impl->GatherMiscs(out);

		return true;
	}

	IFileCrawler* GoodsFileCrawler::Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v1 = new GoodsFileCrawlerImpl_V1(contentsData, workdir, "config/Goods.pef", strCountryName);

		return new GoodsFileCrawler(impl_v1);
	}

	IFileCrawler* GoodsFileCrawler::Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v2 = new GoodsFileCrawlerImpl_V2(contentsData, workdir, "config/Goods.pef", strCountryName);

		return new GoodsFileCrawler(impl_v2);
	}

	IFileCrawler* GoodsFileCrawler::Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v3 = new GoodsFileCrawlerImpl_V3(contentsData, workdir, "config/Goods.pef", strCountryName);

		return new GoodsFileCrawler(impl_v3);
	}
}