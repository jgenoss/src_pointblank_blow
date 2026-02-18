// ModifyCountryDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "BuildTool.h"
#include "CountrySettingDlg.h"
#include "afxdialogex.h"
#include "UIDefines.h"
#include "resource.h"
#include "resource/ResourceController.h"
#include "MainFrm.h"
#include "ui/FileView.h"

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

	struct CustomDlgButtonAddCallback : CustomButtonCallback
	{
		CustomDlgButtonAddCallback(const std::function<void(long*, long*)>& f) : m_f(f) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			long rowUpper, rowLower;
			m_f(&rowUpper, &rowLower);
		}
		std::function<void(long*, long*)> m_f;
	};

	IMPLEMENT_DYNAMIC(CountrySettingDlg, CDialogEx)

	CountrySettingDlg::CountrySettingDlg(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_DIALOG_COUNTRY_SETTING, pParent)
	{
		m_brushIvoryBlackLight.CreateSolidBrush(COLOR_IVORY_BLACK_LIGHT);
		m_brushOrange.CreateSolidBrush(COLOR_ORANGE);
	}

	CountrySettingDlg::~CountrySettingDlg()
	{
	}

	void CountrySettingDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL CountrySettingDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_buttonAdd.SubclassDlgItem(ID_ADD_COUNTRY, this);
		m_buttonAdd.SetCallback(std::make_unique<CustomDlgButtonAddCallback>(
			std::bind(&CountrySettingList::AddNewRow, m_countryListCtrl.get(), std::placeholders::_1, std::placeholders::_2)));

		m_buttonDelete.SubclassDlgItem(ID_DELETE_COUNTRY, this);
		m_buttonDelete.SetCallback(std::make_unique<CustomDlgButtonCallback>(std::bind(&CountrySettingList::DeleteCurrRow, m_countryListCtrl.get())));

		m_buttonOK.SubclassDlgItem(IDOK, this);
		m_buttonOK.SetCallback(std::make_unique<CustomDlgButtonCallback>(std::bind(&CountrySettingDlg::OnOK, this)));

		m_buttonCancel.SubclassDlgItem(IDCANCEL, this);
		m_buttonCancel.SetCallback(std::make_unique<CustomDlgButtonCallback>(std::bind(&CountrySettingDlg::OnCancel, this)));

		std::vector<rsc::CountryDataPtr> countryDataList;
		rsc::CountrySettings::i()->GetDataList(countryDataList);
		for (auto &currCountryData : countryDataList)
		{
			m_countryListCtrl->AddNewCountry(currCountryData);
		}

		return TRUE;
	}


	BEGIN_MESSAGE_MAP(CountrySettingDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_DRAWITEM()
	END_MESSAGE_MAP()


	// CountrySettingDlg 메시지 처리기입니다.
	int CountrySettingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1) return -1;

		m_countryListCtrl = std::make_unique<CountrySettingList>(*this);

		return 0;
	}

	void CountrySettingDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);

		RECT rt;
		rt.left = 0;	rt.top = 0;
		rt.right = cx;	rt.bottom = cy;

		if (m_countryListCtrl.get())
			m_countryListCtrl->AdjustLayout(rt);

		RECT rtListCtrl;
		if (m_countryListCtrl.get())
			m_countryListCtrl->GetClientRect(&rtListCtrl);

		const LONG cy_button = 20;
		const LONG cx_button = 80;
		const LONG x_center = static_cast<LONG>(cx * 0.5f);

		RECT rtButtonDel;
		rtButtonDel.top = rtListCtrl.bottom + static_cast<LONG>(((cy - rtListCtrl.bottom) - cy_button) * 0.5f);
		rtButtonDel.bottom = rtButtonDel.top + cy_button;
		rtButtonDel.right = x_center - 10;
		rtButtonDel.left = rtButtonDel.right - cx_button;
		
		if (m_buttonDelete.GetSafeHwnd())
			m_buttonDelete.MoveWindow(&rtButtonDel);

		RECT rtButtonAdd;
		rtButtonAdd.right = rtButtonDel.left - 10;
		rtButtonAdd.left = rtButtonAdd.right - cx_button;
		rtButtonAdd.bottom = rtButtonDel.bottom;
		rtButtonAdd.top = rtButtonDel.top;
		
		if (m_buttonAdd.GetSafeHwnd())
			m_buttonAdd.MoveWindow(&rtButtonAdd);

		RECT rtButtonOK;
		rtButtonOK.left = rtButtonDel.right + 30;
		rtButtonOK.right = rtButtonOK.left + cx_button;
		rtButtonOK.bottom = rtButtonDel.bottom;
		rtButtonOK.top = rtButtonDel.top;

		if (m_buttonOK.GetSafeHwnd())
			m_buttonOK.MoveWindow(&rtButtonOK);

		RECT rtButtonCancel;
		rtButtonCancel.left = rtButtonOK.right + 10;
		rtButtonCancel.right = rtButtonCancel.left + cx_button;
		rtButtonCancel.bottom = rtButtonDel.bottom;
		rtButtonCancel.top = rtButtonDel.top;

		if (m_buttonCancel.GetSafeHwnd())
			m_buttonCancel.MoveWindow(&rtButtonCancel);
	}

	BOOL CountrySettingDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush brush(COLOR_BLACK);

		CBrush* pOldBrush = pDC->SelectObject(&brush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	void CountrySettingDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		if (nIDCtl == ID_ADD_COUNTRY || nIDCtl == ID_DELETE_COUNTRY || nIDCtl == IDOK || nIDCtl == IDCANCEL)
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

	void CountrySettingDlg::OnOK()
	{
		std::vector<rsc::CountryData> listCtrlDataList;
		m_countryListCtrl->GetCountryDataList(listCtrlDataList);

		std::vector<rsc::CountryDataPtr> countryDataList;
		rsc::CountrySettings::i()->GetDataList(countryDataList);

		// 국가 데이터 삭제 처리.
		for (auto &currCountryData : countryDataList)
		{
			struct fn : std::binary_function<rsc::CountryData, std::string, bool>
			{
				bool operator()(const rsc::CountryData& src, const std::string& strCountryName) const
				{
					return i3::generic_is_iequal(src.GetCountryName(), strCountryName);
				}
			};

			const std::string& strCountryName = currCountryData->GetCountryName();
			auto it = std::find_if(listCtrlDataList.begin(), listCtrlDataList.end(), std::bind2nd(fn(), strCountryName));
			if (it == listCtrlDataList.end())
			{
				rsc::CountrySettings::i()->DelData(strCountryName);
			}
		}

		// 국가 데이터 추가 또는 수정.
		for (auto &currListCtrlData : listCtrlDataList)
		{
			const std::string& strCountryName = currListCtrlData.GetCountryName();
			if (rsc::CountrySettings::i()->IsDataExist(strCountryName))
			{
				rsc::CountrySettings::i()->SetData(currListCtrlData);
			}
			else
			{
				rsc::CountrySettings::i()->AddData(currListCtrlData);
			}
		}

		rsc::CountrySettings::i()->Save();

		rsc::ResourceController::i()->OnCountryChanged();

		// PackInnerFile Searchbar recreat.
		if (true == GetMainFrame()->GetFileView().IsEsplorerCreated())
			GetMainFrame()->GetPackInnerFileSearchBar().Init();

		CDialogEx::OnOK();
	}

	void CountrySettingDlg::OnCancel()
	{
		CDialogEx::OnCancel();
	}
}
