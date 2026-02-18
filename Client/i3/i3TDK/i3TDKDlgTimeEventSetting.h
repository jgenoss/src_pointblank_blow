#pragma once
#include "afxwin.h"


// i3TDKDlgTimeEventSetting dialog

class i3TDKDlgTimeEventSetting : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgTimeEventSetting)
protected:
	i3TimeEventGen * m_pTimeEvent = nullptr;

public:
	i3TDKDlgTimeEventSetting(CWnd* pParent = nullptr) : CDialog(i3TDKDlgTimeEventSetting::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgTimeEventSetting() {}

	bool	Execute( i3TimeEventGen * pTimeEvt);

// Dialog Data
	enum { IDD = IDD_TDK_DLG_TIMEEVENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	CButton m_CHK_Repeat;
};
