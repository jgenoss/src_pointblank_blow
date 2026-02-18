#pragma once

#include "EquipmentFileCrawlerImpl_V2_Avatar.h"

namespace rsc
{
	class EquipmentFileCrawlerImpl_V3_V1Info : public EquipmentFileCrawlerImpl_V2_Avatar
	{
	public:
		EquipmentFileCrawlerImpl_V3_V1Info(ContentsDataPtr contentsData,
										   const Path& workdir,
										   const Path& PEFRelPath,
										   const std::string& strCountryName);

		virtual ~EquipmentFileCrawlerImpl_V3_V1Info();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherTextures(std::vector<PathPtr>& out) override;
	};
}
