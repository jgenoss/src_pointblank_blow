// i3ExternRefDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// Ci3ExternRefDlg 대화 상자
class Ci3ExternRefDlg : public CDialog
{
// 생성
public:
	Ci3ExternRefDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3EXTERNREF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
	i3RenderContext *		m_pCtx;
	i3PhysixContext *		m_pPhysixCtx;
	i3ParticleRender *		m_pParticleRender;

protected:
	void		_ReplaceControls( INT32 cx, INT32 cy);
	void		_Process( char * pszPath);

	static bool	_ReportProc( i3NamedElement * pObj, void * pData);

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	CButton m_ChkTextureCtrl;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnToolsOptions();
};
