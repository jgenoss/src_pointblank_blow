// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "PanelAIGraph.h"
#include "PanelLog.h"
#include "PanelLuaEditor.h"
#include "PanelRender.h"
#include "PanelResource.h"
#include "PanelSceneGraph.h"
#include "PanelSpec.h"
#include "PanelTimeTrack.h"


#ifdef _USE_TOOLAM_
#include "AIStateEditor_Def.h"
#endif

//class CDlgNogadaMask;
class CMainFrame : public i3TDKDockFrameWnd
#ifdef _USE_TOOLAM_
	, public AIStateEdit::AIStateEditCallBack
#endif
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

protected:
	char		m_szFilePath[MAX_PATH];

// 특성
public:
	CPanelAIGraph		m_PanelAIGraph;
	CPanelLog			m_PanelLog;
	CPanelLuaEditor		m_PanelLuaEditor;
	CPanelRender		m_PanelRender;
	CPanelResource		m_PanelResource;
	CPanelSceneGraph	m_PanelSceneGraph;
	CPanelSpec			m_PanelSpec;
	CPanelTimeTrack		m_PanelTimeTrack;

	//bool	m_bBackGroundWindow;
	//int		m_nMaskSaveCount;
	//CDlgNogadaMask* m_pMaskWin;

// 작업
public:
	void		OnNewChara(void);
	void		OnOpenChara( const char * pszPath);
	void		OnSaveChara( const char * pszPath);
	void		OnCloseChara(void);

	bool		LoadResFile(const char* resPath, bool silence=false);
	bool		Exporti3Chr(const char* path, COptionInfo* tmpExpOpt=NULL);

	void		removeAllSceneGraph();
	//void		GenerateMask(LPWSTR* argv);

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _USE_TOOLAM_
// AIStateEditCallBack
public:
	virtual void AIStateEditCB_ScanUpperWeaponClassName(AIStateEdit::WeaponClassScanData* out);
#endif

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnToolOption();
	afx_msg void OnFileExporti3chr();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnAutosetCollidee();
	afx_msg void OnUpdateViewLinerender(CCmdUI *pCmdUI);
	afx_msg void OnViewLinerender();
	afx_msg void OnToolsCheckbones();
	afx_msg void OnBuildAnimpack();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnToolDumpAi();
	afx_msg void OnConvertAnimTemplate();
	afx_msg void OnMerge1pv3pv();
	afx_msg void OnCopyRefFile();
	afx_msg void OnDumpSharedAnim();

#ifdef _USE_TOOLAM_
	afx_msg void OnNogadaMask();
	afx_msg void OnNogadaAIState();
	afx_msg void OnNogadaAniAI();
#endif
};


