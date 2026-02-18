// i3DumpDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// Ci3DumpDlg 대화 상자
class Ci3DumpDlg : public CDialog
{
// 생성
public:
	Ci3DumpDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3DUMP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	void		DumpFile( char * pszPath);

// 구현
protected:
	HICON m_hIcon;
	i3TDKLogCtrl		m_LogCtrl;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Encript;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CButton m_CHK_DumpCPP;
	bool				m_bEncript;
	bool				m_bDumpToCPP;

};
