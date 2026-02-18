#pragma once

#include "CharacterFileCrawlerImpl.h"

namespace rsc
{
	class CharacterFileCrawlerImpl_V1 : public CharacterFileCrawlerImpl
	{
	public:
		CharacterFileCrawlerImpl_V1(ContentsDataPtr contentsData,
									const Path& workdir,
									const Path& charaPefRelPath,
									const Path& equipPefRelPath,
									const std::string& countryName);

		virtual ~CharacterFileCrawlerImpl_V1();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherEquipmentAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;

		void GatherCharaAssocEquipContentsList(std::vector<ContentsDataPtr>& out);

	private:
		const Path m_equipPefRelPath;
	};
}
