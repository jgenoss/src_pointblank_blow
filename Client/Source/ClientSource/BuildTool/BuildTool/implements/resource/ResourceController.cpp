#include "stdafx.h"
#include "ResourceController.h"
#include "Settings.h"
#include "RscTool/RSCFileMgr.h"
#include "ContentsCrawlerFactory.h"
#include "i3PackMappedFileTableFactory.h"
#include "ContentsDB.h"
#include "ContentsTable_Map.h"
#include "IContentsCrawler.h"
#include "MainFrm.h"
#include "ui/ContentsView.h"
#include "ContentsCrawlerCallback.h"
#include "ContentsStateChecker.h"
#include "ContentsStateCallback.h"
#include "ContentsSynchronizer.h"
#include "FileSync_Copy.h"
#include "PackScriptMappedTable.h"
#include "RscTool/ExecuteTSVN.h"
#include "RscTool/ExecuteSVN.h"
#include <boost/lexical_cast.hpp>
#include "ContentsTableCallback.h"
#include <boost/filesystem.hpp>
#include "ui/ContentsViewToolbar.h"
#include "ui/LoadingProgressMsgCallback.h"
#include "ui/ContentsView.h"
#include "util/GITCommand.h"
#include "util/TGITCommand.h"
#include "i3Scene/i3RegistrySet.h"
#include "FolderFilesCrawler.h"
#include "ContentsSynchronizerFactory.h"
#include "PathHelper.h"


namespace rsc
{
	struct Task_SyncContents
	{
		void operator()(std::shared_ptr<IContentsSynchronizer> sync)
		{
			sync->operator()();
		}
	};

	ResourceController::ResourceController() 
		: m_thpoolContentsView(std::make_unique<ThreadPool>(1)), m_thpoolSyncPlayer(std::make_unique<ThreadPool>(1))		
	{
	}

	ResourceController::~ResourceController()
	{
	}

	void ResourceController::OnBuildup()
	{
		SettingsContext::i()->Load();

		std::vector<CountryDataPtr> countryDataList;
		CountrySettings::i()->GetDataList(countryDataList);

		m_contentsDB.Clear();

		for (auto &currCountryData : countryDataList)
		{
			const std::string& strCountryName = currCountryData->GetCountryName();
			if (m_contentsDB.CreateTable(strCountryName))
			{
				auto table = m_contentsDB.FindTable(strCountryName);

				table->SetCallback(std::make_shared<ContentsTableCallback>(
					std::bind(&ResourceController::OnInsertContents_View, this, std::placeholders::_1, std::placeholders::_2),
					std::bind(&ResourceController::OnDeleteContents_View, this, std::placeholders::_1, std::placeholders::_2),
					std::bind(&ResourceController::OnUpdateContents_View, this, std::placeholders::_1, std::placeholders::_2) ));
			}
		}
	}

	bool ResourceController::OnClose()
	{
		m_thpoolContentsView->ClearTasks();
		m_thpoolContentsView->Shutdown();

		m_thpoolSyncPlayer->ClearTasks();
		m_thpoolSyncPlayer->Shutdown();

		return true;
	}

