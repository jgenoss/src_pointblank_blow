#include "stdafx.h"
#include "EquipmentFileCrawler.h"
#include "EquipmentFileCrawlerImpl.h"
#include "EquipmentFileCrawlerImpl_V1.h"
#include "EquipmentFileCrawlerImpl_V2_Avatar.h"
#include "EquipmentFileCrawlerImpl_V3.h"

namespace rsc
{
	EquipmentFileCrawler::EquipmentFileCrawler(EquipmentFileCrawlerImpl* impl)
		: m_impl(impl)
	{}

	EquipmentFileCrawler::~EquipmentFileCrawler() 
	{}

	bool EquipmentFileCrawler::_Load()
	{
		return m_impl->LoadPef();
	}

	bool EquipmentFileCrawler::_Gather(std::vector<PathPtr>& out)
	{
		m_impl->GatherPefFiles(out);
		m_impl->GatherMeshAssocFiles(out);
		m_impl->GatherTextures(out);
		m_impl->GatherMiscs(out);

		return true;
	}

	IFileCrawler* EquipmentFileCrawler::Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v1 = 
			new EquipmentFileCrawlerImpl_V1(contentsData, workdir, "config/Equipment.pef", strCountryName);
		
		return new EquipmentFileCrawler(impl_v1);
	}

	IFileCrawler* EquipmentFileCrawler::Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v2 = 
			new EquipmentFileCrawlerImpl_V2_Avatar(contentsData, workdir, "config/Equipment_Backup.pef", strCountryName);
		
		return new EquipmentFileCrawler(impl_v2);
	}

	IFileCrawler* EquipmentFileCrawler::Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v3 =
			new EquipmentFileCrawlerImpl_V3(contentsData, workdir, "config/Equipment.pef", strCountryName);

		return new EquipmentFileCrawler(impl_v3);
	}
}