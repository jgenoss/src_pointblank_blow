#pragma once
#include "afxwin.h"


// CDlgActionSetWorldSection dialog

class CDlgActionSetWorldSection : public CDialog
{
	DECLARE_DYNAMIC(CDlgActionSetWorldSection)

protected:
	i3ActionSetWorldSection * m_pAction;

public:
	bool		Execute( i3ActionSetWorldSection * pAction);

public:
	CDlgActionSetWorldSection(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgActionSetWorldSection();

// Dialog Data
	enum { IDD = IDD_DLG_ACTION_SETWORLDSECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK() override;
public:
	virtual BOOL OnInitDialog() override;
	CEdit m_ED_Section;
};
