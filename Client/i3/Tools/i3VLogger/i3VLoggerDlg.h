// i3VLoggerDlg.h : 헤더 파일
//

#pragma once

#include "UdpSocket.h"
#include "i3VLoggerFramework.h"

// Ci3VLoggerDlg 대화 상자
class Ci3VLoggerDlg : public CDialog
{
protected:
	CUdpSocket *			m_pSocket;
	i3TDKViewerCtrl			m_ViewerCtrl;
	i3VLoggerFramework *	m_pFramework;

// 생성
public:
	Ci3VLoggerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3VLOGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