	void ResourceController::OnSelectCountry(const std::string& strCountryName)
	{
		if (!CountrySettings::i()->IsDataExist(strCountryName))
			return;

		m_strCurrCountryName = strCountryName;

		// clear view / sync cart.
		m_thpoolContentsView->ClearTasks();
		m_thpoolSyncPlayer->ClearTasks();

		m_strDefaultContentsCategoryOrder.clear();

		// 리소스 업데이트.
		const CountryDataPtr countryData = CountrySettings::i()->FindData(strCountryName);
		
		std::array<std::pair<i3::rc_string, std::string>, 5> arrVCSUpdateWorkdirList = 
		{ 
			std::make_pair(countryData->GetPackScriptWorkdir().string().c_str(), countryData->GetPackScriptVCS().c_str()),
			std::make_pair(countryData->GetMediaWorkdir().string().c_str(), countryData->GetMediaVCS().c_str()),
			std::make_pair(countryData->GetQAWorkdir().string().c_str(), countryData->GetQAVCS().c_str()),
			std::make_pair(countryData->GetLiveWorkdir().string().c_str(), countryData->GetLiveVCS().c_str())
		};

		i3::rc_string strSqlite3Path(GetExeDir().string());
		strSqlite3Path += "/sqlite3.exe";
		
		std::wstring wstrWorkdir;

		for (auto &workdir : arrVCSUpdateWorkdirList)
		{
			const i3::rc_string& strWorkdir = std::get<0>(workdir);
			if (strWorkdir.empty())
				continue;

			if (!fs::exists(strWorkdir.c_str()))
			{
				std::wstring wstrWarningMsg(L"폴더가 없습니다. 세팅 파일에서 경로 확인 후 다시 시도 바랍니다. \n");

				std::wstring wstrWorkdir(strWorkdir.begin(), strWorkdir.end());
				wstrWarningMsg += wstrWorkdir.c_str();

				AfxMessageBox(wstrWarningMsg.c_str());
				return;
			}

			const i3::rc_string& strVCS = std::get<1>(workdir);
			if (i3::generic_is_iequal(strVCS, "SVN"))
			{
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(strWorkdir));

				i3::vector<i3::rc_string> conflictAbspathList;
				svn::ProcessConflictStatus(strWorkdir, strSqlite3Path, conflictAbspathList);
			}
			else  // GIT
			{
				if (tgit::TGitExePath::get().GetPath().empty())
				{
					::AfxMessageBox(L"TortoiseGit 프로그램이 설치되어 있어야 업데이트를 정상적으로 받을 수 있습니다.");
					continue;
				}

				i3::mb_to_utf16(strWorkdir, wstrWorkdir);

				tgit::OpenPullDlg pull(wstrWorkdir);
				pull();
			}
		}

		// 로딩창 생성.
		ui::LoadingProgressMsgCallback cbLoadingMsg;
		
		RECT frameRect;	GetMainFrame()->GetWindowRect(&frameRect);
		const int center = static_cast<int>((frameRect.bottom - frameRect.top) / 2);
		frameRect.top = center + 20;
		frameRect.bottom = center + 60;
		cbLoadingMsg.MoveWindow(frameRect);

		// clear tasks all.
		cbLoadingMsg.OnTextMessage(L"Clear synchronize works.");
		GetMainFrame()->GetSyncPlayerBar().GetDlg().Reset();

		// clear view. change topic text.
		cbLoadingMsg.OnTextMessage(L"Clear View");
		
		std::wstring wstrCountryName;
		i3::mb_to_utf16(strCountryName, wstrCountryName);
		GetMainFrame()->GetContentsView().ClearContents();
		GetMainFrame()->GetContentsView().SetToolbarCountryName(wstrCountryName);
		
		// 미디어 i3RegXML 파일을 PEF로 변환/저장.
		Savei3RegXMLToPEF(countryData->GetMediaWorkdir(), cbLoadingMsg);

		// loading RSC files.
		cbLoadingMsg.OnTextMessage(L"Loading RSC Files...");
		SaveAllRSGToRSC(countryData->GetQAWorkdir(), cbLoadingMsg);
		RSCFileMgr::i()->SetMediaPath(countryData->GetMediaWorkdir().wstring());
		RSCFileMgr::i()->LoadRSCGenFiles(countryData->GetQAWorkdir().string());

		// loading TexChange files.
		wchar_t currDir[MAX_PATH] = { 0, };
		::GetCurrentDirectoryW(MAX_PATH, currDir);
		::SetCurrentDirectoryW(countryData->GetMediaWorkdir().wstring().c_str());

		i3SGTexChangeManager::i()->Clear();

		cbLoadingMsg.OnTextMessage(L"Loading texture_change_Weapon.pef");
		i3SGTexChangeManager::i()->LoadRegistryFile("config/texture_change_Weapon.pef");

