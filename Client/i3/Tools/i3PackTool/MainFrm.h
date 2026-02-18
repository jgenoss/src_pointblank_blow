// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "PanelTree.h"
#include "PanelFileList.h"
#include "PanelFileInfo.h"
#include "PanelLog.h"
#include "PanelListProjcet.h"

#include "PanelTreeScript.h"
#include "PanelTreeLive.h"

class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성
public:
//	char	m_szPath[ MAX_PATH];

// 작업
public:
	CPanelTree			m_PanelTree;
	CPanelTreeScript	m_PanelTreeScript;
	CPanelTreeLive		m_PanelTreeLive;
	CPanelFileList		m_PanelFileList;
	CPanelFileInfo		m_PanelFileInfo;
	CPanelLog			m_PanelLog;
	CPanelListProjcet	m_PanelProjcet;

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

	void	NewPack(void);
	void	ClosePack(void);
	void	LoadPack( const char * pszPath);
	void	SavePack( const char * pszPath, const char * folderPath = NULL);

	void	Add( const char * pszPath, BOOL IncludeSubDir);
	void	Remove( const char * pszPath, const BOOL includeSubDirOK);
	void	EncFile( const char * pszPath);
	void	SetChecksum( const char * pszPath, const char * folderPath = NULL);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnDestroy(void);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnFileOption();
	afx_msg void OnFileExport();
};


