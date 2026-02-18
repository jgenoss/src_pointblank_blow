#pragma once

#include "ContentsDefines.h"
#include "IPefable.h"

namespace rsc
{
	class GoodsFileCrawlerImpl : public IPefable
	{
	public:
		GoodsFileCrawlerImpl(ContentsDataPtr contentsData,
							 const Path& workdir,
							 const Path& PEFRelPath,
							 const std::string& strCountryName);

		virtual ~GoodsFileCrawlerImpl() = 0;

		virtual bool LoadPef() = 0;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) = 0;
		virtual void GatherUITextures(std::vector<PathPtr>& out) = 0;
		virtual void GatherMiscs(std::vector<PathPtr>& out) {}

	protected:
		ContentsDataPtr m_contentsData = nullptr;
		i3ResourceFileObjectInfo2* m_currRegistryKey = nullptr;
		Path m_workdir;
	};
}