		if (i3::generic_is_iequal("v1", countryData->GetVersion()))
		{
			cbLoadingMsg.OnTextMessage(L"Loading texture_Change_Equip_i3s.pef");
			i3SGTexChangeManager::i()->LoadRegistryFile("config/texture_Change_Equip_i3s.pef");
		}
		else
		{
			cbLoadingMsg.OnTextMessage(L"Loading texture_change_Avatar.pef");
			i3SGTexChangeManager::i()->LoadRegistryFile("config/texture_change_Avatar.pef");
		}

		::SetCurrentDirectoryW(currDir);

		// loading i3Pack name <-> orig file name
		// 팩파일 <-> 원본 파일 매핑. 리소스 파일 비교 시 사용.
		cbLoadingMsg.OnTextMessage(L"Loading Pack Files...");

		i3PackMappedFileTableFactory::i()->Buildup();
		auto table = i3PackMappedFileTableFactory::i()->RequestObj(countryData->GetLiveWorkdir().string());
		table->Buildup(countryData->GetLiveWorkdir().string());

		// 파일 <-> 팩스크립트 매핑 테이블 생성.
		cbLoadingMsg.OnTextMessage(L"Loading Pack Scripts...");

		PackScriptMappedTable::i()->Buildup(countryData->GetPackScriptWorkdir(), countryData->GetMediaWorkdir(), countryData->GetVersion());

		// 컨텐츠 출력.
		cbLoadingMsg.OnTextMessage(L"Print all contents to view");

		auto tbl = m_contentsDB.FindTable(strCountryName);
		tbl->Clear();

		auto crawlers = ContentsCrawlerFactory::i()->Create(countryData->GetVersion(), countryData->GetMediaWorkdir(), strCountryName);

		m_thpoolContentsView->ClearTasks();

		for (auto &crawler : crawlers->list)
		{
			crawler->SetCallback(std::make_unique<ContentsCrawlerCallback>(
				std::bind(&ResourceController::OnPreCrawl, this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&ResourceController::OnCrawl, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
				std::bind(&ResourceController::OnPostCrawl, this, std::placeholders::_1, std::placeholders::_2)));
		
			crawler->SetCountryName(strCountryName);

			struct Task_CrawlContents
			{
				void operator()(std::shared_ptr<IContentsCrawler> crawler)
				{
					crawler->operator()();
				}
			};

			m_thpoolContentsView->Enqueue(Task_CrawlContents(), crawler);
		}
	}

	void ResourceController::OnCountryChanged()
	{
		OnBuildup();

		m_thpoolContentsView->ClearTasks();
		m_thpoolSyncPlayer->ClearTasks();

		m_strCurrCountryName.clear();

		// clear tasks all.
		GetMainFrame()->GetSyncPlayerBar().GetDlg().Reset();

		// clear view. change topic text.
		GetMainFrame()->GetContentsView().ClearContents();
		GetMainFrame()->GetContentsView().SetToolbarCountryName(L"");

		GetMainFrame()->GetContentsDescBar().Clear();
		GetMainFrame()->GetMQContentsStatusBar().Clear();

		std::vector<CountryDataPtr> countryDataList;
		CountrySettings::i()->GetDataList(countryDataList);

		std::vector<std::string> strCountryNameList;
		for (auto &currCountryData : countryDataList)
			strCountryNameList.push_back(currCountryData->GetCountryName());
		
		GetMainFrame()->GetTasksBar().BuildupCountries(strCountryNameList);
	}

	void ResourceController::OnPreCrawl(const std::string& strCountryName, const std::string& strContentsCategory)
	{
		m_strDefaultContentsCategoryOrder.push_back(strContentsCategory);
	}

	void ResourceController::OnCrawl(const std::string& strCountryName, const std::string& strContentsName, const std::string& strContentsCategory, const std::string& strContentsSubCategory)
	{
		auto tbl = m_contentsDB.FindTable(strCountryName);
		tbl->Insert(strContentsName, strContentsCategory, strContentsSubCategory, ContentsStatusFlag());
	}

