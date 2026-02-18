#include "stdafx.h"
#include "WorldFileCrawlerImpl_V2.h"
#include "FolderFilesCrawler.h"
#include <boost/format.hpp>

namespace rsc
{
	WorldFileCrawlerImpl_V2::WorldFileCrawlerImpl_V2(ContentsDataPtr contentsData,
													 const Path& workdir,
													 const Path& PEFRelPath,
													 const std::string& strCountryName)
		: WorldFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{}

	WorldFileCrawlerImpl_V2::~WorldFileCrawlerImpl_V2()
	{}

	bool WorldFileCrawlerImpl_V2::LoadPef()
	{
		return WorldFileCrawlerImpl::LoadPef();
	}

	void WorldFileCrawlerImpl_V2::GatherPefFiles(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::GatherPefFiles(out);
	}

	void WorldFileCrawlerImpl_V2::GatherUIFiles(std::vector<PathPtr>& out)
	{}

	void WorldFileCrawlerImpl_V2::Gatheri3INS(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::Gatheri3INS(out);
	}

	void WorldFileCrawlerImpl_V2::GatherMinimapFiles(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::GatherMinimapFiles(out);

		GatherTabMinimapFiles(out);
	}

	void WorldFileCrawlerImpl_V2::GatherTabMinimapFiles(std::vector<PathPtr>& out)
	{
		// ÅÇ¹Ì´Ï¸Ê.
		std::vector<PathPtr> tabMinimapFileRelPathList;
		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(GetWorkdir());
		folderCwl.SetRelativePath(boost::io::str(boost::format("World/%s/TabMinimap") % m_contentsData->strContentsName));
		folderCwl.operator()(tabMinimapFileRelPathList);

		std::copy(tabMinimapFileRelPathList.begin(), tabMinimapFileRelPathList.end(), std::back_inserter(out));

		std::vector<PathPtr> imagesRelPathList;
		folderCwl.SetRelativePath(boost::io::str(boost::format("World/%s/Images_NoText") % m_contentsData->strContentsName));
		folderCwl.operator()(imagesRelPathList);

		std::copy(imagesRelPathList.begin(), imagesRelPathList.end(), std::back_inserter(out));
	
	}

	void WorldFileCrawlerImpl_V2::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::GatherRSCAssocFiles(out);
	}

	void WorldFileCrawlerImpl_V2::GatherMiscs(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>("config/BCV3.i3Game"));
		out.push_back(std::make_shared<Path>("config/BCNewUI.i3Game"));

		// .dds ÆÄÀÏ : ±¤¿ø È¿°ú °ü·Ã ÆÄÀÏ.
		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(GetWorkdir());
		folderCwl.SetRelativePath(boost::io::str(boost::format("World/%s/") % m_contentsData->strContentsName));
		folderCwl.SetRecursive(false);
		folderCwl.SetFileSearchKeys({ ".dds" });
		folderCwl.operator()(out);
	}
}