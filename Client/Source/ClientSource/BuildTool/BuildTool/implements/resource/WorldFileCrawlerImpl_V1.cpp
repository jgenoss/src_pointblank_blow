#include "stdafx.h"
#include "WorldFileCrawlerImpl_V1.h"
#include "WorldFileCrawlerImpl.h"
#include <boost/lexical_cast.hpp>
#include "FolderFilesCrawler.h"
#include <boost/format.hpp>

namespace rsc
{
	WorldFileCrawlerImpl_V1::WorldFileCrawlerImpl_V1(ContentsDataPtr contentsData,
													 const Path& workdir,
													 const Path& PEFRelPath,
													 const std::string& strCountryName)
		: WorldFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{}

	WorldFileCrawlerImpl_V1::~WorldFileCrawlerImpl_V1() 
	{}

	bool WorldFileCrawlerImpl_V1::LoadPef()
	{
		return WorldFileCrawlerImpl::LoadPef();
	}

	void WorldFileCrawlerImpl_V1::GatherPefFiles(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::GatherPefFiles(out);
	}

	void WorldFileCrawlerImpl_V1::GatherUIFiles(std::vector<PathPtr>& out)
	{
		const int idxNation = IPefable::GetPefNationIdx();

		i3ResourceFileObjectInfo2* registryKey = nullptr;
		registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_UIShapeIndex");
		if (registryKey)
		{
			INT32 shapeIdx = 0;
			if (GetPef()->Readi3RegIntArrayElement(registryKey, idxNation, shapeIdx))
			{
				const int shapeCapacity_i3i = 66;
				const int idx_i3i = shapeIdx / shapeCapacity_i3i;

				std::string strPostfix(boost::lexical_cast<std::string>(idx_i3i));
				strPostfix += ".i3i";

				std::string strMapi3iRelPath("Gui/Interface/UI_Source_MapImage");
				strMapi3iRelPath += strPostfix;

				out.push_back(std::make_shared<Path>(strMapi3iRelPath.c_str()));
				//outRelPathPtrList.push_back(std::make_shared<Path>("Gui/Interface/UI_Source_MapMode.i3i"));	// 국가별 번역이 필요한 이미지이므로 주석 처리함.
			}
		}
	}

	void WorldFileCrawlerImpl_V1::Gatheri3INS(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::Gatheri3INS(out);
	}

	void WorldFileCrawlerImpl_V1::GatherMinimapFiles(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::GatherMinimapFiles(out);
	}

	void WorldFileCrawlerImpl_V1::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		WorldFileCrawlerImpl::GatherRSCAssocFiles(out);
	}

	void WorldFileCrawlerImpl_V1::GatherMiscs(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>("config/BC.i3Game"));
		out.push_back(std::make_shared<Path>("config/BCNewUI.i3Game"));	// 이건 1.1 버전용이라는데 같이 포함시켜주자. 1.1 버전 분기 처리가 안되어 있기 때문.

		// .dds 파일 : 광원 효과 관련 파일.
		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(GetWorkdir());
		folderCwl.SetRelativePath(boost::io::str(boost::format("World/%s/") % m_contentsData->strContentsName));
		folderCwl.SetRecursive(false);
		folderCwl.SetFileSearchKeys({ ".dds" });
		folderCwl.operator()(out);
	}
}