#pragma once
#include "SceneGraphTreeCtrl.h"

// CPanel_SceneGraph dialog

class CPanel_SceneGraph : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_SceneGraph)

protected:
	CSceneGraphTreeCtrl		m_SgCtrl;

	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPanel_SceneGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_SceneGraph();

// Dialog Data
	enum { IDD = IDD_PANEL_SG };

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
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
