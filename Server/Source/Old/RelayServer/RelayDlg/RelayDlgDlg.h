// RelayDlgDlg.h : 헤더 파일
//

#pragma once


// CRelayDlgDlg 대화 상자
class CRelayDlgDlg : public CDialog
{
// 생성
public:
	CRelayDlgDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_RELAYDLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


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
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	CString m_sServerList;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
