// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "Panel_Files.h"
#include "Panel_Log.h"
#include "Panel_Opt.h"
#include "Panel_Palette.h"
#include "Panel_Viewer.h"

#include "OptionInfo.h"

class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	CPanel_Files		m_Panel_Files;
	CPanel_Log			m_Panel_Log;
	CPanel_Opt			m_Panel_Opt;
	CPanel_Palette		m_Panel_Palette;
	CPanel_Viewer		m_Panel_Viewer;

// Operations
public:
	void		StartProcessing(void);
	void		EndProcessing(void);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnToolsProjectconfiguration();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


