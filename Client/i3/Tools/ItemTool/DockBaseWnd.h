#pragma once


class cBaseToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class cDockBaseWnd : public CDockablePane
{
	//DECLARE_DYNAMIC(cDockBaseWnd)
protected:
	cBaseToolBar m_BaseToolBar;

public:
	cDockBaseWnd();
	virtual ~cDockBaseWnd();

protected:
	DECLARE_MESSAGE_MAP()

public://사용자 함수
	virtual void	LoadToolBar(UINT resID);


public:	//MFC함수
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);

};

