#include "stdafx.h"
#include "EquipmentFileCrawlerImpl_V3.h"
#include "Settings.h"
#include "EquipmentFileCrawlerImpl_V3_V1Info.h"
#include "EquipmentFileCrawlerImpl_V3_V2Info.h"

namespace rsc
{
	EquipmentFileCrawlerImpl_V3::EquipmentFileCrawlerImpl_V3(ContentsDataPtr contentsData,
															 const Path& workdir,
															 const Path& PEFRelPath,
															 const std::string& strCountryName)
		: EquipmentFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{
		const CountryDataPtr currCountryData = CountrySettings::i()->FindData(strCountryName);
		if (currCountryData->IsEnableV3UI_1_0() || currCountryData->IsEnableV3UI_1_1())
		{
			m_V3ImplList.push_back(
				std::make_unique<EquipmentFileCrawlerImpl_V3_V1Info>(contentsData, workdir, PEFRelPath, strCountryName));
		}

		if (currCountryData->IsEnableV3UI_1_5())
		{
			m_V3ImplList.push_back(
				std::make_unique<EquipmentFileCrawlerImpl_V3_V2Info>(contentsData, workdir, PEFRelPath, strCountryName));
		}
	}

	EquipmentFileCrawlerImpl_V3::~EquipmentFileCrawlerImpl_V3()
	{
	}

	bool EquipmentFileCrawlerImpl_V3::LoadPef()
	{
		V3ImplPtrList::iterator it = m_V3ImplList.begin();
		for (; it != m_V3ImplList.end();)
		{
			if ((*it)->LoadPef() == false)
				it = m_V3ImplList.erase(it);
			else
				++it;
		}

		return !m_V3ImplList.empty();
	}

	void EquipmentFileCrawlerImpl_V3::GatherPefFiles(std::vector<PathPtr>& out)
	{
		for (auto &impl : m_V3ImplList)
			impl->GatherPefFiles(out);
	}

	void EquipmentFileCrawlerImpl_V3::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		for (auto &impl : m_V3ImplList)
			impl->GatherMeshAssocFiles(out);
	}

	void EquipmentFileCrawlerImpl_V3::GatherTextures(std::vector<PathPtr>& out)
	{
		for (auto &impl : m_V3ImplList)
			impl->GatherTextures(out);
	}

	void EquipmentFileCrawlerImpl_V3::GatherMiscs(std::vector<PathPtr>& out)
	{
		for (auto &impl : m_V3ImplList)
			impl->GatherMiscs(out);
	}
}