#pragma once


// CPaneLog dialog

class CPaneLog : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneLog)

public:
	CPaneLog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneLog();

// Dialog Data
	enum { IDD = IDD_PANE_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
