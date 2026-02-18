#pragma once
#include "afxwin.h"


// CDlgISSVersion dialog

class CDlgISSVersion : public CDialog
{
	DECLARE_DYNAMIC(CDlgISSVersion)

protected:
	i3ShaderCache *		m_pCache;

public:
	bool		Execute( i3ShaderCache * pCache);

public:
	CDlgISSVersion(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgISSVersion();

// Dialog Data
	enum { IDD = IDD_ISS_VERSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_VersionCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
