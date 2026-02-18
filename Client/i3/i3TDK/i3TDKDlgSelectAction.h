#pragma once
#include "afxwin.h"

#include "i3TDKDialogBase.h"

// i3TDKDlgSelectAction dialog

class i3TDKDlgSelectAction : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKDlgSelectAction)

protected:
	char		m_szSelActionName[MAX_PATH] = { 0 };

public:
	i3TDKDlgSelectAction(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKDlgSelectAction::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgSelectAction() {}

	char *		getSelectedActionClassName(void)		{ return m_szSelActionName; }

// Dialog Data
	enum { IDD = IDD_TDK_DLG_SELECTACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CComboBox m_ActionCtrl;
	CStatic m_HelpCtrl;
	afx_msg void OnCbnSelchangeTdkCbAction();
	afx_msg void OnDestroy();
};
