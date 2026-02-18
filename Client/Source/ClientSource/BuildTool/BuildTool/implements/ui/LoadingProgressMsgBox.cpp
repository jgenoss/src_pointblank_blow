#include "stdafx.h"
#include "BuildTool.h"
#include "LoadingProgressMsgBox.h"
#include "afxdialogex.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	// LoadingProgressMsgBox 대화 상자입니다.

	IMPLEMENT_DYNAMIC(LoadingProgressMsgBox, CDialogEx)

	LoadingProgressMsgBox::LoadingProgressMsgBox(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_DIALOG_LOADING_PROGRESS, pParent)
	{

	}

	LoadingProgressMsgBox::~LoadingProgressMsgBox()
	{
	}

	void LoadingProgressMsgBox::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL LoadingProgressMsgBox::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		LOGFONT lf;	GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_SEMIBOLD;
		lf.lfHeight = 16;
		m_font.CreateFontIndirect(&lf);

		m_textLoadingWhat.SubclassDlgItem(IDC_STATIC_LOADING_WHAT, this);
		m_textLoadingWhat.SetFont(&m_font, TRUE);
		m_textLoadingWhat.SetWindowText(L"");

		return TRUE;
	}

	void LoadingProgressMsgBox::SetText(const std::wstring& wstrText)
	{
		m_textLoadingWhat.SetWindowTextW(wstrText.c_str());
	}


	BEGIN_MESSAGE_MAP(LoadingProgressMsgBox, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_CTLCOLOR()
	END_MESSAGE_MAP()


	// LoadingProgressMsgBox 메시지 처리기입니다.
	int LoadingProgressMsgBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrushBlack.CreateSolidBrush(COLOR_BLACK);

		return 0;
	}

	void LoadingProgressMsgBox::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);

		if (m_textLoadingWhat.GetSafeHwnd())
		{
			RECT rt;	
			rt.left = 0;	rt.right = cx;

			const LONG cy_text = static_cast<LONG>(cy * 0.7f);
			const LONG cy_margin = cy - cy_text;
			const LONG cy_margin_half = static_cast<LONG>(cy_margin * 0.5f);

			rt.top = cy_margin_half;
			rt.bottom = cy_margin_half + cy_text;

			m_textLoadingWhat.MoveWindow(&rt);
		}
	}

	BOOL LoadingProgressMsgBox::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrushBlack);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	HBRUSH LoadingProgressMsgBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		switch (nCtlColor)
		{
		case CTLCOLOR_STATIC:
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOADING_WHAT)
			{
				pDC->SetBkColor(COLOR_BLACK);
				pDC->SetTextColor(COLOR_ORANGE);

				hbr = m_bkGndBrushBlack;
			}
			break;
		}

		return hbr;
	}
}
