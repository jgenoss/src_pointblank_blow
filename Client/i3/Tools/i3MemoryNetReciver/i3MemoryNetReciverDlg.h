// i3MemoryNetReciverDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CUdpRecv; 

// Ci3MemoryNetReciverDlg 대화 상자
class Ci3MemoryNetReciverDlg : public CDialog
{
// 생성
public:
	Ci3MemoryNetReciverDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3MEMORYNETRECIVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	BOOL		m_bRecv; 
	CUdpRecv *	m_pUdpRecv; 
	


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_ListControl;
	afx_msg void OnBnClickedButtonRecvOnoff();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonPrint();
	CButton m_OnOffButton;
	afx_msg void OnBnClickedButtonClearList();
	afx_msg void OnBnClickedButtonFilePrint();
	CProgressCtrl m_ProgressBar;
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	int m_ReadCount;
};
