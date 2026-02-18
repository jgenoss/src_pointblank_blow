#pragma once
#include "afxwin.h"


// CDlgRegenOption dialog

class CDlgRegenOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgRegenOption)

public:
	I3G_SHADER_VERSION		m_vsVersion;
	I3G_SHADER_VERSION		m_psVersion;
	I3G_GPU_VENDOR			m_Vendor;


public:
	CDlgRegenOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRegenOption();

	bool	Execute(void);

// Dialog Data
	enum { IDD = IDD_REGEN_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CB_Vendor;
	CComboBox m_CB_VSVersion;
	CComboBox m_CB_PSVersion;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
public:
	CButton m_chkOverride;

	bool					m_bOverride;
};
