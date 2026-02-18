#include "stdafx.h"
#include "EquipmentFileCrawlerImpl_V3_V1Info.h"

namespace rsc
{
	EquipmentFileCrawlerImpl_V3_V1Info::EquipmentFileCrawlerImpl_V3_V1Info(ContentsDataPtr contentsData,
																		   const Path& workdir,
																		   const Path& PEFRelPath,
																		   const std::string& strCountryName)
		: EquipmentFileCrawlerImpl_V2_Avatar(contentsData, workdir, PEFRelPath, strCountryName)
	{
	}

	EquipmentFileCrawlerImpl_V3_V1Info::~EquipmentFileCrawlerImpl_V3_V1Info()
	{
	}

	bool EquipmentFileCrawlerImpl_V3_V1Info::LoadPef()
	{
		if (!EquipmentFileCrawlerImpl_V2_Avatar::LoadPef())
			return false;

		i3ResourceFileObjectInfo2* registryKey = GetPef()->FindChildi3TreeElement(m_currRegistryKey, "V1_Info");
		if (registryKey == nullptr)
			return false;

		m_currRegistryKey = registryKey;

		return true;
	}

	void EquipmentFileCrawlerImpl_V3_V1Info::GatherPefFiles(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl_V2_Avatar::GatherPefFiles(out);
	}

	void EquipmentFileCrawlerImpl_V3_V1Info::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl_V2_Avatar::GatherMeshAssocFiles(out);
	}

	void EquipmentFileCrawlerImpl_V3_V1Info::GatherTextures(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl_V2_Avatar::GatherTextures(out);
	}
}