// PackedFileSearchDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "BuildTool.h"
#include "PackedFileSearchDlg.h"
#include "afxdialogex.h"

#include "UIDefines.h"
#include "PackedFileSearchBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	// PackedFileSearchDlg 대화 상자입니다.

	IMPLEMENT_DYNAMIC(PackedFileSearchDlg, CDialogEx)

		PackedFileSearchDlg::PackedFileSearchDlg(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_DIALOG_PACKEDFILESEARCH, pParent)
		, m_pParent((PackedFileSearchBar*)pParent)
		, m_strSearch(_T(""))
	{
	}

	PackedFileSearchDlg::~PackedFileSearchDlg()
	{
		m_pParent = nullptr;
	}


	void PackedFileSearchDlg::Init(std::vector<rsc::CountryDataPtr> countryList)
	{
		m_SelectNation.ResetContent();

		bool bDisable = (countryList.size() == 0);
		_SetDisableCombobox(bDisable);

		if (true == bDisable)
			return;

		CString strCountryName;

		for (const auto& country : countryList)
		{
			strCountryName = country->GetCountryName().c_str();
			m_SelectNation.AddString(strCountryName);
		}

		// 데이터를 채운 후, 제일 첫 국가를 선택한다.
		// 국가가 선택되어 있으면 선택한 국가, 아니라면 제일 첫 인덱스 국가.
		CString curCountryname(rsc::ResourceController::i()->GetCurrCountryName().c_str());

		if (curCountryname.IsEmpty())
			m_SelectNation.SetCurSel(0);
		else
		{
			int maxIdx = m_SelectNation.GetCount();
			
			CString selectedCountryName;
			for (int curIdx = 0; curIdx < maxIdx; ++curIdx)
			{
				m_SelectNation.GetLBText(curIdx, selectedCountryName);

				if (selectedCountryName == curCountryname)
				{
					m_SelectNation.SetCurSel(curIdx);
					break;
				}
			}
		}
	}

	void PackedFileSearchDlg::_SetDisableCombobox(bool bDisable)
	{
		m_SelectNation.EnableWindow(!bDisable);
	}

	void PackedFileSearchDlg::_SetSearchedFilesToParent()
	{
		if (TRUE != m_SelectNation.IsWindowEnabled())
		{
			::AfxMessageBox(_T("국가정보를 설정해주세요."));
			return;
		}

		if (0 == m_pParent->GetTotalNumOfPackinnerFiles(m_SelectNation.GetCurSel()))
		{
			::AfxMessageBox(_T("로드된 데이터가 없습니다. Load 버튼을 눌러주세요."));
			return;
		}
		
		// remove whitespace.
		m_strSearch.Replace(_T(" "), NULL);

		if (true == m_strSearch.IsEmpty())
		{
			::AfxMessageBox(_T("입력된 값이 없습니다."));
			return;
		}

		if (true != m_pParent->SearchPackInnerFiles(m_SelectNation.GetCurSel()
			, m_strSearch.operator LPCWSTR()))
		{
			::AfxMessageBox(_T("검색된 파일이 없습니다."));
		}
	}

	void PackedFileSearchDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT_PACKEDFILESEARCH, m_strSearch);
		DDX_Control(pDX, IDC_COMBO_NATION, m_SelectNation);
	}

	BOOL PackedFileSearchDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_textDesc.SubclassDlgItem(IDC_STATIC_PACKEDFILESEARCH, this);
		m_editSearcher.SubclassDlgItem(IDC_EDIT_PACKEDFILESEARCH, this);
		m_buttonSearch.SubclassDlgItem(IDC_BUTTON_PACKEDFILESEARCH, this);
		m_ButtonRefresh.SubclassDlgItem(IDC_BUTTON_REFRESH, this);
				
		LOGFONT lf;	
		GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_MEDIUM;
		lf.lfHeight = 13;
		m_font.CreateFontIndirect(&lf);
		
		m_textDesc.SetFont(&m_font, TRUE);

		_SetDisableCombobox(true);

		return TRUE;
	}
	
	BOOL PackedFileSearchDlg::PreTranslateMessage(MSG* pMsg)
	{
		ShowWindow(SW_SHOW);

		if (pMsg->message == WM_KEYUP)
		{
			if (pMsg->wParam == VK_RETURN)
			{
				// EditControl에서 엔터 눌러도 Search
				if (m_editSearcher.GetFocus())
				{
					_SetSearchedFilesToParent();
				}
			}
		}     
  
		return CDialogEx::PreTranslateMessage(pMsg);
	}
	
	BEGIN_MESSAGE_MAP(PackedFileSearchDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_CTLCOLOR()
		ON_BN_CLICKED(IDC_BUTTON_PACKEDFILESEARCH, &PackedFileSearchDlg::OnBnClickedButtonPackedfilesearch)
		ON_BN_CLICKED(IDC_BUTTON_PACKEDFILESEARCH, &PackedFileSearchDlg::OnBnClickedButtonRefresh)
		ON_BN_CLICKED(IDC_BUTTON_REFRESH, &PackedFileSearchDlg::OnBnClickedButtonRefresh)
	END_MESSAGE_MAP()


	// PackedFileSearchDlg 메시지 처리기입니다.

	int PackedFileSearchDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);
	
		return 0;
	}


	void PackedFileSearchDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);

		const int INTERVAL = 20;

		RECT rcTextDesc, rcCombo, rcEdit, rcButtonSearch, rcButtonRefresh;

		// 컨트롤들 상대 좌표로 박아넣자
		// static text.
		if (m_textDesc.GetSafeHwnd())
		{
			rcTextDesc.left = (LONG)(cx * 0.02f);
			rcTextDesc.right = (LONG)(cx * 0.18f);
			rcTextDesc.top = (LONG)(cy * 0.5f) - 10;
			rcTextDesc.bottom = (LONG)(cy * 0.5f) + 10;
			m_textDesc.MoveWindow(&rcTextDesc);
		}

		// nation combobox.
		if (m_SelectNation.GetSafeHwnd())
		{
			rcCombo.left = rcTextDesc.right + INTERVAL;
			rcCombo.right = (LONG)(cx * 0.3f);
			rcCombo.top = rcTextDesc.top;
			rcCombo.bottom = rcTextDesc.bottom;
			m_SelectNation.MoveWindow(&rcCombo);
		}

		// search edit.
		if (m_editSearcher.GetSafeHwnd())
		{
			rcEdit.left = rcCombo.right + INTERVAL;
			rcEdit.right = (LONG)(cx * 0.78f);
			rcEdit.top = rcTextDesc.top;
			rcEdit.bottom = rcTextDesc.bottom;
			m_editSearcher.MoveWindow(&rcEdit);
		}

		// search button.
		if (m_buttonSearch.GetSafeHwnd())
		{
			rcButtonSearch.left = rcEdit.right + INTERVAL;
			rcButtonSearch.right = (LONG)(cx * 0.9f);
			rcButtonSearch.top = rcTextDesc.top;
			rcButtonSearch.bottom = rcTextDesc.bottom;
			m_buttonSearch.MoveWindow(&rcButtonSearch);
		}

		// refresh button.
		if (m_buttonSearch.GetSafeHwnd())
		{
			rcButtonRefresh.left = rcButtonSearch.right + INTERVAL;
			rcButtonRefresh.right = (LONG)(cx * 0.98f);
			rcButtonRefresh.top = rcTextDesc.top;
			rcButtonRefresh.bottom = rcTextDesc.bottom;
			m_ButtonRefresh.MoveWindow(&rcButtonRefresh);
		}
	}


	BOOL PackedFileSearchDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}


	HBRUSH PackedFileSearchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		switch (nCtlColor)
		{
		case CTLCOLOR_EDIT:			
			pDC->SetBkColor(COLOR_BLACK);
			pDC->SetTextColor(COLOR_WHITE);
			hbr = m_bkGndBrush;			
			break;

		case CTLCOLOR_STATIC:			
			pDC->SetBkColor(COLOR_IVORY_BLACK);
			pDC->SetTextColor(COLOR_GREEN);
			hbr = m_bkGndBrush;			
			break;
		}

		return hbr;
	}

	// Search Button 클릭
	void PackedFileSearchDlg::OnBnClickedButtonPackedfilesearch()
	{
		_SetSearchedFilesToParent();
	}

	// refresh button으로 로드 반영.
	void PackedFileSearchDlg::OnBnClickedButtonRefresh()
	{
		if (true == m_pParent->Load(m_SelectNation.GetCurSel()))
		{
			CString notice, selectedCountryName;
			m_SelectNation.GetLBText(m_SelectNation.GetCurSel(), selectedCountryName);
			notice.Format(_T("%s 데이터 Load가 완료되었습니다."), selectedCountryName);

			::AfxMessageBox(notice);
		}
		m_pParent->SetChangedState(false);
	}
}
