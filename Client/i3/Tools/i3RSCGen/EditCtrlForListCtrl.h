#pragma once

// CEditCtrlForListCtrl

class CEditCtrlForListCtrl : public CEdit
{
	DECLARE_DYNAMIC(CEditCtrlForListCtrl)

private:
	INT32	m_nItem;
	INT32	m_nSubItem;
	CString m_strText;
	BOOL	m_bESC;
public:
	CEditCtrlForListCtrl(INT32 nItem, INT32 nSubItem, CString strInit);
	virtual ~CEditCtrlForListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};


