#pragma once


// i3EditCtrl

class i3EditCtrl : public CEdit
{
	DECLARE_DYNAMIC(i3EditCtrl)
protected:
	bool	m_bFloatMode;
	BOOL	m_bEntered;

public:
	i3EditCtrl();
	virtual ~i3EditCtrl();

	BOOL	IsFloatMode(void)		{ return m_bFloatMode; }
	void	SetFloatMode( bool bF)	{ m_bFloatMode = bF; }

	BOOL	IsEntered(void)			{ return m_bEntered; }

	void	SetFont( HFONT hFont);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedChkDepthtest();
};


