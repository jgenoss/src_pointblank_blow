#pragma once

#include "i3ObjectEditorFramework.h"

// CPanelViewer dialog

class CPanelViewer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelViewer)

protected:
	i3TDKViewerCtrl				m_ViewerCtrl;
	i3ObjectEditorFramework *	m_pFramework;

public:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);


public:
	CPanelViewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelViewer();

// Dialog Data
	enum { IDD = IDD_PANEL_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
