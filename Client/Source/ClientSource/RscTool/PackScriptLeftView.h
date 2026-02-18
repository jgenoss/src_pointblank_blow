#pragma once


// PackScriptLeftView view

// 팩스크립트의 폴더만 표시한다.

#include "i3MFCTreeCtrl.h"
#include "i3MFCButton.h"
#include "ShellChangeWatcher.h"

class PackScriptFolder;

class PackScriptLeftView : public CView
{
	DECLARE_DYNCREATE(PackScriptLeftView)

protected:
	PackScriptLeftView();           // protected constructor used by dynamic creation
	virtual ~PackScriptLeftView();
public:	
	void					SetRightView(class PackScriptRightView* pView) { m_pRightView = pView; }
	PackScriptRightView*	GetRightView() const { return m_pRightView; }
	void					SetPackDiffView(class PackDiffView* pView) { m_pPackDiffView = pView ; }
	PackDiffView*			GetPackDiffView() const { return m_pPackDiffView; }
	void					SetLocaleStringDiffView(class LocaleStringDiffView* pView) { m_pLocaleStringDiffView = pView; }
	LocaleStringDiffView*	GetLocaleStringDiffView() const { return m_pLocaleStringDiffView; }

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	
	i3MFCTreeCtrl*				GetTreeCtrl() { return &m_treeCtrl; }
	void						UpdatePackScriptFolder();

protected:
	DECLARE_MESSAGE_MAP()

private:

	i3MFCButton					m_button_exception;
	i3MFCTreeCtrl				m_treeCtrl;
	PackScriptRightView*		m_pRightView;
	PackDiffView*				m_pPackDiffView;
	LocaleStringDiffView*		m_pLocaleStringDiffView;
	ShellChangeWatcher			m_watcher;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};



