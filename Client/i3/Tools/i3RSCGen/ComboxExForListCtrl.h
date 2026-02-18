#pragma once


// CComboxExForListCtrl
class CComboxExForListCtrl : public CComboBox
{
	DECLARE_DYNAMIC(CComboxExForListCtrl)
private:
	INT32 m_nItem;
	INT32 m_nSubItem;
	CStringList m_lstItems;
	INT32 m_nSel;
	BOOL  m_bESC;

public:
	CComboxExForListCtrl(INT32 nItem, INT32 nSubItem, CStringList *plstItems, INT32 nSel);
	virtual ~CComboxExForListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg void OnCbnCloseup();
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};


