#pragma once

#include "EquipmentFileCrawlerImpl.h"

namespace rsc
{
	class EquipmentFileCrawlerImpl_V3 : public EquipmentFileCrawlerImpl
	{
	public:
		EquipmentFileCrawlerImpl_V3(ContentsDataPtr contentsData,
									const Path& workdir,
									const Path& PEFRelPath,
									const std::string& strCountryName);

		virtual ~EquipmentFileCrawlerImpl_V3();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherTextures(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;

	private:
		using V3ImplPtr = std::unique_ptr<EquipmentFileCrawlerImpl>;
		using V3ImplPtrList = std::vector<V3ImplPtr>;
		V3ImplPtrList m_V3ImplList;
	};
}
