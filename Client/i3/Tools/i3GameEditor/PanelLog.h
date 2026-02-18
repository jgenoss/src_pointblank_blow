#pragma once


// PanelLog dialog

class PanelLog : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(PanelLog)

public:
	PanelLog(CWnd* pParent = NULL);   // standard constructor
	virtual ~PanelLog();

// Dialog Data
	enum { IDD = IDD_PANEL_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
public:
	i3TDKLogCtrl	m_LogCtrl;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
