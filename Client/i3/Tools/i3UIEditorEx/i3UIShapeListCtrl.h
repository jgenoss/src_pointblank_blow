#pragma once
#include "afxcmn.h"

class i3UIShapeListCtrl :	public CListCtrl
{
protected:
	i3UITemplate	*	m_pCurrentTemplate;

public:
	i3UIShapeListCtrl(void);
	~i3UIShapeListCtrl(void);
	DECLARE_MESSAGE_MAP()

public:
	void	setCurrentTemplate( i3UITemplate * pTemplate)	{	I3_REF_CHANGE( m_pCurrentTemplate, pTemplate); UpdateAllShape();}
	void	UpdateShape( INT32 idxItem, INT32 idxShape, i3UIShape * pShape);
	void	UpdateAllShape(void);

	INT32	getSelectedItem(bool bSelClick = false);

	void	PopUpShapeMenu(void);
	void	resortTemplateCount(i3UITemplate_User *);

	INT32	AddShape( void);
	INT32	RemoveShape( INT32 idx);
	INT32	EditShape( INT32 idx);
	INT32	MakeClone( INT32 idx);
	INT32	MoveShape( INT32 nSel, INT32 nMove );

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

protected:
	virtual LRESULT	WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);	
};
