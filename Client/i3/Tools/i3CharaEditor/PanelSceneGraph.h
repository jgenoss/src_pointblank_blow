#pragma once


// CPanelSceneGraph dialog

class CPanelSceneGraph : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelSceneGraph)
protected:

public:
	i3TDKSceneGraphTreeCtrl		m_TreeCtrl;

	i3SceneGraphInfo *			m_pSg;

	void				SetSceneGraph( i3Node * pNode);

public:
	CPanelSceneGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSceneGraph();

// Dialog Data
	enum { IDD = IDD_PANEL_SCENEGRAPH };

protected:
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
