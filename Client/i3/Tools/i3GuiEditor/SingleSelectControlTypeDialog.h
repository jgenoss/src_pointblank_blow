#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSingleSelectControlTypeDialog dialog

class CSingleSelectControlTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(CSingleSelectControlTypeDialog)

public:
	CSingleSelectControlTypeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSingleSelectControlTypeDialog();

// Dialog Data
	enum { IDD = IDD_SELECT_CONTROL };

	char	m_szSelectedMeta[256];
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_TypeCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
