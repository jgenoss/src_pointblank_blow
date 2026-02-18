#pragma once
#include "afxwin.h"


// CDlgSetMapSectorIndex dialog

class CDlgSetMapSectorIndex : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetMapSectorIndex)

protected:
	i3ActionSetMapSector * m_pAction;

public:
	bool		Execute( i3ActionSetMapSector * pAction);

public:
	CDlgSetMapSectorIndex(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetMapSectorIndex();

// Dialog Data
	enum { IDD = IDD_DLG_SEMAPSECTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CEdit m_edSectorIndex;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
