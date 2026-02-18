// i3MemReportDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// Ci3MemReportDlg 대화 상자
class Ci3MemReportDlg : public CDialog
{
// 생성
public:
	Ci3MemReportDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3MEMREPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	void		Parse( const char * pszPath);
	void		UpdateInfos(bool bAscend = true);

	INT32		m_nCurSortMode;
	bool		m_bAscendOrder;
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
	CListCtrl m_ListCtrl;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	CEdit m_TotalSizeCtrl;
	afx_msg void OnBnClickedSaveButton();
	afx_msg void OnHdnItemclickList(NMHDR *pNMHDR, LRESULT *pResult);
};
