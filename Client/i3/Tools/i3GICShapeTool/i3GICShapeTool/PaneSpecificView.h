#pragma once

class CPaneSpecificView : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneSpecificView)

private:
	CMFCPropertyGridCtrl		m_GridCtrl;

public:
	CPaneSpecificView();
	virtual ~CPaneSpecificView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize( UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
	afx_msg void OnSelectObject();
	afx_msg void OnUnselectObject();
};

