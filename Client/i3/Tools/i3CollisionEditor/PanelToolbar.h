#pragma once


// CPanelToolbar dialog

class CPanelToolbar : public CDialog
{
	DECLARE_DYNAMIC(CPanelToolbar)

public:
	CPanelToolbar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelToolbar();

// Dialog Data
	enum { IDD = IDD_PANEL_TOOLBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
