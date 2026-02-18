// i3EffectEditorDlg.h : 헤더 파일
//

#pragma once

#include "TimeTrackCtrl.h"
#include "DummyWnd.h"

#include "TimeTrackSpec.h"
#include "PlayControlDlg.h"
#include "i3PropertySheetCtrl.h"

#include "ParticleSpecParticle.h"
#include "ParticleSpecObject.h"
#include "ParticleSpecEmitter.h"
#include "ParticleSpecPosition.h"
#include "ParticleSpecRotate.h"
#include "ParticleSpecScale.h"
#include "ParticleSpecColor.h"

#include "PropertyKeyframe.h"

#include "GlobalVariable.h"

#define SPINCTRL_MAX_COUNT 10000
#define SPINCTRL_NEGATIVE_MAX_COUNT -10000

// Ci3EffectEditorDlg 대화 상자
class Ci3EffectEditorDlg : public CDialog
{
// 생성
public:
	Ci3EffectEditorDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3EFFECTEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
	i3Viewer *			m_pViewer;
	i3ViewerFramework *	m_pFramework;
	CDummyWnd 			m_wndViewerDummy;
	i3TDKSplitterWnd		m_BaseSplitter;
	i3TDKSplitterWnd		m_Split1, m_Split2, m_Split3;
	i3TDKSplitterWnd		m_Split4;
	i3TDKWndFoldCtrl		m_WndFoldCtrl;
	CTimeTrackCtrl		m_TimeTrackCtrl;
	BOOL				m_bPlay;
	BOOL				m_bPaused;

	CParticleSpecParticle		m_ParticleSpecParticle;
	CParticleSpecObject			m_ParticleSpecObject;
	CParticleSpecEmitter		m_ParticleSpecEmitter;
	CParticleSpecPosition		m_ParticleSpecPosition;
	CParticleSpecRotate			m_ParticleSpecRotate;
	CParticleSpecScale			m_ParticleSpecScale;
	CParticleSpecColor			m_ParticleSpecColor;

	CTimeTrackSpec				m_TimeTrackSpec;
	CPlayControlDlg				m_PlayControlDlg;

	i3PropertySheetCtrl			m_PropertyHolder;
	CPropertyKeyframe			m_PropertyKeyframe;

	i3AttrSet *			m_pRoot;
	i3TimeSequence *	m_pTimeSeq;
	i3AttrSet *			m_pWorld;
	i3Node *			m_pSky;
	i3Node *			m_pModel;
	i3Transform *		m_pEffectTrans;

	i3Transform *		m_pSelectedBone;
	BOOL				m_bAttach;

	HACCEL				m_hAccel;

	char				m_szFileName[MAX_PATH];

	i3TDKRibbonColor *		m_pCOL_ClearColor;

protected:
	i3Node *			LoadI3Scene( const char * pszPath);
	BOOL				SaveI3Scene( const char * pszPath, i3Node * pNode);

	BOOL				LoadBinaryProject( const char * pszPath);
	BOOL				LoadXMLProject(const char * pszPath);
	void				UnloadProject(void);
	BOOL				SaveBinaryProject( const char * pszPath);
	BOOL				SaveXMLProject(const char * pszPath);

	void				SetFileName( const char * pszPath);

	void				SetWorld( i3Node * pNode);
	void				SetSky( i3Node * pNode);
	void				SetModel( i3Node * pNode);

public :
	void				SetTimeSequence( i3TimeSequence * pTimeSeq);
	void				SelectObject( i3ElementBase * pObj);
	void				SelectParticle( i3Particle * pInfo);
	void				Reset(void);
	void				Play(void);
	void				Stop(void);
	void				Pause(void);
	void				OnDropFiles(HDROP hDropInfo);

	void				StopRender(void);
	void				GoRender(void);

	void				Edit( i3ElementBase * pObj, INT32 hint);
	void				OnUpdateEffect( i3ElementBase * pObj);

	void				ReorderSequence(void);

	void				SetFPS( UINT32 fps);

	void				OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
//	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT nIDEvent);
	afx_msg void OnFileOpensceneasworld();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnFileSaveas();
	afx_msg void OnFileSave32774();
	afx_msg void OnFileOpenscene();
	afx_msg void OnNewparticleSmoke();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnNewparticleFountain();
	afx_msg void OnNewparticleDust();
	afx_msg void OnTimetrackmenuDelete();
	afx_msg void OnTimetrackmenuAddclone();
	afx_msg void OnControlPlay();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnFileOpensceneassky();
	afx_msg void OnFileOpensceneasmodel();
	afx_msg void OnFileExporttoi3s();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnFileRemoveworld();
	afx_msg void OnFileRemovesky();
//	afx_msg void OnFileRemovemodel();
	afx_msg void OnFileNewproject();
	afx_msg void OnModelSelectbone();
	afx_msg void OnUpdateModelSelectbone(CCmdUI *pCmdUI);
	afx_msg void OnFileRemovemodel();
	afx_msg void OnToolsOptions();
	afx_msg void OnToolsBgcolor();
};
