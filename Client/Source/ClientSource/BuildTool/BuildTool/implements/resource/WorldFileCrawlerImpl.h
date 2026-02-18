#pragma once

#include "ContentsDefines.h"
#include "IPefable.h"

namespace rsc
{
	class WorldFileCrawlerImpl : public IPefable
	{
	public:
		WorldFileCrawlerImpl(ContentsDataPtr contentsData,
							 const Path& workdir,
							 const Path& PEFRelpath,
							 const std::string& countryName);

		virtual ~WorldFileCrawlerImpl() = 0;

		virtual bool LoadPef() = 0;

		virtual void GatherPefFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) = 0;
		virtual void Gatheri3INS(std::vector<PathPtr>& out) = 0;
		virtual void GatherMinimapFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherRSCAssocFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherMiscs(std::vector<PathPtr>& out) {}

	protected:
		ContentsDataPtr m_contentsData = nullptr;
		i3ResourceFileObjectInfo2* m_currRegistryKey = nullptr;
		Path m_workdir;
	};
}