#pragma once


// CPanelSceneGraph dialog

class CPanelSceneGraph : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelSceneGraph)

public:
	i3TDKSceneGraphTreeCtrl		m_TreeCtrl;

	void				SetSceneGraph( i3Node * pNode);
	void				OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPanelSceneGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSceneGraph();

// Dialog Data
	enum { IDD = IDD_PANEL_SCENEGRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
