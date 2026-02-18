#pragma once
#include "afxwin.h"


// CDlgOption dialog

class CDlgOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgOption)
protected:
	i3TDKColorCtrl		m_A_Ctrl;
	i3TDKColorCtrl		m_B_Ctrl;
	i3TDKColorCtrl		m_LightD_Ctrl;
	i3TDKColorCtrl		m_LightS_Ctrl;

public:
	CDlgOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOption();

// Dialog Data
	enum { IDD = IDD_DLG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ST_A;
	CStatic m_ST_B;
	CEdit m_ED_ShadowSize;
	CStatic m_ST_LitD;
	CStatic m_ST_LitS;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CButton m_CHK_Glow;
};
