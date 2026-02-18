#pragma once
#include "afxwin.h"
#include "resource.h"


// CErrorDlg 대화 상자입니다.

class CErrorDlg : public CDialog
{
	DECLARE_DYNAMIC(CErrorDlg)

public:
	CErrorDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CErrorDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ERROR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CEdit		m_editServerIdx;		// 서버인덱스 출력
	CEdit		m_editErrorNum;			// 에러코드 출력
	afx_msg void OnBnClickedOk();
};
