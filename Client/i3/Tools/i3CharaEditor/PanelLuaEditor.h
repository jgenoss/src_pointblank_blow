#pragma once


// CPanelLuaEditor dialog

class CPanelLuaEditor : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelLuaEditor)

public:
	CPanelLuaEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelLuaEditor();

// Dialog Data
	enum { IDD = IDD_PANEL_LUA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
