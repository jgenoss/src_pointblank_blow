#pragma once
#include "resource.h"
class CCalenderDlg : public CDialog
{
	DECLARE_DYNAMIC(CCalenderDlg)

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual	void OnOK();

public:
	CCalenderDlg(COleDateTime tmBegin, COleDateTime tmEnd, CWnd* pParent = NULL);
	virtual ~CCalenderDlg();

	enum { IDD = IDD_DIALOG_CALENDER };

	COleDateTime m_tmBegin;
	COleDateTime m_tmEnd;

};

