#pragma once

class cPropertyEditToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


// cPropertyEdit

class cPropertyEdit : public CDockablePane
{
	DECLARE_DYNAMIC(cPropertyEdit)

public:
	cPropertyEdit();
	virtual ~cPropertyEdit();

	cPropertyEditToolBar m_PropertyEditBar;
protected:
	DECLARE_MESSAGE_MAP()
};


