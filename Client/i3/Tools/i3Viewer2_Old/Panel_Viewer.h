#pragma once

#include "i3Viewer2Framework.h"

// CPanel_Viewer dialog

class CPanel_Viewer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_Viewer)

protected:
	i3TDKViewerCtrl				m_ViewerCtrl;
	i3Viewer2Framework *		m_pFramework;

public:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPanel_Viewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Viewer();

// Dialog Data
	enum { IDD = IDD_PANEL_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
