#pragma once

#include "RSCGenDefine.h"

// CListViewCtrl
class CListViewCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CListViewCtrl)

public:
	CListViewCtrl();
	virtual ~CListViewCtrl();

//	INT32	HitTestEx( CPoint &point, INT32 * col);

	RSC_FIELD_INFO	*	m_pCurField;
	CWnd			*	m_pCurCtrl;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);


public:
	CEdit		*	EditSubLabel( RSC_FIELD_INFO * pInfo, INT32 nItem, INT32 nCol); 
	CComboBox	*	SelectValue( RSC_FIELD_INFO * pInfo, INT32 nItem, INT32 nCol, CStringList &lstItems, INT32 nSel);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


