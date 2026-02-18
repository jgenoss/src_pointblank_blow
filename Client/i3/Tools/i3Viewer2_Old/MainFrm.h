// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "Panel_SceneGraph.h"
#include "Panel_Spec.h"
#include "Panel_Toolbar.h"
#include "Panel_Viewer.h"

enum enumEngineFileType
{
	I3FILE_TYPE_UNKNOWN = 0,
	I3FILE_TYPE_SG,
	I3FILE_TYPE_ANIM,
	I3FILE_TYPE_UI,
};

class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

	char				m_szFileName[ MAX_PATH];
	CMenu *				m_pMRUPopup;

// 특성
public:
	CPanel_Toolbar		m_Panel_Toolbar;
	CPanel_SceneGraph	m_Panel_Sg;
	CPanel_Spec			m_Panel_Spec;
	CPanel_Viewer		m_Panel_Viewer;

	i3UIScene	*		m_pUI;

protected:
	enumEngineFileType	_checkFileName( char * FileName);
	void	_setCaption( const char * pszPath);

// 작업
public:
	bool	CloseI3S(void);
	bool	LoadI3S( const char * pszPath);
	bool	SaveI3S( const char * pszPath);

	bool	LoadAnimation( const char * pszPath);

	bool	LoadFile( const char * pszPath);

	bool	CloseUI(void);
	bool	LoadUI(const char * pszPath);

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
	afx_msg void OnToolProjectConfig();
	afx_msg void OnDestroy();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnUpdateFileMruDef(CCmdUI *pCmdUI);
	afx_msg void OnFileMru();
	afx_msg void OnSetCameraPos1(void);
	afx_msg void OnSetCameraPos2(void);
	afx_msg void OnSetCameraPos3(void);
	afx_msg void OnSetCameraPos4(void);
	afx_msg void OnCameraPos1(void);
	afx_msg void OnCameraPos2(void);
	afx_msg void OnCameraPos3(void);
	afx_msg void OnCameraPos4(void);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnToolOptions();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnEditGeometrytool();
	afx_msg void OnEditTextool();

	afx_msg void OnViewMapScale();
};


