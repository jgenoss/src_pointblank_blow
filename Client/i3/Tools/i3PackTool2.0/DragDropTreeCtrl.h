// DragDropTreeCtrl.h
// For OLE Drag and Drop between tree controls
// Designed and developed by Vinayak Tadas
// vinayakt@aditi.com
// 

#if !defined(AFX_DRAGDROPTREECTRL_H__29F8894F_897C_11D3_A59E_00A02411D21E__INCLUDED_)
#define AFX_DRAGDROPTREECTRL_H__29F8894F_897C_11D3_A59E_00A02411D21E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragDropTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragDropTreeCtrl window
#include "TreeDropTarget.h"

// *****************************************************************
// CDragDropTreeCtrl control
// *****************************************************************

class PTFile;

class CDragDropTreeCtrl : public CTreeCtrl
{
// Construction
public:
	
	CDragDropTreeCtrl();

// Attributes
public:
	// Adds an item to the tree control
	virtual HTREEITEM AddItem(HTREEITEM hParent, CString csItemName, HTREEITEM hInsAfter, int iSelImage , int iNonSelImage, long lParam);
	virtual HTREEITEM AddItem(HTREEITEM hParent, PTFile* fileSelected, HTREEITEM hInsAfter, int iSelImage , int iNonSelImage, long lParam) { return 0; }


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragDropTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragDropTreeCtrl();

	// Generated message map functions
private:
	//{{AFX_MSG(CDragDropTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	//CTreeDropTarget m_CTreeDropTarget;
	CTreeDropTarget* m_CTreeDropTarget; // 파생클래스에 대해 동적으로 생성. 지우근.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGDROPTREECTRL_H__29F8894F_897C_11D3_A59E_00A02411D21E__INCLUDED_)
