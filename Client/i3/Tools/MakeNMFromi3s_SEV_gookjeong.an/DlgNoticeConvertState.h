#pragma once

#include "resource.h"
#include "afxwin.h"

class CDlgNoticeConvertState : public CDialog
{
	DECLARE_DYNAMIC(CDlgNoticeConvertState)

public:
	CDlgNoticeConvertState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNoticeConvertState();

// Dialog Data
	enum { IDD = IDD_NOTICE_CONVERT_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_stcMessage;
	void	SetMessage( char* msg );
	char	strMsg[4096];
};
