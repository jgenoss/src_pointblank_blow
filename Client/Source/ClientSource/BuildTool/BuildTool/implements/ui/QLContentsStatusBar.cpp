#include "stdafx.h"
#include "QLContentsStatusBar.h"
#include "QLContentsStatusTree.h"
#include "UIDefines.h"
#include "resource/FileCrawlerFactory.h"
#include "resource/i3PackMappedFileTableFactory.h"
#include "resource/LiveFileClassifier.h"
#include "MQContentsStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	QLContentsStatusBar::QLContentsStatusBar()
	{
	}

	QLContentsStatusBar::~QLContentsStatusBar()
	{
	}

	void QLContentsStatusBar::SetOwner(MQContentsStatusBar* owner)
	{
		m_owner = owner;
	}

	void QLContentsStatusBar::ShowContentsStatusTree(const std::vector<PathPtr>& fileRelPathList,
		const rsc::ContentsDataPtr& contentsData, const rsc::CountryDataPtr& countryData)
	{
		// 원본 파일 리스트 중 라이브 비교에 부적합한 파일들을 제거.
		std::vector<PathPtr> filtedRelPathList;
		std::copy(fileRelPathList.begin(), fileRelPathList.end(), std::back_inserter(filtedRelPathList));

		rsc::LiveFileClassifiers::i()->Classify(contentsData->strCategoryName, filtedRelPathList);

		m_currContentsData = contentsData;
		m_currCountryData = countryData;

		const std::string& strLiveWorkdir = m_currCountryData->GetLiveWorkdir().string();
		auto tblLive = rsc::i3PackMappedFileTableFactory::i()->RequestObj(strLiveWorkdir);

		using Livei3PackTable = std::map<std::wstring, std::unique_ptr<std::vector<std::wstring>>>;
		Livei3PackTable i3PackTbl;

		std::vector<std::wstring> wstrNativeRelPathList;

		Path i3PackAbsPath;
		std::wstring wstri3PackRelPath;

		for (auto &currRelPath : filtedRelPathList)
		{
			const std::wstring& wstrRelPath = currRelPath->wstring();

			if (!tblLive->Find(wstrRelPath, i3PackAbsPath))
			{
				wstrNativeRelPathList.push_back(wstrRelPath);
				continue;
			}

			wstri3PackRelPath = L"Pack\\";
			wstri3PackRelPath += i3PackAbsPath.filename().wstring();

			auto it = i3PackTbl.find(wstri3PackRelPath);
			if (it != i3PackTbl.end())
			{
				(it->second)->push_back(wstrRelPath);
			}
			else
			{
				auto pr = i3PackTbl.insert(Livei3PackTable::value_type(wstri3PackRelPath, std::make_unique<std::vector<std::wstring>>()));
				if (pr.second == true)
				{
					((pr.first)->second)->push_back(wstrRelPath);
				}
			}
		}

		std::vector<std::wstring> wstrPackWrappedRelPathList;
		std::wstring wstr;

		Livei3PackTable::const_iterator it = i3PackTbl.begin();
		for (; it != i3PackTbl.end(); ++it)
		{
			const std::wstring& wstri3PackRelPath = it->first;
			const std::vector<std::wstring>& wstrRelPathList = *(it->second).get();
			for (auto &wstrRelPath : wstrRelPathList)
			{
				// 'Pack/팩파일명' 하위 경로로 가상 설정.
				wstr.clear();
				wstr = wstri3PackRelPath;
				wstr += L"\\";
				wstr += wstrRelPath;

				wstrPackWrappedRelPathList.push_back(wstr);
			}
		}

		std::wstring wstrContentsName;
		i3::mb_to_utf16(contentsData->strContentsName, wstrContentsName);

		std::wstring wstrCountryName;
		i3::mb_to_utf16(countryData->GetCountryName(), wstrCountryName);

		m_treeCtrl->InsertItems(wstrContentsName, wstrCountryName, wstrNativeRelPathList, wstrPackWrappedRelPathList);
	}

	void QLContentsStatusBar::Clear()
	{
		m_treeCtrl->ClearItems();
	}

	void QLContentsStatusBar::SelectTreeItem(const std::wstring& wstrPath)
	{
		m_treeCtrl->SelectItem(wstrPath);
	}

	BEGIN_MESSAGE_MAP(QLContentsStatusBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	int QLContentsStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		m_treeCtrl = std::make_unique<QLContentsStatusTree>(*this);

		return 0;
	}

	void QLContentsStatusBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		if (m_treeCtrl.get())
			m_treeCtrl->AdjustLayout(cx, cy);
	}

	HBRUSH QLContentsStatusBar::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		UNREFERENCED_PARAMETER(nCtlColor);

		pDC->SetBkColor(COLOR_IVORY_BLACK);

		return m_bkGndBrush;
	}

	BOOL QLContentsStatusBar::OnEraseBkgnd(CDC* pDC)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect, &m_bkGndBrush);

		return 1;
	}
}
