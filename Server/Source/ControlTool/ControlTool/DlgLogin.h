#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgLogin 대화 상자입니다.

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLogin();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString				m_strID;
	CString				m_strPass;
	CEdit				m_ctPass;
	CIPAddressCtrl		m_ctIPAddress;
	UINT				m_ui32Port;
	DWORD				m_dwIP;
	CButton				m_ctCheckWarning;
	INT32				m_i32WarningSwitch;

	INT_PTR				DoModalEx( TCHAR* strID, DWORD dwIP, UINT ui32Port );
	virtual BOOL		OnInitDialog();
	afx_msg void		OnBnClickedOk();
	
};
