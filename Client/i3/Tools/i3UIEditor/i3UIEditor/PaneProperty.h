#pragma once

#include "PropCommon.h"
#include "PaneSpec.h"

// PaneUVEdit dialog
class CPaneProperty : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneProperty)

public:
	CPaneProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneProperty();

public:
// Dialog Data
	enum { IDD = IDD_PANE_PROPERTY };

	i3TDKWndFoldCtrl	m_WndFoldCtrl;

	CPropCommon			m_PropCommon;
	CPaneSpec			m_PaneSpec;

	void				_SetInfo(i3UIControl * pObj);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
