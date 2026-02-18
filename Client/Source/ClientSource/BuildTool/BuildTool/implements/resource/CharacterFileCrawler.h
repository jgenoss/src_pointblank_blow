#pragma once

#include "IFileCrawler.h"
#include "ContentsDefines.h"

namespace rsc
{
	class CharacterFileCrawlerImpl;

	class CharacterFileCrawler : public IFileCrawler
	{
	public:
		CharacterFileCrawler(CharacterFileCrawlerImpl* impl);
		virtual ~CharacterFileCrawler();

		static IFileCrawler* Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);
		static IFileCrawler* Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);
		static IFileCrawler* Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;

	private:
		std::unique_ptr<CharacterFileCrawlerImpl> m_impl;
	};
}