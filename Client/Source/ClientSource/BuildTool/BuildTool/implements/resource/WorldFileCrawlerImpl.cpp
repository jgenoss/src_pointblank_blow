#include "stdafx.h"
#include "WorldFileCrawlerImpl.h"
#include "RscTool/RSCFileMgr.h"
#include "RscTool/i3RscPartialFile.h"
#include "FolderFilesCrawler.h"
#include "i3aCrawler.h"
#include "i3sCrawler.h"
#include "i3Base/string/ext/extract_fileext.h"

namespace rsc
{
	WorldFileCrawlerImpl::WorldFileCrawlerImpl(ContentsDataPtr contentsData,
											   const Path& workdir,
											   const Path& PEFRelPath,
											   const std::string& strCountryName)
		: IPefable(workdir, PEFRelPath, strCountryName), m_contentsData(contentsData), m_workdir(workdir)
	{}

	WorldFileCrawlerImpl::~WorldFileCrawlerImpl() {}

	bool WorldFileCrawlerImpl::LoadPef()
	{
		if (m_contentsData.get() == nullptr)
			return false;

		if (!OpenPef())
			return false;

		if (IPefable::GetPefNationIdx() == -1)
			return false;

		i3ResourceFileObjectInfo2* registryKey = GetPef()->Findi3RegKeyRoot();
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, "Stage");
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strContentsName.c_str());
		if (registryKey == nullptr)
			return false;

		m_currRegistryKey = registryKey;

		return true;
	}

	void WorldFileCrawlerImpl::GatherPefFiles(std::vector<PathPtr>& out)
	{
		Path PEFRelPath(IPefable::GetRelativePath());

		out.push_back(std::make_shared<Path>(PEFRelPath));	// stage.pef
		out.push_back(std::make_shared<Path>(PEFRelPath.replace_extension(".i3RegXML")));	// stage.i3RegXML.
	}

	void WorldFileCrawlerImpl::GatherUIFiles(std::vector<PathPtr>& out)
	{}

	void WorldFileCrawlerImpl::Gatheri3INS(std::vector<PathPtr>& out)
	{
		const std::string& strResourceFolderName = m_contentsData->strContentsName;	// stage.pef의 registry key 값은 리소스 폴더명과 파일명으로 사용되고 있다.

		Path i3INSRelPath("Scene");
		i3INSRelPath /= strResourceFolderName;
		i3INSRelPath /= strResourceFolderName;
		i3INSRelPath += ".i3INS";

		out.push_back(std::make_shared<Path>(i3INSRelPath));
	}

	void WorldFileCrawlerImpl::GatherMinimapFiles(std::vector<PathPtr>& out)
	{
		const std::string& strResourceFolderName = m_contentsData->strContentsName;	// stage.pef의 regitry key 값은 리소스 폴더명과 파일명으로 사용되고 있다.

		Path imagesRelPath("World");
		imagesRelPath /= strResourceFolderName;
		imagesRelPath /= "Images";

		std::vector<PathPtr> imagesRelPathList;
		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(GetWorkdir());
		folderCwl.SetRelativePath(imagesRelPath);
		folderCwl.operator()(imagesRelPathList);

		std::copy(imagesRelPathList.begin(), imagesRelPathList.end(), std::back_inserter(out));
	}

	void WorldFileCrawlerImpl::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		const std::string& strResourceFolderName = m_contentsData->strContentsName;

		Path i3RSCFileRelPath("Scene");
		i3RSCFileRelPath /= strResourceFolderName;
		i3RSCFileRelPath /= strResourceFolderName;
		i3RSCFileRelPath += ".RSC";

		out.push_back(std::make_shared<Path>(i3RSCFileRelPath));

		RscFileInfoList info;
		if (LoadRscFileList_Acc(GetWorkdir().string().c_str(), i3RSCFileRelPath.string().c_str(), info))
		{
			for (auto &relPath : info.m_path_list)
				out.push_back(std::make_shared<Path>(relPath.c_str()));
		}

		i3::stack_string ext;
		i3aCrawler i3aCwl;	i3aCwl.SetWorkdir(GetWorkdir());
		i3sCrawler i3sCwl;	i3sCwl.SetWorkdir(GetWorkdir());

		for (auto &relPath : info.m_path_list)
		{
			i3::extract_fileext(relPath, ext);

			if (i3::generic_is_iequal(ext, "I3A"))
			{
				i3aCwl.SetRelativePath(relPath.c_str());
				i3aCwl.operator()(out);
			}
			else
			if (i3::generic_is_iequal(ext, "I3S"))
			{
				i3sCwl.SetRelativePath(relPath.c_str());
				i3sCwl.operator()(out);
			}
		}
	}
}