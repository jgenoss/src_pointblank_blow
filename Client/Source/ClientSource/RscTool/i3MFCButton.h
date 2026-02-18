#pragma once


class i3MFCButton;


struct i3MFCButtonCallback
{
	virtual void OnClick(i3MFCButton* ctrl) {}
	virtual void OnLButtonDown(i3MFCButton* ctrl) {}
	virtual void OnLButtonUp(i3MFCButton* ctrl) {}

	virtual ~i3MFCButtonCallback() {}
};


class i3MFCButton : public CMFCButton
{
	DECLARE_DYNAMIC(i3MFCButton)
public:

	i3MFCButton();
	virtual ~i3MFCButton();
	
	void					SetCallback(i3MFCButtonCallback* cb) { m_callback = cb; }
	i3MFCButtonCallback*	GetCallback() const { return m_callback; }

private:
	DECLARE_MESSAGE_MAP()
	
private:
	i3MFCButtonCallback*	m_callback;

public:
	afx_msg void OnBnClicked();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
