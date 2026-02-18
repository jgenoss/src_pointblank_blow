#pragma once

// CPaneGuiTreeView dialog
#include "GuiTreeCtrl.h"

class CPaneGuiTreeView : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneGuiTreeView)

public:
	CPaneGuiTreeView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneGuiTreeView();

// Dialog Data
	enum { IDD = IDD_PANE_GUITREEVIEW };

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CGuiTreeCtrl m_TreeViewCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