	void ResourceController::OnPostCrawl(const std::string& strCountryName, const std::string& strContentsCategory)
	{
		// DB에 저장한 컨텐츠를 UI에 출력.
		auto tbl = m_contentsDB.FindTable(strCountryName);

		std::vector<ContentsDataPtr> contentsDataList;
		tbl->GetContentsDatasByCategory(strContentsCategory, contentsDataList);

		if (!contentsDataList.empty())
			GetMainFrame()->GetContentsView().AddContents(contentsDataList);

		// 컨텐츠 상태 검사가 필요 없어 보여 주석 처리함. ugeun.ji. 17.01.04.
		// 컨텐츠 상태 검사 작업을 스레드풀에 넣는다.
		/*for (auto &currContentsData : contentsDataList)
		{
			auto checker = std::make_shared<ContentsStateChecker>(currContentsData, CountrySettings::i()->FindData(strCountryName));

			checker->SetCallback(std::make_unique<ContentsStateCallback>(
				std::bind(&ResourceController::OnUpdateContentsState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

			struct Task_CheckContentsState
			{
				void operator()(std::shared_ptr<ContentsStateChecker> checker)
				{
					checker->operator()();
				}
			};
			m_thpoolContentsView->Enqueue(Task_CheckContentsState(), checker);
		}*/
	}

	void ResourceController::OnInsertContents_View(const ContentsDataPtr& contentsData, const std::string& strCountryName)
	{
	}

	void ResourceController::OnDeleteContents_View(const ContentsDataPtr& contentsData, const std::string& strCountryName)
	{
	}

	void ResourceController::OnUpdateContents_View(const ContentsDataPtr& contentsData, const std::string& strCountryName)
	{
		GetMainFrame()->GetContentsView().UpdateContents({ contentsData });
	}

	void ResourceController::OnViewToolbarSearchEditChanged(const std::wstring& wstrKeyword)
	{
		std::vector<ContentsDataPtr> contentsDataList;
		const IContentsTable& tbl = *(m_contentsDB.FindTable(m_strCurrCountryName).get());

		GetMainFrame()->GetContentsView().ClearContents();

		for (auto &strCategory : m_strDefaultContentsCategoryOrder)
		{
			if (wstrKeyword.empty())
				tbl.GetContentsDatasByCategory(strCategory, contentsDataList);
			else
				ContentsTableStuff::SearchContentsByKeyword(tbl, wstrKeyword, strCategory, contentsDataList);
		}

		GetMainFrame()->GetContentsView().AddContents(contentsDataList);
	}

	void ResourceController::OnUpdateContentsState(const ContentsDataPtr& contentsData,
												   const std::string& strCountryName,
												   const ContentsStatusFlag& status)
	{
		ContentsTablePtr &tbl = m_contentsDB.FindTable(strCountryName);

		tbl->SetContentsStatus(contentsData->strContentsName, 
			contentsData->strCategoryName, contentsData->strSubcategoryName, status);
	}

	void ResourceController::OnRequestSync(const rsc::ContentsSyncDataPtr& contentsData)
	{
		const CountryDataPtr countryData = CountrySettings::i()->FindData(m_strCurrCountryName);
		auto task = ContentsSynchronizerFactory::i()->Request(contentsData);

		task->SetProgressCallback(std::make_shared<SyncProgressCallback>(
			std::bind(&ResourceController::OnPreSync, this), 
			std::bind(&ResourceController::OnSync, this, std::placeholders::_1), 
			std::bind(&ResourceController::OnPostSync, this, std::placeholders::_1)));

		m_thpoolSyncPlayer->Enqueue(Task_SyncContents(), task);
	}

	void ResourceController::OnPreSync()
	{
		GetMainFrame()->GetSyncPlayerBar().GetDlg().OnPreSync(m_strCurrCountryName);
	}

	void ResourceController::OnSync(const float progressRate)
	{
		GetMainFrame()->GetSyncPlayerBar().GetDlg().OnSync(progressRate);
	}

	void ResourceController::OnPostSync(const bool success)
	{
		GetMainFrame()->GetSyncPlayerBar().GetDlg().OnPostSync(success);
	}

