#pragma once


// CDlgNotice 대화 상자입니다.

class CDlgNotice : public CDialog
{
	DECLARE_DYNAMIC(CDlgNotice)

public:
	CDlgNotice(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNotice();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_NOTICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString				m_strNotice;
};
