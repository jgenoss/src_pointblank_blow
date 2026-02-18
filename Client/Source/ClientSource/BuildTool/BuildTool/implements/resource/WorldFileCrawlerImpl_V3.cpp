#include "stdafx.h"
#include "WorldFileCrawlerImpl_V3.h"
#include "WorldFileCrawlerImpl_V2.h"
#include "FolderFilesCrawler.h"
#include <boost/format.hpp>

namespace rsc
{
	WorldFileCrawlerImpl_V3::WorldFileCrawlerImpl_V3(ContentsDataPtr contentsData,
													 const Path& workdir,
													 const Path& PEFRelPath,
													 const std::string& strCountryName)
		: WorldFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName),
		m_V2Impl(std::make_unique<WorldFileCrawlerImpl_V2>(contentsData, workdir, PEFRelPath, strCountryName))
	{}

	WorldFileCrawlerImpl_V3::~WorldFileCrawlerImpl_V3()
	{}

	bool WorldFileCrawlerImpl_V3::LoadPef()
	{
		return m_V2Impl->LoadPef();
	}

	void WorldFileCrawlerImpl_V3::GatherPefFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherPefFiles(out);
	}

	void WorldFileCrawlerImpl_V3::GatherUIFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherUIFiles(out);
	}

	void WorldFileCrawlerImpl_V3::Gatheri3INS(std::vector<PathPtr>& out)
	{
		m_V2Impl->Gatheri3INS(out);
	}

	void WorldFileCrawlerImpl_V3::GatherMinimapFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherTabMinimapFiles(out);
	}

	void WorldFileCrawlerImpl_V3::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherRSCAssocFiles(out);
	}

	void WorldFileCrawlerImpl_V3::GatherMiscs(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherMiscs(out);
	}
}