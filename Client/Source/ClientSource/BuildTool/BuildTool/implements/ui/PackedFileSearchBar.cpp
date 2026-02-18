// PackedFileSearchBar.cpp 

#include "stdafx.h"
#include "BuildTool.h"
#include "PackedFileSearchBar.h"

#include "UIDefines.h"
#include "i3PackToolTree.h"
#include "RscTool/FindPefDiff.h"	
#include "../resource/CountrySettings.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	// PackedFileSearch

	IMPLEMENT_DYNAMIC(PackedFileSearchBar, CDockablePane)

	void PackedFileSearchBar::SetMsgBoxText(CString Text)
	{
		m_msgBox->SetText(Text.GetString());
	}

	size_t PackedFileSearchBar::GetTotalNumOfPackinnerFiles(int countryIdx)
	{
		return m_DataMgr->GetTotalNumOfPackinnerFiles(countryIdx);
	}

	// init packSearchbar only for making country name list and reserving container.
	bool PackedFileSearchBar::Init()
	{
		m_countryList.clear();

		// 빌드툴 내장 국가 세팅 정보 이용 검색.
		rsc::CountrySettings::i()->GetDataList(m_countryList);
		size_t countyListCnt = m_countryList.size();

		// init country list in search dlg.
		m_SearchDlg->Init(m_countryList);

		if (0 == countyListCnt)
		{
			::AfxMessageBox(_T("국가정보를 설정해주세요."));
			return false;
		}

		// release and reserve data containers.
		m_DataMgr->Init(countyListCnt);

		return true;
	}

	bool PackedFileSearchBar::Load(int countryIdx)
	{
		if (0 == m_countryList.size())
		{
			::AfxMessageBox(_T("국가정보를 설정해주세요."));
			return false;
		}

		// showing LoadingProgressMsgBox when load.
		m_msgBox->ShowWindow(SW_SHOW);

		bool res = m_DataMgr->Load(countryIdx
			, m_countryList[countryIdx]->GetLiveWorkdir().generic_wstring(), this);

		// hiding LoadingProgressMsgBox.
		m_msgBox->ShowWindow(SW_HIDE);

		return res;
	}

	// this function called from PackedFileSearchDlg.
	bool PackedFileSearchBar::SearchPackInnerFiles(int countryIdx, std::wstring text)
	{
		bool res = false;

		if (true == m_bChanged)
		{
			::AfxMessageBox(_T("수정된 Pack 파일이 있습니다. 수정된 파일 검색은 Load 버튼을 누르기 전까지 적용되지 않습니다."));
		}

		// get serched file list from PackInnerFileDataMgr.
		std::vector<std::wstring> packfilenames;
		m_DataMgr->SearchPackInnerFiles(countryIdx, text, &packfilenames);

		if (0 == packfilenames.size())
		{
			res = false;
		}
		else
		{
			// add to file list.
			m_FileList->Add(packfilenames);
			res = true;
		}

		packfilenames.clear();

		return res;
	}

	BEGIN_MESSAGE_MAP(PackedFileSearchBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	int PackedFileSearchBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		// search bar dlg at top.
		m_SearchDlg = std::make_unique<PackedFileSearchDlg>(this);
		m_SearchDlg->Create(IDD_DIALOG_PACKEDFILESEARCH, this);
		m_SearchDlg->ShowWindow(SW_SHOW);

		// pack and pack inner file names data manager.
		m_DataMgr = std::make_unique<PackInnerFileDataMgr>();

		// searched pack file list.
		m_FileList = std::make_unique<SearchedPackFiles>(*this);

		m_msgBox = std::make_unique<LoadingProgressMsgBox>();
		m_msgBox->Create(IDD_DIALOG_LOADING_PROGRESS);
		m_msgBox->ShowWindow(SW_HIDE);

		CenterWindow();
		
		return 0;
	}

	void PackedFileSearchBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		RECT rect;
		GetClientRect(&rect);

		// search bar dlg at top.
		if (m_SearchDlg.get())
		{
			RECT rcDlg(rect);
			rcDlg.bottom = SEARCHDLG_HEIGHT;
			m_SearchDlg->MoveWindow(&rcDlg);
		}

		// searched pack file list.
		if (m_FileList.get())
		{
			RECT rcList(rect);
			rcList.top = SEARCHDLG_HEIGHT;
			m_FileList->AdjustLayout(rcList);
		}

		if (m_msgBox.get())
		{
			RECT frameRect;	
			GetMainFrame()->GetWindowRect(&frameRect);
			const int center = static_cast<int>((frameRect.bottom - frameRect.top) / 2);
			frameRect.top = center + 20;
			frameRect.bottom = center + 60;
			m_msgBox->MoveWindow(&frameRect);
		}
	}

	HBRUSH PackedFileSearchBar::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		UNREFERENCED_PARAMETER(nCtlColor);
		pDC->SetBkColor(COLOR_IVORY_BLACK);

		return m_bkGndBrush;
	}

	BOOL PackedFileSearchBar::OnEraseBkgnd(CDC* pDC)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect, &m_bkGndBrush);

		return TRUE;
	}

} // namespace ui