	const std::string& ResourceController::GetCurrCountryName() const
	{
		return m_strCurrCountryName;
	}

	const ContentsTablePtr ResourceController::GetCurrContentsTable()
	{
		return m_contentsDB.FindTable(m_strCurrCountryName);
	}
}

void Savei3RegXMLToPEF(const Path& workdir, ui::ILoadingProgressMsgCallback& cbLoadingMsg)
{
	rsc::FolderFilesCrawler i3RegXMLCwl;
	i3RegXMLCwl.SetWorkdir(workdir);
	i3RegXMLCwl.SetRelativePath("config\\");
	i3RegXMLCwl.SetRecursive(false);
	i3RegXMLCwl.SetFileSearchKeys({ ".i3RegXML" });

	std::vector<PathPtr> i3RegXMLRelPathList;
	i3RegXMLCwl.operator()(i3RegXMLRelPathList);

	Path XMLAbsPath, PEFAbsPath;
	std::wstring wstrLoadingMsg;

	const int progressIncr = int(100 / i3RegXMLRelPathList.size());
	int progress = 0;
	cbLoadingMsg.OnGaugeProgress(0);

	for (auto &XMLRelPath : i3RegXMLRelPathList)
	{
		wstrLoadingMsg = L"[Save i3RegXML to Pef] ";
		wstrLoadingMsg += XMLRelPath->wstring();
		cbLoadingMsg.OnTextMessage(wstrLoadingMsg);

		progress += progressIncr;
		cbLoadingMsg.OnGaugeProgress(progress);

		i3RegistrySet* reg = i3RegistrySet::new_object();

		reg->SetCodePage(I3_LANG_CODE_ANSI);

		XMLAbsPath = workdir / *XMLRelPath;
		if (reg->LoadRegistrySet(XMLAbsPath.string().c_str(), REGISTRY_FILE_XML_BY_UTF8) != STREAM_ERR)
		{
			PEFAbsPath = XMLAbsPath;
			PEFAbsPath.replace_extension(".Pef");
			reg->SaveRegistrySet(PEFAbsPath.string().c_str(), REGISTRY_FILE_BINARY);
		}

		I3_SAFE_RELEASE(reg);
	}

	cbLoadingMsg.OnTextMessage(L"");
	cbLoadingMsg.OnGaugeProgress(100);
}

void SaveAllRSGToRSC(const Path& workdir, ui::ILoadingProgressMsgCallback& cbLoadingMsg)
{
	std::vector<std::string> i3RSGRelPathList;
	RSCFileMgr::i()->GetAllRSCGenRelPaths(i3RSGRelPathList);

	Path i3RSGAbsPath;
	
	const int progressIncr = int(100 / i3RSGRelPathList.size());
	int progress = 0;
	cbLoadingMsg.OnGaugeProgress(0);

	for (const auto& i3RSGRelPath : i3RSGRelPathList)
	{
		i3RSGAbsPath = workdir;	i3RSGAbsPath /= i3RSGRelPath;
		if (!fs::exists(i3RSGAbsPath))
			continue;

		std::wstring wstrLoadingMsg;
		std::wstring wstr;
		i3RSCGen rsg;
		if (rsg.OpenRSCFile(i3RSGAbsPath.string().c_str()))
		{
			wstrLoadingMsg = L"[Save RSG to RSC] ";
			i3::mb_to_utf16(i3RSGRelPath, wstr);
			wstrLoadingMsg += wstr;
			cbLoadingMsg.OnTextMessage(wstrLoadingMsg);

			progress += progressIncr;
			cbLoadingMsg.OnGaugeProgress(progress);

			rsg.BuildRSCFile(i3RSGAbsPath.string().c_str());

			char szi3RSCGenAbsPath[MAX_PATH] = { 0, };
			strcpy(szi3RSCGenAbsPath, i3RSGAbsPath.string().c_str());
			rsg.ExportRSCBin(szi3RSCGenAbsPath);
		}
	}

	cbLoadingMsg.OnTextMessage(L"");
	cbLoadingMsg.OnGaugeProgress(100);
}