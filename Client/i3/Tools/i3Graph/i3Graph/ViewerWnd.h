#pragma once


// CViewerWnd

class CViewerWnd : public CWnd
{
	DECLARE_DYNAMIC(CViewerWnd)

protected:
	i3Viewer		*	m_pViewer;

public:
	CViewerWnd();
	virtual ~CViewerWnd();

	BOOL Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	void	SetViewer( i3Viewer * pViewer);
//	BOOL	MouseWheel(UINT nFlags , short zDelta , CPoint pt);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


