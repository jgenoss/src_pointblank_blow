#pragma once
#include "afxwin.h"


// CDlgLight dialog

class CDlgLight : public CDialog
{
	DECLARE_DYNAMIC(CDlgLight)

protected:
	i3TDKColorCtrl		m_DiffuseCtrl;
	i3TDKColorCtrl		m_SpecularCtrl;

public:
	CDlgLight(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLight();

// Dialog Data
	enum { IDD = IDD_DLG_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ST_DiffusePH;
	CStatic m_ST_SpecularPH;
	CEdit m_ED_ShadowMapSize;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
