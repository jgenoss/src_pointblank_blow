// i3LoggerDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"

class CUdpReceiver; 

// Ci3LoggerDlg 대화 상자
class Ci3LoggerDlg : public CDialog
{
// 생성
public:
	Ci3LoggerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3LOGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;
	CUdpReceiver * m_pUdpReceiver; 

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_Log_Control;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonBlank();
};
