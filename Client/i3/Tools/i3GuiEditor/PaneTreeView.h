#pragma once
#include "afxcmn.h"
//#include "TreeViewCtrl.h"

// CPaneTreeView dialog

class CPaneTreeView : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneTreeView)

public:
	CPaneTreeView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneTreeView();

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );

public:

// Dialog Data
	enum { IDD = IDD_PANE_TREEVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void	setSceneGraph( i3Node * pSG )	{ m_TreeViewCtrl.SetSceneGraph( pSG );}
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	i3TDKSceneGraphTreeCtrl m_TreeViewCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
