#include "stdafx.h"
#include "BuildTool.h"
#include "ContentsViewToolbar.h"
#include "UIDefines.h"
#include "ContentsView.h"
#include "afxdialogex.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	// ContentsViewToolbar 대화 상자입니다.

	IMPLEMENT_DYNAMIC(ContentsViewToolbar, CDialogEx)

	ContentsViewToolbar::ContentsViewToolbar(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_CONTENTSVIEW_TOOLBAR, pParent)
	{
	}

	ContentsViewToolbar::~ContentsViewToolbar()
	{
	}

	void ContentsViewToolbar::SetTopicText(const std::wstring& wstrText)
	{
		m_textTopic.SetWindowText(wstrText.c_str());
	}

	void ContentsViewToolbar::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL ContentsViewToolbar::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_editSearchContents.SubclassDlgItem(IDC_EDIT_SEARCH_CONTENTS, this);
		
		m_textFindByKeywords.SubclassDlgItem(IDC_STATIC_FINDBYKEYWORDS, this);

		m_textTopic.SubclassDlgItem(IDC_STATIC_CONTENTSVIEW_TOPIC, this);
		LOGFONT lf;	GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_SEMIBOLD;
		lf.lfHeight = 20;
		m_fontTopic.CreateFontIndirect(&lf);
		m_textTopic.SetFont(&m_fontTopic, TRUE);

		
		return TRUE;
	}

	BEGIN_MESSAGE_MAP(ContentsViewToolbar, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_CTLCOLOR()
		ON_EN_CHANGE(IDC_EDIT_SEARCH_CONTENTS, &ContentsViewToolbar::OnEnChangeEditSearchContents)
		ON_BN_CLICKED(IDOK, &ContentsViewToolbar::OnBnClickedOK)
		ON_BN_CLICKED(IDCANCEL, &ContentsViewToolbar::OnBnClickedCancel)
	END_MESSAGE_MAP()


	// ContentsViewToolbar 메시지 처리기입니다.
	int ContentsViewToolbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		return 0;
	}

	void ContentsViewToolbar::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);
	}

	BOOL ContentsViewToolbar::OnEraseBkgnd(CDC* pDC)
	{
		// TODO: Add your message handler code here and/or call default
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	HBRUSH ContentsViewToolbar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		switch (nCtlColor)
		{
		case CTLCOLOR_EDIT:
			if (pWnd->GetDlgCtrlID() == IDC_EDIT_SEARCH_CONTENTS)
			{
				pDC->SetBkColor(COLOR_BLACK);
				pDC->SetTextColor(COLOR_WHITE);

				hbr = m_bkGndBrush;
			}
		break;

		case CTLCOLOR_STATIC:
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_FINDBYKEYWORDS || pWnd->GetDlgCtrlID() == IDC_STATIC_CONTENTSVIEW_TOPIC)
			{
				pDC->SetBkColor(COLOR_IVORY_BLACK);
				pDC->SetTextColor(COLOR_SILVER);

				hbr = m_bkGndBrush;
			}
		break;
		}

		return hbr;
	}

	void ContentsViewToolbar::OnEnChangeEditSearchContents()
	{
		CString keyword;
		m_editSearchContents.GetWindowText(keyword);

		rsc::ResourceController::i()->OnViewToolbarSearchEditChanged(keyword.GetString());
	}
}