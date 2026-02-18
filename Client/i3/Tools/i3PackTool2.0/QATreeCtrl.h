#if !defined(_QA_TREE_CTRL_H_)
#define _QA_TREE_CTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DragDropTreeCtrl.h"

class CQATreeCtrl : public CDragDropTreeCtrl
{
	// Construction
public:
	CQATreeCtrl();

	// Attributes
public:


	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragDropTreeCtrl)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CQATreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragDropTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif