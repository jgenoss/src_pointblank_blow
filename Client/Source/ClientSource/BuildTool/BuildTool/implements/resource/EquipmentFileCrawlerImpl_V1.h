#pragma once

#include "EquipmentFileCrawlerImpl.h"

namespace rsc
{
	class EquipmentFileCrawlerImpl_V1 : public EquipmentFileCrawlerImpl
	{
	public:
		EquipmentFileCrawlerImpl_V1(ContentsDataPtr contentsData,
									const Path& workdir,
									const Path& PEFRelPath,
									const std::string& strCountryName);

		virtual ~EquipmentFileCrawlerImpl_V1();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherTextures(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;
	};
}