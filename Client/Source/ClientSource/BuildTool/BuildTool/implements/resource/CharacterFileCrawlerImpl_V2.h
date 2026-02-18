#pragma once

#include "CharacterFileCrawlerImpl.h"

namespace rsc
{
	class CharacterFileCrawlerImpl_V2 : public CharacterFileCrawlerImpl
	{
	public:
		CharacterFileCrawlerImpl_V2(ContentsDataPtr contentsData,
									const Path& workdir,
									const Path& charaPefRelPath,
									const Path& equipPefRelPath,
									const std::string& countryName);

		virtual ~CharacterFileCrawlerImpl_V2();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherEquipmentAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;

		void GatherEquipmentContentsDataList(std::vector<ContentsDataPtr>& out);

	private:
		const Path m_equipPefRelPath;

		struct AvatarPartsInfo
		{
			INT32 itemID;
			std::string strEquipRegKeyName;

			AvatarPartsInfo(const INT32 id, const std::string& regKeyName)
				: itemID(id), strEquipRegKeyName(regKeyName) {}
		};
		using AvatarPartsItemIDInfoPtr = std::unique_ptr<AvatarPartsInfo>;
		
		std::vector<AvatarPartsItemIDInfoPtr> m_avatarPartsInfoList;
	};
}
