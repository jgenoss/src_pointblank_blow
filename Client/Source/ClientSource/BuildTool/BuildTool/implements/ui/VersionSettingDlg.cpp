#include "stdafx.h"
#include "VersionSettingDlg.h"
#include "BuildTool.h"
#include "UIDefines.h"
#include "VersionSettingList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct CustomDlgButtonCallback : CustomButtonCallback
	{
		CustomDlgButtonCallback(const std::function<void()>& f) : m_f(f) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			m_f();
		}
		std::function<void()> m_f;
	};

	IMPLEMENT_DYNAMIC(VersionSettingDlg, CDialogEx)

	VersionSettingDlg::VersionSettingDlg(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_DIALOG_VERSION_SETTING, pParent)
	{
		m_brushIvoryBlackLight.CreateSolidBrush(COLOR_IVORY_BLACK_LIGHT);
		m_brushOrange.CreateSolidBrush(COLOR_ORANGE);
	}

	VersionSettingDlg::~VersionSettingDlg()
	{}

	void VersionSettingDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL VersionSettingDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_btnOK.SubclassDlgItem(IDOK, this);
		m_btnOK.SetCallback(std::make_unique<CustomDlgButtonCallback>(std::bind(&VersionSettingDlg::OnOK, this)));
		
		m_btnCancel.SubclassDlgItem(IDCANCEL, this);
		m_btnCancel.SetCallback(std::make_unique<CustomDlgButtonCallback>(std::bind(&VersionSettingDlg::OnCancel, this)));

		std::vector<rsc::VerDataPtr> dataList;
		rsc::VersionSettings::i()->GetAllDataList(dataList);
		for (const auto& data : dataList)
		{
			const long row = m_versionListCtrl->AddNewRow();
			m_versionListCtrl->SetVersionData(row, data);
		}

		return TRUE;
	}

	BEGIN_MESSAGE_MAP(VersionSettingDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_DRAWITEM()
	END_MESSAGE_MAP()

	int VersionSettingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1) return -1;

		m_versionListCtrl = std::make_unique<VersionSettingList>(*this);

		return 0;
	}

	void VersionSettingDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);

		RECT rt;
		rt.left = 0;	rt.top = 0;
		rt.right = cx;	rt.bottom = cy;

		if (m_versionListCtrl.get())
			m_versionListCtrl->AdjustLayout(rt);
	}

	BOOL VersionSettingDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush brush(COLOR_BLACK);

		CBrush* pOldBrush = pDC->SelectObject(&brush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	void VersionSettingDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		// 다이얼로그 리소스에 owner draw 속성을 추가해야 호출된다.
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

	void VersionSettingDlg::OnOK()
	{
		std::vector<rsc::VersionData> listCtrlDataList;
		m_versionListCtrl->GetAllDataList(listCtrlDataList);

		std::vector<rsc::VerDataPtr> verDataList;
		rsc::VersionSettings::i()->GetAllDataList(verDataList);

		for (const auto& ctrlData : listCtrlDataList)
		{
			rsc::VerDataPtr data = rsc::VersionSettings::i()->FindData(ctrlData.GetVersion());
			data->SetPointBlankAbsPath(ctrlData.GetPointBlankAbsPath());
			data->Seti3EngineAbsPath(ctrlData.Geti3EngineAbsPath());
		}

		rsc::VersionSettings::i()->Save();

		CDialogEx::OnOK();
	}

	void VersionSettingDlg::OnCancel()
	{
		CDialogEx::OnCancel();
	}
}