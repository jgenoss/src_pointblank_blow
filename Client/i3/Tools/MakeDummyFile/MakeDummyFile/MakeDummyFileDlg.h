// MakeDummyFileDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CMakeDummyFileDlg 대화 상자
class CMakeDummyFileDlg : public CDialog
{
// 생성
public:
	CMakeDummyFileDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_MAKEDUMMYFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	void	PutEncriptCode( HANDLE hFile);

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
	int m_Size;
	afx_msg void OnBnClickedBtnCreate();
	CEdit m_PathCtrl;
	BOOL m_bFillRandom;
	BOOL m_bChecksum;
};
