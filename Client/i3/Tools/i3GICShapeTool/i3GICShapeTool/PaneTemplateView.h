#pragma once

class CPaneTemplateView : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneTemplateView)

public:
	INT32			m_iCurrentTemplate;

	CMFCListCtrl	m_TemplateListCtrl;
	i3GICShapeTemplate *	m_pClippedTemplate;

public:
	CPaneTemplateView();
	virtual ~CPaneTemplateView();

	void		AddTemplate( void);
	void		RemoveTemplate( INT32 idx);
	void		RemoveAllTemplate( void);
	void		PasteTemplate( void);

	void		AttachShapeToTemplate( i3GICShape * pShape);
	void		DetachShapeFromTemplate( i3GICShape * pShape);

	void		UpdateListCtrl( void);

	void		ProcessRButtonUp( UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
	afx_msg void OnNMDBClickTemplateList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTemplateList(NMHDR * pNMHDR, LRESULT * pResult);

	afx_msg void OnEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult);
};

