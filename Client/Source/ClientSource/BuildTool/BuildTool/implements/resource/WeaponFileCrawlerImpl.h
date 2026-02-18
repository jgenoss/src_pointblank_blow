#pragma once

#include "ContentsDefines.h"
#include "IPefable.h"

namespace rsc
{
	class WeaponFileCrawlerImpl : public IPefable
	{
	public:
		WeaponFileCrawlerImpl(ContentsDataPtr contentsData, 
							  const Path& workdir, 
							  const Path& PEFRelpath, 
							  const std::string& strCountryName);

		virtual ~WeaponFileCrawlerImpl() = 0;

		virtual bool LoadPef() = 0;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMeshFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherEffectFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherSoundFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherCharaAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherRSCFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMiscs(std::vector<PathPtr>& out) {}

	protected:
		bool GatherMeshSharingAssocFiles(const std::string& I3SRelPath, std::vector<PathPtr>& out);
		void GatherCommonUIFiles(std::vector<PathPtr>& out);

	protected:
		ContentsDataPtr m_contentsData = nullptr;
		i3ResourceFileObjectInfo2* m_currRegistryKey = nullptr;
		Path m_workdir;
	};

	const i3::rc_string RetrieveRegStringData(int idxNation, i3RscPartialFile* rscfilePef
		, i3ResourceFileObjectInfo2* registryKey);

	bool PushbackRegStringData(int idxNation, const char* regString
		, i3RscPartialFile* rscfilePef
		, i3ResourceFileObjectInfo2* currRegistryKey
		, std::vector<PathPtr>& out);
}