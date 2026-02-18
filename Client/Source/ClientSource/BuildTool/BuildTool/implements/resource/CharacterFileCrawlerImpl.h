#pragma once

#include "ContentsDefines.h"
#include "IPefable.h"

namespace rsc
{
	class CharacterFileCrawlerImpl : public IPefable
	{
	public:
		CharacterFileCrawlerImpl(ContentsDataPtr contentsData,
								 const Path& workdir,
								 const Path& pefRelPath,
								 const std::string& countryName);

		virtual ~CharacterFileCrawlerImpl() = 0;

		virtual bool LoadPef() = 0;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherEquipmentAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMiscs(std::vector<PathPtr>& out) {}

	protected:
		ContentsDataPtr m_contentsData = nullptr;
		i3ResourceFileObjectInfo2* m_contentsRegistryKey = nullptr;
	};
}
