#if !defined(_LIVE_TREE_CTRL_H_)
#define _LIVE_TREE_CTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DragDropTreeCtrl.h"

class PTFile;

class CLiveTreeCtrl : public CDragDropTreeCtrl
{
	// Construction
public:
	CLiveTreeCtrl();

	// Attributes
public:
	virtual HTREEITEM AddItem(HTREEITEM hParent, PTFile* fileSelected, HTREEITEM hInsAfter, int iSelImage , int iNonSelImage, long lParam);


	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragDropTreeCtrl)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CLiveTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragDropTreeCtrl)
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnRClicked(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnPacking();
	afx_msg void	OnDelete();
	afx_msg void	OnExport();
	afx_msg void	OnNewPack();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif