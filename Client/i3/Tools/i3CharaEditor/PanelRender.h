#pragma once

#include "i3CharaEditorFramework.h"

// CPanelRender dialog

class CPanelRender : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelRender)
protected:
	i3TDKViewerCtrl				m_ViewerCtrl;
	i3CharaEditorFramework *	m_pFramework;

public:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPanelRender(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelRender();

// Dialog Data
	enum { IDD = IDD_PANEL_RENDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
