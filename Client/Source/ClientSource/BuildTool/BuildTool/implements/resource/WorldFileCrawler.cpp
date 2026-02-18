#include "stdafx.h"
#include "WorldFileCrawler.h"
#include "WorldFileCrawlerImpl.h"
#include "WorldFileCrawlerImpl_V1.h"
#include "WorldFileCrawlerImpl_V2.h"

namespace rsc
{
	WorldFileCrawler::WorldFileCrawler(WorldFileCrawlerImpl* impl)
		: m_impl(impl)
	{}

	WorldFileCrawler::~WorldFileCrawler() {}

	bool WorldFileCrawler::_Load()
	{
		return m_impl->LoadPef();
	}

	bool WorldFileCrawler::_Gather(std::vector<PathPtr>& out)
	{
		m_impl->GatherPefFiles(out);
		m_impl->GatherUIFiles(out);
		m_impl->Gatheri3INS(out);
		m_impl->GatherMinimapFiles(out);
		m_impl->GatherRSCAssocFiles(out);
		m_impl->GatherMiscs(out);

		return true;
	}

	IFileCrawler* WorldFileCrawler::Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v1 =
			new WorldFileCrawlerImpl_V1(contentsData, workdir, "config/Stage.pef", strCountryName);

		return new WorldFileCrawler(impl_v1);
	}

	IFileCrawler* WorldFileCrawler::Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v2 =
			new WorldFileCrawlerImpl_V2(contentsData, workdir, "config/Stage.pef", strCountryName);

		return new WorldFileCrawler(impl_v2);
	}

	IFileCrawler* WorldFileCrawler::Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		return Creator_V2(contentsData, workdir, strCountryName);	// V2와 동일하다.
	}
}