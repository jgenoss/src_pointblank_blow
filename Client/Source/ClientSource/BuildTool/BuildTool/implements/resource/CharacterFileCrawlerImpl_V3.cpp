#include "stdafx.h"
#include "CharacterFileCrawlerImpl_V3.h"
#include "CharacterFileCrawlerImpl_V2.h"
#include "FileCrawlerFactory.h"

namespace rsc
{
	CharacterFileCrawlerImpl_V3::CharacterFileCrawlerImpl_V3(ContentsDataPtr contentsData,
															 const Path& workdir,
															 const Path& charaPefRelPath,
															 const Path& equipPefRelPath,
															 const std::string& strCountryName)
		: CharacterFileCrawlerImpl(contentsData, workdir, charaPefRelPath, strCountryName),
		m_V2Impl(std::make_unique<CharacterFileCrawlerImpl_V2>(contentsData, workdir, charaPefRelPath, equipPefRelPath, strCountryName))
	{}

	CharacterFileCrawlerImpl_V3::~CharacterFileCrawlerImpl_V3()
	{}

	bool CharacterFileCrawlerImpl_V3::LoadPef()
	{
		return m_V2Impl->LoadPef();
	}

	void CharacterFileCrawlerImpl_V3::GatherPefFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherPefFiles(out);
	}

	void CharacterFileCrawlerImpl_V3::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherMeshAssocFiles(out);
	}

	void CharacterFileCrawlerImpl_V3::GatherEquipmentAssocFiles(std::vector<PathPtr>& out)
	{
		std::vector<ContentsDataPtr> equipContentsDataList;
		m_V2Impl->GatherEquipmentContentsDataList(equipContentsDataList);

		for (auto &currContentsData : equipContentsDataList)
		{
			auto equipCrawler = FileCrawlerFactory::i()->Create(
				std::make_pair("v3", "Equipment"), currContentsData, GetWorkdir(), GetCountryName());

			equipCrawler->operator()(out);
		}
	}

	void CharacterFileCrawlerImpl_V3::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherRSCAssocFiles(out);
	}
}