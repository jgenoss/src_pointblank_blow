#pragma once
#include "afxwin.h"


// CDlgBuildConvex dialog

class CDlgBuildConvex : public CDialog
{
	DECLARE_DYNAMIC(CDlgBuildConvex)

public:
	CDlgBuildConvex(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBuildConvex();

// Dialog Data
	enum { IDD = IDD_DLG_BUILDCONVEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
public:
	CButton m_GlobalHullCtrl;
	CButton m_DynamicCtrl;
	CEdit m_ElasticityCtrl;
	CEdit m_FrictionCtrl;
};
