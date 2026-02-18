// BAServerDlg.h : 헤더 파일
//
#pragma once

#ifdef BUILDMODE_DLG

#include "afxwin.h"

// CBAServerDlg 대화 상자
class CBAServerDlg : public CDialog
{
// 생성
public:
	CBAServerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_BASERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;

	void CreateTimer(void); 
	void DeleteTimer(void); 

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	UINT32	m_ui32SessionCount;
	INT32	m_i32RoomCount;
	UINT32	m_ui32ServerTime;
	CString m_szNotice;
	BOOL	m_bNotice;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedUpdateButton();
	afx_msg void OnTimer(UINT nIDEvent);	
	afx_msg void OnBnClickedDumpUserButton();
	afx_msg void OnBnClickedBtNotice();
};

#endif