#pragma once


// CDlgAuth 대화 상자입니다.

class CDlgAuth : public CDialog
{
	DECLARE_DYNAMIC(CDlgAuth)

public:
	CDlgAuth(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAuth();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_AUTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString				m_strID;
	CString				m_strPass;

	afx_msg void		OnBnClickedOk();
};
