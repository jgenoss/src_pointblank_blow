#include "stdafx.h"
#include "UIFileCrawler.h"
#include "UIFileCrawlerImpl.h"
#include "UIFileCrawlerImpl_V1_Gui.h"
#include "UIFileCrawlerImpl_V2_UIRe.h"
#include "UIFileCrawlerImpl_V3.h"
#include "ResourceController.h"

namespace rsc
{
	UIFileCrawler::UIFileCrawler(UIFileCrawlerImpl* impl)
		: m_impl(impl)
	{}

	UIFileCrawler::~UIFileCrawler() 
	{}

	bool UIFileCrawler::_Load()
	{
		return m_impl->Load();
	}

	bool UIFileCrawler::_Gather(std::vector<PathPtr>& out)
	{
		m_impl->GatherSceneFiles(out);
		m_impl->GatherSceneAssocFiles(out);
		m_impl->GatherMiscs(out);

		return true;
	}

	IFileCrawler* UIFileCrawler::Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		return new UIFileCrawler(new UIFileCrawlerImpl_V1_Gui(contentsData, workdir, "Gui\\Project", strCountryName));
	}

	IFileCrawler* UIFileCrawler::Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		return new UIFileCrawler(new UIFileCrawlerImpl_V2_UIRe(contentsData, workdir, "UIRe\\", strCountryName));
	}

	IFileCrawler* UIFileCrawler::Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		const CountryDataPtr countryData = CountrySettings::i()->FindData(strCountryName);

		return new UIFileCrawler(new UIFileCrawlerImpl_V3(contentsData, workdir, strCountryName));
	}
}