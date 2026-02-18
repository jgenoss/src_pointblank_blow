#pragma once

#include <map>
#include "util/ThreadPool.hpp"
#include "ContentsDB.h"
#include "ui/SyncButton.h"
#include "SyncLogger.h"
#include "Settings.h"
#include "ContentsSynchronizer.h"

namespace ui { class ContentsView; }

namespace rsc
{
	struct ContentsCrawlers;
	class IContentsCrawlerCallback;
	class IContentsDBCallback;
	class ICalcContentsStateCallback;

	class ResourceController : public i3::shared_ginst<ResourceController>
	{
	public:
		ResourceController();
		~ResourceController();

	public:
		void OnBuildup();
		bool OnClose();
		void OnSelectCountry(const std::string& strCountryName);
		void OnCountryChanged();

		void OnPreCrawl(const std::string& strCountryName, const std::string& strContentsCategory);
		void OnCrawl(const std::string& strCountryName, const std::string& strContentsName, const std::string& strContentsCategory, const std::string& strContentsSubCategory);
		void OnPostCrawl(const std::string&, const std::string&);

		void OnInsertContents_View(const ContentsDataPtr& contentsData, const std::string& strCountryName);
		void OnDeleteContents_View(const ContentsDataPtr& contentsData, const std::string& strCountryName);
		void OnUpdateContents_View(const ContentsDataPtr& contentsData, const std::string& strCountryName);

		void OnViewToolbarSearchEditChanged(const std::wstring& wstrKeyword);

		void OnUpdateContentsState(const ContentsDataPtr& contentsData, const std::string& strCountryName, const ContentsStatusFlag& contentsStatus);

		void OnRequestSync(const rsc::ContentsSyncDataPtr& contentsData);
		void OnPreSync();
		void OnSync(const float progressRate);
		void OnPostSync(const bool success);

		const std::string& GetCurrCountryName() const;
		const ContentsTablePtr GetCurrContentsTable();

	private:
		ContentsDB<> m_contentsDB;

		std::unique_ptr<ThreadPool> m_thpoolContentsView;
		std::unique_ptr<ThreadPool> m_thpoolSyncPlayer;
		
		std::shared_ptr<ContentsCrawlers> m_crawlers;
		
		std::string m_strCurrCountryName;

		std::vector<std::string> m_strDefaultContentsCategoryOrder;
	};
}

namespace ui { class ILoadingProgressMsgCallback; }
void Savei3RegXMLToPEF(const Path& workdir, ui::ILoadingProgressMsgCallback& cbLoadingMsg);
void SaveAllRSGToRSC(const Path& workdir, ui::ILoadingProgressMsgCallback& cbLoadingMsg);