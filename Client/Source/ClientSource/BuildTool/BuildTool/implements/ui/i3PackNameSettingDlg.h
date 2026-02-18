#pragma once

#include "CustomEditBox.h"

namespace ui
{
	class i3PackNameSettingDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(i3PackNameSettingDlg)

	public:
		i3PackNameSettingDlg(CWnd* pParent = NULL);
		virtual ~i3PackNameSettingDlg();

		const std::string& GetNewi3PackName() const { return m_strNewi3PackName; }

	private:
		CustomEditBox m_editi3PackName;
		std::string m_strNewi3PackName;
		CBrush m_bkGndBrush;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
		afx_msg void OnOK();
	};
}
