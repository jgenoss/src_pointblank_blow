#pragma once


// i3TDKFloatingEditCtrl

#define		WM_TDK_NOTIFY_ENTER			(WM_USER + 33009)

class i3TDKFloatingEditCtrl : public CEdit
{
	DECLARE_DYNAMIC(i3TDKFloatingEditCtrl)

protected:
	bool		m_bNotifyEnter = false;

public:
	i3TDKFloatingEditCtrl() {}
	virtual ~i3TDKFloatingEditCtrl() {}

	bool		isNotifyEnterKeyPress(void)					{ return m_bNotifyEnter; }
	void		setNotifyEnterKeyPress( bool bFlag)			{ m_bNotifyEnter = bFlag; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


