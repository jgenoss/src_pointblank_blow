#include "stdafx.h"
#include "MQContentsStatusBar.h"
#include "MQContentsStatusTree.h"
#include "UIDefines.h"
#include "resource/FileCrawlerFactory.h"
#include "QLContentsStatusBar.h"
#include "util/PathTokenizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	MQContentsStatusBar::MQContentsStatusBar() : m_thpool(std::make_unique<ThreadPool>(1))
	{
	}

	MQContentsStatusBar::~MQContentsStatusBar()
	{
	}

	void MQContentsStatusBar::OnClose()
	{
		m_thpool->ClearTasks();
		m_thpool->Shutdown();
	}

	void MQContentsStatusBar::ShowContentsStatusTree(const rsc::ContentsDataPtr& contentsData, const std::wstring& wstrCountryName)
	{
		m_treeCtrl->ClearItems();

		if (m_QLStatusBar)
			m_QLStatusBar->Clear();

		m_thpool->ClearTasks();

		m_currContentsData = contentsData;

		std::string strCountryName;		i3::utf16_to_mb(wstrCountryName, strCountryName);
		m_currCountryData = rsc::CountrySettings::i()->FindData(strCountryName);

		struct Task_ShowContentsStatus_MQ_QL
		{
			void operator()(MQContentsStatusBar* owner, QLContentsStatusBar* QLStatusBar, ThreadPool* pool,
				const rsc::ContentsDataPtr& contentsData, const rsc::CountryDataPtr& countryData)
			{
				// MEDIA <-> QA 비교 추가.
				auto key = std::make_pair(countryData->GetVersion(), contentsData->strCategoryName);

				auto crawler = rsc::FileCrawlerFactory::i()->Create(
					key, contentsData, countryData->GetMediaWorkdir(), countryData->GetCountryName());

				std::vector<PathPtr> fileRelPathList;
				crawler->operator()(fileRelPathList);

				const std::wstring& wstrMediaWorkdir = countryData->GetMediaWorkdir().wstring();
				for (auto &currRelPath : fileRelPathList)
				{
					ClassifyCapitalAndSmallLetter(currRelPath, wstrMediaWorkdir);
				}

				std::vector<std::wstring> wstrRelPathList;
				wstrRelPathList.reserve(fileRelPathList.size());
				for (auto &currRelPath : fileRelPathList)
				{
					wstrRelPathList.push_back(currRelPath->wstring());
				}

				std::wstring wstrContentsName;
				i3::mb_to_utf16(contentsData->strContentsName, wstrContentsName);

				const std::string& strCountryName = countryData->GetCountryName();
				if (!strCountryName.empty())
				{
					std::wstring wstrCountryName;		i3::mb_to_utf16(strCountryName, wstrCountryName);

					owner->m_treeCtrl->InsertItems(wstrRelPathList, wstrContentsName, wstrCountryName);
				}

				// QA <-> LIVE 비교 스레드 추가.
				struct Task_ShowContentsStatus_QL
				{
					void operator()(QLContentsStatusBar* QLStatusBar, const std::vector<PathPtr>& fileRelPathList,
						const rsc::ContentsDataPtr& contentsData, const rsc::CountryDataPtr& countryData)
					{
						if (QLStatusBar)
							QLStatusBar->ShowContentsStatusTree(fileRelPathList, contentsData, countryData);
					}
				};

				if (pool)
					pool->Enqueue(Task_ShowContentsStatus_QL(), QLStatusBar, fileRelPathList, contentsData, countryData);
			}
		};

		m_thpool->Enqueue(Task_ShowContentsStatus_MQ_QL(), this, m_QLStatusBar, m_thpool.get(), m_currContentsData, m_currCountryData);
	}

	void MQContentsStatusBar::SelectQLTreeItem(const std::wstring& wstrPath)
	{
		m_QLStatusBar->SelectTreeItem(wstrPath);
	}

	void MQContentsStatusBar::SelectTreeItem(const std::wstring& wstrPath)
	{
		m_treeCtrl->SelectItem(wstrPath);
	}

	void MQContentsStatusBar::Clear()
	{
		m_treeCtrl->ClearItems();
		m_QLStatusBar->Clear();
	}

	BEGIN_MESSAGE_MAP(MQContentsStatusBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	int MQContentsStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		m_treeCtrl = std::make_unique<MQContentsStatusTree>(*this);

		return 0;
	}

	void MQContentsStatusBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		if (m_treeCtrl.get())
			m_treeCtrl->AdjustLayout(cx, cy);
	}

	HBRUSH MQContentsStatusBar::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		UNREFERENCED_PARAMETER(nCtlColor);

		pDC->SetBkColor(COLOR_IVORY_BLACK);

		return m_bkGndBrush;
	}

	BOOL MQContentsStatusBar::OnEraseBkgnd(CDC* pDC)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect, &m_bkGndBrush);

		return 1;
	}
}
