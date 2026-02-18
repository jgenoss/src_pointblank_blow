// i3GraphDlg.h : 헤더 파일
//

#pragma once

#include "ViewerWnd.h"		
#include "PropertyWnd.h"	
#include "MessageWnd.h"		
//#include "i3Tree.h"		
#include "SceneGraphTreeCtrl.h"
#include "SpecWnd.h"
#include "OptimizeWnd.h"
#include "BaseSpecDialog.h"
#include "i3treePropertyBase.h"
#include "i3GraphFramework.h"

// Ci3GraphDlg 대화 상자
class Ci3GraphDlg : public CDialog
{
protected:	
	i3SceneGraphInfo *		m_pSgInfo;
	i3Viewer	*			m_pViewer; 
	i3GraphFramework *		m_pFramework;
	
	//Window
	CViewerWnd				m_ViewerWnd; 
	CPropertyWnd			m_PropertyWnd; 
	CSceneGraphTreeCtrl		m_TreeWnd;	
	CMessageWnd				m_MessageWnd; 
	COptimizeWnd			m_OptiWnd; 
	CSpecWnd				m_SpecWnd;

	i3TDKSplitterWnd		m_BaseSplitter;
	i3TDKSplitterWnd		m_SplitWnd1;  
	i3TDKSplitterWnd		m_SplitWnd2;
	i3TDKSplitterWnd		m_SplitWnd3;
	i3TDKSplitterWnd		m_SplitWnd4;

	BOOL				m_bViewAsWireFrame;
	BOOL				m_bUpdateAnim;

	char				m_i3sFileName[256]; 
	CString				m_RootDirectory;

	const char*			m_szCmdLineArg;

	bool				m_bSaveExternRef;

	void Seti3sFileName( const char *szPath );

// 생성
public:
	Ci3GraphDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3GRAPH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원
		
public:
	void SetCommandLineArg( const char *szCmdLineArg ){ m_szCmdLineArg = szCmdLineArg; }
	const char *GetRootDir(){ return LPCTSTR(m_RootDirectory); }

	// 절대 경로 구하기
	BOOL GetAbsolutePath( char* szOut, const char *szIn );

	// 상대 경로 구해줌
	BOOL GetRelativePath( char* szOut, const char *szIn, BOOL bUnixStyle = TRUE );

	//Scene Root
	i3SceneGraphInfo	*	getSgInfo(void)				{ return m_pSgInfo; }
	void		setSgInfo( i3SceneGraphInfo * pSgInfo);
	void	Log(INT32 LogLevel, const char *format, ...);  

	i3Node *				getSceneRoot(void)			{ return m_TreeWnd.getAddPoint(); }

	//SpecDialog
	BOOL	SetDialogWnd(CBaseSpecDialog * pDialog); 
	void	SetPropertyWnd(i3ElementBase * pProperty); 
// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//i3File Open And Save 
	i3SceneGraphInfo 	*	OpenI3sFile(char * FileName);
	void		SaveI3sFile(char * FileName); 

	//Check File Name
	BOOL		CheckFileName(char * FileName);

	//i3Scene Root Control 
	void		CloseSceneRoot(void); 
	void		InitSceneRoot(void); 

	void		CreateBoundBox(i3Node *pNode); 
	void		DeleteBoundBox(i3Node * pNode); 
	void		DeleteAttr( i3Node * pNode, i3ClassMeta * pMeta ); 

	void		UpdateAnimationState(void);
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void		ImportXFile(const char * FullPathName);

	void		SetSafeCheckMenuItem(UINT MenuID, BOOL IsCheck);
	void		InitMenuItemCheck();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileOpen();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileClose();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
//	afx_msg void OnHelpImage();
	afx_msg void OnToolTextureusagestate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnToolCreateboundbox();
	afx_msg void OnToolDeleteboundbox();
	afx_msg void OnFileExit();
	afx_msg void OnToolDirectory();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnViewViewaswireframe();
	afx_msg void OnNodeGeometrytool();
	afx_msg void OnOptDeleteattr();
	afx_msg void OnNodeBuildrenderablecollidermesh();
	afx_msg void OnViewUpdateanimation();
//	afx_msg void OnUpdateViewUpdateanimation(CCmdUI *pCmdUI);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNodeViewuvs();
	afx_msg void OnHelpDumpmemorytofile();
	afx_msg void OnViewLighting();
	afx_msg void OnViewGrid();
	afx_msg void OnViewGloweffect();
	afx_msg void OnViewDetails();
	afx_msg void OnBuildRenderableOctree();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnOptionBackgroundcolor();
	afx_msg void OnOptionAmbientcolor();
	afx_msg void OnGloweffectDownscale();
	afx_msg void OnGloweffectBrightpassextract();
	afx_msg void OnGloweffectGaussianblur();
	afx_msg void OnGloweffectCombine();
	afx_msg void OnRasterizechainAllclear();
	afx_msg void OnRasterizechainAllset();
	afx_msg void OnNodeStatistics();
	afx_msg void OnNodeExportgeometrylist();
	afx_msg void OnViewViewmapscale();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnFileSaveexternalresources();
	afx_msg void OnOptionGloballightsetting();
	afx_msg void OnRasterizechainTonemapping();
	afx_msg void OnFileImport();
};
