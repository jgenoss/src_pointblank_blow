#pragma once
#include "afxwin.h"


// CDlgMd5Key 대화 상자입니다.

class CDlgMd5Key : public CDialog
{
	DECLARE_DYNAMIC(CDlgMd5Key)

public:
	CDlgMd5Key(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMd5Key();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MD5KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString				m_strMd5Key00;
	CString				m_strMd5Key01;
	CEdit				m_ctMd5Key00;
	CEdit				m_ctMd5Key01;

	virtual BOOL		OnInitDialog();
	INT_PTR				DoModalEx( TCHAR strMD5[ 2 ][ NET_MD5_KEY_SIZE+1 ] );
};
