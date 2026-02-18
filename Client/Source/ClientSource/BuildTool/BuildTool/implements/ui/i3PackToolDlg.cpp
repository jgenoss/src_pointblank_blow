#include "stdafx.h"
#include "i3PackToolDlg.h"
#include "BuildTool.h"
#include "UIDefines.h"
#include "i3PackToolTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct i3PackToolBrowsePathCallback : CustomUGEditBrowseCallback
	{
		using FuncType = std::function<void(const std::string)>;
		FuncType fnSetPath = nullptr;

		i3PackToolBrowsePathCallback(const FuncType& f) : fnSetPath(f)
		{}

		virtual void OnSelectOK(const std::wstring& wstrPath) override
		{
			if (wstrPath.empty())
				return;

			std::string strPath;
			i3::utf16_to_mb(wstrPath, strPath);

			fnSetPath(strPath);
		}
	};

	struct i3PackToolButtonCallback : CustomButtonCallback
	{
		using FuncType = std::function<void()>;
		FuncType fn = nullptr;

		i3PackToolButtonCallback(const FuncType& f) : fn(f)
		{}

		virtual void OnClick(CustomButton* ctrl) override
		{
			fn();
		}
	};

	IMPLEMENT_DYNAMIC(i3PackToolDlg, CDialogEx)

	i3PackToolDlg::i3PackToolDlg(CWnd* pParent) : CDialogEx(IDD_DIALOG_PACKTOOL, pParent)
	{}

	i3PackToolDlg::~i3PackToolDlg()
	{}
	
	void i3PackToolDlg::OnClose()
	{
		m_treeCtrl->ShutdownThreads();
	}

	void i3PackToolDlg::SetBrowsePackPathText(const std::wstring& wstrPath) 
	{ 
		if (wstrPath.empty())
			return;

		m_browsePackPath.SetText(wstrPath.c_str());

		std::string strPath;
		i3::utf16_to_mb(wstrPath, strPath);
		m_strPackTgtPath = strPath;
	}

	void i3PackToolDlg::SetBrowseSourcePathText(const std::wstring& wstrPath) 
	{ 
		if (wstrPath.empty())
			return;

		m_browseSourcePath.SetText(wstrPath.c_str());

		std::string strPath;
		i3::utf16_to_mb(wstrPath, strPath);
		m_strPackSrcWorkdir = strPath;
	}

	void i3PackToolDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL i3PackToolDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_treeCtrl = std::make_unique<i3PackToolTree>(*this);

		m_textSourcePath.SubclassDlgItem(IDC_STATIC_PACKTOOL_SOURCE_PATH, this);
		m_textPackPath.SubclassDlgItem(IDC_STATIC_PACKTOOL_PACK_PATH, this);

		LOGFONT lf;	GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_MEDIUM;
		lf.lfHeight = 13;
		m_font.CreateFontIndirect(&lf);

		m_textSourcePath.SetFont(&m_font, TRUE);
		m_textPackPath.SetFont(&m_font, TRUE);

		m_btnNewPack.SubclassDlgItem(IDC_BUTTON_PACKTOOL_NEWPACK, this);
		m_btnSavePack.SubclassDlgItem(IDC_BUTTON_PACKTOOL_SAVEPACK, this);
		m_btnAddFiles.SubclassDlgItem(IDC_BUTTON_PACKTOOL_ADDFILE, this);
		m_btnAddFolder.SubclassDlgItem(IDC_BUTTON_PACKTOOL_ADDFOLDER, this);

		m_btnNewPack.SetCallback(std::make_unique<i3PackToolButtonCallback>(std::bind(&i3PackToolTree::OnNewPack, m_treeCtrl.get())));
		m_btnSavePack.SetCallback(std::make_unique<i3PackToolButtonCallback>(std::bind(&i3PackToolTree::OnSavePack, m_treeCtrl.get())));
		m_btnAddFiles.SetCallback(std::make_unique<i3PackToolButtonCallback>(std::bind(&i3PackToolTree::OnAddFilesToPack, m_treeCtrl.get())));
		m_btnAddFolder.SetCallback(std::make_unique<i3PackToolButtonCallback>(std::bind(&i3PackToolTree::OnAddFolderFilesToPack, m_treeCtrl.get())));

		return TRUE;
	}

	BEGIN_MESSAGE_MAP(i3PackToolDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_DRAWITEM()
		ON_WM_CTLCOLOR()
	END_MESSAGE_MAP()

	int i3PackToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		m_browseSourcePath.SetCallback(std::make_unique<i3PackToolBrowsePathCallback>(std::bind(&i3PackToolDlg::SetPackSrcWorkdir, this, std::placeholders::_1)));
		m_browseSourcePath.SetBackColor(COLOR_BLACK);
		m_browseSourcePath.SetTextColor(COLOR_TAN3_PERU);
		m_browseSourcePath.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, ID_BROWSE_PACKTOOL_SOURCE_PATH);
		
		m_browsePackPath.SetCallback(std::make_unique<i3PackToolBrowsePathCallback>(std::bind(&i3PackToolDlg::SetPackTgtPath, this, std::placeholders::_1)));
		m_browsePackPath.SetBackColor(COLOR_BLACK);
		m_browsePackPath.SetTextColor(COLOR_TAN3_PERU);
		m_browsePackPath.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, ID_BROWSE_PACKTOOL_PACK_PATH);

		return 0;
	}

	void i3PackToolDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);

		RECT rt;

		if (m_textSourcePath.GetSafeHwnd())
		{
			m_textSourcePath.GetWindowRect(&rt);
			ScreenToClient(&rt);
			rt.left = rt.right;
			rt.right = cx - 10;
			m_browseSourcePath.AdjustLayout(rt);
		}

		if (m_textPackPath.GetSafeHwnd())
		{
			m_textPackPath.GetWindowRect(&rt);
			ScreenToClient(&rt);
			rt.left = rt.right;
			rt.right = cx - 10;
			m_browsePackPath.AdjustLayout(rt);
		}
		if (m_btnNewPack.GetSafeHwnd() && m_btnSavePack.GetSafeHwnd() && m_btnAddFiles.GetSafeHwnd() && m_btnAddFolder.GetSafeHwnd())
		{
			if (m_textPackPath.GetSafeHwnd())
			{
				m_textPackPath.GetWindowRect(&rt);
				ScreenToClient(&rt);

				const int cx_gap = 5;
				const int cx_buttons = cx - (rt.left * 2) - (cx_gap * 2);
				const int cx_button = cx_buttons / 4;

				rt.top = rt.bottom + 10;
				rt.bottom = rt.top + 25;
				rt.right = rt.left + cx_button;
				m_btnNewPack.MoveWindow(&rt);

				rt.left = rt.right + cx_gap;
				rt.right = rt.left + cx_button;
				m_btnSavePack.MoveWindow(&rt);

				rt.left = rt.right + cx_gap;
				rt.right = rt.left + cx_button;
				m_btnAddFiles.MoveWindow(&rt);

				rt.left = rt.right + cx_gap;
				rt.right = rt.left + cx_button;
				m_btnAddFolder.MoveWindow(&rt);
			}
		}

		if (m_btnNewPack.GetSafeHwnd())
		{
			rt.left = 0;
			rt.right = cx;
			rt.top = rt.bottom + 30;
			rt.bottom = cy;
			m_treeCtrl->AdjustLayout(rt);
		}
	}

	BOOL i3PackToolDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}
	
	HBRUSH i3PackToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		switch (nCtlColor)
		{
		case CTLCOLOR_STATIC:
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_PACKTOOL_SOURCE_PATH || pWnd->GetDlgCtrlID() == IDC_STATIC_PACKTOOL_PACK_PATH)
			{
				pDC->SetBkColor(COLOR_IVORY_BLACK);
				pDC->SetTextColor(COLOR_GREEN);

				hbr = m_bkGndBrush;
			}
			break;
		}

		return hbr;
	}
	
	void i3PackToolDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		if (nIDCtl == IDC_BUTTON_PACKTOOL_NEWPACK || nIDCtl == IDC_BUTTON_PACKTOOL_SAVEPACK 
			|| nIDCtl == IDC_BUTTON_PACKTOOL_ADDFILE || nIDCtl == IDC_BUTTON_PACKTOOL_ADDFOLDER)
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
}