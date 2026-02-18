// i3AnimOptimizerDlg.h : 헤더 파일
//

#pragma once

#include "PaneView.h"
#include "PaneTrack.h"
#include "PaneAnim.h"
#include "PaneBone.h"
#include "PaneSpec.h"

// Ci3AnimOptimizerDlg 대화 상자
class Ci3AnimOptimizerDlg : public CDialog
{
protected:
	i3TDKSplitterWnd	m_Splitter0;
	i3TDKSplitterWnd	m_Splitter1; 
	i3TDKSplitterWnd	m_Splitter2;
	i3TDKSplitterWnd	m_Splitter3;

	i3Viewer *			m_pViewer;

	CPaneView		m_PaneView;
	CPaneTrack		m_PaneTrack;
	CPaneAnim		m_PaneAnim;
	CPaneBone		m_PaneBone;
	CPaneSpec		m_PaneSpec;

public:

// 생성
public:
	Ci3AnimOptimizerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3ANIMOPTIMIZER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
