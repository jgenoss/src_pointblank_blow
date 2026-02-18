#pragma once


// CDummyWnd

class CDummyWnd : public CWnd
{
	DECLARE_DYNAMIC(CDummyWnd)

protected:
	i3Viewer *		m_pViewer;

public:
	CDummyWnd();
	virtual ~CDummyWnd();

	BOOL Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	void	SetViewer( i3Viewer * pViewer);

protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};


