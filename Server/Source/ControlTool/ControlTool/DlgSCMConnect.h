#pragma once
#include "afxcmn.h"


// CDlgSCMConnect 대화 상자입니다.

class CDlgSCMConnect : public CDialog
{
	DECLARE_DYNAMIC(CDlgSCMConnect)

public:
	CDlgSCMConnect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSCMConnect();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_CONNECT_SCM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	
	CIPAddressCtrl		m_ctIPAddress;
	UINT				m_ui32Port;
	DWORD				m_dwIP;

	INT_PTR				DoModalEx( DWORD dwIp, UINT ui32Port );
	virtual	BOOL		OnInitDialog();
	afx_msg void		OnBnClickedOk();
};
