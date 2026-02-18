// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3VTexView.h : interface of the Ci3VTexView class
//


#pragma once

#include "i3VTexFramework.h"

class Ci3VTexView : public CView
{
protected: // create from serialization only
	Ci3VTexView();
	DECLARE_DYNCREATE(Ci3VTexView)

// Attributes
public:
	Ci3VTexDoc* GetDocument() const;

	i3TDKViewerCtrl		m_ViewerCtrl;
	i3VTexFramework *	m_pFramework;

	INT32		m_X, m_Y;
	INT32		m_W, m_H;

	i3UIRenderer *	m_pUIRenderer;
	INT32		m_hSprite;

// Operations
public:
	void		_UpdateSprite(void);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~Ci3VTexView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void	UpdateHScroll(void);
	void	UpdateVScroll(void);
	void	InitScroll(void);

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

#ifndef _DEBUG  // debug version in i3VTexView.cpp
inline Ci3VTexDoc* Ci3VTexView::GetDocument() const
   { return reinterpret_cast<Ci3VTexDoc*>(m_pDocument); }
#endif

