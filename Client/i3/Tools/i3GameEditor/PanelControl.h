#pragma once


// PanelControl dialog

class PanelControl : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(PanelControl)

public:
	PanelControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~PanelControl();

// Dialog Data
	enum { IDD = IDD_PANEL_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
