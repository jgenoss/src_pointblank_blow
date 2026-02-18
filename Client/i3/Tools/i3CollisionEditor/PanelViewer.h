#pragma once

#include "CollisionEditorFramework.h"

// CPanelViewer dialog

class CPanelViewer : public CDialog
{
	DECLARE_DYNAMIC(CPanelViewer)

protected:
	i3TDKViewerCtrl				m_ViewerCtrl;
	CollisionEditorFramework *			m_pFramework;

public:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPanelViewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelViewer();

// Dialog Data
	enum { IDD = IDD_PANEL_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
