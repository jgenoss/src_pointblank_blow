#include "stdafx.h"
#include "UIFileCrawlerImpl_V3.h"
#include "UIFileCrawlerImpl_V2_UIRe.h"
#include <boost/filesystem.hpp>
#include <array>
#include "i3UIeCrawler.h"
#include "Settings.h"

namespace rsc
{
	UIFileCrawlerImpl_V3::UIFileCrawlerImpl_V3(ContentsDataPtr contentsData,
											   const Path& workdir,
											   const std::string& strCountryName)
		: UIFileCrawlerImpl(contentsData, workdir, "", strCountryName)
	{
		CountryDataPtr countryData = CountrySettings::i()->FindData(strCountryName);

		if (countryData->IsEnableV3UI_1_0())
			m_V2ImplList.push_back(std::make_unique<UIFileCrawlerImpl_V2_UIRe>(contentsData, workdir, "UI_V10\\", strCountryName));

		if (countryData->IsEnableV3UI_1_1())
			m_V2ImplList.push_back(std::make_unique<UIFileCrawlerImpl_V2_UIRe>(contentsData, workdir, "UI_V11\\", strCountryName));

		if (countryData->IsEnableV3UI_1_5())
			m_V2ImplList.push_back(std::make_unique<UIFileCrawlerImpl_V2_UIRe>(contentsData, workdir, "UI_V15\\", strCountryName));
	}

	UIFileCrawlerImpl_V3::~UIFileCrawlerImpl_V3()
	{}

	bool UIFileCrawlerImpl_V3::Load()
	{
		V2UICrawlerImplPtrList::iterator it = m_V2ImplList.begin();
		for (; it != m_V2ImplList.end();)
		{
			if ((*it)->Load() == false)
				it = m_V2ImplList.erase(it);
			else
				++it;
		}

		return !m_V2ImplList.empty();
	}

	void UIFileCrawlerImpl_V3::GatherSceneFiles(std::vector<PathPtr>& out)
	{
		for (auto &impl : m_V2ImplList)
			impl->GatherSceneFiles(out);
	}

	void UIFileCrawlerImpl_V3::GatherSceneAssocFiles(std::vector<PathPtr>& out)
	{
		for (auto &impl : m_V2ImplList)
			impl->GatherSceneAssocFiles(out);
	}
}