#pragma once

#include "ContentsDefines.h"
#include "IPefable.h"

namespace rsc
{
	class EquipmentFileCrawlerImpl : public IPefable
	{
	public:
		EquipmentFileCrawlerImpl(ContentsDataPtr contentsData, 
								 const Path& workdir,
								 const Path& PEFRelPath, 
								 const std::string& strCountryName);

		virtual ~EquipmentFileCrawlerImpl() = 0;

		virtual bool LoadPef() = 0;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMeshAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherTextures(std::vector<PathPtr>& out) = 0;
		virtual void GatherMiscs(std::vector<PathPtr>& out) {}

	protected:
		ContentsDataPtr m_contentsData = nullptr;
		i3ResourceFileObjectInfo2* m_currRegistryKey = nullptr;
		Path m_workdir;
	};

	bool GatherEquipMeshSharingAssocFiles(const Path& workdir, const Path& i3sRelPath, const char* pszPefRelPath, std::vector<PathPtr>& out);
}