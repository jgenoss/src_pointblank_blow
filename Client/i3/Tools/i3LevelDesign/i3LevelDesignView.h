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

// i3LevelEditorView.h : interface of the Ci3LevelDesignView class
//


#pragma once


class Ci3LevelDesignView : public CView
{
protected: // create from serialization only
	Ci3LevelDesignView();
	DECLARE_DYNCREATE(Ci3LevelDesignView)

// Attributes
public:
	Ci3LevelDesignDoc* GetDocument() const;

	i3LevelViewport			m_Viewport;

// Operations
public:
	void			DropObject( i3LevelRes * pObj, POINT pt);

protected:
	void			ReplaceControl( int cx, int cy);
	void			OnUpdateSys( I3_TDK_UPDATE_INFO * pInfo);

// Overrides
public:
	virtual void OnDraw(CDC* pDC) override;  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;

// Implementation
public:
	virtual ~Ci3LevelDesignView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

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
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};

#ifndef _DEBUG  // debug version in i3LevelEditorView.cpp
inline Ci3LevelDesignDoc* Ci3LevelDesignView::GetDocument() const
   { return reinterpret_cast<Ci3LevelDesignDoc*>(m_pDocument); }
#endif

