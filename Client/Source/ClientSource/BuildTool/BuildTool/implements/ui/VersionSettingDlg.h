#pragma once

#include "CustomButton.h"
#include "VersionSettingDlg.h"

namespace ui
{
	class VersionSettingList;

	class VersionSettingDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(VersionSettingDlg)

	public:
		VersionSettingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
		virtual ~VersionSettingDlg();

	private:
		CBrush m_brushIvoryBlackLight, m_brushOrange;

		std::unique_ptr<VersionSettingList> m_versionListCtrl;
		CustomButton m_btnOK;
		CustomButton m_btnCancel;

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