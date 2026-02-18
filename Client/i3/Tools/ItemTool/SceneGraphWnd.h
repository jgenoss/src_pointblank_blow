#pragma once

class cSceneGraphToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class cSceneGraphWnd : public CDockablePane
{
	DECLARE_DYNAMIC(cSceneGraphWnd)

	cSceneGraphToolBar m_SceneToolBar;
public:
	cSceneGraphWnd();
	virtual ~cSceneGraphWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


