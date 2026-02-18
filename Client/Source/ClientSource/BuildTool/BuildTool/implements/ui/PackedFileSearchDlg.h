#pragma once
#include "afxwin.h"
#include "../resource/CountrySettings.h"

namespace ui
{
	class PackedFileSearchBar;
	class PackedFileSearchDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(PackedFileSearchDlg)

	public:
		PackedFileSearchDlg(CWnd* pParent = NULL);
		virtual ~PackedFileSearchDlg();

#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DIALOG_PACKEDFILESEARCH };
#endif

	private:
		void _SetDisableCombobox(bool bDisable);
		void _SetSearchedFilesToParent();

	public:
		int GetIndexOfCurrentCountry() {	return m_SelectNation.GetCurSel(); }
		void Init(std::vector<rsc::CountryDataPtr> countryList);
		
	private:
		CBrush	m_bkGndBrush;
		CFont	m_font;
		CStatic m_textDesc;

		CComboBox m_SelectNation;

		CEdit	m_editSearcher;
		CString	m_strSearch; // m_editSearcher Value.
		CButton m_buttonSearch;
		CButton m_ButtonRefresh;

		PackedFileSearchBar* m_pParent;

	public:
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();
		BOOL PreTranslateMessage(MSG* pMsg);

	public:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnBnClickedButtonPackedfilesearch();
		afx_msg void OnBnClickedButtonRefresh();
	};

}
