#pragma once


// CPanelLog dialog

class CPanelLog : public CDialog
{
	DECLARE_DYNAMIC(CPanelLog)

public:
	CPanelLog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelLog();

// Dialog Data
	enum { IDD = IDD_PANEL_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
