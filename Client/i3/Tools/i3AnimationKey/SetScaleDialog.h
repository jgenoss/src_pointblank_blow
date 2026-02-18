#pragma once
#include "afxwin.h"


// CSetScaleDialog dialog

class CSetScaleDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetScaleDialog)
protected:
	VEC3D		m_Scale;

public:
	bool Execute( VEC3D * pScale);

	VEC3D *	GetScale(void)			{ return &m_Scale; }

public:
	CSetScaleDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetScaleDialog();

// Dialog Data
	enum { IDD = IDD_SCALEKEYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_XCtrl;
	CEdit m_YCtrl;
	CEdit m_ZCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
