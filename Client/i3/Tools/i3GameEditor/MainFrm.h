// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once


#include "OptionDialog.h"
#include "PanelGame.h"
#include "PanelSpec.h"
#include "PanelControl.h"
#include "PanelLog.h"


class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성
public:
	PanelGame		m_PanelGame;
	PanelSpec		m_PanelSpec;
	PanelControl	m_PanelControl;
	PanelLog		m_PanelLog;

	char			m_szWorkFileName[256];

// 작업
public:
	void	Init( void);
	BOOL	CheckFileName( const char * pszFileName, char * pszExt);
	BOOL	OpenFile( const char * pszFileName);
	BOOL	SaveFile( const char * pszFileName);

	char *	GetWorkFileName( void)		{	return m_szWorkFileName;	}

	
/////////////////////////////////////////////////////////////////////////////////////////////////
// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnOption();
};


