#pragma once

#include "IFileCrawler.h"
#include "ContentsDefines.h"

namespace rsc
{
	class EquipmentFileCrawlerImpl;

	class EquipmentFileCrawler : public IFileCrawler
	{
	public:
		EquipmentFileCrawler(EquipmentFileCrawlerImpl* impl);
		virtual ~EquipmentFileCrawler();

		static IFileCrawler* Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);
		static IFileCrawler* Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);
		static IFileCrawler* Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;

	private:
		std::unique_ptr<EquipmentFileCrawlerImpl> m_impl;
	};
}