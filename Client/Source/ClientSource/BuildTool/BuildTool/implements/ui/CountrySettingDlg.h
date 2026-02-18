#pragma once

#include "CountrySettingList.h"
#include "CustomButton.h"

namespace ui
{
	class CountrySettingDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(CountrySettingDlg)

	public:
		CountrySettingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
		virtual ~CountrySettingDlg();

	private:
		CBrush m_brushIvoryBlackLight, m_brushOrange;
		
		std::unique_ptr<CountrySettingList> m_countryListCtrl;

		CustomButton m_buttonAdd;
		CustomButton m_buttonDelete;
		CustomButton m_buttonOK;
		CustomButton m_buttonCancel;

		// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DIALOG_MODIFY_COUNTRY };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
		afx_msg void OnOK();
		afx_msg void OnCancel();
	};
}
