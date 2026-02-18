/////////////////////////////////////////////////////////////////////////////
//
//  Written by Georges Kanaan
//  Email: gkanaan@hotmail.com
//
//  Date: Nov 10, 2001
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_SPLITDIALOG_H_INCLUDED_)
#define _SPLITDIALOG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitDialog dialog

#include "Pane.h"

class CSplitDialog : public CDialog
{
// Construction
public:
	CSplitDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSplitDialog();

	CMyPane *m_pMainPane; //The main Pane

	void Initialize();
	CMyPane *CreatePane(int nSplitType, CMyPane *pParent = NULL, int nWhich = 0, BOOL bSizeableControls = TRUE);
	BOOL SetPaneWnd(CMyPane *pPane, CWnd *pWnd, int nWhich, int minWidth = 100, int minHeight = 100, int Width = 0, int Height = 0);
	BOOL ShowPane(CMyPane *pPane, BOOL bShow = TRUE, int nWhich = 0);
	void SetMainRect(RECT rect);
	void SetMainRect(int X, int Y, int nWidth, int nHeight);
	void SetTopOffset(int topOffset);
	void SetLeftOffset(int leftOffset);
	void SetRightOffset(int rightOffset);
	void SetBottomOffset(int bottomOffset);


	//{{AFX_DATA(CSplitDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	enum DRAGFLAG { DRAG_ENTER=0, DRAG_EXIT=1, DRAGGING=2 };
	BOOL m_bMovingSplitter; //Needed to know when we are sizing the splitter
	CMyPane * m_movingPane; //Needed when sizing the splitter to keep track of the pane being sized
	RECT m_OffsetRect;  //this holds the offsets from the dialog borders
	CRect	m_rectDragPrev;
	CRect	m_rectDragCurt;
	void DrawDraggingBar(CPoint point,DRAGFLAG df);

	// Generated message map functions
	//{{AFX_MSG(CSplitDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSizing( UINT nSide, LPRECT lpRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_SPLITDIALOG_H_INCLUDED_)
