#pragma once

class cAttributeToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// cAttributeWnd

class cAttributeWnd : public CDockablePane
{
	DECLARE_DYNAMIC(cAttributeWnd)

public:
	cAttributeWnd();
	virtual ~cAttributeWnd();
private:
	cAttributeToolBar m_AttributeBar;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


