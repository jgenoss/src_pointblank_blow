#pragma once

#include "CharacterFileCrawlerImpl.h"

namespace rsc
{
	class CharacterFileCrawlerImpl_V2;

	class CharacterFileCrawlerImpl_V3 : public CharacterFileCrawlerImpl
	{
	public:
		CharacterFileCrawlerImpl_V3(ContentsDataPtr contentsData,
									const Path& workdir,
									const Path& charaPefRelPath,
									const Path& equipPefRelPath,
									const std::string& countryName);

		virtual ~CharacterFileCrawlerImpl_V3();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherEquipmentAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) override;

	private:
		using V2ImplPtr = std::unique_ptr<CharacterFileCrawlerImpl_V2>;
		V2ImplPtr m_V2Impl;
	};
}