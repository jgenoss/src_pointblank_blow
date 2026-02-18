#include "stdafx.h"
#include "i3PackNameSettingDlg.h"
#include "BuildTool.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	IMPLEMENT_DYNAMIC(i3PackNameSettingDlg, CDialogEx)

	i3PackNameSettingDlg::i3PackNameSettingDlg(CWnd* pParent) : CDialogEx(IDD_DIALOG_MAKE_I3PACK_NAME, pParent)
	{}

	i3PackNameSettingDlg::~i3PackNameSettingDlg()
	{}

	void i3PackNameSettingDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL i3PackNameSettingDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_editi3PackName.SubclassDlgItem(IDC_EDIT_I3PACK_NAME, this);
		m_editi3PackName.SetColor(COLOR_IVORY_BLACK, COLOR_SILVER);

		return TRUE;
	}

	BEGIN_MESSAGE_MAP(i3PackNameSettingDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_DRAWITEM()
		ON_WM_CTLCOLOR()
	END_MESSAGE_MAP()

	int i3PackNameSettingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		return 0;
	}

	void i3PackNameSettingDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);
	}

	BOOL i3PackNameSettingDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	HBRUSH i3PackNameSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		switch (nCtlColor)
		{
		case CTLCOLOR_STATIC:
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_I3PACK_NAME)
			{
				pDC->SetBkColor(COLOR_IVORY_BLACK);
				pDC->SetTextColor(COLOR_GREEN);

				hbr = m_bkGndBrush;
			}
			break;
		}

		return hbr;
	}

	void i3PackNameSettingDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		if (nIDCtl == IDOK || nIDCtl == IDCANCEL)
		{
			CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

			CRect rt(lpDrawItemStruct->rcItem);

			CString caption;
			GetDlgItemText(nIDCtl, caption);

			pDC->SetBkMode(TRANSPARENT);

			pDC->FillSolidRect(rt, COLOR_BLACK);
			pDC->Draw3dRect(rt, COLOR_TAN3_PERU, COLOR_TAN3_PERU);
			pDC->SetTextColor(COLOR_TAN3_PERU);
			pDC->DrawText(caption, rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	void i3PackNameSettingDlg::OnOK()
	{
		CString text;
		m_editi3PackName.GetWindowText(text);

		if (!text.IsEmpty())
		{
			std::wstring wstr;
			i3::utf16_to_mb(text.GetString(), m_strNewi3PackName);
		}

		CDialogEx::OnOK();
	}
}