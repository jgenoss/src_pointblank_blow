#pragma once

#include "afxcmn.h"

class CListSubEdit : public CEdit
{
	DECLARE_DYNAMIC(CListSubEdit)   

public:   
	CListSubEdit(int nItem, int nSubItem, LPCTSTR szpString);   
	virtual ~CListSubEdit();   

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
	DECLARE_MESSAGE_MAP()

public:   
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);   
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);   
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseUp();

private:   
	int         m_nItem;
	int         m_nSubItem;
	LPCTSTR m_String;
	bool        m_bEnter;  
};