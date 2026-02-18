#include "stdafx.h"
#include "EquipmentFileCrawlerImpl_V3_V2Info.h"

namespace rsc
{
	EquipmentFileCrawlerImpl_V3_V2Info::EquipmentFileCrawlerImpl_V3_V2Info(ContentsDataPtr contentsData,
																		   const Path& workdir,
																		   const Path& PEFRelPath,
																		   const std::string& strCountryName)
		: EquipmentFileCrawlerImpl_V2_Avatar(contentsData, workdir, PEFRelPath, strCountryName)
	{
	}

	EquipmentFileCrawlerImpl_V3_V2Info::~EquipmentFileCrawlerImpl_V3_V2Info()
	{
	}

	bool EquipmentFileCrawlerImpl_V3_V2Info::LoadPef()
	{
		if (!EquipmentFileCrawlerImpl_V2_Avatar::LoadPef())
			return false;

		i3ResourceFileObjectInfo2* registryKey = GetPef()->FindChildi3TreeElement(m_currRegistryKey, "V2_Info");
		if (registryKey == nullptr)
			return false;

		m_currRegistryKey = registryKey;

		return true;
	}

	void EquipmentFileCrawlerImpl_V3_V2Info::GatherPefFiles(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl_V2_Avatar::GatherPefFiles(out);
	}

	void EquipmentFileCrawlerImpl_V3_V2Info::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl_V2_Avatar::GatherMeshAssocFiles(out);
	}

	void EquipmentFileCrawlerImpl_V3_V2Info::GatherTextures(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl_V2_Avatar::GatherTextures(out);
	}
}