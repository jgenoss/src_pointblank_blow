#if !defined(_MEDIA_TREE_CTRL_H_)
#define _MEDIA_TREE_CTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DragDropTreeCtrl.h"

class CMediaTreeCtrl : public CDragDropTreeCtrl
{
	// Construction
public:
	CMediaTreeCtrl();

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
	virtual ~CMediaTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragDropTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif